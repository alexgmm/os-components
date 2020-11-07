#pragma once

#include "neighbor_generator.hpp"
#include "time.hpp"

using namespace std;

class AnnealingSimulator
{
public:
    unsigned alpha, minTemperature, iterations, oper;
    Solution solution;

    unsigned absol(unsigned n1)
    {
        if (n1 < 0)
            return -n1;
        return n1;
    }
    bool accept(double t, unsigned v_new, unsigned v_old) { return (rand() / (double)RAND_MAX) <= exp(0 - absol(v_new - v_old) / t); }

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
            setParams(0.2232, 0.0006, 7631);
            break;
        case SWAP_CRITICAL_EDGE:
            setParams(0.3772, 0.0006, 5857);
            break;
        }
    }

    void setSolution(Solution s)
    {
        solution = s;
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
        double t = 1;
        bool shouldStop = false;
        Solution tempS;
        while (t > minTemperature && !shouldStop)
        {
            for (unsigned i = 0; i < iterations && !shouldStop; i++)
            {
                tempMakespan = -1;
                while (tempMakespan <= 0)
                {
                    tempS = NeighborGenerator::getRandomNeighbor(oper, solution);
                    tempMakespan = tempS.computeMakespan();
                }
                if (tempMakespan < makespan || accept(t, tempMakespan, makespan))
                {
                    solution = tempS.copySolution();
                    makespan = tempMakespan;
                }

                shouldStop = isTimeOver();
            }
            t *= alpha;
        }
        return makespan;
    }
};