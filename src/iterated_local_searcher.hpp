#pragma once

#include "neighbor_generator.hpp"
#include "perturbation.hpp"
#include "time.hpp"

using namespace std;

class IteratedLocalSearcher
{
    Solution s, globalBestSolution;
    unsigned perturbationNumber, oper;

    bool applyMutationGreedly(unsigned o)
    {
        bool improved = false;
        vector<Perturbation> perturbations = s.listPossibleMutations(o, oper);

        if (perturbations.size() == 0)
            return false;

        NeighborGenerator n(s);

        unsigned bestValue = s.computeMakespan(), value, globalBestValue = globalBestSolution.computeMakespan();
        Perturbation bestMutation = {0, 0, 0, 0};

        for (Perturbation m : perturbations)
        {
            value = n.applyMutation(m);
            //cout << " " << value;
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

    Perturbation getOneMutation()
    {
        vector<Perturbation> perturbations;
        unsigned o;

        while (perturbations.size() == 0)
        {
            o = s.getOneRandomOperation_shiftWhole();
            perturbations = s.listPossibleMutations_shiftWhole(o);
        }

        auto randomIndex = perturbations.size() == 1 ? 0 : randint(0, perturbations.size() - 1);

        return perturbations[randomIndex];
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
        NeighborGenerator n(s);
        Perturbation m;

        do
        {
            n.restore();
            m = getOneMutation();
            //printMutation(m);
            n.applyMutation(m);
            value = n.getSolution().computeMakespan();
        } while (value == UMAX);
        //cout << br << "perturbation " << value << br;
        s = n.getSolution();
    }

    void iterate()
    {
        improve();
        for (unsigned i = 0; i < perturbationNumber; i++)
            perturbation();
    }

public:
    IteratedLocalSearcher() {}
    IteratedLocalSearcher(Solution solution, unsigned number, unsigned o)
    {
        s = solution;
        globalBestSolution = solution.copySolution();
        oper = o;
        perturbationNumber = number;
    }

    void test()
    {
        vector<unsigned> ops = s.getAllOperations_shiftCritical();
        vector<Perturbation> perturbations;
        for (unsigned o : ops)
        {
            vector<Perturbation> ms = s.listPossibleMutations(o, oper);
            for (Perturbation m : ms)
                perturbations.push_back(m);
        }

        NeighborGenerator n(s);
        for (Perturbation m : perturbations)
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