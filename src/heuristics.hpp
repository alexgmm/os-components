#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP
#define N_ITER 100
#define ALPHA 0.9
#define T_MIN 0.0001

#include "solution.hpp"
#include <math.h>
#include <cmath>

using namespace std;

unsigned absol(unsigned n1){
    if(n1<0) return -n1;
    return n1;
}

bool accept(double t, unsigned v_new, unsigned v_old){
	double e = exp(0-absol(v_new-v_old)/t);
    double p = (rand()/(double)RAND_MAX);
    //cout << br << e << " " << p;
	return p <= e;
}

class Heuristics {
    Solution solution;

public:
    Heuristics(){}
    Heuristics(Solution s){
        solution = s;
    }

    int solve(int h, int oper){
        switch (h){
        case SA:
            sa(oper);
            break;
        case IG:
            ig(oper);
            break;
        case TS:
            ts(oper);
            break;
        default:
            break;
        }
    }

    int sa(int oper){
        srand(0);
        int makespan = solution.calc_makespan(), temp_makespan;
        double t = 1, e, p;
        while(t > T_MIN){
            for(int i=0; i<N_ITER; i++){
                Solution temp = solution.copy_solution();
                temp.apply(oper); //temp.valid_schedule();
                temp_makespan = temp.calc_makespan();

                if(temp_makespan < makespan || accept(t, temp_makespan, makespan)) 
                    makespan = temp_makespan; 
            }
            t = t*ALPHA;
        }
        return makespan;
    }

    void ig(int oper){}

    void ts(int oper){}
};

#endif