#pragma once

unsigned TABU_DURATION = 5;

#include <deque>
#include "time.hpp"
#include "tabu_list.hpp"
#include "neighbor_generator.hpp"

using namespace std;

class TabuSearcher
{
public:
    unsigned duration, oper;
    Solution solution;
    deque<ScheduleChange> tabuList;

    bool isTabu(ScheduleChange change)
    {
        for (unsigned i = 0; i < tabuList.size(); i++)
            if (areEqual(change, tabuList[i]))
                return true;
        return false;
    }

    void setParam(unsigned d)
    {
        duration = d;
    }

    void setDefaultParams()
    {
        switch (oper)
        {
        default:
            break;
        }
    }

    void setSolution(Solution s)
    {
        solution = s;
    }

    TabuSearcher() {}
    TabuSearcher(unsigned o) : oper(o) {}

    unsigned solve()
    {
        startTimeCounting();
        NeighborGenerator n(solution);
        Solution incumbent = solution.copySolution();
        vector<Neighbor> neighbors;

        while (!isTimeOver())
        {
        }
        solution = incumbent.copySolution();

        return incumbent.getMakespan();
    }
};