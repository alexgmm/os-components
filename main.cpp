#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm> //random_shuffle
#include <numeric> //iota
#define RANDOM (0)
#define GREEDY_JOBS (1)
#define GREEDY_MACHINES (2)
#define DUMMY (0)

using namespace std;

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

		for(i = 1; i < cost.size(); i++){
			f >> c;
			cost[i] = c;
		}
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
		iota(jobs_succ.begin() + 1, jobs_succ.end(), 1);
		random_shuffle(jobs_succ.begin() + 1, jobs_succ.end());

		for(int i = 1; i<=jobs_succ.size(); i++) jobs_pred[jobs_succ[i]] = i;

		iota(machines_succ.begin() + 1, machines_succ.end(), 1);
		random_shuffle(machines_succ.begin() + 1, machines_succ.end());

		for(int i = 1; i<=machines_succ.size(); i++) machines_pred[machines_succ[i]] = i;
	}

	void initGreedyJobs(){}
	void initGreedyMachines(){}

	void calc_makespan(){
		makespan = 0;
		int temp_makespan = 0, operation, operation_succ, _operation, queue_top = 0;
		vector<unsigned int> degrees_in(instance.n_ops,0);
		vector<unsigned int> op_queue;

		for(int i = 1; i <= (instance.n_jobs > instance.n_machines ? instance.n_jobs: instance.n_machines); i++){
			if(i <= instance.n_jobs && jobs_succ[i] > 0) degrees_in[i]++;
			if(i <= instance.n_machines && machines_succ[i] > 0) degrees_in[i]++;
			if(!degrees_in[i]) op_queue.push_back(i);
		}

		while(queue_top < op_queue.size()){
			operation = op_queue(queue_top);

			temp_makespan = partial_scheduling[operation] + instance.cost[operation];
			if(temp_makespan > makespan){ 
				makespan = temp_makespan;
				_operation = operation;
			}

			operation_succ = jobs_succ[operation];

			if (operation_succ != DUMMY){
				degrees_in[operation_succ]--;
				if(!degrees_in[operation_succ]) op_queue.push_back(operation_succ);
				if(partial_schedule[operation_succ] < temp_makespan){
					partial_schedule[operation_succ] = temp_makespan;
					prev[operation_succ] = operation; //??????
				}
			}


			operation_succ = machines_succ[operation];
			if (operation_succ != DUMMY){
				degrees_in[operation_succ]--;
				if(!degrees_in[operation_succ]) op_queue.push_back(operation_succ);
				if(partial_schedule[operation_succ] < temp_makespan){
					partial_schedule[operation_succ] = temp_makespan;
					prev[operation_succ] = operation; //??????
				}
			}

			queue_top++; //Proxima operacao da fila
		}
	}
public:
	Solution () {}
	Solution(Instance i, int init_method){
		instance = i;
		jobs_succ.resize(instance.n_jobs + 1);
		jobs_pred.resize(instance.n_jobs + 1);
		machines_succ.resize(instance.n_machines + 1);
		machines_pred.resize(instance.n_machines + 1);
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
};

int main(){
	Instance i("test");
	Solution s(i, RANDOM);
	return 0;
}
