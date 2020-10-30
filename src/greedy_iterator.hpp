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
        unsigned o;
        do
            o = s.getOneRandomOperation(oper);
        while (used[o]);

        return o;
    }

    void applyMutationGreedly(unsigned o)
    {
        vector<Mutation> mutations = s.listPossibleMutations(o, oper);

        if (mutations.size() == 0)
            return;

        Neighborhood n(s);

        unsigned bestValue = s.computeMakespan(), value;
        Mutation bestMutation = {0, 0, 0, 0};

        for (Mutation m : mutations)
        {
            /* s.printJobCluster();
            if (CURRENT_GRAPH_NUMBER < MAX_GRAPH_NUMBER)
            {
                TRACK_SHIFT_OPERATIONS = true;
                printMutation(m);
            } */
            value = n.applyMutation(m);
            //TRACK_SHIFT_OPERATIONS = false;

            //n.getSolution().printJobCluster();
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
        vector<unsigned> used(s.nO + 1, 0);
        unsigned o = getOneRandomOperation(used);
        printl("o", o);
        vector<Mutation> ms = s.listPossibleMutations(o, oper);
        Neighborhood n(s);
        for (Mutation m : ms)
        {
            printMutation(m);
            n.applyMutation(m);
            cout << n.getSolution().computeMakespan() << br;
            n.restore();
        }
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