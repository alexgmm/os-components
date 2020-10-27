#pragma once

#include <iostream>

#include "constants.h"

using namespace std;

struct Mutation{
    unsigned operation, mutationType, blockType, factor;
};

void printMutation(Mutation m){
    string mutationTypeLabel, blockTypeLabel;

    switch(m.mutationType){
        case SWAP_PRED:
            mutationTypeLabel = "swap_predecessor";
            break;
        case SWAP_SUCC:
            mutationTypeLabel = "swap_successor";
            break;
        case SHIFT_WHOLE:
            mutationTypeLabel = "shift " + to_string(m.factor) + " units";
            break;
        default:
            mutationTypeLabel = "shift";
            break;
    }

    switch(m.blockType){
        case BLOCK_J:
            blockTypeLabel = "job block";
            break;
        case BLOCK_M:
            blockTypeLabel = "mach block";
            break;
        default:
            blockTypeLabel = "???";
            break;
    }

    cout << mutationTypeLabel << "(" << m.operation << ") on " << blockTypeLabel << endl;
}