#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP

unsigned N_ITER_TS = 499;
unsigned N_ITER_SA = 404;
unsigned N_ITER = 20;
double ALPHA = 0.001;  // 0.0777;
double T_MIN = 0.0001; //0.0009;

#define BEST_IMPROVEMENT 0
#define FIRST_IMPROVEMENT 1
#define RANDOM_SOLUTION 2
unsigned SELECTION_CRITERIA = BEST_IMPROVEMENT;

#include "neighborhood.hpp"
#include "utilities.hpp"
#include "time.hpp"
#include <math.h>
#include <cmath>

using namespace std;

unsigned absol(unsigned n1)
{
    if (n1 < 0)
        return -n1;
    return n1;
}
bool accept(double t, unsigned v_new, unsigned v_old) { return (rand() / (double)RAND_MAX) <= exp(0 - absol(v_new - v_old) / t); }
//bool accept(unsigned makespan, unsigned newMakespan, bool legal){ return ( newMakespan < makespan  && (legal || (newMakespan < makespan && !legal) ) ); }

class Heuristics
{
    Solution solution;

    unsigned sa(unsigned oper)
    {
        //solution.print();
        unsigned makespan = solution.calcMakespan();
        int tempMakespan;
        assert(makespan > 0);
        double t = 1;
        bool shouldStop = false;
        Solution tempS;
        while (t > T_MIN && !shouldStop)
        {
            for (unsigned i = 0; i < N_ITER_SA && !shouldStop; i++)
            {
                tempMakespan = -1;
                while (tempMakespan <= 0)
                {
                    tempS = Neighborhood::getNeighbor(oper, solution);
                    tempMakespan = tempS.calcMakespan();
                }
                if (tempMakespan < makespan || accept(t, tempMakespan, makespan))
                {
                    solution = tempS.copySolution();
                    makespan = tempMakespan;
                }

                shouldStop = isTimeOver();
            }
            t *= ALPHA;
        }
        return makespan;
    }

    unsigned ig(unsigned oper) {}

    unsigned ts(unsigned oper)
    {
        Neighborhood n(solution);
        Solution incumbent = solution.copySolution();
        vector<Neighbor> neighbors;

        for (CURRENT_ITER = 0; CURRENT_ITER < N_ITER_TS; CURRENT_ITER++)
        {
            neighbors = n.getNeighborhood(oper);

            for (Neighbor neighbor : neighbors)
            {
                //cout << neighbor.getValue() << br;
                if (neighbor.getValue() < incumbent.getMakespan() && neighbor.isLegal())
                {
                    incumbent = neighbor.getSolution().copySolution();
                }
            }

            n.setSolution(incumbent);
        }
        solution = incumbent.copySolution();

        return incumbent.getMakespan();
    }

public:
    Heuristics() {}
    Heuristics(Solution s) : solution(s) {}
    Solution getSolution() { return solution; }
    unsigned solve(unsigned h, unsigned oper)
    {
        switch (h)
        {
        case SA:
            return sa(oper);
        case IG:
            return ig(oper);
        case TS:
            return ts(oper);
        default:
            break;
        }
    }

    unsigned ls()
    {
        unsigned global = solution.getMakespan(), mlocal;
        Solution local = solution;
        for (unsigned i = 0; i < N_ITER; i++)
        {
            mlocal = UMAX;
            Neighborhood n(local);
            vector<Neighbor> vi = n.getNeighborhood(SWAP_CRITICAL_EDGE);
            for (Neighbor v : vi)
            {
                if (mlocal > v.getSolution().getMakespan())
                {
                    mlocal = v.getSolution().getMakespan();
                    local = v.getSolution();
                }
            }
            global = min(global, mlocal);
        }

        return global;
    }
};

#endif