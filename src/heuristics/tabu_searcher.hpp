#pragma once

unsigned TABU_DURATION = 5;

#include <map>
#include <deque>
#include "../helpers/time.hpp"
#include "tabu_list.hpp"
#include "neighbor_generator.hpp"

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
        //cout << wrapStringInLabel("SEARCHING BEST NEIGHBOR");
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
        if (bestValue == UMAX)
            return;

        incumbent = bestSchedule;
        //incumbent.printSchedule();
        /* Printer::printJobCluster(incumbent);
        Printer::printSolution(incumbent); */
        tabuList.insertTabuMovement(bestMovement);
        if (bestValue < globalBestSolution.getMakespan())
        {
            globalBestSolution = incumbent.getCopy();
            //cout << wrapStringInLabel("/\\ GLOBAL FOUND: /\\");
        }

        //tabuList.printCurrentTabu();
        incumbent.assertValidSchedule();
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