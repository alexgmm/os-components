#pragma once

unsigned TABU_DURATION = 5;

#include <map>
#include <deque>
#include "../helpers/time.hpp"
#include "tabu_list.hpp"
#include "../neighbor_generator.hpp"

using namespace std;

class TabuSearcher
{
public:
    unsigned duration, oper;
    Schedule globalBestSolution, incumbent;
    TabuList tabuList;

    void setParam(unsigned d)
    {
        duration = d;
    }
    TabuList newList()
    {
        unsigned type = oper == SWAP_CRITICAL || oper == SWAP_CRITICAL_EDGE ? SWAP : SHIFT;
        TabuList t(globalBestSolution.nO, duration, type);
        return t;
    }
    void applyBestMovement()
    {
        cout << wrapStringInLabel("SEARCHING BEST NEIGHBOR");
        incumbent.assertValidSchedule();
        NeighborGenerator n(incumbent);
        Movement bestMovement;
        Schedule bestSchedule;
        unsigned value, bestValue = UMAX;
        auto mpn = n.getMovementsPerNeighbor(oper);

        for (auto i : mpn)
        {
            //printMovement(i.first);
            value = i.second.getMakespan();
            //printl("found:", value);
            if (value < bestValue && !tabuList.isTabu(i.first))
            {
                bestMovement = i.first;
                bestSchedule = i.second.getCopy();
                bestValue = value;
            }
        }
        cout << "is there a valid movement?\n";
        if (isMovementEmpty(bestMovement))
            return;
        cout << "yes\n";
        incumbent = bestSchedule;
        //printMovement(bestMovement);
        cout << br << "new incumbent" << br;
        /* Printer::printJobCluster(incumbent);
        Printer::printSolution(incumbent); */
        tabuList.insertTabuMovement(bestMovement);
        cout << "inserting in tabu\n";
        if (bestValue < globalBestSolution.getMakespan())
        {
            globalBestSolution = incumbent.getCopy();
            cout << wrapStringInLabel("/\\ GLOBAL FOUND: /\\");
        }

        //tabuList.printCurrentTabu();
    }
    void setDefaultParams()
    {
        switch (oper)
        {
        default:
            break;
        }
    }

    void setSolution(Schedule s)
    {
        globalBestSolution = s;
        incumbent = globalBestSolution.getCopy();
        tabuList = newList();
    }

    TabuSearcher() {}
    TabuSearcher(unsigned o, unsigned d)
    {
        oper = o;
        duration = d;
    }

    unsigned solve()
    {
        startTimeCounting();
        Perturbation p;

        while (!isTimeOver())
        {
            applyBestMovement();
            //Printer::printJobCluster(incumbent);
            tabuList.iterate();
        }

        return globalBestSolution.getMakespan();
    }
};