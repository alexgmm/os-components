#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>
#include <vector>
#include <algorithm> //random_shuffle
#include <numeric> //iota
#include <random>
#include "instance.hpp"

using namespace std;

int randint(int ini, int end){
        random_device dev;
        mt19937 rng(dev());
        uniform_int_distribution<mt19937::result_type> dist6(ini,end);

        return dist6(rng);
}

class Solution {
	vector<unsigned> j_succ, j_pred, m_succ, m_pred;
	unsigned makespan, n_mach, n_jobs, n_ops, last, first;
	Instance instance;

	// Numero/indice da operacao (j,m)
	// Indice 0 e desconsiderado para todos os vetores
	int op(int j, int m){ return (j-1)*n_mach + m;}
	bool adj_job(unsigned op1, unsigned op2){ return j_pred[op1] == op2 || j_pred[op2] == op1; }
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
		vector<unsigned> j_ord(n_jobs+1, 0), m_ord(n_mach+1, 0), j_used(n_jobs+1, 0);
		unsigned  best_job, best_mkspn, mkspn;
		fill_vec_random(m_ord); 
		unsigned num_jobs = n_jobs;
		for(int i = n_jobs; i>0; i--){ //cout<<br<<"i="<<i;
			n_jobs = num_jobs - i + 1; //cout<<br<<"temp_n_j="<<temp_n_j;
			best_mkspn = 999999;
			for(int j = num_jobs; j>0; j--){ if(j_used[j] == 1) continue;

				j_ord[i] = j; //print_ord(j_ord,m_ord);
				for(int m = n_mach; m>0; m--){
					if(j>1) m_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
					if(m>1) j_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
					if(j<n_jobs) m_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
					if(m<n_mach) j_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
				}
				first = op(j,m_ord[1]); //cout<<br<<"temp_first="<<temp_first;
				mkspn = calc_makespan();
				/* vector<vector<unsigned>> g  = { j_succ, j_pred, m_succ, m_pred };
				Solution s(g, n_mach, temp_n_j, temp_n_j*n_mach,temp_first,instance);
				mkspn = s.calc_makespan(); //cout<<br<<"makespan obtido="<<mkspn; */

				if(mkspn < best_mkspn){ best_job = j; best_mkspn = mkspn; j_used[j] = 1; }
			}
			j_ord[i] = best_job;
		}
		makespan = best_mkspn;
		n_jobs = num_jobs;
		fill_ord(j_ord, m_ord);
	}
	void init_greedy_machines(){
		vector<unsigned> j_ord(n_jobs+1, 0), m_ord(n_mach+1, 0), m_used(n_mach+1, 0);
		unsigned best_mach, best_mkspn, mkspn;
		fill_vec_random(j_ord); //printv(j_ord,1,"j_ord");
		unsigned num_mach = n_mach;
		for(int i = n_mach; i>0; i--){
			n_mach = num_mach - i + 1;
			best_mkspn = 999999;
			for(int m = num_mach; m>0; m--){ if(m_used[m] == 1) continue;

				m_ord[i] = m;
				for(int j = n_jobs; j>0; j--){
					if(j>1) m_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
					if(m>1) j_pred[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
					if(j<n_jobs) m_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
					if(m<num_mach) j_succ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
				}
				first = op(j_ord[1],m);
				mkspn = calc_makespan();
				/* temp_first = op(j_ord[1],m);
				vector<vector<unsigned>> g  = { j_succ, j_pred, m_succ, m_pred };
				Solution s(g, temp_n_m, n_jobs, n_jobs*temp_n_m,temp_first,instance);
				mkspn = s.calc_makespan(); */
				if(mkspn < best_mkspn){ best_mach = m; best_mkspn = mkspn; m_used[m] = 1; }
			}
			m_ord[i] = best_mach;
		}
		makespan = best_mkspn;
		n_mach = num_mach;
		fill_ord(j_ord, m_ord);}
	void print_ord(vector<unsigned> &j, vector<unsigned> &m){
		cout<<br;
		for(int i=1; i<j.size(); i++){
			for(int k=1; k<m.size(); k++){
				if(j[i]!=0 && m[k]!=0) cout<<op(j[i],m[k])<<" ";
				else cout<<0<<" ";

			}
			cout<<br;
		}}
    void print_q(vector<unsigned> &s, int lookat){
		cout<<br<<"Pilha:"<<br;
		for(int i=s.size()-1; i>=0; i--){
			cout<<"|"<<s[i]<<"|";
			if(i==lookat) cout<<"<";
			cout<<br;}}
	void fill_heads(unsigned op, vector<unsigned>& heads, int cont, vector<unsigned>& visited){
		if(cont>n_ops || op == 0 || visited[op] == 1) return;
		//cout<<br<<"fill_heads.op "<<op; printv(j_pred,1,"j_pred"); printv(m_pred,1,"m_pred");
		if(op != first)	
			heads[op] = max(heads[j_pred[op]] + instance.cost[j_pred[op]],
							heads[m_pred[op]] + instance.cost[m_pred[op]]);
	
		if(heads[op] > heads[last]) last = op;
        visited[op] = 1;
		fill_heads(j_succ[op], heads, cont + 1, visited);
		fill_heads(m_succ[op], heads, cont + 1, visited);}

	void shift(unsigned pos_op, vector<unsigned> &p, int end_b){
        int pos = randint(pos_op,end_b);
        for(pos_op; pos_op < pos; pos_op++)
            swap(p[pos_op], p[pos_op + 1]);}

	vector<unsigned> crit_path(){
		vector<unsigned> late_pred = fill_late_pred();
		vector<unsigned> p;
		unsigned op = last;

		while(op > 0){
			p.push_back(op);
			op = late_pred[op];
		}
		reverse(p.begin(),p.end());
		return p;
	}

	void blocks_j(vector<unsigned> p, vector<unsigned> &b_begin, vector<unsigned> &b_end){
        b_begin.push_back(p[0]);
        for(int i=0; i<p.size()-1; i++){
            if(instance.o_job[p[i]] != instance.o_job[p[i+1]]){
                b_end.push_back(i);
                b_begin.push_back(i+1);
            }
        }
        b_end.push_back(p[p.size()-1]);
	}

    void blocks_m(vector<unsigned> p, vector<unsigned> &b_begin, vector<unsigned> &b_end){
        int n = 0;
        b_begin.push_back(p[0]);
        for(int i=0; i<p.size()-1; i++){
            if((instance.o_machine[p[i]] != instance.o_machine[p[i+1]]) || n > 1){
                b_end.push_back(i);
                b_begin.push_back(i+1);
                n = 0;
            } else n++;
        }
        b_end.push_back(p[p.size()-1]);
    }

	vector<unsigned> fill_late_pred(){
		vector<unsigned> late_pred(n_ops+1, 0);
		for(int i=1; i<=n_ops; i++)
			if(instance.cost[m_pred[i]] > instance.cost[j_pred[i]]) late_pred[i] = m_pred[i];
			else late_pred[i] = j_pred[i];
		return late_pred;
	}
public:
	Solution () {}
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
		n_ops = n_o;
	}
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
		init(init_method);
	}
	
	int calc_makespan(){
		makespan = 0;
		int temp_makespan = 0, op, q_lookat = 0, q_pushed = 0;
		vector<unsigned> degrees_in(n_ops + 1,0), op_queue(n_ops), heads(n_ops + 1, 0), visited(n_ops + 1, 0), late_pred = fill_late_pred(); 

		fill_heads(first, heads, 1, visited);//printv(heads, 1, "heads"); printv(late_pred, 1, "late_pred");
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
		return makespan;
	}

	void swap(unsigned op1, unsigned op2){
        if(op1 == first) first = op2;
        else if(op2 == first) first = op1;

		int js1=j_succ[op1], jp1=j_pred[op1], ms1=m_succ[op1], mp1=m_pred[op1],
			js2=j_succ[op2], jp2=j_pred[op2], ms2=m_succ[op2], mp2=m_pred[op2];

		if(jp1) j_succ[jp1] = op2;
		if(js1) j_pred[js1] = op2;
		if(mp1) m_succ[mp1] = op2;
		if(ms1) m_pred[ms1] = op2;

		if(jp2) j_succ[jp2] = op1;
		if(js2) j_pred[js2] = op1;
		if(mp2) m_succ[mp2] = op1;
		if(ms2) m_pred[ms2] = op1;

		if(adj_job(op1,op2)){
			j_pred[op1] = op2; j_succ[op1] = op2;
			j_pred[op2] = op1; j_succ[op2] = op1;
		} else {
			j_pred[op1] = jp2; j_succ[op1] = js2;
			j_pred[op2] = jp1; j_succ[op2] = js1;
		}
		if(adj_mach(op1, op2)){
			m_pred[op1] = op2; m_succ[op1] = op2;
			m_pred[op2] = op1; m_succ[op2] = op1;
		} else {
			m_pred[op1] = mp2; m_succ[op1] = ms2;
			m_pred[op2] = mp1; m_succ[op2] = ms1; 
		}
		
		/* j_succ[j_pred[op1]] = op2;
		m_succ[m_pred[op1]] = op2;
		j_pred[j_succ[op1]] = op2;
		m_pred[m_succ[op1]] = op2;

		j_succ[j_pred[op2]] = op1;
		m_succ[m_pred[op2]] = op1;
		j_pred[j_succ[op2]] = op1;
		m_pred[m_succ[op2]] = op1;

		unsigned a1 = j_pred[op1],
					 a2 = j_succ[op1], 
					 a3 = m_pred[op1], 
					 a4 = m_succ[op1];

		j_pred[op1] = j_pred[op2];
		j_succ[op1] = j_succ[op2];
		m_pred[op1] = m_pred[op2];
		m_succ[op1] = m_succ[op2];

		j_pred[op2] = a1;
		j_succ[op2] = a2;
		m_pred[op2] = a3;
		m_succ[op2] = a4; */

	}
	void print(){
		int o = first, o_;
		for(int j=1; j<=n_jobs; j++){
			cout << br << o << " ";
			o_ = o;
			for(int m=2; m<=n_mach; m++){
				cout << m_succ[o_] << " ";
				o_ = m_succ[o_];
			}
			o = j_succ[o];
		}
		cout << br;

	}

	void print_o(){
		int ini = 0;
		printv(j_succ, ini,"Sucessores nos jobs:");
		printv(j_pred, ini,"Predecessores nos jobs:");
		printv(m_succ, ini,"Sucessores nas maquinas:");
		printv(m_pred, ini,"Predecessores nas maquinas:");
	} 
	friend class Heuristics;
};

#endif