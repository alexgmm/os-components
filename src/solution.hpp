#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>
#include <vector>
#include <algorithm> //random_shuffle
#include <numeric> //iota
#include <assert.h>
#include "instance.hpp"
#include "neighborhood.hpp"

using namespace std;

class Solution {
public:
	vector<unsigned> sJ, pJ, sM, pM;
	unsigned makespan = 0, nM, nJ, nO, last, first;
	Instance instance;
	bool partial;

	bool sameJob(unsigned op1, unsigned op2){ return instance.o_job[op1] == instance.o_job[op2]; }
	bool sameMach(unsigned op1, unsigned op2){ return instance.o_machine[op1] == instance.o_machine[op2]; }
	unsigned op(unsigned j, unsigned m){
		if(j==0 || m==0) return 0;
		assert(j>=0);
		assert(m>=0);
		return (j-1)*nM + m;	
	}
	bool adjJob (unsigned op1, unsigned op2){ return pJ[op1] == op2 || pJ[op2] == op1; }
	bool adjMach(unsigned op1, unsigned op2){ return pM[op1] == op2 || pM[op2] == op1; }
	
	/////////////////////////////////////////////////////////////////////////
	//////// MÉTODOS DE INICIALIZAÇÃO E AUXILIARES //////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void initRandom(){
		vector<unsigned> j_ord(nJ+1, 0),
						 m_ord(nM+1, 0);
							 
		fillVecRandom(j_ord);//printv(j_ord,1,"Ordem randomizada - jobs");
		fillVecRandom(m_ord);//printv(m_ord,1,"Ordem randomizada - machines");
		fillOrd(j_ord, m_ord);
	}

	void initGreedyJobs(){
		vector<unsigned> oJ(nJ+1,0), oM(nM+1,0), used(nJ+1,0);
		unsigned bestJob, bestMakespan, tempMakespan;
		fillVecRandom(oM);
		for(int i = nJ; i>0; i--){
			partial = true;
			bestMakespan = 999999;
			for(int j = nJ; j>=1; j--){ 
				if(used[j]) continue;

				oJ[i] = j;
				for(int m = nM; m>0; m--){
					if(oJ[j-1]) pM[op(oJ[j],oM[m])] = op(oJ[j-1],oM[m]);
					if(oM[m-1]) pJ[op(oJ[j],oM[m])] = op(oJ[j],oM[m-1]);
					if(j<nJ) sM[op(oJ[j],oM[m])] = op(oJ[j+1],oM[m]);
					if(m<nM) sJ[op(oJ[j],oM[m])] = op(oJ[j],oM[m+1]);
				}
				first = op(j,oM[1]);
				tempMakespan = calcMakespan();
				if(tempMakespan < bestMakespan){
					bestJob = j; 
					bestMakespan = tempMakespan; 
				}
			}
			oJ[i] = bestJob;
			used[bestJob]++;
		}
		partial = false;
		makespan = bestMakespan;
		fillOrd(oJ, oM);	
	}

	void initGreedyMachines(){
		vector<unsigned> oJ(nJ+1, 0), oM(nM+1, 0), used(nM+1, 0);
		unsigned bestMach, bestMakespan, tempMakespan;
		fillVecRandom(oJ);
		for(int i = nM; i>0; i--){
			partial = true;
			bestMakespan = 999999;
			for(int m = nM; m>=1; m--){	
				if(used[m]) continue;

				oM[i] = m;
				for(int j = nJ; j>0; j--){
					if(oJ[j-1]) pM[op(oJ[j],oM[m])] = op(oJ[j-1],oM[m]);
					if(oM[m-1]) pJ[op(oJ[j],oM[m])] = op(oJ[j],oM[m-1]);
					if(j<nJ) sM[op(oJ[j],oM[m])] = op(oJ[j+1],oM[m]);
					if(m<nM) sJ[op(oJ[j],oM[m])] = op(oJ[j],oM[m+1]);
				}
				first = op(oJ[1],m);
				tempMakespan = calcMakespan();
				if(tempMakespan < bestMakespan){ 
					bestMach = m; 
					bestMakespan = tempMakespan;
				}
			}
			oM[i] = bestMach;
			used[bestMach]++;
		}
		partial = false;
		makespan = bestMakespan;
		fillOrd(oJ, oM);	
	}
	
	void initTest(){
		first = 2;

		pJ = { 0, 4, 0, 5, 0, 2, 1, 0, 7, 8 };
		sJ = { 0, 6, 5, 0, 1, 3, 0, 8, 9, 0 };
		pM = { 0, 5, 0, 0, 2, 0, 3, 4, 1, 6 };
		sM = { 0, 8, 4, 6, 7, 1, 9, 0, 0, 0 };
	}
	void fillOrd(vector<unsigned> &j_ord, vector<unsigned> &m_ord){
		first = op(j_ord[1], m_ord[1]);

		for(int m=1; m<=nM; m++){
			for(int j=1; j<=nJ; j++){ 
				if(j>1) pM[op(j_ord[j],m_ord[m])] = op(j_ord[j-1],m_ord[m]);
				if(m>1) pJ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m-1]);
				if(j<nJ) sM[op(j_ord[j],m_ord[m])] = op(j_ord[j+1],m_ord[m]);
				if(m<nM) sJ[op(j_ord[j],m_ord[m])] = op(j_ord[j],m_ord[m+1]);
			}
		}
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
				initGreedyMachines();
				break;
			default:
				initTest();
				break;
		}
	}
	void fillVecRandom(vector<unsigned> &v){
		iota(v.begin()+1, v.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(v.begin()+1,v.end());
	}

	/////////////////////////////////////////////////////////////////////////
	//////// AUXILIARES PARA CÁLCULO DE MAKESPAN ////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void findDegrees(vector<unsigned> &deg, vector<unsigned> &queue, unsigned &pushed){
		if(partial){
			for(unsigned i = 1; i <= nO; i++){
				if(!pJ[i] && !pM[i] && i!= first) continue;
				 
				if(pJ[i]>0) deg[i]++;
				if(pM[i]>0) deg[i]++;
				if(deg[i]==0){
					queue[pushed++] = i;
				}
			}
		}

		for(unsigned i = 1; i <= nO; i++){
			if(pJ[i]>0) deg[i]++;
			if(pM[i]>0) deg[i]++;
			if(deg[i]==0){
				queue[pushed++] = i;
			}
		}
	}
	void fillHeads(unsigned op, vector<unsigned>& heads, int cont, vector<unsigned>& visited){
		validOp(op);

		if(op != first)	
			heads[op] = max(heads[pJ[op]] + instance.cost[pJ[op]],
							heads[pM[op]] + instance.cost[pM[op]]);
	
		if(heads[op] > heads[last]) last = op;

        visited[op] = 1;

		if(cont < nO){
			if(sJ[op] != DUMMY && visited[sJ[op]] == 0)
				fillHeads(sJ[op], heads, cont + 1, visited);
			if(sM[op] != DUMMY && visited[sM[op]] == 0)
				fillHeads(sM[op], heads, cont + 1, visited);
		}
	}
	vector<unsigned> getLatestPred(){
		vector<unsigned> late_pred(nO+1, 0);
		for(int i=1; i<=nO; i++){
			if(instance.cost[pM[i]] > instance.cost[pJ[i]]) late_pred[i] = pM[i];
			else late_pred[i] = pJ[i];
		}
		return late_pred;	
	}
	vector<unsigned> getHeads(){
		vector<unsigned> heads(nO + 1, 0), visited(nO + 1, 0);
		if(first < 1 || !partial && first > nO)
			error("First invalido: " + to_string(first));
		fillHeads(first, heads, 1, visited); //printv(heads, 1, "heads");
		return heads;	
	}
public:
	Solution () {}
	Solution(Instance i, int init_method){
		instance = i;
		last = 0;
		makespan = 0;
		partial = false;
		nO = instance.n_ops;
		nJ = instance.n_jobs;
		nM = instance.n_machines;
		sJ.resize(nO+1,0);
		pJ.resize(nO+1,0);
		sM.resize(nO+1,0);
		pM.resize(nO+1,0);
		init(init_method);
	}
	int calcMakespan(){
		makespan = 0;
		unsigned temp_makespan = 0, op, lookat = 0, pushed = 0;
		vector<unsigned> queue(nO),
						 deg(nO + 1,0), 
						 heads = getHeads(), 
						 latePred = getLatestPred(); 

        findDegrees(deg, queue, pushed);

		// Ciclo
		if(pushed == 0) return -1;

		while(lookat < pushed){
			op = queue[lookat++];
			temp_makespan = heads[op] + instance.cost[op];

			if(temp_makespan > makespan){
				makespan = temp_makespan;
				last = op;
			}

			for(unsigned succ : { sJ[op], sM[op] }){
				if (succ != DUMMY){
					deg[succ]--;
					if(deg[succ] == 0) queue[pushed++] = succ;
					if(heads[succ] < temp_makespan){
						heads[succ] = temp_makespan;
						latePred[succ] = op;
					}
				}
			}

		}
		return makespan;	
	}
	Solution copySolution(){
		Solution s;
		s.nO = nO;
		s.nJ = nJ;
		s.nM = nM;
		s.pJ = pJ;
		s.sJ = sJ;
		s.pM = pM;
		s.sM = sM;
		s.first = first;
		s.last = last;
		s.instance = instance;
		return s;
	}
	Solution getNeighbor(unsigned oper){
		Solution c = copySolution();
		Neighborhood n = Neighborhood(c);

		switch (oper){
		case SWAP_ALL:
			n.swapAll();
			break;
		case SWAP_CRITICAL:
			n.swapCritical();
			break;
		case SWAP_CRITICAL_EDGE:
			n.swapCriticalEdge();
			break;
		default:
			break;
		}
		return n.getSolution();
	}
	/////////////////////////////////////////////////////////////////////////
	//////// DEBUG //////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void print_o(){
		cout << br << "======================================" << br;
		int ini = 1;
		printv(sJ, ini,"Sucessores nos jobs:");
		printv(pJ, ini,"Predecessores nos jobs:");
		printv(sM, ini,"Sucessores nas maquinas:");
		printv(pM, ini,"Predecessores nas maquinas:");
		printv(instance.cost,1,"custo");
		printl("first",first);
		cout << br << "======================================" << br;
	}

	unsigned checkSequence(unsigned op, vector<unsigned> &v){
		if(op == DUMMY) return 0;
		return 1 + checkSequence(v[op], v);
	}

	void validSchedule(){
		unsigned noPredM = 0, noPredJ = 0, noSuccM = 0, noSuccJ = 0;

		// First tem predecessores?
		if(pM[first] != DUMMY || pJ[first] != DUMMY){
			print_o();
			error("First possui predecessor(es)");
		}

		// DUMMY tem predecessor ou sucessor?
		if(pJ[0]) error("j_pred[0] != 0");
		if(sJ[0]) error("j_succ[0] != 0");
		if(pM[0]) error("m_pred[0] != 0");
		if(pJ[0]) error("m_succ[0] != 0");

		// 
		for(int i = 1; i < nO + 1; i++){
			if(pJ[i] == i) error("j_pred[i] == i");
			if(sJ[i] == i) error("j_succ[i] == i");
			if(pJ[i] == 0) {
				if(checkSequence(i, sJ) != nJ){
					printv(sJ,1,"INCORRETO: j_succ");
					error("Sequencia comecada em " + to_string(i) + " incorreta!");
				}
				noPredJ++;
			}
			if(sJ[i] == 0) {
				if(checkSequence(i, pJ) != nJ){
					printv(pJ,1,"INCORRETO: j_pred");
					error("Sequencia terminada em " + to_string(i) + " incorreta!");
				}
				noSuccJ++;
			}
			if(pM[i] == i) error("m_pred[i] == i");
			if(sM[i] == i) error("m_succ[i] == i");
			if(pM[i] == 0) {
				if(checkSequence(i, sM) != nM){
					printv(sM,1,"INCORRETO: m_succ");
					error("Sequencia comecada em " + to_string(i) + " incorreta!");
				}
				noPredM++;
			}
			if(sM[i] == 0){
				if(checkSequence(i, pM) != nJ){
					printv(pM,1,"INCORRETO: m_pred");
					error("Sequencia terminada em " + to_string(i) + " incorreta!");
				}
				noSuccM++;
			}

		}

		if(noPredJ > nJ){
			printv(pJ, 1, "j_pred");
			error("Muitas ops sem predecessores em job");
		}

		if(noPredM > nM){
			printv(pM, 1, "m_pred");
			error("Muitas ops sem predecessores em mach");
		}

		if(noSuccJ > nJ){
			printv(sJ, 1, "j_succ");
			error("Muitas ops sem sucessores em job");
		}

		if(noSuccM > nM){
			printv(sM, 1, "m_succ");
			error("Muitas ops sem sucessores em mach");
		}
	}

	void validOp(unsigned op){
		if(!partial)
			assert(op <= nJ*nJ && op <= nM*nM); 
		assert( op > 0 ); 
	}

	void print(){
		unsigned op = first, op1;

		cout << br;
		for(unsigned j=1; j<=nJ; j++){
			if(op == first) cout << "(" << op << ") ";
			else cout << " " << op << "  ";
			op1 = op;
			for(unsigned m=2; m<=nM; m++){
				op1 = sJ[op1];
				if(op==last) cout << "[" << op1 << "] ";
				else cout << op1 << "  ";
			}
			cout << br;
			op = sM[op];
		}
		cout << br;

		/* if(makespan == 0) calcMakespan();
		cout << br << "makespan = " << makespan << br;

		printv(instance.cost, 1, "Custo");
		printl("first:",first);
		printl("last:",last); */
	}
	friend class Heuristics;
};

#endif