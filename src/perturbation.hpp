#pragma once

#include <iostream>

#include "constants.h"

using namespace std;

struct Perturbation
{
    unsigned operation, perturbationType, blockType, factor;
};

string getPerturbationString(Perturbation p)
{
    string perturbationTypeLabel, blockTypeLabel;

    switch (p.perturbationType)
    {
    case SWAP_PRED:
        perturbationTypeLabel = "swap_predecessor";
        break;
    case SWAP_SUCC:
        perturbationTypeLabel = "swap_successor";
        break;
    case SHIFT_WHOLE:
        perturbationTypeLabel = "shift " + to_string(p.factor) + " units";
        break;
    default:
        perturbationTypeLabel = "shift";
        break;
    }

    switch (p.blockType)
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

    return perturbationTypeLabel + "(" + to_string(p.operation) + ") on " + blockTypeLabel + br;
}

void printPerturbation(Perturbation p)
{
    cout << getPerturbationString(p);
}