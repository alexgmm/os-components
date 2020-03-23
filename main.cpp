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

using namespace std;

void printv(vector<unsigned int>&v, int ini, string name){
	cout << "\n" << name << "\n";
	for(int i=ini; i<v.size(); i++) cout << v[i] << " ";
	cout << "\n";
}

void swap(vector<unsigned int>&v, int a, int b){
	int c = v[a];
	v[a] = v[b];
	v[b] = c;
}

class Instance {
	vector<unsigned int> cost;
	vector<unsigned int> o_job;
	vector<unsigned int> o_machine;
	int n_jobs, n_machines, n_ops;

	int array_index(int i, int j){ return i*n_machines + j + 1;}
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
		for(int i=1; i<n_ops; i++)
			cout << o_job[i] << " ";

		cout << "\n" << "O -> machines" << "\n";
		for(int i=1; i<n_ops; i++)
			cout << o_machine[i] << " ";
		cout << "\n";
	}

	friend class Solution;
};

class Solution {
	vector<unsigned int> jobs_succ;
	vector<unsigned int> jobs_pred;
	vector<unsigned int> machines_succ;
	vector<unsigned int> machines_pred;
	unsigned int makespan;
	Instance instance;

	void initRandom(){ //Gera sol. inicial rand.
		unsigned int index;
		random_device rd;
		mt19937 rng(rd());
		uniform_int_distribution<unsigned int> uni(0,instance.n_ops-1);
		vector<unsigned int> random(instance.n_ops);

		//Gerando sucessores nos jobs
		iota(random.begin(), random.end(), 1);
		srand(unsigned(time(0)));
		random_shuffle(random.begin(), random.end());

		index = uni(rng);
		random[index] = 0; //Operacao maximal = index+1
		for(int i=1; i<jobs_succ.size(); i++){
			if(random[i-1] == i) swap(random, i-1, random.size()-1);
			jobs_succ[i] = random[i-1];
		} 

		if(jobs_succ[jobs_succ.size()-1] == jobs_succ.size()) 
			swap(jobs_succ, jobs_succ.size()-1, 1);
		//Gerando predecessores nos jobs
		for(int i = 1; i<=jobs_succ.size(); i++) jobs_pred[jobs_succ[i]] = i;


		//Gerando sucessores nas maquinas
		iota(random.begin(), random.end(), 1);
		srand(unsigned(time(0)));
		random_shuffle(random.begin(), random.end());

		index = uni(rng);
		random[index] = 0;

		for(int i=1; i<machines_succ.size(); i++){
			if(random[i-1] == i) swap(random, i-1, random.size()-1);
			machines_succ[i] = random[i-1];
		} 

		if(machines_succ[machines_succ.size()-1] == machines_succ.size()) 
			swap(machines_succ, machines_succ.size()-1, 1);
		//Gerando predecessores na máquinas
		for(int i = 1; i<=machines_succ.size(); i++) machines_pred[machines_succ[i]] = i;
	}

	void initGreedyJobs(){}
	void initGreedyMachines(){}

	void fill_heads(unsigned int op, vector<unsigned int>& heads, vector<unsigned int>& late_pred, int cont){
		if(cont==instance.n_ops) return;
		heads[op] = instance.cost[op] + instance.cost[late_pred[op]];
		fill_heads(jobs_succ[op], heads, late_pred, cont + 1);
		fill_heads(machines_succ[op], heads, late_pred, cont + 1);
	}

	void calc_makespan(){
		makespan = 0;
		int temp_makespan = 0, operation, operation_succ, _operation, queue_lookat = 0;
		vector<unsigned int> degrees_in(instance.n_ops,0); //Graus de entrada por operacao
		vector<unsigned int> op_queue; //Lista de operacoes
		vector<unsigned int> late_pred(instance.n_ops); //Predecessores mais tardios
		vector<unsigned int> heads(instance.n_ops + 1);

		heads[0] = 0;
		for(int i=1; i<=instance.n_ops; i++){
			late_pred[i] = max(instance.cost[jobs_pred[i]], instance.cost[machines_pred[i]]);
		}
			
		
		fill_heads(1, heads, late_pred, 1);
		
		for(int i = 1; i <= (instance.n_jobs > instance.n_machines ? instance.n_jobs: instance.n_machines); i++){
			if(i <= instance.n_jobs && jobs_succ[i] > 0) degrees_in[i]++;
			if(i <= instance.n_machines && machines_succ[i] > 0) degrees_in[i]++;
			if(!degrees_in[i]) op_queue.push_back(i);
		}
		
		while(queue_lookat < op_queue.size()){
			operation = op_queue[queue_lookat];

			temp_makespan = heads[operation] + instance.cost[operation];
			if(temp_makespan > makespan){ 
				makespan = temp_makespan;
				_operation = operation;
			}

			operation_succ = jobs_succ[operation];

			if (operation_succ != DUMMY){
				degrees_in[operation_succ]--;
				if(!degrees_in[operation_succ]) op_queue.push_back(operation_succ);
				if(heads[operation_succ] < temp_makespan){
					heads[operation_succ] = temp_makespan;
					late_pred[operation_succ] = operation;
				}
			}


			operation_succ = machines_succ[operation];
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
		jobs_succ.resize(instance.n_ops + 1);
		jobs_pred.resize(instance.n_ops + 1);
		machines_succ.resize(instance.n_ops + 1);
		machines_pred.resize(instance.n_ops + 1);
		//Cria solucao inicial
		init(init_method);
		
		calc_makespan();
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
		printv(jobs_succ, ini,"Sucessores nos jobs:");
		//printv(jobs_pred, ini,"Predecessores nos jobs:");
		printv(machines_succ, ini,"Sucessores nas maquinas:");
		//printv(machines_pred, ini,"Predecessores nas maquinas:");
		cout << "\nmakespan: " << makespan << "\n" ;
	}
};

int main(){
	Instance i("test2"); i.print();
	Solution s(i, RANDOM);
	s.print();
	return 0;
}
