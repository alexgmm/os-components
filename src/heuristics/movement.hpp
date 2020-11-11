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

string getScheduleChangeString(Movement mv)
{
    string t = mv.type == SWAP ? "swap" : "shift";
    return t + "(" + to_string(mv.o1) + "," + to_string(mv.o2) + ")";
}

void printScheduleChange(Movement mv)
{
    cout << getScheduleChangeString(mv);
}

bool isScheduleChangeEmpty(Movement mv)
{
    return mv.o1 == 0 && mv.o2 == 0;
}

void assertNonNullScheduleChange(Movement mv)
{
    if (isScheduleChangeEmpty(mv))
    {
        cout << "(!) Invalid schedule change: " << getScheduleChangeString(mv) << br;
        //assert(sc.o2);
    }
}

bool areEqual(Movement mv1, Movement mv2)
{
    return mv1.type == mv2.type && mv1.o1 == mv2.o1 && mv1.o2 == mv2.o2;
}