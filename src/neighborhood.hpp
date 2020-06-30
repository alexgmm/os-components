#ifndef NEIGHBORHOOD_HPP
#define NEIGHBORHOOD_HPP

#define BLOCK_J 0
#define BLOCK_M 1

#define SWAP_J 0
#define SWAP_M 1
#define SHIFT_J 0
#define SHIFT_M 1

#define SWAP_ALL 0
#define SWAP_CRITICAL 1
#define SWAP_CRITICAL_EDGE 2
#define SHIFT_CRITICAL 3
#define SHIFT_WHOLE 4

#define SWAP_PRED 0
#define SWAP_SUCC 1

#define SWAP 0
#define SHIFT 1
#define TABU_DURATION 20
unsigned TABU_OPERATION;

#include "solution.hpp"
#include <utility>

using namespace std;

int CURRENT_ITER;

#endif