#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>
#include <vector>
#include <algorithm> //random_shuffle
#include <numeric> //iota
#include "instance.hpp"

using namespace std;

class Solution {
	vector<unsigned int> j_succ;
	vector<unsigned int> j_pred;
	vector<unsigned int> m_succ;
	vector<unsigned int> m_pred;
	unsigned int makespan, n_mach, n_jobs, n_ops, last, first;
	Instance instance;

	// Numero/indice da operacao (j,m)
	// Indice 0 e desconsiderado para todos os vetores
	int op(int j, int m){ return (j-1)*n_mach + m;}
	void fill_ord(vector<unsigned int> &j_ord, vector<unsigned int> &m_ord){
		first = op(j_ord[1], m_ord[1]);

		for(int m=1; m<=n_mach; m++){
			for(int j=1; j<=n_jobs; j++){ 
				if(j>1) m_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
				if(m>1) j_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
				if(j<n_jobs) m_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
				if(m<n_mach) j_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
			}
		}

		/* int ini = 1;
		printv(j_succ, ini,"Sucessores nos jobs:");
		printv(j_pred, ini,"Predecessores nos jobs:");
		printv(m_succ, ini,"Sucessores nas maquinas:");
		printv(m_pred, ini,"Predecessores nas maquinas:"); */}
	void init(int method){
		switch(method){
			case RANDOM:
				initRandom();
				break;
			case GREEDY_JOBS:
				initGreedyJobs();
				break;
			case GREEDY_MACHINES:
				initGreedyMachines();
				break;
			default:
				break;}}

	void initRandom(){ //Gera sol. inicial rand.
		vector<unsigned int> j_ord(n_jobs+1, 0),
							 m_ord(n_mach+1, 0);
							 
		//Gerando ordem dos jobs
		iota(j_ord.begin()+1, j_ord.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(j_ord.begin()+1, j_ord.end());
		//printv(j_ord,1,"Ordem randomizada - jobs");

		//Gerando ordem nas maquinas
		iota(m_ord.begin()+1, m_ord.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(m_ord.begin()+1, m_ord.end());
		//printv(m_ord,1,"Ordem randomizada - machines");
	
		fill_ord(j_ord, m_ord);}

	void initGreedyJobs(){
		vector<unsigned int> j_ord(n_jobs+1, 0),
							 m_ord(n_mach+1, 0),
							 j_used(n_jobs+1, 0);

		iota(m_ord.begin()+1, m_ord.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(m_ord.begin()+1, m_ord.end());

		unsigned int temp_n_j, temp_first, best_job, best_mkspn, mkspn;

		for(int i = n_jobs; i>0; i--){
			temp_n_j = n_jobs - i + 1;
			best_mkspn = 999999;
			for(int j = n_jobs; j>=i; j--){
				if(j_used[j] == 1) continue;

				j_ord[i] = j;
				for(int m = n_mach; m>0; m--){
					if(j>1) m_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
					if(m>1) j_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
					if(j<temp_n_j) m_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
					if(m<n_mach) j_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
				}
				temp_first = op(j,m_ord[1]);
				Solution s(j_succ, j_pred, m_succ, m_pred, n_mach, temp_n_j, temp_n_j*n_mach,temp_first,instance);
				mkspn = s.calc_makespan();
				if(mkspn < best_mkspn){
					best_job = j;
					best_mkspn = mkspn;
					j_used[j] = 1;
				}
			}
			j_ord[i] = best_job;
		}

		fill_ord(j_ord, m_ord);}
	void initGreedyMachines(){
		vector<unsigned int> j_ord(n_jobs+1, 0),
							 m_ord(n_mach+1, 0),
							 m_used(n_mach+1, 0);

		iota(j_ord.begin()+1, j_ord.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(j_ord.begin()+1, j_ord.end());

		unsigned int temp_n_m, temp_first, best_mach, best_mkspn, mkspn;

		for(int i = n_mach; i>0; i--){
			temp_n_m = n_mach - i + 1;
			best_mkspn = 999999;
			for(int m = n_mach; m>=i; m--){
				if(m_used[m] == 1) continue;

				m_ord[i] = m;
				for(int j = n_jobs; j>0; j--){
					if(j>1) m_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
					if(m>1) j_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
					if(j<n_jobs) m_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
					if(m<temp_n_m) j_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
				}
				temp_first = op(j_ord[1],m);
				Solution s(j_succ, j_pred, m_succ, m_pred, temp_n_m, n_jobs, n_jobs*temp_n_m,temp_first,instance);
				mkspn = s.calc_makespan();
				if(mkspn < best_mkspn){
					best_mach = m;
					best_mkspn = mkspn;
					m_used[m] = 1;
				}
			}
			m_ord[i] = best_mach;
		}

		fill_ord(j_ord, m_ord);}
    void print_q(vector<unsigned int> &s, int lookat){
		cout<<br<<"Pilha:"<<br;
		for(int i=s.size()-1; i>=0; i--){
			cout<<"|"<<s[i]<<"|";
			if(i==lookat) cout<<"<";
			cout<<br;}}
	void fill_heads(unsigned int op, vector<unsigned int>& heads, vector<unsigned int>& late_pred, int cont, vector<unsigned int>& visited){
		if(cont>n_ops || op == 0 || visited[op] == 1) return;
		//cout<<br<<"fill_heads.op "<<op;
		if(op != first)	
			heads[op] = max(heads[j_pred[op]] + instance.cost[j_pred[op]],
							heads[m_pred[op]] + instance.cost[m_pred[op]]);
	
		if(heads[op] > heads[last]) last = op;
        visited[op] = 1;
		fill_heads(j_succ[op], heads, late_pred, cont + 1, visited);
		fill_heads(m_succ[op], heads, late_pred, cont + 1, visited);}
public:
	Solution () {}
	Solution(vector<unsigned int> j_s, vector<unsigned int>j_p, vector<unsigned int> m_s, vector<unsigned int> m_p,
		     unsigned int n_m, unsigned int n_j, unsigned int n_o, unsigned int f, Instance i){
		j_succ = j_s;
		j_pred = j_p;
		m_succ = m_s;
		m_pred = m_p;
		n_mach = n_m;
		n_jobs = n_j;
		first = f;
		instance = i;}
	Solution(Instance i, int init_method){
		instance = i;
		last = 0;
		makespan = 0;
		n_ops = instance.n_ops;
		n_jobs = instance.n_jobs;
		n_mach = instance.n_machines;
		j_succ.resize(n_ops+1);
		j_pred.resize(n_ops+1);
		m_succ.resize(n_ops+1);
		m_pred.resize(n_ops+1);

		init(init_method);}
	
	int calc_makespan(){
		makespan = 0;
		int temp_makespan = 0, op, op_succ, q_lookat = 0, q_pushed = 0;
		vector<unsigned int> degrees_in(n_ops + 1,0); //Graus de entrada por operacao
		vector<unsigned int> op_queue(n_ops); //Lista de operacoes
		vector<unsigned int> late_pred(n_ops + 1); //Predecessores mais tardios
		vector<unsigned int> heads(n_ops + 1, 0);
        vector<unsigned int> visited(n_ops + 1, 0);

		for(int i=1; i<=n_ops; i++)
			if(m_pred[i] > j_pred[i]) late_pred[i] = m_pred[i];
			else late_pred[i] = j_pred[i];

		fill_heads(first, heads, late_pred, 1, visited);//printv(heads, 1, "heads"); printv(late_pred, 1, "late_pred");

        for(int i = 1; i <= n_ops; i++){
			if(j_pred[i]>0) degrees_in[i]++;
			if(m_pred[i]>0) degrees_in[i]++;
			if(degrees_in[i]==0) op_queue[q_pushed++] = i;}
		

		while(q_lookat < q_pushed){
			op = op_queue[q_lookat++];
			//cout<<br<<"op = "<<op<<br<<"op.heads = "<<heads[op]<<br<<"op.cost = "<<instance.cost[op]<<br;
			temp_makespan = heads[op] + instance.cost[op];

			if(temp_makespan > makespan){
				//cout <<br<< "makespan = "<<makespan<<br;
				makespan = temp_makespan; //cout <<br<< "makespan atu = "<<makespan<<br;
				last = op;}

			op_succ = j_succ[op];
			if (op_succ != DUMMY){
				degrees_in[op_succ]--;//printv(degrees_in,1,"Gr entrada:");
				if(degrees_in[op_succ] == 0) op_queue[q_pushed++] = op_succ;//print_q(op_queue,q_lookat);
				if(heads[op_succ] < temp_makespan){
					heads[op_succ] = temp_makespan;
					late_pred[op_succ] = op;
				}
			}

			op_succ = m_succ[op];
			if (op_succ != DUMMY){
				degrees_in[op_succ]--;//printv(degrees_in,1,"Gr entrada:");
				if(degrees_in[op_succ] == 0) op_queue[q_pushed++] = op_succ;//print_q(op_queue,q_lookat);
				if(heads[op_succ] < temp_makespan){
					heads[op_succ] = temp_makespan;
					late_pred[op_succ] = op;
				}
			}
		}
		return makespan;}
};

#endif