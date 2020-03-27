#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm> //random_shuffle
#include <numeric> //iota
#include <cstdlib> 
#include <random>
#define RANDOM (0)
#define GREEDY_JOBS (1)
#define GREEDY_MACHINES (2)
#define DUMMY (0)
#define DEBUG (1)
#define br "\n"

using namespace std;

void printv(vector<unsigned int>&v, int ini, string name){
	cout << "\n" << name << "\n";
	for(int i=ini; i<v.size(); i++) cout << v[i] << " ";
	cout << "\n";}

void swap(vector<unsigned int>&v, int a, int b){
	int c = v[a];
	v[a] = v[b];
	v[b] = c;}

class Instance {
	vector<unsigned int> cost;
	vector<unsigned int> o_job;
	vector<unsigned int> o_machine;
	int n_jobs, n_machines, n_ops;
public:
	Instance() {}
	Instance (string filename){
		int i, j, c;
		fstream f(filename);
		
		f >> i; n_jobs = i;
		f >> j; n_machines = j; n_ops = i*j;

		cost.resize(n_jobs*n_machines + 1);
		o_job.resize(n_jobs*n_machines + 1);
		o_machine.resize(n_jobs*n_machines + 1);

		for(i = 1; i < cost.size(); i++){
			f >> c;
			cost[i] = c;
			o_job[i] = (i-1)/n_machines;
			o_machine[i] = (i-1)%n_machines;
		}
	}

	void print(){
		cout << "\n" << "Custos" << "\n";
		for(int i=1; i<=n_ops; i++)
			cout << i << ": " << cost[i] << "\n";
		cout << "\n" << "O -> jobs" << "\n";
		for(int i=1; i<=n_ops; i++)
			cout << o_job[i] << " ";

		cout << "\n" << "O -> machines" << "\n";
		for(int i=1; i<=n_ops; i++)
			cout << o_machine[i] << " ";
		cout << "\n";
	}
	
	friend class Solution;};

class Solution {
	vector<unsigned int> j_succ;
	vector<unsigned int> j_pred;
	vector<unsigned int> m_succ;
	vector<unsigned int> m_pred;
	unsigned int makespan, n_machines, n_jobs, n_ops;
	Instance instance;

	// Numero/indice da operacao (j,m)
	// Indice 0 e desconsiderado para todos os vetores
	int op(int j, int m){ return (j-1)*n_machines + m;}

	void initRandom(){ //Gera sol. inicial rand.
		vector<unsigned int> j_ord(n_jobs+1, 0),
							 m_ord(n_machines+1, 0);

		//Gerando ordem dos jobs
		iota(j_ord.begin()+1, j_ord.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(j_ord.begin()+1, j_ord.end());
		printv(j_ord,1,"Ordem randomizada - jobs");

		//Gerando ordem nas maquinas
		iota(m_ord.begin()+1, m_ord.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(m_ord.begin()+1, m_ord.end());
		printv(m_ord,1,"Ordem randomizada - machines");

		//Sucessores
		for(int m=1; m<=n_machines; m++){
			for(int j=1; j<=n_jobs; j++){
				if(j!=1) m_pred[op(j,m)] = op(j_ord[j-1],m_ord[m]);
				if(m!=1) j_pred[op(j,m)] = op(j_ord[j],m_ord[m-1]);
				if(j<n_jobs) m_succ[op(j,m)] = op(j_ord[j+1],m_ord[m]);
				if(m<n_machines) j_succ[op(j,m)] = op(j_ord[j],m_ord[m+1]);
			}
		}
	}

	void initGreedyJobs(){}
	void initGreedyMachines(){}

	void fill_heads(unsigned int op, vector<unsigned int>& heads, vector<unsigned int>& late_pred, int cont){
		if(cont==n_ops) return;
		heads[op] = instance.cost[op] + instance.cost[late_pred[op]];
		fill_heads(j_succ[op], heads, late_pred, cont + 1);
		fill_heads(m_succ[op], heads, late_pred, cont + 1);
	}

	void calc_makespan(){
		makespan = 0;
		int temp_makespan = 0, operation, operation_succ, _operation, queue_lookat = 0;
		vector<unsigned int> degrees_in(n_ops,0); //Graus de entrada por operacao
		vector<unsigned int> op_queue; //Lista de operacoes
		vector<unsigned int> late_pred(n_ops); //Predecessores mais tardios
		vector<unsigned int> heads(n_ops + 1);

		heads[0] = 0;
		fill_heads(1, heads, late_pred, 1);
		for(int i=1; i<=n_ops; i++)
			if(heads[m_pred[i]] > heads[j_pred[i]]) 
				late_pred[i] = m_pred[i];
			else late_pred[i] = j_pred[i];
			//late_pred[i] = max(instance.cost[jobs_pred[i]], instance.cost[machines_pred[i]]);
		
		printv(heads, 1, "heads"); printv(late_pred, 1, "late_pred");

		for(int i = 1; i <= (n_jobs > n_machines ? n_jobs: n_machines); i++){
			if(i <= n_jobs && j_succ[i] > 0) degrees_in[i]++;
			if(i <= n_machines && m_succ[i] > 0) degrees_in[i]++;
			if(!degrees_in[i]) op_queue.push_back(i);
		}
		
		while(queue_lookat < op_queue.size()){
			operation = op_queue[queue_lookat];

			temp_makespan = heads[operation] + instance.cost[operation];
			if(temp_makespan > makespan){ 
				makespan = temp_makespan;
				_operation = operation;
			}

			operation_succ = j_succ[operation];

			if (operation_succ != DUMMY){
				degrees_in[operation_succ]--;
				if(!degrees_in[operation_succ]) op_queue.push_back(operation_succ);
				if(heads[operation_succ] < temp_makespan){
					heads[operation_succ] = temp_makespan;
					late_pred[operation_succ] = operation;
				}
			}


			operation_succ = m_succ[operation];
			if (operation_succ != DUMMY){
				degrees_in[operation_succ]--;
				if(!degrees_in[operation_succ]) op_queue.push_back(operation_succ);
				if(heads[operation_succ] < temp_makespan){
					heads[operation_succ] = temp_makespan;
					late_pred[operation_succ] = operation;
				}
			}

			queue_lookat++; //Proxima operacao da fila
		}
	}
public:
	Solution () {}
	Solution(Instance i, int init_method){
		instance = i;
		n_ops = instance.n_ops;
		n_jobs = instance.n_jobs;
		n_machines = instance.n_machines;
		j_succ.resize(n_ops+1);
		j_pred.resize(n_ops+1);
		m_succ.resize(n_ops+1);
		m_pred.resize(n_ops+1);

		init(init_method);
		
		//calc_makespan();
	}

	void init(int method){
		switch(method){
			case RANDOM:
				initRandom();
				break;
			case GREEDY_JOBS:
				initGreedyJobs();
				break;
			case GREEDY_MACHINES:
				initGreedyJobs();
				break;
			default:
				break;
		}
	}

	void print(){
		if(!DEBUG) return;
		int ini = 1;
		printv(j_succ, ini,"Sucessores nos jobs:");
		printv(j_pred, ini,"Predecessores nos jobs:");
		printv(m_succ, ini,"Sucessores nas maquinas:");
		printv(m_pred, ini,"Predecessores nas maquinas:");
		//cout << "\nmakespan: " << makespan << "\n" ;
	}
};

int main(){
	Instance i("test2"); //i.print();
	Solution s(i, RANDOM);
	s.print();
	return 0;
}
