#pragma once

#include <iostream>

#include "constants.h"

using namespace std;

struct Perturbation
{
    unsigned operation, perturbationType, blockType, factor;
};

void printMutation(Perturbation m)
{
    string perturbationTypeLabel, blockTypeLabel;

    switch (m.perturbationType)
    {
    case SWAP_PRED:
        perturbationTypeLabel = "swap_predecessor";
        break;
    case SWAP_SUCC:
        perturbationTypeLabel = "swap_successor";
        break;
    case SHIFT_WHOLE:
        perturbationTypeLabel = "shift " + to_string(m.factor) + " units";
        break;
    default:
        perturbationTypeLabel = "shift";
        break;
    }

    switch (m.blockType)
    {
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

    cout << perturbationTypeLabel << "(" << m.operation << ") on " << blockTypeLabel << endl;
}