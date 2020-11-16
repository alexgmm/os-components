#pragma once

#include "../constants.h"
#include <iostream>

using namespace std;

struct Movement
{
    unsigned type, o1, o2;
};

bool isChangeInitialized(Movement mv)
{
    return mv.o1 != 0;
}

string getMovementString(Movement mv)
{
    string t = mv.type == SWAP ? "swap" : "shift";
    return t + "(" + to_string(mv.o1) + "," + to_string(mv.o2) + ")";
}

void printMovement(Movement mv)
{
    cout << getMovementString(mv);
}

bool isMovementEmpty(Movement mv)
{
    return mv.o1 == 0 && mv.o2 == 0;
}

void assertNonNullMovement(Movement mv)
{
    if (isMovementEmpty(mv))
    {
        cout << "(!) Invalid schedule change: " << getMovementString(mv) << br;
        //assert(sc.o2);
    }
}

bool areEqual(Movement mv1, Movement mv2)
{
    return mv1.type == mv2.type && mv1.o1 == mv2.o1 && mv1.o2 == mv2.o2;
}