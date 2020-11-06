#pragma once

#include "neighbor_generator.hpp"
#include "perturbation.hpp"
#include "printer.hpp"
#include "time.hpp"

using namespace std;

class IteratedLocalSearcher
{
    Solution solution, globalBestSolution;
    unsigned perturbationNumber, oper;
    //string outfile = "/home/hal/running_report.txt";
    bool applyPerturbationGreedly(unsigned o)
    {
        //Printer::appendToFile(s.getSolutionString(), outfile);
        bool improved = false;
        vector<Perturbation> perturbations = solution.listPossiblePerturbations(o, oper);

        if (perturbations.size() == 0)
            return false;

        NeighborGenerator n(solution);

        unsigned bestValue = solution.computeMakespan(), value, globalBestValue = globalBestSolution.computeMakespan();
        Perturbation bestPerturbation = {0, 0, 0, 0};

        for (Perturbation m : perturbations)
        {
            //Printer::appendToFile(getPerturbationString(m), outfile);
            value = n.applyPerturbation(m);
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
                bestPerturbation = m;
            }
        }

        n.applyPerturbation(bestPerturbation);

        solution = n.getSolution();
        return improved;
    }

    Perturbation getOnePerturbation()
    {
        vector<Perturbation> perturbations;
        unsigned o;

        while (perturbations.size() == 0)
        {
            o = solution.getOneRandomOperation_shiftWhole();
            perturbations = solution.listPossiblePerturbations_shiftWhole(o);
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
            vector<unsigned> operations = solution.getAllOperations(oper);
            //printv(operations, 0, "all ops");
            for (auto o : operations)
                isImproving = isImproving || applyPerturbationGreedly(o);

        } while (isImproving && !isTimeOver());
    }

    void perturbation()
    {
        unsigned value;
        NeighborGenerator n(solution);
        Perturbation m;

        do
        {
            n.restore();
            m = getOnePerturbation();
            //printPerturbation(m);
            n.applyPerturbation(m);
            value = n.getSolution().computeMakespan();
        } while (value == UMAX);
        //cout << br << "perturbation " << value << br;
        solution = n.getSolution();
    }

    void iterate()
    {
        improve();
        for (unsigned i = 0; i < perturbationNumber; i++)
            perturbation();
    }

public:
    IteratedLocalSearcher() {}
    IteratedLocalSearcher(unsigned number, unsigned o)
    {
        oper = o;
        perturbationNumber = number;
    }

    void setSolution(Solution s)
    {
        solution = s;
        globalBestSolution = s.copySolution();
    }

    void test()
    {
        vector<unsigned> ops = solution.getAllOperations_shiftCritical();
        vector<Perturbation> perturbations;
        for (unsigned o : ops)
        {
            vector<Perturbation> ms = solution.listPossiblePerturbations(o, oper);
            for (Perturbation m : ms)
                perturbations.push_back(m);
        }

        NeighborGenerator n(solution);
        for (Perturbation m : perturbations)
        {
            printPerturbation(m);
            n.applyPerturbation(m);
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
        return solution;
    }
};