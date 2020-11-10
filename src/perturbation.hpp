#pragma once

#include <iostream>

#include "constants.h"

using namespace std;

struct Perturbation
{
    unsigned operation, perturbationType, blockType, factor;
};

bool areEqual(Perturbation p1, Perturbation p2)
{
    return p1.operation == p2.operation && p1.perturbationType == p2.perturbationType && p1.blockType == p2.blockType && p1.factor == p2.factor;
}
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
    default:
        perturbationTypeLabel = "shift " + to_string(p.factor) + " units";
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
