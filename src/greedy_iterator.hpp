#pragma once

#include "neighborhood.hpp"
#include "mutation.hpp"
#include "time.hpp"

using namespace std;

class GreedyIterator
{
    Solution s;
    unsigned numberOps, oper;

    unsigned getOneRandomOperation(vector<unsigned> &used)
    {
        unsigned o = 0;
        while(used[o])
            o = s.getOneRandomOperation(oper);
        return o;
    }

    void applyMutationGreedly(unsigned o)
    {
        vector<Mutation> mutations = s.listPossibleMutations(o, oper);

        if (mutations.size() == 0)
            return;

        Neighborhood n(s);

        unsigned bestValue = UMAX, value;
        Mutation bestMutation = {0, 0, 0, 0};

        for (Mutation m : mutations)
        {
            //printMutation(m);
            value = n.applyMutation(m);
            n.restore();

            if (value < bestValue && value > 0)
            {
                bestValue = value;
                bestMutation = m;
            }
        }

        n.applyMutation(bestMutation);

        s = n.getSolution();
    }

public:
    GreedyIterator() {}
    GreedyIterator(Solution solution, unsigned number, unsigned o) : s(solution),
                                                                     numberOps(number),
                                                                     oper(o) {}

    void iterate()
    {
        vector<unsigned> used(s.nO + 1, 0);
        used[0] = 1;
        for (unsigned count = 1; count <= numberOps; count++)
        {
            //cout <<getElapsedTime() << br;
            unsigned o = getOneRandomOperation(used);
            if (o == 0)
                break;
            applyMutationGreedly(o);
        }
    }

    void test()
    {
        
    }

    unsigned solve()
    {
        startTimeCounting();

        while (!isTimeOver())
            iterate();

        return s.computeMakespan();
    }

    Solution getSolution()
    {
        return s;
    }
};