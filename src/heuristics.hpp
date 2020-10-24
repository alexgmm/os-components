#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP

unsigned N_ITER = 20;

#include "neighborhood.hpp"
#include "utilities.hpp"
#include "time.hpp"
#include <math.h>
#include <cmath>

using namespace std;
using namespace std::chrono;

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
    unsigned N_ITER_SA = 100, N_ITER_TS = 100, ALPHA, T_MIN;

    unsigned sa(unsigned oper)
    {
        startTimeCounting();
        unsigned makespan = solution.computeMakespan();
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
                    tempMakespan = tempS.computeMakespan();
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
        startTimeCounting();
        Neighborhood n(solution);
        Solution incumbent = solution.copySolution();
        vector<Neighbor> neighbors;

        while(!isTimeOver())
        {
            neighbors = n.getNeighborhood(oper);

            for (Neighbor neighbor : neighbors)
                if (neighbor.getValue() < incumbent.getMakespan() && neighbor.isLegal())
                    incumbent = neighbor.getSolution().copySolution();
            n.setSolution(incumbent);
        }
        solution = incumbent.copySolution();

        return incumbent.getMakespan();
    }
    void setSimulatedAnnealingDefaultParams(unsigned oper)
    {
        switch (oper)
        {
        case SWAP_CRITICAL_EDGE:
            setSimulatedAnnealingParams(0.3772, 0.0006, 5857);
            break;
        case SWAP_CRITICAL:
            setSimulatedAnnealingParams(0.2232, 0.0006, 7631);
            break;
        default:
            break;
        }
    }

    void setTabuSearchDefaultParams(unsigned oper)
    {
        switch (oper)
        {
        case SWAP_CRITICAL:
            TABU_DURATION = 47;
            break;
        case SWAP_CRITICAL_EDGE:
            TABU_DURATION = 27;
            break;
        default:
            break;
        }
    }

public:
    Heuristics() {}
    Heuristics(Solution s) : solution(s) {}
    void setSimulatedAnnealingParams(unsigned alpha, unsigned temp, unsigned iter)
    {
        ALPHA = alpha;
        T_MIN = temp;
        N_ITER_SA = iter;
    }
    void setTabuSearchParams(unsigned duration){
        TABU_DURATION = duration;

    }

    Solution getSolution() { return solution; }
    unsigned solve(unsigned h, unsigned oper)
    {
        switch (h)
        {
        case SA:
            setSimulatedAnnealingDefaultParams(oper);
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

/* old
414  --i 2997 --a 0.7047 --t 0.0003
292  --i 2583 --a 0.6949 --t 0.0003
222  --i 804 --a 0.6575 --t 0.0002 */

/* 911   --i 5857 --a 0.3772 --t 0.0006
598   --i 5323 --a 0.3391 --t 0.0005
1588  --i 5014 --a 0.364 --t 0.0005
 */

/* 1019  --i 7631 --a 0.2232 --t 0.0006
606   --i 5022 --a 0.718 --t 0.0009
993   --i 6260 --a 0.8916 --t 0.0008 */

#endif