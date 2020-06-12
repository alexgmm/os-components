#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm> //random_shuffle
#include <numeric>  //iota
#include <assert.h>
#include "instance.hpp"
#include "neighborhood.hpp"

using namespace std;

class Solution {
public:
	vector<unsigned> sJ, pJ, sM, pM;
	unsigned nM, nJ, nO, last, first;//, tsirf;
	int makespan = 0;
	Instance instance;
	bool partial;

	unsigned lowerBound(){
		unsigned lower = makespan;

		for(unsigned i=1; i<nO; i++){
			unsigned sum = 0, op = i;
			if(pJ[i]==0){
				for(unsigned j=1; j<=nJ; j++){
					sum+= instance.cost[op];
					op = sJ[op];
				}
			}
			if(sum > lower)	lower = sum;
		}

		for(unsigned i=1; i<nO; i++){
			unsigned sum = 0, op = i;
			if(pM[i]==0){
				for(unsigned j=1; j<=nJ; j++){
					sum+= instance.cost[op];
					op = sM[op];
				}
			}
			if(sum > lower) lower = sum;
		}
		return lower;
	}
	void error(string e){ 
		cout << br << "ERRO: " << e << br;
		print();
		exit(0); 
	}
	bool sameJob(unsigned op1, unsigned op2){ return instance.o_job[op1] == instance.o_job[op2]; }
	bool sameMach(unsigned op1, unsigned op2){ return instance.o_machine[op1] == instance.o_machine[op2]; }
	bool adjJob (unsigned op1, unsigned op2){ return pJ[op1] == op2 || pJ[op2] == op1; }
	bool adjMach(unsigned op1, unsigned op2){ return pM[op1] == op2 || pM[op2] == op1; }
	unsigned op(unsigned j, unsigned m){
		if(j==0 || m==0) return 0;
		assert(j>=0);
		assert(m>=0);
		return (j-1)*nM + m;	
	}
	vector<unsigned> critical(){
		assert(last <= nO);
		vector<unsigned> late_pred = getLatestPred(), p;
		unsigned op = last;

		while(op > 0){
			p.push_back(op);
			op = late_pred[op];
		}
		reverse(p.begin(),p.end());
		return p;	
	}
	void blocksJ(vector<unsigned> &begins, vector<unsigned> &ends){
		vector<unsigned> p = critical();
		int block_size = 0;
        begins.push_back(0);
        for(int i=0; i<p.size()-1; i++){
			if (instance.o_job[p[i]] != instance.o_job[p[i + 1]]){
				if (block_size > 1){
					ends.push_back(i);
					begins.push_back(i + 1);
				}
				block_size = 0;
			}
			else
				block_size++;
        }
        ends.push_back(p.size()-1);
		if(begins[begins.size()-1] == ends[ends.size()-1]){
			begins.pop_back();
			ends.pop_back();
		}
		if(begins[0] - ends[0] < 2){
			reverse(begins.begin(), begins.end());
			begins.pop_back();
			reverse(begins.begin(), begins.end());
			reverse(ends.begin(), ends.end());
			ends.pop_back();
			reverse(ends.begin(), ends.end());
		}
	}
    void blocksM(vector<unsigned> &begins, vector<unsigned> &ends){
		vector<unsigned> p = critical();
        int block_size = 0;
        begins.push_back(0);
        for(int i=0; i<p.size()-1; i++){
			if (instance.o_machine[p[i]] != instance.o_machine[p[i + 1]]){
				if (block_size > 1){
					ends.push_back(i);
					begins.push_back(i + 1);
				}
				block_size = 0;
			}
			else
				block_size++;
        }
        ends.push_back(p.size()-1);
		if(begins[begins.size()-1] == ends[ends.size()-1]){
			begins.pop_back();
			ends.pop_back();
		}
		if(begins[0] - ends[0] < 2){
			reverse(begins.begin(), begins.end());
			begins.pop_back();
			reverse(begins.begin(), begins.end());
			reverse(ends.begin(), ends.end());
			ends.pop_back();
			reverse(ends.begin(), ends.end());
		}	
	}
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
			bestMakespan = UMAX;
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
				tempMakespan = calcMakespanPartial();
				if(tempMakespan < bestMakespan){
					bestJob = j; 
					bestMakespan = tempMakespan; 
				}
			}
			oJ[i] = bestJob;
			used[bestJob]++;
		}
		makespan = bestMakespan;
		fillOrd(oJ, oM);	
	}
	void initGreedyMachines(){
		vector<unsigned> oJ(nJ+1, 0), oM(nM+1, 0), used(nM+1, 0);
		unsigned bestMach, bestMakespan, tempMakespan;
		fillVecRandom(oJ);
		for(int i = nM; i>0; i--){
			bestMakespan = UMAX;
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
				tempMakespan = calcMakespanPartial();
				if(tempMakespan < bestMakespan){ 
					bestMach = m; 
					bestMakespan = tempMakespan;
				}
			}
			oM[i] = bestMach;
			used[bestMach]++;
		}
		makespan = bestMakespan;
		fillOrd(oJ, oM);	
	}
	void initTest(){
		first = 1;

		pJ = { 0, 0, 1, 2, 0, 4, 5, 0, 7, 8 };
		sJ = { 0, 2, 3, 0, 5, 6, 0, 8, 9, 0 };
		pM = { 0, 0, 0, 0, 1, 2, 3, 4, 5, 6 };
		sM = { 0, 4, 5, 6, 7, 8, 9, 0, 0, 0 };
		instance.cost = { 0, 5, 1, 1, 5, 1, 1, 5, 5, 5 };
	}
	void fillOrd(vector<unsigned> &j_ord, vector<unsigned> &m_ord){
		first = op(j_ord[1], m_ord[1]);
		//tsirf = op(j_ord[j_ord.size()-1], m_ord[m_ord.size()-1]);

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
				partial = true;
				initGreedyJobs();
				partial = false;
				break;
			case GREEDY_MACHINES:
				partial = true;
				initGreedyMachines();
				partial = false;
				break;
			default:
				initTest();
				break;
		}
		makespan = calcMakespan();
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

			return;
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
			if(instance.cost[pM[i]] > instance.cost[pJ[i]]) 
				late_pred[i] = pM[i];
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

	/////////////////////////////////////////////////////////////////////////
	//////// DEBUG //////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	unsigned checkSequence(unsigned op, vector<unsigned> &v){
		if(op == DUMMY) return 0;
		return 1 + checkSequence(v[op], v);
	}
	void validSchedule(){
		unsigned noPredM = 0, noPredJ = 0, noSuccM = 0, noSuccJ = 0;

		// First tem predecessores?
		if(pM[first] != DUMMY || pJ[first] != DUMMY){
			error("First possui predecessor(es)");
		}

		// DUMMY tem predecessor ou sucessor?
		if(pJ[0]) error("j_pred[0] != 0");
		if(sJ[0]) error("j_succ[0] != 0");
		if(pM[0]) error("m_pred[0] != 0");
		if(pJ[0]) error("m_succ[0] != 0");



		// 
		for(int i = 1; i < nO + 1; i++){
			if(i!=first && pJ[i]==0 && pM[i]==0) error(to_string(i) + " sem predecessor!");
			//if(i!=tsirf && sJ[i]==0 && sM[i]==0) error(to_string(i) + " sem sucessor!");
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
	/* void print(){
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

		if(makespan == 0) calcMakespan();
		cout << br << "makespan = " << makespan << br;

		printv(instance.cost, 1, "Custo");
		printl("first:",first);
		printl("last:",last); 
	} */
	int calcMakespanPartial(){
		makespan = 0;
		unsigned temp_makespan = 0, op, lookat = 0, pushed = 0;
		vector<unsigned> queue(nO),
						 deg(nO + 1,0), 
						 heads = getHeads(), 
						 latePred = getLatestPred(); 

        findDegrees(deg, queue, pushed);
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
					if(deg[succ] == 0)	queue[pushed++] = succ;
					if(heads[succ] < temp_makespan){
						heads[succ] = temp_makespan;
						latePred[succ] = op;
					}
				}
			}

		}

		return makespan;	
	}

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

        findDegrees(deg, queue, pushed);// printv(deg, 0, "deg"); printv(heads,1,"heads"); printv(latePred,1,"prev");

		// Ciclo
		if(pushed == 0) return 0;

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
					if(deg[succ] == 0) {
						//if(pushed >= queue.size()-1) return 0;
						queue[pushed++] = succ;
					}
					if(heads[succ] < temp_makespan){
						heads[succ] = temp_makespan;
						latePred[succ] = op;
					}
				}
			}

		}

		if(makespan < lowerBound()) return 0;
			/* error("makespan(" + to_string(makespan) + ") < lowerBound(" + to_string(lowerBound()) + ")"); */

		return makespan;	
	}
	int getMakespan(){ return makespan; }
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
		s.makespan = calcMakespan();
		return s;
	}
	void print(){
		cout << br << "======================================" << br;
		int ini = 1;
		printv(sJ, ini,"Sucessores nos jobs:");
		printv(pJ, ini,"Predecessores nos jobs:");
		printv(sM, ini,"Sucessores nas maquinas:");
		printv(pM, ini,"Predecessores nas maquinas:");
		printv(instance.cost,1,"custo");
		printl("first",first);
		printl("last",last);
		printl("makespan",makespan);printO();
		cout << br << "======================================" << br;
	}
	void printO(){
		vector<unsigned> p = critical();
		cout << "op J M\n";
		for(unsigned op : p)
			cout << op << " " << instance.o_job[op] << " " << instance.o_machine[op] << br;
	}
	friend class Heuristics;
	friend class Neighbor;
	friend class Neighborhood;
};
class Neighbor{
	Solution neighbor;
	unsigned value;
	bool legal;
public:
	Neighbor(){}
	Neighbor(Solution n, unsigned v, bool l){
		if(v==0){
			neighbor.makespan = UMAX;
			value = UMAX;
		}else {
			value = v;
			neighbor.makespan = v;
		}
		neighbor = n;
		legal = l;
	}
	Solution getSolution(){ return neighbor; }
	unsigned getValue(){ return value; }
	bool isLegal(){ return legal; }
};
class Neighborhood {
    Solution sol;
    Solution previous;
	vector<vector<unsigned>> tabu;

	/////////////////////////////////////////////////////////////////////////
	//////// GERAÇÃO DE VIZINHO /////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void swapSucc(unsigned op){
		if(randint(0,1)==SWAP_J)
			swapJ(op, sol.sJ[op]);
		else
			swapM(op, sol.sM[op]);
	}
	void swapPred(unsigned op){
		if(randint(0,1)==SWAP_J)
			swapJ(op, sol.pJ[op]);
		else
			swapM(op, sol.pM[op]);
	}

	/////////////////////////////////////////////////////////////////////////
	//////// GERAÇÃO DE VIZINHANÇA COMPLETA /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	bool legal(unsigned op1, unsigned op2){
		if(CURRENT_ITER - TABU_DURATION > tabu[op1][op2] || tabu[op1][op2] == 0)
			return true;
		return false;
	}
	void traverseSwapJ(vector<Neighbor> &neighborhood, unsigned op, vector<bool> &visited){ 
		//cout << br << "traverseSwapJ(" << op << ")" << br;
		//sol.print();
		bool l = swapJ(op, sol.sJ[op]);
		//sol.print();

		Neighbor n(sol, sol.calcMakespan(), l);
		neighborhood.push_back(n);
		restore();

		visited[op] = true;

		if(!visited[sol.sJ[op]] && !isLast(sol.sJ[op]) && sol.sJ[sol.sJ[op]]) 
			traverseSwapJ(neighborhood, sol.sJ[op], visited);
		if(!visited[sol.sM[op]] && !isLast(sol.sM[op]) && sol.sJ[sol.sM[op]]) 
			traverseSwapJ(neighborhood, sol.sM[op], visited);
	}
	void traverseSwapM(vector<Neighbor> &neighborhood, unsigned op, vector<bool> &visited){
		if(op != sol.first){
			//cout << br << "traverseSwapJ(" << op << ")" << br;
			//sol.print();
			bool l = swapM(op, sol.sM[op]);
			//sol.print();
			Neighbor n(sol, sol.calcMakespan(), l);
			neighborhood.push_back(n);

			restore();

			visited[op] = true;
		}
			
		if(!visited[sol.sJ[op]] && !isLast(sol.sJ[op]) && sol.sM[sol.sJ[op]]) 
			traverseSwapM(neighborhood, sol.sJ[op], visited);
		if(!visited[sol.sM[op]] && !isLast(sol.sM[op]) && sol.sM[sol.sM[op]]) 
			traverseSwapM(neighborhood, sol.sM[op], visited);
	}
	vector<Neighbor> swapAllNeighbors(){
        vector<Neighbor> neighborhood;
		vector<bool> visited(sol.nO + 1, false), visited2(sol.nO + 1, false);
		visited[0] = true;
		visited2[0] = true;
		
		traverseSwapJ(neighborhood, sol.first, visited);
		traverseSwapM(neighborhood, sol.first, visited2);
        
        return neighborhood;
    }
	void swapJAdd(unsigned op1, unsigned op2, vector<Neighbor> &neighborhood){
		bool l = swapJ(op1, op2);
		Neighbor n(sol, sol.calcMakespan(), l);
		neighborhood.push_back(n);
		restore();
	}
	void swapMAdd(unsigned op1, unsigned op2, vector<Neighbor> &neighborhood){
		bool l = swapM(op1, op2);
		Neighbor n(sol, sol.calcMakespan(), l);
		neighborhood.push_back(n);
		restore();
	}
	vector<Neighbor> swapCriticalNeighbors(){
		vector<Neighbor> neighborhood;
		vector<unsigned> p = sol.critical();

		for(unsigned i=0; i<p.size()-1; i++){
			if(sol.sameJob(p[i], p[i+1])) 
				swapJAdd(p[i],p[i+1], neighborhood);
			if(sol.sameMach(p[i], p[i+1])) 
				swapMAdd(p[i],p[i+1], neighborhood);
		}

		return neighborhood;
	}
	bool isN5EdgeJ(unsigned index1, unsigned index2, vector<unsigned> &p){
		if(p[index1] == sol.first || p[index2 == sol.last]) return false;
		if(sol.sameJob(p[index1],p[index2]) && (!sol.sameJob(p[index1],p[index1-1]) || !sol.sameJob(p[index2],p[index2+1])))
			return true;
		return false;
	}
	bool isN5EdgeM(unsigned index1, unsigned index2, vector<unsigned> &p){
		if(p[index1] == sol.first || p[index2 == sol.last]) return false;
		if(sol.sameMach(p[index1],p[index2]) && (!sol.sameMach(p[index1],p[index1-1]) || !sol.sameMach(p[index2],p[index2+1])))
			return true;
		return false;
	}
	vector<Neighbor> swapCriticalEdgeNeighbors(){
		vector<Neighbor> neighborhood;
		vector<unsigned> p = sol.critical();

		for(unsigned i=1; i<p.size()-2; i++){
			if(isN5EdgeJ(i,i+1,p)) 
				swapJAdd(p[i],p[i+1], neighborhood);
			if(isN5EdgeM(i,i+1,p))
				swapMAdd(p[i],p[i+1], neighborhood);
		}

		return neighborhood;
	}

public:
    Neighborhood(){}
    Neighborhood(Solution s){
		setSolution(s);
		tabu.resize(s.nO + 1);
		for(unsigned i=0; i<= s.nO; i++)
			tabu[i].resize(s.nO + 1, 0);
	}

    void restore(){ sol = previous.copySolution(); }
	void setSolution(Solution s){ sol = s.copySolution(); previous = s.copySolution(); }

	/////////////////////////////////////////////////////////////////////////
	//////// OPERADORES DE VIZINHANÇA ///////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////


	Solution getNeighbor(int oper){
		vector<Neighbor> neighbors;

		switch (oper){
		case SWAP_ALL:
			neighbors = swapAllNeighbors();
			break;
		case SWAP_CRITICAL:
			neighbors = swapCriticalNeighbors();
			break;
		case SWAP_CRITICAL_EDGE:
			neighbors = swapCriticalEdgeNeighbors();
			break;
		default:
			break;
		}
		return neighbors[randint(0,neighbors.size()-1)].getSolution();
	}
	bool isLast(unsigned op){ return sol.sJ[op] == DUMMY && sol.sM[op] == DUMMY; }
    vector<Neighbor> getNeighborhood(int oper){
        switch (oper){
        case SWAP_ALL:
            return swapAllNeighbors();
            break;
		case SWAP_CRITICAL:
			return swapCriticalNeighbors();
			break;
		case SWAP_CRITICAL_EDGE:
			return swapCriticalEdgeNeighbors();
			break;
        default:
            break;
        }
    }

    static Solution getNeighbor(unsigned oper, Solution& s){
		Solution copied = s.copySolution(), selected;
		vector<Neighbor> neighbors;
		Neighborhood n = Neighborhood(copied);

		switch (oper){
		case SWAP_ALL:
			neighbors = n.swapAllNeighbors();
			break;
		case SWAP_CRITICAL:
			neighbors = n.swapCriticalNeighbors();
			break;
		case SWAP_CRITICAL_EDGE:
			neighbors = n.swapCriticalEdgeNeighbors();
			break;
		default:
			break;
		}
		bool invalid = true;
		while(invalid){
			selected = neighbors[randint(0,neighbors.size()-1)].getSolution();
			invalid = selected.calcMakespan() < selected.lowerBound();
		}
		return selected;
	}

    Solution getSolution(){ return sol; }

	bool swapJ(unsigned op1, unsigned op2){//cout << br << "swapJ(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0);
		assert(op2 > 0);
		assert(op1 != op2);//printl("op1 j",sol.instance.o_job[op1]);printl("op2 j",sol.instance.o_job[op2]);
		assert(sol.adjJob(op1,op2));

		if(op1 == sol.first) sol.first = op2;
		//if(op2 == sol.tsirf) sol.tsirf = op1;

		if(sol.sJ[op2] == op1){
			unsigned o = op1;
			op1 = op2;
			op2 = o;
		}

		unsigned jp=sol.pJ[op1], js=sol.sJ[op2]; 
			
		if(jp) sol.sJ[jp] = op2;
		if(js) sol.pJ[js] = op1;
		
		sol.sJ[op1] = js; sol.pJ[op1] = op2;
		sol.pJ[op2] = jp; sol.sJ[op2] = op1;
		
		//sol.validSchedule();
		sol.calcMakespan();
		//sol.print();
		if(legal(op1, op2)){
			tabu[op1][op2] = CURRENT_ITER;
			return true;
		}
		return false;
	}
	bool swapM(unsigned op1, unsigned op2){//cout << br << "swapM(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0); 
		assert(op2 > 0);
		assert(op1 != op2); //printl("op1 m",sol.instance.o_machine[op1]);printl("op2 m",sol.instance.o_machine[op2]);
		assert(sol.adjMach(op1,op2));

		if(op1 == sol.first) sol.first = op2;
		//if(op2 == sol.tsirf) sol.tsirf = op1;

		if(sol.sM[op2] == op1){
			unsigned o = op1;
			op1 = op2;
			op2 = o;
		}

		unsigned mp=sol.pM[op1], ms=sol.sM[op2]; 
			
		if(mp) sol.sM[mp] = op2;
		if(ms) sol.pM[ms] = op1;
		
		sol.sM[op1] = ms; sol.pM[op1] = op2;
		sol.pM[op2] = mp; sol.sM[op2] = op1;

		//sol.validSchedule();
		sol.calcMakespan();
		//sol.print();
		if(legal(op1, op2)){
			tabu[op1][op2] = CURRENT_ITER;
			return true;
		}
		return false;
	}
};


#endif