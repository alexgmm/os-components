#pragma once

#include "neighborhood.hpp"
#include "mutation.hpp"
#include "time.hpp"

using namespace std;

class GreedyIterator
{
    Solution s, globalBestSolution;
    unsigned perturbationNumber, oper;

    bool applyMutationGreedly(unsigned o)
    {
        bool improved = false;
        vector<Mutation> mutations = s.listPossibleMutations(o, oper);

        if (mutations.size() == 0)
            return false;

        Neighborhood n(s);

        unsigned bestValue = s.computeMakespan(), value, globalBestValue = globalBestSolution.computeMakespan();
        Mutation bestMutation = {0, 0, 0, 0};

        for (Mutation m : mutations)
        {
            value = n.applyMutation(m);

            if (value < globalBestValue)
            {
                globalBestValue = value;
                globalBestSolution = n.getSolution().copySolution();
            }

            n.restore();

            if (value < bestValue && value > 0)
            {
                improved = true;
                bestValue = value;
                bestMutation = m;
            }
        }

        n.applyMutation(bestMutation);

        s = n.getSolution();
        return improved;
    }

    Mutation getOneMutation()
    {
        vector<Mutation> mutations;
        unsigned o;

        while (mutations.size() == 0)
        {
            o = s.getOneRandomOperation_shiftWhole();
            mutations = s.listPossibleMutations_shiftWhole(o);
        }

        auto randomIndex = mutations.size() == 1 ? 0 : randint(0, mutations.size() - 1);
    }

    void improve()
    {
        bool isImproving;

        do
        {
            isImproving = false;
            vector<unsigned> operations = s.getAllOperations(oper);
            //printv(operations, 0, "all ops");
            for (auto o : operations)
                isImproving = isImproving || applyMutationGreedly(o);

        } while (isImproving && !isTimeOver());
    }

    void perturbation()
    {
        unsigned value;
        Neighborhood n(s);
        Mutation m;

        do
        {
            n.restore();
            n.applyMutation(m);
            value = n.getSolution().computeMakespan();
        } while (value == UMAX);

        s = n.getSolution();
    }

    void iterate()
    {
        improve();
        for (unsigned i = 0; i < perturbationNumber; i++)
            perturbation();
    }

public:
    GreedyIterator() {}
    GreedyIterator(Solution solution, unsigned number, unsigned o)
    {
        s = solution;
        globalBestSolution = solution.copySolution();
        oper = o;
        perturbationNumber = number;
    }

    void test()
    {
        vector<unsigned> ops = s.getAllOperations_shiftCritical();
        vector<Mutation> mutations;
        for (unsigned o : ops)
        {
            vector<Mutation> ms = s.listPossibleMutations(o, oper);
            for (Mutation m : ms)
                mutations.push_back(m);
        }

        Neighborhood n(s);
        for (Mutation m : mutations)
        {
            printMutation(m);
            n.applyMutation(m);
            n.getSolution().printJobCluster();
            n.restore();
        }
    }

    unsigned solve()
    {
        startTimeCounting();

        while (!isTimeOver())
            iterate();

        return globalBestSolution.computeMakespan();
    }

    Solution getSolution()
    {
        return s;
    }
};