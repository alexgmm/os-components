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
	for(int i=ini; i<v.size(); i++) cout<<i<<" "; cout<<br;
	for(int i=ini; i<v.size()-1; i++) cout<<"--"; cout<<br;
	for(int i=ini; i<v.size(); i++) cout << v[i] << " ";
	cout << "\n";}

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

		cost.resize(n_jobs*n_machines + 1, 0);
		o_job.resize(n_jobs*n_machines + 1);
		o_machine.resize(n_jobs*n_machines + 1);

		for(i = 1; i < cost.size(); i++){
			f >> c;
			cost[i] = c;
			o_job[i] = (i-1)/n_machines + 1;
			o_machine[i] = (i-1)%n_machines + 1;
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
		cout << "\n";}
	
	friend class Solution;};

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
	
		first = op(j_ord[1], m_ord[1]); //cout<<br<<"first = "<<first;

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
		printv(m_pred, ini,"Predecessores nas maquinas:"); */

		calc_makespan();
	}

	void initGreedyJobs(){}
	void initGreedyMachines(){}

	void fill_heads(unsigned int op, vector<unsigned int>& heads, vector<unsigned int>& late_pred, int cont){
		if(cont>n_ops || op == 0) return;
		//cout<<br<<"fill_heads.op "<<op;
		if(op != first)	
			heads[op] = max(heads[j_pred[op]] + instance.cost[j_pred[op]],
							heads[m_pred[op]] + instance.cost[m_pred[op]]);
	
		if(heads[op] > heads[last]) last = op;
		fill_heads(j_succ[op], heads, late_pred, cont + 1);
		fill_heads(m_succ[op], heads, late_pred, cont + 1);
	}

	void print_q(vector<unsigned int> &s, int lookat){
		cout<<br<<"Pilha:"<<br;
		for(int i=s.size()-1; i>=0; i--){
			cout<<"|"<<s[i]<<"|";
			if(i==lookat) cout<<"<";
			cout<<br;}}
	
	void calc_makespan(){
		makespan = 0;
		int temp_makespan = 0, op, op_succ, q_lookat = 0, q_pushed = 0;
		vector<unsigned int> degrees_in(n_ops + 1,0); //Graus de entrada por operacao
		vector<unsigned int> op_queue(n_ops); //Lista de operacoes
		vector<unsigned int> late_pred(n_ops + 1); //Predecessores mais tardios
		vector<unsigned int> heads(n_ops + 1, 0);

		for(int i=1; i<=n_ops; i++)
			if(m_pred[i] > j_pred[i]) late_pred[i] = m_pred[i];
			else late_pred[i] = j_pred[i];

		fill_heads(first, heads, late_pred, 1);

		//if(DEBUG){printv(heads, 1, "heads"); printv(late_pred, 1, "late_pred");}

		for(int i = 1; i <= n_ops; i++){
			if(j_pred[i]>0) degrees_in[i]++;
			if(m_pred[i]>0) degrees_in[i]++;
			if(degrees_in[i]==0) op_queue[q_pushed++] = i;
		}
		

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
				//if(degrees_in[operation_succ]>0)
				degrees_in[op_succ]--;//printv(degrees_in,1,"Gr entrada:");
				if(degrees_in[op_succ] == 0) 
					op_queue[q_pushed++] = op_succ;//print_q(op_queue,q_lookat);
				if(heads[op_succ] < temp_makespan){
					heads[op_succ] = temp_makespan;
					late_pred[op_succ] = op;
				}
			}

			op_succ = m_succ[op];
			if (op_succ != DUMMY){
				//if(degrees_in[op_succ]>0)
				degrees_in[op_succ]--;//printv(degrees_in,1,"Gr entrada:");
				if(degrees_in[op_succ] == 0)
					op_queue[q_pushed++] = op_succ;//print_q(op_queue,q_lookat);
				if(heads[op_succ] < temp_makespan){
					heads[op_succ] = temp_makespan;
					late_pred[op_succ] = op;
				}
			}
		}
	}
public:
	Solution () {}
	Solution(Instance i, int init_method){
		instance = i;
		last = 0;
		n_ops = instance.n_ops;
		n_jobs = instance.n_jobs;
		n_mach = instance.n_machines;
		j_succ.resize(n_ops+1);
		j_pred.resize(n_ops+1);
		m_succ.resize(n_ops+1);
		m_pred.resize(n_ops+1);

		init(init_method);
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
		cout << "\nmakespan: " << makespan << "\n" ;}
};

int main(){
	Instance i("test2"); //i.print();
	Solution s(i, RANDOM);
	s.print();
	return 0;
}
//Otimizar a ordem nas maquinas:
//-Sortear uma ordem pros jobs
//-Para cada maquina, 

/* unsigned computeMakes(TopoData & topoData, vector<unsigned> & heads, vector<unsigned> & prev, unsigned & lastOp) const {
		assert(isAlloced());
		assert(heads.size() == osinst.nO);
		assert(topoData.indeg.size() == osinst.nO);
		assert(topoData.Q.size() == osinst.nO);
		assert(prev.size() == osinst.nO);

		unsigned qInsert = 0;
		unsigned qAccess = 0;

		unsigned curOp;
		unsigned newOp;

		unsigned newMax;
		unsigned makespan = 0;

		fill(heads.begin(), heads.end(), 0);
		fill(topoData.indeg.begin(), topoData.indeg.end(), 0);
		fill(prev.begin(), prev.end(), 0);

		//initializing indeg - puting minimal elements in queue
		for (unsigned o = 1; o < osinst.nO; o++) {
			if (_job[o] != 0) topoData.indeg[o]++;
			if (_mach[o] != 0) topoData.indeg[o]++;
			if (topoData.indeg[o] == 0) topoData.Q[qInsert++] = o;
		}

		assert(qInsert > 0); //Q can only be empty if there is a cycle

		while (qAccess < qInsert) {
			assert(qAccess < topoData.Q.size()); // each operation can get into the queue once and only once

			curOp = topoData.Q[qAccess++];
			assert(topoData.indeg[curOp] == 0); //all predecessors of curOp already vizited
			assert(curOp != 0); //curOp is not dummy
			newMax = heads[curOp] + osinst.P[curOp];
			if (makespan < newMax) {
				makespan = newMax;
				lastOp = curOp;
			}
			

			//job order
			newOp = job[curOp];
			if (newOp != 0) {
				assert(topoData.indeg[newOp] > 0);
				topoData.indeg[newOp]--;
				if (topoData.indeg[newOp] == 0) {
					assert(qInsert < topoData.Q.size() - 1);
					topoData.Q[qInsert++] = newOp;
				}
				if (heads[newOp] < newMax) {
					heads[newOp] = newMax;
					prev[newOp] = curOp;
				}
			}
			//mach order
			newOp = mach[curOp];
			if (newOp != 0) {
				assert(topoData.indeg[newOp] > 0);
				topoData.indeg[newOp]--;
				if (topoData.indeg[newOp] == 0) {
					assert(qInsert < topoData.Q.size() - 1);
					topoData.Q[qInsert++] = newOp;
				}
				if (heads[newOp] < newMax) {
					heads[newOp] = newMax;
					prev[newOp] = curOp;
				}
			}
		}//while (qAccess < qInsert)
		assert(qAccess == osinst.nO - 1);
		assert(lastOp != 0);

		return makespan;
	} */