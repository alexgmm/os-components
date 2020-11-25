#pragma once

#include "../heuristics/tabu_searcher.hpp"
#include "../heuristics/annealing_simulator.hpp"
#include "../heuristics/iterated_local_searcher.hpp"

using namespace std;

class HeuristicResultGenerator
{
    IteratedLocalSearcher getNewILS(unsigned oper)
    {
        IteratedLocalSearcher ils(oper);
        return ils;
    }

public:
    HeuristicResultGenerator() {}
};