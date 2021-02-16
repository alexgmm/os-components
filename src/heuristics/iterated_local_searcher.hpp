#pragma once

#include "neighbor_generator.hpp"
#include "../perturbation/perturbation.hpp"
#include "../helpers/printer.hpp"
#include "../helpers/time.hpp"

using namespace std;

class IteratedLocalSearcher
{
    Schedule solution, globalBest;
    NeighborGenerator generator;
    unsigned perturbationNumber, oper;
    unsigned timesImproved = 0, solutionsReached = 0, timesLocalSearch = 0;
    //string outfile = "/home/hal/running_report.txt";

    void setDefaultParams()
    {
        switch (oper)
        {
        case SHIFT_WHOLE:
            perturbationNumber = 2;
            break;
        default:
            perturbationNumber = 1;
            break;
        }
    }

    void improve()
    {
        //cout << wrapStringInLabel("\\/ IMPROVING \\/");
        bool isImproving = false;

        do
        {
            auto best = generator.getBestNeighbor(oper);

            unsigned found = best.getMakespan(),
                     current = solution.getMakespan(),
                     global = globalBest.getMakespan();

            if (found < current)
            {
                setSolution(best);
                isImproving = true;
                solutionsReached++;

                if (found < global)
                {
                    timesImproved++;
                    globalBest = best.getCopy();
                }
            }
            else
                isImproving = false;

        } while (isImproving && !isTimeOver());
    }

    void perturbation()
    {
        //cout << wrapStringInLabel("\\/ PERTURBATING \\/");
        Schedule perturbated = generator.getRandomNeighbor(SHIFT_WHOLE);
        //cout << perturbated.getMakespan() << br;
        setSolution(perturbated);
    }

    void iterate()
    {
        improve();
        timesLocalSearch++;
        for (unsigned i = 0; i < perturbationNumber && !isTimeOver(); i++)
            perturbation();
    }

public:
    IteratedLocalSearcher() {}
    IteratedLocalSearcher(unsigned o)
    {
        oper = o;
        setDefaultParams();
    }

    void setSolution(Schedule s)
    {
        solution = s;
        generator.setSchedule(solution);
    }

    unsigned solve()
    {
        startTimeCounting();
        globalBest = solution.getCopy();

        while (!isTimeOver())
            iterate();

        return globalBest.computeMakespan();
    }

    Schedule getSolution()
    {
        return solution;
    }

    friend class HeuristicResultGenerator;
};