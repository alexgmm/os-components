#pragma once

#include "../neighbor_generator.hpp"
#include "../helpers/time.hpp"

using namespace std;

class AnnealingSimulator
{
public:
    unsigned alpha, minTemperature, iterations, oper;
    double T = 1;
    Schedule solution;
    NeighborGenerator generator;

    unsigned absol(unsigned n1)
    {
        if (n1 < 0)
            return -n1;
        return n1;
    }
    bool accept(unsigned v0, unsigned v1)
    {
        return (v0 < v1) || (rand() / (double)RAND_MAX) <= exp(0 - absol(v0 - v1) / T);
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
            setParams(0.3277, 0.0007, 20000);
            break;
        case SWAP_CRITICAL_EDGE:
            setParams(0.3772, 0.0006, 5857);
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
        int tempMakespan;
        assert(makespan > 0);
        bool shouldStop = false;
        Schedule tempS;

        for (T; T > minTemperature && !shouldStop; T *= alpha)
        {
            for (unsigned i = 0; i < iterations && !shouldStop; i++)
            {
                //cout << "current is:" << br;
                //Printer::printJobCluster(solution);
                tempS = generator.getRandomNeighbor(oper);
                //cout << "new temp is:" << br;
                //Printer::printJobCluster(tempS);
                //Printer::printSolution(tempS);
                tempMakespan = tempS.computeMakespan();
                //printl("temp makespan found:", tempMakespan);
                if (accept(tempMakespan, makespan))
                {
                    //cout << "accepted" << br;
                    solution = tempS.getCopy();
                    generator.setSchedule(tempS);
                    makespan = tempMakespan;
                }
                else
                    generator.restore();

                shouldStop = isTimeOver();
            }
        }

        return makespan;
    }
};