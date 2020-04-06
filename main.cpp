#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm> //random_shuffle
#include <numeric> //iota
#include <cstdlib> 
#include <random>
#include <map>

#define RANDOM (0)
#define GREEDY_JOBS (1)
#define GREEDY_MACHINES (2)
#define DUMMY (0)
#define br "\n"

using namespace std;

class Files {
public:
	vector<string> path = {"inst/taillard/", "inst/brucker/", "inst/gueret-prins/"};
	vector<vector<string>> names = {
		{"tai10_10_09.txt", "tai04_04_02.txt", "tai20_20_06.txt", "tai07_07_08.txt", "tai04_04_04.txt", "tai10_10_04.txt", "tai05_05_09.txt", "tai04_04_06.txt", "tai10_10_03.txt", "tai10_10_07.txt", "tai07_07_05.txt", "tai20_20_09.txt", "tai15_15_08.txt", "tai05_05_01.txt", "tai15_15_04.txt", "tai04_04_09.txt", "tai05_05_04.txt", "tai15_15_01.txt", "tai15_15_03.txt", "tai15_15_07.txt", "tai10_10_10.txt", "tai20_20_07.txt", "tai05_05_10.txt", "tai10_10_06.txt", "tai20_20_08.txt", "tai05_05_06.txt", "tai05_05_05.txt", "tai15_15_10.txt", "tai04_04_08.txt", "tai05_05_08.txt", "tai07_07_07.txt", "tai20_20_05.txt", "tai20_20_01.txt", "tai04_04_03.txt", "tai15_15_02.txt", "tai15_15_06.txt", "tai05_05_02.txt", "tai04_04_10.txt", "tai15_15_09.txt", "tai20_20_10.txt", "tai07_07_10.txt", "tai04_04_07.txt", "tai20_20_02.txt", "tai07_07_04.txt", "tai04_04_05.txt", "tai05_05_03.txt", "tai07_07_02.txt", "tai10_10_01.txt", "tai05_05_07.txt", "tai07_07_01.txt", "tai10_10_08.txt", "tai20_20_04.txt", "tai10_10_05.txt", "tai20_20_03.txt", "tai15_15_05.txt", "tai07_07_06.txt", "tai10_10_02.txt", "tai07_07_03.txt", "tai07_07_09.txt", "tai04_04_01.txt"},
		{"j4-per0-2", "j5-per10-1", "j3-per20-0", "j3-per0-2", "j7-per0-0", "j8-per10-2", "j5-per0-1", "j3-per0-1", "j5-per20-0", "j3-per20-1", "j8-per20-2", "j8-per0-1", "j5-per10-0", "j5-per0-0", "j4-per20-1", "j4-per10-0", "j8-per0-2", "j4-per10-2", "j8-per10-1", "j4-per0-1", "j4-per20-0", "j6-per10-0", "j6-per0-0", "j3-per20-2", "j6-per0-1", "j8-per20-1", "j6-per10-1", "j5-per20-2", "j8-per10-0", "j6-per20-1", "j3-per10-2", "j7-per0-1", "j4-per0-0", "j5-per20-1", "j7-per10-1", "j3-per10-1", "j3-per10-0", "j4-per10-1", "j5-per10-2", "j7-per10-0", "j5-per0-2", "j8-per20-0", "j4-per20-2", "j7-per20-1", "j6-per10-2", "j6-per20-0", "j6-per20-2", "j7-per20-2", "j6-per0-2", "j7-per0-2", "j7-per20-0", "j7-per10-2"},
		{"GP06-01.txt", "GP04-08.txt", "GP06-09.txt", "GP06-03.txt", "GP10-06.txt", "GP03-08.txt", "GP04-07.txt", "GP10-07.txt", "GP10-03.txt", "GP07-04.txt", "GP09-05.txt", "GP06-04.txt", "GP09-04.txt", "GP10-02.txt", "GP07-06.txt", "GP04-04.txt", "GP04-05.txt", "GP10-09.txt", "GP08-09.txt", "GP06-10.txt", "GP08-08.txt", "GP05-07.txt", "GP09-03.txt", "GP05-10.txt", "GP10-01.txt", "GP07-07.txt", "GP10-10.txt", "GP03-10.txt", "GP04-10.txt", "GP07-01.txt", "GP05-05.txt", "GP09-01.txt", "GP10-08.txt", "GP07-08.txt", "GP05-02.txt", "GP05-06.txt", "GP05-08.txt", "GP08-06.txt", "GP03-04.txt", "GP08-07.txt", "GP04-01.txt", "GP10-05.txt", "GP09-02.txt", "GP03-05.txt", "GP10-04.txt", "GP03-09.txt", "GP05-09.txt", "GP07-10.txt", "GP06-05.txt", "GP04-09.txt", "GP08-03.txt", "GP03-06.txt", "GP05-04.txt", "GP03-02.txt", "GP03-03.txt", "GP05-03.txt", "GP04-03.txt", "GP03-07.txt", "GP08-05.txt", "GP03-01.txt", "GP08-02.txt", "GP09-08.txt", "GP05-01.txt", "GP08-01.txt", "GP06-07.txt", "GP04-06.txt", "GP07-05.txt", "GP08-10.txt", "GP09-09.txt", "GP04-02.txt", "GP07-09.txt", "GP08-04.txt", "GP07-03.txt", "GP06-08.txt", "GP07-02.txt", "GP06-02.txt", "GP06-06.txt", "GP09-06.txt", "GP09-10.txt", "GP09-07.txt"}
	};
	Files (){}
};

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
		if(f.fail()) cout <<br<<"Erro ao abrir "<<filename;

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

		f.close();
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
		printv(m_pred, ini,"Predecessores nas maquinas:"); */}

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
		fill_heads(m_succ[op], heads, late_pred, cont + 1);}
public:
	Solution () {}
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
	void print_q(vector<unsigned int> &s, int lookat){
		cout<<br<<"Pilha:"<<br;
		for(int i=s.size()-1; i>=0; i--){
			cout<<"|"<<s[i]<<"|";
			if(i==lookat) cout<<"<";
			cout<<br;}}
	
	int calc_makespan(){
		cout << "calc_makespan()"<<br;
		makespan = 0;
		int temp_makespan = 0, op, op_succ, q_lookat = 0, q_pushed = 0;
		vector<unsigned int> degrees_in(n_ops + 1,0); //Graus de entrada por operacao
		vector<unsigned int> op_queue(n_ops); //Lista de operacoes
		vector<unsigned int> late_pred(n_ops + 1); //Predecessores mais tardios
		vector<unsigned int> heads(n_ops + 1, 0);

		for(int i=1; i<=n_ops; i++)
			if(m_pred[i] > j_pred[i]) late_pred[i] = m_pred[i];
			else late_pred[i] = j_pred[i];
			
		fill_heads(first, heads, late_pred, 1);	//printv(heads, 1, "heads"); printv(late_pred, 1, "late_pred");

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
		return makespan;
	}
};

int main(){
	/* Files f;
	string name;
	for(int i=0; i<f.path.size(); i++){
		cout<<br<<"###"<<f.path[i]<<"###"<<br;
		for(int j=0; j<f.names[i].size(); j++){
			string fp = f.path[i],
				   fn = f.names[i][j];
			Instance i(fp + fn);
			Solution s(i, RANDOM);
			cout<<fn<<":"<<s.makespan<<br;
		}
	} */

	//Funciona
	//Instance i("inst/taillard/tai04_04_03.txt");
	//Nao funciona
	Instance i("inst/taillard/tai04_04_01.txt"); i.print();
	Solution s(i, RANDOM);
	cout <<br<< s.calc_makespan()<<br;
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