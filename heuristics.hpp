#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP
#define N_ITER 100
#define ALPHA 0.8
#define T_MIN 0.0001
#define SW1J 1
#define SW1M 2
#define SW2J 3
#define SW2M 4
#define SW3J 5
#define SW3M 6
#define SH1J 7
#define SH1M 8
#define SH2J 9
#define SH2M 10
#define SH3J 11
#define SH3M 12

#include <random>
#include "solution.hpp"

using namespace std;

class Heuristics {
    Solution solution;

    int randint(int ini, int end){
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> dist6(ini,end);

        return dist6(rng);}
    void swap(unsigned int op1, unsigned int op2){
        if(op1 == solution.first) solution.first = op2;
        else if(op2 == solution.first) solution.first = op1;

		solution.j_succ[solution.j_pred[op1]] = op2;
		solution.m_succ[solution.m_pred[op1]] = op2;
		solution.j_pred[solution.j_succ[op1]] = op2;
		solution.m_pred[solution.m_succ[op1]] = op2;

		solution.j_succ[solution.j_pred[op2]] = op1;
		solution.m_succ[solution.m_pred[op2]] = op1;
		solution.j_pred[solution.j_succ[op2]] = op1;
		solution.m_pred[solution.m_succ[op2]] = op1;

		unsigned int a1 = solution.j_pred[op1],
					 a2 = solution.j_succ[op1], 
					 a3 = solution.m_pred[op1], 
					 a4 = solution.m_succ[op1];

		solution.j_pred[op1] = solution.j_pred[op2];
		solution.j_succ[op1] = solution.j_succ[op2];
		solution.m_pred[op1] = solution.m_pred[op2];
		solution.m_succ[op1] = solution.m_succ[op2];

		solution.j_pred[op2] = a1;
		solution.j_succ[op2] = a2;
		solution.m_pred[op2] = a3;
		solution.m_succ[op2] = a4;}
	vector<unsigned int> crit_path(){
		vector<unsigned int> p;
		unsigned int op = solution.last;

		while(op > 0){
			p.push_back(op);
			op = solution.late_pred[op];
		}
		reverse(p.begin(),p.end());
		return p;}
	
    void blocks_j(vector<unsigned int> p, vector<unsigned int> &b_begin, vector<unsigned int> &b_end){
        b_begin.push_back(p[0]);
        for(int i=0; i<p.size()-1; i++){
            if(solution.instance.o_job[p[i]] != solution.instance.o_job[p[i+1]]){
                b_end.push_back(i);
                b_begin.push_back(i+1);
            }
        }
        b_end.push_back(p[p.size()-1]);}
    void blocks_m(vector<unsigned int> p, vector<unsigned int> &b_begin, vector<unsigned int> &b_end){
        int n = 0;
        b_begin.push_back(p[0]);
        for(int i=0; i<p.size()-1; i++){
            if((solution.instance.o_machine[p[i]] != solution.instance.o_machine[p[i+1]]) || n > 1){
                b_end.push_back(i);
                b_begin.push_back(i+1);
                n = 0;
            } else n++;
        }
        b_end.push_back(p[p.size()-1]);}
    void sw1j(){}
    void sw1m(){}
    void sw2j(){
        vector<unsigned int> b_begin, b_end, p = crit_path();
        blocks_j(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1), j = b_end[i]; //Indices de comeco e fim de um bloco
        i = b_begin[i];
        int a = randint(i,j), b = randint(i,j);
        while(a==b) b = randint(i,j);
        swap(p[a], p[b]);
    }
    void sw2m(){
        vector<unsigned int> b_begin, b_end, p = crit_path();
        blocks_m(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1), j = b_end[i];
        i = b_begin[i];
        int a = randint(i,j), b = randint(i,j);
        while(a==b) b = randint(i,j);
        swap(p[a], p[b]);
    }
    void sw3j(){
        vector<unsigned int> b_begin, b_end, p = crit_path();
        blocks_j(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1);
        swap(p[b_begin[i]], p[b_begin[i] + 1]);
    }
    void sw3m(){
        vector<unsigned int> b_begin, b_end, p = crit_path();
        blocks_m(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1);
        swap(p[b_begin[i]], p[b_begin[i] + 1]);
    }
    void sh1j(){}
    void sh1m(){}
    void sh2j(){}
    void sh2m(){}
    void sh3j(){}
    void sh3m(){}
    void apply(int oper){
        switch (oper){
            case SW1J:
                sw1j();
                break;
            case SW1M:
                sw1m();
                break;
            case SW2J:
                sw2j();
                break;
            case SW2M:
                sw2m();
                break;
            case SW3J:
                sw3j();
                break;
            case SW3M:
                sw3m();
                break;
            case SH1J:
                sh1j();
                break;
            case SH1M:
                sh1m();
                break;
            case SH2J:
                sh2j();
                break;
            case SH2M:
                sh2m();
                break;
            case SH3J:
                sh3j();
                break;
            case SH3M:
                sh3m();
                break;
            default:
                break;
        }}
public:
    Heuristics(){}
    Heuristics(Solution s){
        solution = s;
    }

    void sa(int oper){
        int makespan = solution.calc_makespan(), temp_makespan;
        float t = 1;
        while(t > T_MIN){
            for(int i=0; i<N_ITER; i++){
                Solution temp(solution.j_succ, solution.j_pred, solution.m_succ, solution.m_pred, solution.n_mach, solution.n_jobs, solution.n_ops, solution.first, solution.instance);
                apply(oper);
                temp_makespan = temp.calc_makespan();

                if(temp_makespan <= makespan || exp((makespan - temp_makespan)/t) > ((float) randint(1,1000))/1000) 
                    makespan = temp_makespan;
            }
            t = t*ALPHA;
        }
    }

    void ig(){}

    void ts(){}
};

#endif