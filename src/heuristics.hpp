#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP

unsigned N_ITER_TS = 499;
unsigned N_ITER_SA = 404;
double ALPHA = 0.0777;
double T_MIN = 0.0009;

#define BEST_IMPROVEMENT  0
#define FIRST_IMPROVEMENT  1
#define RANDOM_SOLUTION  2
unsigned SELECTION_CRITERIA = BEST_IMPROVEMENT;

#include "neighborhood.hpp"
#include "utilities.hpp"
#include <math.h>
#include <cmath>

using namespace std;

unsigned absol(unsigned n1){  if(n1<0) return -n1; return n1;  }
bool accept(double t, unsigned v_new, unsigned v_old){	return (rand()/(double)RAND_MAX) <= exp(0-absol(v_new-v_old)/t);  }
//bool accept(unsigned makespan, unsigned newMakespan, bool legal){ return ( newMakespan < makespan  && (legal || (newMakespan < makespan && !legal) ) ); }

class Heuristics {
    Solution solution;

    unsigned sa(unsigned oper){
        //solution.print();
        unsigned makespan = solution.calcMakespan();
        int tempMakespan;
        assert(makespan > 0);
        double t = 1;
        Solution tempS;
        while(t > T_MIN){
            for(unsigned i=0; i<N_ITER_SA; i++){
                tempMakespan = -1;
                while (tempMakespan <= 0){
                    tempS  = Neighborhood::getNeighbor(oper, solution);
                    tempMakespan = tempS.calcMakespan();
                    //tempS.print();
                }
                //if(tempMakespan <= 193) exit(0);
                if(tempMakespan < makespan || accept(t, tempMakespan, makespan)) {
                    /* cout << br << makespan << br; */
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
        Solution incumbent = solution.copySolution();
        vector<Neighbor> neighbors;
        
        for(CURRENT_ITER = 0; CURRENT_ITER<N_ITER_TS; CURRENT_ITER++){
            neighbors = n.getNeighborhood(oper);

            for(Neighbor neighbor: neighbors){
                //cout << neighbor.getValue() << br;
                if(neighbor.getValue() < incumbent.getMakespan() && neighbor.isLegal()){
                    incumbent = neighbor.getSolution().copySolution();
                }
            }
            
            n.setSolution(incumbent);
        }
        solution = incumbent.copySolution();

        return incumbent.getMakespan();
    }
    
public:
    
    Heuristics(){}
    Heuristics(Solution s):solution(s){}
    Solution getSolution(){ return solution; }
    unsigned solve(unsigned h, unsigned oper){
        switch (h){
        case SA: return sa(oper);
        case IG: return ig(oper);
        case TS: return ts(oper);
        default: break;
        }
    }

    
};

#endif