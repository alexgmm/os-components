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
    PerturbationGenerator solution, incumbent;
    TabuList tabuList;

    void setParam(unsigned d)
    {
        duration = d;
    }

    TabuList newList()
    {
        unsigned type = oper == SWAP_CRITICAL || oper == SWAP_CRITICAL_EDGE ? SWAP : SHIFT;
        TabuList t(solution.nO, duration, type);
        return t;
    }

    void selectAndApplyBestMovement()
    {
        NeighborGenerator n(incumbent);
        Movement sc, bestAllowedChange = {0, 0, 0}, bestTabuChange = {0, 0, 0}; //, bestTabuChange = {0, 0, 0};
        Perturbation bestAllowedPerturbation, bestTabuPerturbation;             //, bestTabuPerturbation = {0, 0, 0, 0};
        unsigned incumbentValue = incumbent.computeMakespan(), value;
        unsigned bestValue = incumbentValue * 100;

        auto perts = incumbent.listAllPerturbations(oper);

        for (auto p : perts)
        {
            sc = n.applyPerturbation(p);
            value = n.getSchedule().computeMakespan();
            n.restore();
            if (value < bestValue)
            {
                bestValue = value;
                if (!tabuList.isTabu(sc))
                {
                    printl("found tabu:", value);
                    bestAllowedChange = sc;
                    bestAllowedPerturbation = p;
                }
                else
                {
                    printl("found not tabu:", value);
                    bestTabuChange = sc;
                    bestTabuPerturbation = p;
                }
            }
        }

        if (!isScheduleChangeEmpty(bestAllowedChange))
        {
            n.applyPerturbation(bestAllowedPerturbation);
            incumbent = n.getSchedule();
            tabuList.insertTabuMovement(bestAllowedChange);
        }
        else if (!isScheduleChangeEmpty(bestTabuChange))
        {
            n.applyPerturbation(bestTabuPerturbation);
            incumbent = n.getSchedule();
            tabuList.insertTabuMovement(bestTabuChange);
        }

        if (incumbent.computeMakespan() < solution.computeMakespan())
            solution = incumbent.getCopy();
    }

    void setDefaultParams()
    {
        switch (oper)
        {
        default:
            break;
        }
    }

    void setSolution(PerturbationGenerator s)
    {
        solution = s;
        incumbent = solution.getCopy();
    }

    TabuSearcher() {}
    TabuSearcher(unsigned o, unsigned d)
    {
        oper = o;
        duration = d;
        tabuList = newList();
    }

    unsigned solve()
    {
        startTimeCounting();
        Perturbation p;
        NeighborGenerator n(solution);

        while (!isTimeOver())
        {
            selectAndApplyBestMovement();
            tabuList.iterate();
        }

        return solution.getMakespan();
    }
};