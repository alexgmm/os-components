#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>
#include <vector>
#include <algorithm> //random_shuffle
#include <numeric> //iota
#include <assert.h>
#include "instance.hpp"

using namespace std;

class Solution {
	vector<unsigned> j_succ, j_pred, m_succ, m_pred;
	unsigned makespan, n_mach, n_jobs, n_ops, last, first;
	Instance instance;

	// Numero/indice da operacao (j,m)
	// Indice 0 e desconsiderado para todos os vetores
	bool same_job(unsigned op1, unsigned op2){ return instance.o_job[op1] == instance.o_job[op2]; }
	bool same_mach(unsigned op1, unsigned op2){ return instance.o_machine[op1] == instance.o_machine[op2]; }
	void shift(unsigned pos_op, vector<unsigned> &p, int end_b){
        int pos = randint(pos_op,end_b);
        for(pos_op; pos_op < pos; pos_op++)
            swap(p[pos_op], p[pos_op + 1]);
	}
	void swap(unsigned op1, unsigned op2){
		//valid_op(op1); valid_op(op2);
        /* unsigned prev;
		unsigned post;

		if (same_job(op1, op2)) {
			//JOB swap
			assert(j_succ[op1] == op2);
			assert(j_pred[op2] == op1);
			prev = j_pred[op1];
			post = j_succ[op2];
			//forward
			if (prev != 0)
				j_succ[prev] = op2;
			j_succ[op1] = post;
			j_succ[op2] = op1;
			//backward
			j_pred[op1] = op2;
			j_pred[op2] = prev;
			if (post != 0)	j_pred[post] = op1;
		}
		else {
			assert(same_mach(op1,op2));
			//MACH swap
			assert(m_succ[op1] == op2);
			assert(m_pred[op2] == op1);
			prev = m_pred[op1];
			post = m_succ[op2];
			//forward
			if (prev != 0)
				m_succ[prev] = op2;
			m_succ[op1] = post;
			m_succ[op2] = op1;
			//backward
			m_pred[op1] = op2;
			m_pred[op2] = prev;
			if (post != 0)	m_pred[post] = op1;
		} */

	    if(op1 == first) first = op2;
        else if(op2 == first) first = op1;
		unsigned js1=j_succ[op1], jp1=j_pred[op1], ms1=m_succ[op1], mp1=m_pred[op1],
				js2=j_succ[op2], jp2=j_pred[op2], ms2=m_succ[op2], mp2=m_pred[op2];
	
		if(jp1) j_succ[jp1] = op2; if(mp1) m_succ[mp1] = op2; 
		if(jp2) j_succ[jp2] = op1; if(mp2) m_succ[mp2] = op1;
		if(js1) j_pred[js1] = op2; if(ms1) m_pred[ms1] = op2;
		if(js2) j_pred[js2] = op1; if(ms2) m_pred[ms2] = op1;
		if(j_succ[op1]==op2){
			j_succ[op1] = js2;
			j_succ[op2] = op1;
			j_pred[op1] = op2;
			j_pred[op2] = jp1;
		} else if(j_succ[op2]==op1){
			j_succ[op1] = op2;
			j_succ[op2] = js1;
			j_pred[op1] = jp2;
			j_pred[op2] = op1;
		} else {
			j_succ[op1] = js2;
			j_succ[op2] = js1;
			j_pred[op1] = jp2;
			j_pred[op2] = jp1; //valid_schedule();
		}
		if(m_succ[op1]==op2){
			m_succ[op1] = ms2;
			m_succ[op2] = op1;
			m_pred[op1] = op2;
			m_pred[op2] = mp1;
		} else if(m_succ[op2]==op1){
			m_succ[op1] = op2;
			m_succ[op2] = ms1;
			m_pred[op1] = mp2;
			m_pred[op2] = op1;
		} else {
			m_succ[op1] = ms2;
			m_succ[op2] = ms1;
			m_pred[op1] = mp2;
			m_pred[op2] = mp1;
		}

	}
	int op(int j, int m){
		assert(j>0);
		assert(m>=0);
		return (j-1)*n_mach + m;	}
	
	bool adj_job (unsigned op1, unsigned op2){ return j_pred[op1] == op2 || j_pred[op2] == op1; }
	bool adj_mach(unsigned op1, unsigned op2){ return m_pred[op1] == op2 || m_pred[op2] == op1; }
	void fill_ord(vector<unsigned> &j_ord, vector<unsigned> &m_ord){
		first = op(j_ord[1], m_ord[1]);

		for(int m=1; m<=n_mach; m++){
			for(int j=1; j<=n_jobs; j++){ 
				if(j>1) m_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
				if(m>1) j_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
				if(j<n_jobs) m_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
				if(m<n_mach) j_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
			}
		}}
	void init(int method){
		switch(method){
			case RANDOM:
				init_random();
				break;
			case GREEDY_JOBS:
				init_greedy_jobs();
				break;
			case GREEDY_MACHINES:
				init_greedy_machines();
				break;
			default:
				break;}}
	void fill_vec_random(vector<unsigned> &v){
		iota(v.begin()+1, v.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(v.begin()+1,v.end());}
	void init_random(){
		vector<unsigned> j_ord(n_jobs+1, 0),
							 m_ord(n_mach+1, 0);
							 
		fill_vec_random(j_ord);//printv(j_ord,1,"Ordem randomizada - jobs");
		fill_vec_random(m_ord);//printv(m_ord,1,"Ordem randomizada - machines");
		fill_ord(j_ord, m_ord);}

	void init_greedy_jobs(){
		vector<unsigned int> j_ord(n_jobs+1,0), m_ord(n_mach+1,0), used(n_jobs+1,0);
		unsigned int temp_n_j, temp_first, best_j, best_ms, ms;
		fill_vec_random(m_ord);
		for(int i = n_jobs; i>0; i--){
			temp_n_j = n_jobs - i + 1;
			best_ms = 999999;
			for(int j = n_jobs; j>=1; j--){ if(used[j]) continue;

				j_ord[i] = j;
				for(int m = n_mach; m>0; m--){
					if(j>1 && j_ord[j-1] > 0) m_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
					if(m>1) j_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
					if(j<temp_n_j) m_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
					if(m<n_mach) j_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
				}
				temp_first = op(j,m_ord[1]);
				vector<vector<unsigned>> g  = { j_succ, j_pred, m_succ, m_pred };
				Solution s(g, n_mach, temp_n_j, temp_n_j*n_mach,temp_first,instance);
				ms = s.calc_makespan();
				if(ms < best_ms){ best_j = j; best_ms = ms; used[j]++; }
			}
			j_ord[i] = best_j;
		}
		makespan = best_ms;
		fill_ord(j_ord, m_ord);	}
	void init_greedy_machines(){
		vector<unsigned int> j_ord(n_jobs+1, 0), m_ord(n_mach+1, 0), used(n_mach+1, 0);
		unsigned int temp_n_m, temp_first, best_m, best_ms, ms;
		fill_vec_random(j_ord);
		for(int i = n_mach; i>0; i--){
			temp_n_m = n_mach - i + 1;
			best_ms = 999999;
			for(int m = n_mach; m>=1; m--){	if(used[m]) continue;

				m_ord[i] = m;
				for(int j = n_jobs; j>0; j--){
					if(j>1) m_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
					if(m>1) j_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
					if(j<n_jobs) m_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
					if(m<temp_n_m) j_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
				}
				temp_first = op(j_ord[1],m);
				vector<vector<unsigned>> g  = { j_succ, j_pred, m_succ, m_pred };
				Solution s(g, n_mach, temp_n_m, n_jobs*temp_n_m,temp_first,instance);
				ms = s.calc_makespan();
				if(ms < best_ms){ best_m = m; best_ms = ms; used[m]++; }
			}
			m_ord[i] = best_m;
		}
		makespan = best_ms;
		fill_ord(j_ord, m_ord);	}
	
	void fill_heads(unsigned op, vector<unsigned>& heads, int cont, vector<unsigned>& visited){
		//if(op > n_ops) cout << br << "op " << op << br;
		assert(op >=0);
		//assert(op <= n_jobs*n_jobs && op <= n_mach*n_mach);
		if(cont>n_ops || op == 0 || visited[op] == 1) return;
		//cout<<br<<"fill_heads.op "<<op; printv(j_pred,1,"j_pred"); printv(m_pred,1,"m_pred");
		if(op != first)	
			heads[op] = max(heads[j_pred[op]] + instance.cost[j_pred[op]],
							heads[m_pred[op]] + instance.cost[m_pred[op]]);
	
		if(heads[op] > heads[last]) last = op;
        visited[op] = 1;
		fill_heads(j_succ[op], heads, cont + 1, visited);
		fill_heads(m_succ[op], heads, cont + 1, visited);}


	vector<unsigned> crit_path(){
		assert(last <= n_ops);
		vector<unsigned> late_pred = fill_late_pred(), p;
		unsigned op = last;

		while(op > 0){
			p.push_back(op);
			op = late_pred[op];
		}
		reverse(p.begin(),p.end());
		return p;	}

	void blocks_j(vector<unsigned> p, vector<unsigned> &b_begin, vector<unsigned> &b_end){
		int block_size = 0;
        b_begin.push_back(0);
        for(int i=0; i<p.size()-1; i++){
            if(instance.o_job[p[i]] != instance.o_job[p[i+1]] && block_size > 1){
                b_end.push_back(i);
                b_begin.push_back(i+1);
				block_size = 0;
            } else block_size++;
        }
        b_end.push_back(p.size()-1);
		if(b_begin[b_begin.size()-1] == b_end[b_end.size()-1]){
			b_begin.pop_back();
			b_end.pop_back();
		}		
	}

    void blocks_m(vector<unsigned> p, vector<unsigned> &b_begin, vector<unsigned> &b_end){
        int block_size = 0;
        b_begin.push_back(0);
        for(int i=0; i<p.size()-1; i++){
            if((instance.o_machine[p[i]] != instance.o_machine[p[i+1]]) && block_size > 1){
                b_end.push_back(i);
                b_begin.push_back(i+1);
                block_size = 0;
            } else block_size++;
        }
        b_end.push_back(p.size()-1);
		if(b_begin[b_begin.size()-1] == b_end[b_end.size()-1]){
			b_begin.pop_back();
			b_end.pop_back();
		}	
	}

	vector<unsigned> fill_late_pred(){
		vector<unsigned> late_pred(n_ops+1, 0);
		for(int i=1; i<=n_ops; i++)
			if(instance.cost[m_pred[i]] > instance.cost[j_pred[i]]) late_pred[i] = m_pred[i];
			else late_pred[i] = j_pred[i];
		return late_pred;	}

	void sw1j(){} void sw1m(){}
    //swap em operacoes de mesmo bloco critico (job)
    void sw2j(){
        vector<unsigned> b_begin, b_end, p = crit_path(); assert(p.size()>0);
		//Indice de um bloco que comeca em b_begin[index] e termina em b_end[index]
		int index;

        blocks_j(p,b_begin, b_end);
		if(b_begin.size() < 1) return;
        //Selecao de um bloco critico aleatorio
		(b_begin.size() == 1) ? index = 0 : index = randint(0,b_begin.size()-1);
        
		int j = b_end[index],i = b_begin[index]; //Indices de comeco e fim de um bloco
        //Selecao de operacoes dentro desse bloco; a, b sao indices caminho critico
        int a = randint(i,j), b = randint(i,j);
        while(a==b) b = randint(i,j); 
		assert(a>=0); assert(b<=p.size()-1);
        swap(p[a], p[b]);
	}
	//swap em operacoes de mesmo bloco critico (maquina)
    void sw2m(){
		vector<unsigned> b_begin, b_end, p = crit_path(); assert(p.size()>0);
		int index;

        blocks_m(p,b_begin, b_end);
		if(b_begin.size() < 1) return;
		(b_begin.size() == 1) ? index = 0 : index = randint(0,b_begin.size()-1);
        
		int j = b_end[index],i = b_begin[index]; 
        int a = randint(i,j), b = randint(i,j);
        while(a==b) b = randint(i,j); 
		assert(a>=0); assert(b<=p.size()-1);
        swap(p[a], p[b]);
	} 
	void sw3j(){
		vector<unsigned> b_begin, b_end, p = crit_path(); assert(p.size()>0);
		int index;

        blocks_j(p,b_begin, b_end);
		if(b_begin.size() < 1) return;
		(b_begin.size() == 1) ? index = 0 : index = randint(0,b_begin.size()-1); 
        swap(p[index], p[index+ 1]);
	}
	void sw3m(){
		vector<unsigned> b_begin, b_end, p = crit_path(); assert(p.size()>0);
		int index;

        blocks_m(p,b_begin, b_end);
		if(b_begin.size() < 1) return;
		(b_begin.size() == 1) ? index = 0 : index = randint(0,b_begin.size()-1); 
        swap(p[index], p[index+ 1]);
	} 
	void sh1j(){} void sh1m(){}  void sh2j(){} void sh2m(){} void sh3j(){} void sh3m(){}

	vector<unsigned> get_heads(){
		vector<unsigned> heads(n_ops + 1, 0), visited(n_ops + 1, 0);
		assert(first <= n_ops);
		assert(first >= 1);
		fill_heads(first, heads, 1, visited);
		return heads;	}
public:
	Solution () {}
	Solution(Instance i, int init_method){
		instance = i;
		last = 0;
		makespan = 0;
		n_ops = instance.n_ops;
		n_jobs = instance.n_jobs;
		n_mach = instance.n_machines;
		j_succ.resize(n_ops+1,0);
		j_pred.resize(n_ops+1,0);
		m_succ.resize(n_ops+1,0);
		m_pred.resize(n_ops+1,0);
		init(init_method);	}
	
	Solution(vector<vector<unsigned>> g, unsigned n_m, unsigned n_j, unsigned n_o, unsigned f, Instance i){
		j_succ = g[0];
		j_pred = g[1];
		m_succ = g[2];
		m_pred = g[3];
		n_mach = n_m;
		n_jobs = n_j;
		first = f;
		instance = i;
		last = 0;
		n_ops = n_o;	}

	int calc_makespan(){ 
		//valid_schedule();
		//print_o();
		makespan = 0;
		int temp_makespan = 0, op, q_lookat = 0, q_pushed = 0;
		vector<unsigned> degrees_in(n_ops + 1,0), op_queue(n_ops), heads = get_heads(), late_pred = fill_late_pred(); 

        for(int i = 1; i <= n_ops; i++){
			if(j_pred[i]>0) degrees_in[i]++;
			if(m_pred[i]>0) degrees_in[i]++;
			if(degrees_in[i]==0) op_queue[q_pushed++] = i;}
		

		while(q_lookat < q_pushed){
			op = op_queue[q_lookat++];
			temp_makespan = heads[op] + instance.cost[op];
			//cout<<br<<"op = "<<op<<br<<"op.heads = "<<heads[op]<<br<<"op.cost = "<<instance.cost[op]<<br;

			if(temp_makespan > makespan){
				makespan = temp_makespan;
				last = op;
			}

			for(int op_succ : { j_succ[op], m_succ[op] }){
				if (op_succ != DUMMY){
					degrees_in[op_succ]--;//printv(degrees_in,1,"Gr entrada:");
					if(degrees_in[op_succ] == 0) op_queue[q_pushed++] = op_succ;//print_q(op_queue,q_lookat);
					if(heads[op_succ] < temp_makespan){
						heads[op_succ] = temp_makespan;
						late_pred[op_succ] = op;
					}
				}
			}

		}
		return makespan;	}

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

	void print_o(){
		int ini = 0;
		printv(j_succ, ini,"Sucessores nos jobs:");
		printv(j_pred, ini,"Predecessores nos jobs:");
		printv(m_succ, ini,"Sucessores nas maquinas:");
		printv(m_pred, ini,"Predecessores nas maquinas:");
	} 
	Solution copy_solution(){
		//valid_schedule();
		Solution s;
		s.n_ops = n_ops;
		s.n_jobs = n_jobs;
		s.n_mach = n_mach;
		s.j_pred = j_pred;
		s.j_succ = j_succ;
		s.m_pred = m_pred;
		s.m_succ = m_succ;
		s.first = first;
		s.last = last;
		s.instance = instance;
		//s.valid_schedule();
		return s;
	}

	void valid_schedule(){
		/* int no_pred_m = 0, no_pred_j = 0, no_succ_m = 0, no_succ_j = 0;
		for(int i=1; i<=n_jobs; i++){
			if(m_pred[i] == 0) no_pred_m++;
			if(m_succ[i] == 0) no_succ_m++;
			if(j_pred[i] == 0) no_pred_j++;
			if(j_succ[i] == 0) no_succ_j++;
		}
		assert(no_pred_m==n_mach);
		assert(no_pred_j==n_jobs);
		assert(no_succ_m==n_mach);
		assert(no_succ_j==n_jobs); */
		assert(*max_element(j_pred.begin(), j_pred.end()) <= n_ops); assert(*min_element(j_pred.begin(), j_pred.end()) == 0);
		assert(*max_element(j_succ.begin(), j_succ.end()) <= n_ops); assert(*min_element(j_succ.begin(), j_succ.end()) == 0);
		assert(*max_element(m_pred.begin(), m_pred.end()) <= n_ops); assert(*min_element(m_pred.begin(), m_pred.end()) == 0);
		assert(*max_element(m_succ.begin(), m_succ.end()) <= n_ops); assert(*min_element(m_succ.begin(), m_succ.end()) == 0);
		
	}

	void valid_op(unsigned op){
		assert(op <= n_jobs*n_jobs && op <= n_mach*n_mach);
	}
	friend class Heuristics;
};

#endif