#ifndef HEURISTICS_HPP
#define HEURISTICS_HPP
#define N_ITER 50
#define ALPHA 0.7
#define T_MIN 0.001
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
#define SA 1
#define IG 2
#define TS 3

#include "solution.hpp"

using namespace std;

class Heuristics {
    Solution solution;

	
    
    void sw1j(){}
    void sw1m(){}
    //swap em operacoes de mesmo bloco critico (job)
    void sw2j(){
        vector<unsigned> b_begin, b_end, p = solution.crit_path();
        solution.blocks_j(p,b_begin, b_end);
        //Selecao de um bloco critico aleatorio
        int i = randint(0,b_begin.size()-1), j = b_end[i]; //Indices de comeco e fim de um bloco
        i = b_begin[i]; cout << i <<" "<< j<<br;
        //Selecao de operacoes dentro desse bloco
        int a = randint(i,j), b = randint(i,j);
        while(a==b) b = randint(i,j);
        swap(p[a], p[b]);}
    //swap em operacoes de mesmo bloco critico (maquina)
    void sw2m(){
        vector<unsigned> b_begin, b_end, p = solution.crit_path();
        solution.blocks_m(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1), j = b_end[i];
        i = b_begin[i];
        int a = randint(i,j), b = randint(i,j);
        while(a==b) b = randint(i,j);
        swap(p[a], p[b]);}
    void sw3j(){
        vector<unsigned> b_begin, b_end, p = solution.crit_path();
        solution.blocks_j(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1);
        solution.swap(p[b_begin[i]], p[b_begin[i] + 1]);}
    void sw3m(){
        vector<unsigned> b_begin, b_end, p = solution.crit_path();
        solution.blocks_m(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1);
        solution.swap(p[b_begin[i]], p[b_begin[i] + 1]);
    }
    void sh1j(){}
    void sh1m(){}
    //shift na primeira operacao de um bloco critico (job)
    void sh2j(){
        vector<unsigned> b_begin, b_end, p = solution.crit_path();
        solution.blocks_j(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1), j = b_end[i];
        i = b_begin[i];
        solution.shift(i,p,j);
    }
    //shift na primeira operacao de um bloco critico (maquina)
    void sh2m(){
        vector<unsigned> b_begin, b_end, p = solution.crit_path();
        solution.blocks_m(p,b_begin, b_end);
        int i = randint(0,b_begin.size()-1), j = b_end[i];
        i = b_begin[i];
        solution.shift(i,p,j);
    }
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

    int sa(int oper){
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
            t = t*ALPHA; cout << br << "t=" << t;
        }
        return makespan;
    }

    void ig(){}

    void ts(){}
};

#endif