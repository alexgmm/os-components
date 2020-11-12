#pragma once

#include "../neighbor_generator.hpp"
#include "../perturbation/perturbation.hpp"
#include "../helpers/printer.hpp"
#include "../helpers/time.hpp"

using namespace std;

class IteratedLocalSearcher
{
    Schedule solution, globalBest;
    NeighborGenerator generator;
    unsigned perturbationNumber, oper;
    //string outfile = "/home/hal/running_report.txt";

    void setDefaultParams()
    {
        switch (oper)
        {
        case SWAP_CRITICAL:
            perturbationNumber = 1;
        case SWAP_CRITICAL_EDGE:
            perturbationNumber = 1;
            break;
        }
    }

    bool applyPerturbationGreedly(unsigned o)
    {
        Schedule bestNeighbor = generator.getBestNeighbor(o, oper);

        if (bestNeighbor.computeMakespan() < globalBest.computeMakespan())
        {
            setSolution(bestNeighbor.getCopy());
            return true;
        }
        else
        {
            generator.restore();
            return false;
        }
    }

    void improve()
    {
        //cout << wrapStringInLabel("improving");
        bool isImproving;
        PerturbationGenerator gen(solution);
        do
        {
            isImproving = false;
            vector<unsigned> operations = gen.getAllOperations(oper);
            //printv(operations, 0, "all ops");
            for (auto o : operations)
                isImproving = isImproving || applyPerturbationGreedly(o);

        } while (isImproving && !isTimeOver());
    }

    void perturbation()
    {
        //cout << wrapStringInLabel("perturbating");
        Schedule perturbated = generator.getRandomNeighbor(SHIFT_WHOLE);
        setSolution(perturbated);
    }

    void iterate()
    {
        improve();
        for (unsigned i = 0; i < perturbationNumber && !isTimeOver(); i++)
            perturbation();
    }

public:
    IteratedLocalSearcher() {}
    IteratedLocalSearcher(unsigned number, unsigned o)
    {
        oper = o;
        perturbationNumber = number;
    }

    void setSolution(Schedule s)
    {
        solution = s;
        globalBest = s.getCopy();
        generator.setSchedule(solution);
    }

    unsigned solve()
    {
        startTimeCounting();

        while (!isTimeOver())
            iterate();

        return globalBest.computeMakespan();
    }

    Schedule getSolution()
    {
        return solution;
    }
};