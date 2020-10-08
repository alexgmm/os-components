#pragma once

#define INVALID 0
#define RANDOM 0
#define GREEDY_MACHINES 1
#define GREEDY_JOBS 2
#define TEST 3
#define DUMMY (0)
#define br "\n"
#define tb "  "
#define sp " "
#define SA 0
#define IG 1
#define TS 2
#define UMAX 4294967295

#define BLOCK_J 0
#define BLOCK_M 1

#define SWAP_J 0
#define SWAP_M 1
#define SHIFT_J 0
#define SHIFT_M 1

#define SWAP_ALL 0
#define SWAP_CRITICAL 1
#define SWAP_CRITICAL_EDGE 2
#define SHIFT_CRITICAL 4
#define SHIFT_WHOLE 3

#define SWAP_PRED 0
#define SWAP_SUCC 1

#define BLOCK_START 0
#define BLOCK_END 1

#define SWAP 0
#define SHIFT 1
unsigned TABU_DURATION = 5;

unsigned CURRENT_ITER;
unsigned CURRENT_GRAPH_NUMBER = 1;

bool SAVE_GRAPHS = false;
bool TRACK_OPERATIONS = false;
unsigned VERBOSE = false;