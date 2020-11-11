#pragma once

#include "movement.hpp"
#include <vector>
#include <deque>
#include <utility>

using namespace std;

class TabuList
{
    vector<vector<unsigned>> list;
    deque<Movement> activeTabu;
    unsigned duration, type;

    unsigned remainingLoops(Movement &m)
    {
        return list[m.o1][m.o2];
    }

    void decreaseRemainingLoops(Movement &m)
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

    void insertTabuMovement(Movement sc)
    {
        list[sc.o1][sc.o2] = duration;
        activeTabu.push_back(sc);

        list[sc.o2][sc.o1] = duration;
        unsigned aux = sc.o1;
        sc.o1 = sc.o2;
        sc.o2 = aux;
        activeTabu.push_back(sc);
    }

    bool isTabu(Movement sc)
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