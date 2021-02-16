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
    unsigned timesImproved = 0, solutionsReached = 0;
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
                solutionsReached++;
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
            timesImproved++;

            globalBestSolution = incumbent.getCopy();
        }
        //cout << wrapStringInLabel("/\\ GLOBAL FOUND: /\\");

        //tabuList.printCurrentTabu();
        //incumbent.assertValidSchedule();
    }
    void setDefaultParams()
    {
        switch (oper)
        {
        case SWAP_CRITICAL:
            duration = 42;
            break;
        case SWAP_CRITICAL_EDGE:
            duration = 62;
            break;
        case SHIFT_CRITICAL:
            duration = 99;
            break;
        case SHIFT_WHOLE:
            duration = 76;
            break;
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
    TabuSearcher(unsigned o)
    {
        oper = o;
        setDefaultParams();
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
    friend class HeuristicResultGenerator;
};