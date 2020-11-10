#pragma once

#include "solution.hpp"

using namespace std;

vector<string> movementsNames = {"", "swap critical", "swap critical edge", "shift whole", "shift critical"};

class SolutionTester
{
    Solution s;

public:
    SolutionTester() {}
    SolutionTester(Solution t) : s(t) {}

    void printCriticalBlocks()
    {
        auto jb = s.getCriticalJBlocks();
        auto mb = s.getCriticalJBlocks();
        Printer::printBlockSet(jb, "critical j-blocks");
        Printer::printBlockSet(jb, "critical m-blocks");
    }

    void printPossibleMovements(unsigned oper)
    {
        auto ps = s.listAllPerturbations(oper);
        cout << "Possible movements (" << movementsNames[oper] << "):" << br;
        for (auto p : ps)
            printPerturbation(p);
    }

    void printCriticalEdges()
    {
        auto je = s.getJEdgesN5(), me = s.getMEdgesN5();
        cout << "j-edges:" << br;
        for (auto e : je)
            cout << "[" << e.first << " " << e.second << "]" << br;
        cout << "m-edges:" << br;
        for (auto e : me)
            cout << "[" << e.first << " " << e.second << "]" << br;
    }
};