#pragma once

#include "constants.h"
#include <iostream>
#include <vector>
#include <deque>
#include <utility>

using namespace std;

struct ScheduleChange
{
    unsigned type, o1, o2;
};

bool isChangeInitialized(ScheduleChange sc)
{
    return sc.o1 != 0;
}

string getScheduleChangeString(ScheduleChange sc)
{
    string t = sc.type == SWAP ? "swap" : "shift";
    return t + "(" + to_string(sc.o1) + "," + to_string(sc.o2) + ")";
}

void printScheduleChange(ScheduleChange sc)
{
    cout << getScheduleChangeString(sc);
}

bool isScheduleChangeEmpty(ScheduleChange sc)
{
    return sc.o1 == 0 && sc.o2 == 0;
}

void assertNonNullScheduleChange(ScheduleChange sc)
{
    if (isScheduleChangeEmpty(sc))
    {
        cout << "(!) Invalid schedule change: " << getScheduleChangeString(sc) << br;
        //assert(sc.o2);
    }
}

bool areEqual(ScheduleChange sc1, ScheduleChange sc2)
{
    return sc1.type == sc2.type && sc1.o1 == sc2.o1 && sc1.o2 == sc2.o2;
}

class TabuList
{
    vector<vector<unsigned>> list;
    deque<ScheduleChange> activeTabu;
    unsigned duration, type;

    unsigned remainingLoops(ScheduleChange &m)
    {
        return list[m.o1][m.o2];
    }

    void decreaseRemainingLoops(ScheduleChange &m)
    {
        list[m.o1][m.o2] -= 1;
    }

public:
    TabuList() {}
    TabuList(unsigned nO, unsigned d, unsigned t)
    {
        list.resize(nO + 1);
        for (unsigned i = 0; i < list.size(); i++)
            list[i].resize(nO + 1, 0);

        type = t;
        duration = d;
    }

    void insertTabuMovement(ScheduleChange sc)
    {
        list[sc.o1][sc.o2] = duration;
        activeTabu.push_back(sc);

        list[sc.o2][sc.o1] = duration;
        unsigned aux = sc.o1;
        sc.o1 = sc.o2;
        sc.o2 = aux;
        activeTabu.push_back(sc);
    }

    bool isTabu(ScheduleChange sc)
    {
        return list[sc.o1][sc.o2] > 0 || list[sc.o2][sc.o1] > 0;
    }

    void iterate()
    {
        //cout << br << "iterating..." << br;
        for (auto movement : activeTabu)
        {
            if (remainingLoops(movement) == 1)
                activeTabu.pop_front();
            else
                decreaseRemainingLoops(movement);
        }
    }

    void printCurrentTabu()
    {
        cout << br << "current tabu ops " << br;
        for (auto m : activeTabu)
        {
            cout << remainingLoops(m) << sp;
            printScheduleChange(m);
        }
    }

    friend class TabuSearcher;
};