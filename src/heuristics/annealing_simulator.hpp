#pragma once

#include <cmath>
#include "neighbor_generator.hpp"
#include "../helpers/time.hpp"

using namespace std;

class AnnealingSimulator
{
public:
    unsigned alpha, minTemperature, iterations, oper;
    double T = 1;
    const double e = 2.718281828;
    Schedule solution, globalBest;
    NeighborGenerator generator;

    bool accept(unsigned v0, unsigned v1)
    {
        return (v0 < v1) ||
               (rand() / (double)RAND_MAX) <= pow(e, -1 * (v0 - v1) / T);
    }

    void setParams(unsigned a, unsigned t, unsigned i)
    {
        alpha = a;
        minTemperature = t;
        iterations = i;
    }

    void setDefaultParams()
    {
        switch (oper)
        {
         case SWAP_CRITICAL:
            setParams(0.0837, 0.0002, 97708);
            break;
        case SWAP_CRITICAL_EDGE:
            setParams(0.249, 0.0006, 95952);
        case SHIFT_CRITICAL:
            setParams(0.5645, 0.0008, 71323);
        case SHIFT_WHOLE:
            setParams(0.3443, 0.0006, 67573);
            break;
        }
    }

    void setSolution(Schedule s)
    {
        solution = s;
        generator.setSchedule(solution);
    }

    void setOper(unsigned o)
    {
        oper = o;
    }

    AnnealingSimulator() {}
    AnnealingSimulator(unsigned o)
    {
        oper = o;
        setDefaultParams();
    }

    unsigned solve()
    {
        startTimeCounting();

        unsigned makespan = solution.computeMakespan();
        unsigned tempMakespan, globalBestMakespan = makespan;
        bool shouldStop = false;
        Schedule tempS;

        for (T; T > minTemperature && !shouldStop; T *= alpha)
        {
            for (unsigned i = 0; i < iterations && !shouldStop; i++)
            {
                tempS = generator.getRandomNeighbor(oper);
                tempMakespan = tempS.computeMakespan();

                if (accept(tempMakespan, makespan))
                {
                    solution = tempS.getCopy();
                    generator.setSchedule(tempS);
                    makespan = tempMakespan;
                    if (tempMakespan < globalBestMakespan)
                    {
                        globalBestMakespan = tempMakespan;
                        globalBest = solution.getCopy();
                    }
                }
                else
                    generator.restore();

                shouldStop = isTimeOver();
            }
        }

        return globalBestMakespan;
    }
};