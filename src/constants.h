#pragma once

#define RANDOM 0
#define GREEDY_MACHINES 1
#define GREEDY_JOBS 2
#define TEST 3
#define DUMMY (0)
#define br "\n"
#define tb "  "
#define sp " "
#define UMAX 4294967295

#define BLOCK_J 0
#define BLOCK_M 1

#define SWAP_ALL 0
#define SWAP_CRITICAL 1
#define SWAP_CRITICAL_EDGE 2
#define SHIFT_WHOLE 3
#define SHIFT_CRITICAL 4

#define SWAP_PRED 5
#define SWAP_SUCC 6

#define BLOCK_START 0
#define BLOCK_END 1

#define SWAP 0
#define SHIFT 1

#define SIMULATED_ANNEALING 0
#define TABU_SEARCH 1
#define ITERATED_LOCAL_SEARCH 2

bool TRACK_SWAP_OPERATIONS = false;
bool TRACK_SHIFT_OPERATIONS = false;
unsigned VERBOSE = false;