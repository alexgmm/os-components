#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP
unsigned N_ITER = 125;
double ALPHA = 0.5;
double T_MIN = 0.0001;

#include "solution.hpp"
#include "neighborhood.hpp"
#include <math.h>
#include <cmath>

using namespace std;

unsigned absol(unsigned n1){  if(n1<0) return -n1; return n1;  }
bool accept(double t, unsigned v_new, unsigned v_old){	return (rand()/(double)RAND_MAX) <= exp(0-absol(v_new-v_old)/t);  }

class Heuristics {


public:
    Solution solution;
    Heuristics(){}
    Heuristics(Solution s):solution(s){}

    unsigned solve(unsigned h, unsigned oper){
        switch (h){
        case SA: return sa(oper);
        case IG: return ig(oper);
        case TS: return ts(oper);
        default: break;
        }
    }

    unsigned sa(unsigned oper){
        unsigned makespan = solution.calcMakespan(), tempMakespan;
        assert(makespan > 0);
        double t = 1;
        Solution tempS;
        while(t > T_MIN){
            for(unsigned i=0; i<N_ITER; i++){
                tempMakespan = -1;
                while (tempMakespan < 0){
                    tempS  = solution.copySolution();
                    tempS.getNeighbor(oper);
                    tempMakespan = tempS.calcMakespan();
                }

                if(tempMakespan < makespan || accept(t, tempMakespan, makespan)) {
                    solution = tempS.copySolution();
                    makespan = tempMakespan; 
                }
            }
            t *= ALPHA;
        }
        return makespan;
    }

    unsigned ig(unsigned oper){}

    unsigned ts(unsigned oper){
        Neighborhood n(solution);

        
    }
};

#endif