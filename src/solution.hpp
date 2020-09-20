#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include <iostream>
#include <vector>
#include <utility>
#include <algorithm> //random_shuffle
#include <numeric>	 //iota
#include <assert.h>
#include "utilities.hpp"
#include "instance.hpp"

using namespace std;

class Solution
{
public:
	vector<unsigned> sJ, pJ, sM, pM;
	unsigned nM, nJ, nO, last, first;
	int makespan = 0;
	Instance instance;
	bool partial;

	unsigned lowerBound()
	{
		unsigned lower = makespan;

		for (unsigned i = 1; i < nO; i++)
		{
			unsigned sum = 0, op = i;
			if (pJ[i] == 0)
			{
				for (unsigned j = 1; j <= nJ; j++)
				{
					sum += instance.cost[op];
					op = sJ[op];
				}
			}
			if (sum > lower)
				lower = sum;
		}

		for (unsigned i = 1; i < nO; i++)
		{
			unsigned sum = 0, op = i;
			if (pM[i] == 0)
			{
				for (unsigned j = 1; j <= nJ; j++)
				{
					sum += instance.cost[op];
					op = sM[op];
				}
			}
			if (sum > lower)
				lower = sum;
		}
		return lower;
	}
	void error(string e)
	{
		cout << br << "ERRO: " << e << br;
		print();
		exit(0);
	}
	bool sameJob(unsigned op1, unsigned op2) { return instance.o_job[op1] == instance.o_job[op2]; }
	bool sameMach(unsigned op1, unsigned op2) { return instance.o_machine[op1] == instance.o_machine[op2]; }
	bool adjJob(unsigned op1, unsigned op2) { return pJ[op1] == op2 || pJ[op2] == op1; }
	bool adjMach(unsigned op1, unsigned op2) { return pM[op1] == op2 || pM[op2] == op1; }
	unsigned op(unsigned j, unsigned m)
	{
		//cout << "op(" << j << "," << m << ")\n";
		if (j == 0 || m == 0)
			return 0;
		assert(j >= 0);
		assert(m >= 0);
		return (j - 1) * nM + m;
	}
	vector<unsigned> critical()
	{
		assert(last <= nO);
		vector<unsigned> late_pred = getLatestPred(), p;
		unsigned op = last;

		while (op > 0)
		{
			p.push_back(op);
			op = late_pred[op];
		}
		reverse(p.begin(), p.end());
		return p;
	}
	void blocksJ(vector<unsigned> &begins, vector<unsigned> &ends)
	{
		vector<unsigned> p = critical(), blocksBegins, blocksEnds;
		assert(p.size() > 0);
		blocksBegins.push_back(0);
		for (unsigned i = 1; i < p.size(); i++)
		{
			if (!sameJob(p[i], p[i - 1]))
			{
				if (p[i] == first)
					blocksBegins.push_back(i + 1);
				else
					blocksBegins.push_back(i);
				blocksEnds.push_back(i - 1);
			}
		}
		blocksEnds.push_back(p.size() - 1);

		for (unsigned i = 0; i < blocksBegins.size(); i++)
		{
			if (blocksEnds[i] - blocksBegins[i] > 0)
			{
				begins.push_back(blocksBegins[i]);
				ends.push_back(blocksEnds[i]);
			}
		}
	}
	void blocksM(vector<unsigned> &begins, vector<unsigned> &ends)
	{
		vector<unsigned> p = critical(), blocksBegins, blocksEnds;
		assert(p.size() > 0);
		blocksBegins.push_back(0);
		for (unsigned i = 1; i < p.size(); i++)
		{
			if (!sameMach(p[i], p[i - 1]))
			{
				if (p[i] == first)
					blocksBegins.push_back(i + 1);
				else
					blocksBegins.push_back(i);
				blocksEnds.push_back(i - 1);
			}
		}
		blocksEnds.push_back(p.size() - 1);

		for (unsigned i = 0; i < blocksBegins.size(); i++)
		{
			if (blocksEnds[i] - blocksBegins[i] > 0)
			{
				begins.push_back(blocksBegins[i]);
				ends.push_back(blocksEnds[i]);
			}
		}
	}
	vector<unsigned> starters(vector<unsigned> predecessors)
	{
		vector<unsigned> s;

		for (unsigned i = 0; i < predecessors.size(); i++)
			if (predecessors[i] == DUMMY)
				s.push_back(i);

		return s;
	}
	vector<vector<unsigned>> wholeMachines()
	{
		vector<vector<unsigned>> machines;
		vector<unsigned> mStarters = starters(pM), block;
		//printv(mStarters, 0, "starters");
		unsigned op;

		for (unsigned starter : mStarters)
		{
			op = starter;
			block.clear();
			while (op != DUMMY)
			{
				block.push_back(op);
				op = sM[op];
			}
			if (block.size() > 0)
				machines.push_back(block);
		}
		return machines;
	}
	vector<vector<unsigned>> wholeJobs()
	{
		vector<vector<unsigned>> jobs;
		vector<unsigned> jStarters = starters(pJ), block;
		//printv(jStarters, 0, "starters");
		unsigned op;

		for (unsigned starter : jStarters)
		{
			op = starter;
			block.clear();
			while (op != DUMMY)
			{
				block.push_back(op);
				op = sJ[op];
			}
			if (block.size() > 0)
				jobs.push_back(block);
		}
		return jobs;
	}
	/////////////////////////////////////////////////////////////////////////
	//////// MÉTODOS DE INICIALIZAÇÃO E AUXILIARES //////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void initRandom()
	{
		vector<unsigned> j_ord(nJ + 1, 0),
			m_ord(nM + 1, 0);

		fillVecRandom(j_ord); //printv(j_ord,1,"Ordem randomizada - jobs");
		fillVecRandom(m_ord); //printv(m_ord,1,"Ordem randomizada - machines");
		fillOrd(j_ord, m_ord);
	}
	void evaluateResult(unsigned &tM, unsigned &bM, unsigned &tV, unsigned &bV)
	{ //testMakespan, bestMakespan, testValue, bestValue
		if (tM < bM)
		{
			bV = tV;
			bM = tM;
			//cout << tb << tb << tb << "so far: job " << tV << " and " << tM << " subs " << bM << br;
		}
	}
	void initGreedyJobs()
	{
		vector<unsigned> oJ(nJ + 2, 0), oM(nM + 1, 0), used(nJ + 1, 0);
		unsigned bestJob, bestMakespan, tempMakespan;
		fillVecRandom(oM);
		oM.push_back(0);

		oM = {0, 1, 2, 3, 4, 0};
		//printv(oM, 1, "chosen mach order");

		partial = true;
		for (int jobSlot = nJ; jobSlot > 0; jobSlot--)
		{
			//cout << br << "job slot " << jobSlot << br;
			bestMakespan = UMAX;
			for (unsigned testJob = 1; testJob <= nJ; testJob++)
			{
				if (used[testJob])
					continue;
				//cout << tb << "testing job " << testJob << br;

				oJ[jobSlot] = testJob;
				fillJobSlot(oJ, oM, testJob, jobSlot);
				first = op(testJob, oM[1]);
				//printPartial();
				tempMakespan = calcMakespanPartial();
				//cout << tb << tb << "found " << tempMakespan << br;
				evaluateResult(tempMakespan, bestMakespan, testJob, bestJob);
				resetJobSlot(oM, testJob);
			}
			oJ[jobSlot] = bestJob;
			used[bestJob]++;
			first = op(bestJob, oM[1]);
			fillJobSlot(oJ, oM, bestJob, jobSlot);
			//cout << tb << tb << "kept " << bestMakespan << " and mach " << bestJob << br;
		}
		partial = false;
		makespan = bestMakespan;
		fillOrd(oJ, oM);
	}
	void fillJobSlot(vector<unsigned> &oJ, vector<unsigned> &oM, unsigned job, unsigned jobSlot)
	{
		for (int mach = nM; mach > 0; mach--)
		{
			unsigned o = op(job, oM[mach]);
			pM[o] = op(oJ[jobSlot - 1], oM[mach]);
			pJ[o] = op(oJ[jobSlot], oM[mach - 1]);
			sM[o] = op(oJ[jobSlot + 1], oM[mach]);
			sJ[o] = op(oJ[jobSlot], oM[mach + 1]);
		}
	}
	void resetJobSlot(vector<unsigned> &oM, unsigned job)
	{
		for (int mach = 1; mach <= nJ; mach++)
		{
			unsigned o = op(oM[mach], job);
			pM[o] = 0;
			pJ[o] = 0;
			sM[o] = 0;
			sJ[o] = 0;
		}
	}

	void printClusterGraph(vector<unsigned> &oJ, vector<unsigned> &oM, unsigned machSlot)
	{
		cout << "digraph G {";
		for (int mach = machSlot; mach <= nM; mach++)
		{

			for (int job = 1; job < nJ; job++)
				cout << op(oJ[job], oM[mach]) << "->";

			cout << op(oJ[nJ], oM[mach]) << ";";
		}

		if (machSlot != nM)
			for (int job = 1; job <= nJ; job++)
			{

				for (int mach = machSlot; mach < nM; mach++)
					cout << op(oJ[job], oM[mach]) << "->";

				cout << op(oJ[job], oM[nM]) << ";";
			}
		cout << "}" << br;
	}
	void printMachClusterGraph(vector<unsigned> &oJ, vector<unsigned> &oM, unsigned machSlot)
	{
		cout << "digraph G {";
		for (int mach = machSlot; mach <= nM; mach++)
		{
			cout << "subgraph cluster" << mach << "{color=blue;";

			for (int job = 1; job < nJ; job++)
				cout << op(oJ[job], oM[mach]) << "->";

			cout << op(oJ[nJ], oM[mach]) << "; label = \"mach" << mach << "\";";
			cout << "}";
		}

		if (machSlot != nM)
			for (int job = 1; job <= nJ; job++)
			{

				for (int mach = machSlot; mach < nM; mach++)
					cout << op(oJ[job], oM[mach]) << "->";

				cout << op(oJ[job], oM[nM]) << ";";
			}
		cout << "}" << br;
	}
	void initGreedyMachines()
	{
		vector<unsigned> oJ(nJ + 1, 0), oM(nM + 2, 0), used(nM + 1, 0);
		unsigned bestMach, bestMakespan, tempMakespan;
		fillVecRandom(oJ);
		oJ.push_back(0);
		//oJ = {0, 1, 2, 3, 4, 0};
		//printv(oJ, 0, "chosen job order");

		partial = true;
		for (int machSlot = nM; machSlot > 0; machSlot--)
		{
			//cout << br << "mach slot " << machSlot << br;
			bestMakespan = UMAX;
			for (unsigned testMach = 1; testMach <= nM; testMach++)
			{
				if (used[testMach])
					continue;
				//cout << tb << "testing machine " << testMach << br;

				oM[machSlot] = testMach;
				fillMachSlot(oJ, oM, testMach, machSlot);
				unsigned o1 = op(oJ[1], testMach), o2 = op(oJ[2], testMach), o3 = op(oJ[3], testMach), o4 = op(oJ[4], testMach);
				//cout << tb << tb << "order " << o1 << ":" << instance.cost[o1] << sp << o2 << ":" << instance.cost[o2] << sp << o3 << ":" << instance.cost[o3] << sp << o4 << ":" << instance.cost[o4] << br;

				first = op(oJ[1], testMach);
				//printPartial();
				//printClusterGraph(oJ, oM, machSlot);
				tempMakespan = calcMakespanPartial();
				//cout << tb << tb << "found " << tempMakespan << br;

				evaluateResult(tempMakespan, bestMakespan, testMach, bestMach);
				resetMachSlot(oJ, testMach);
			}
			oM[machSlot] = bestMach;
			used[bestMach]++;
			first = op(oJ[1], bestMach);
			fillMachSlot(oJ, oM, bestMach, machSlot);
			//cout << tb << tb << "kept " << bestMakespan << " and mach " << bestMach << br;
			//printPartial();
		}
		partial = false;
		makespan = bestMakespan;
		fillOrd(oJ, oM);
	}
	void fillMachSlot(vector<unsigned> &oJ, vector<unsigned> &oM, unsigned mach, unsigned machSlot)
	{
		for (int job = 1; job <= nJ; job++)
		{
			unsigned o = op(oJ[job], mach);
			pM[o] = op(oJ[job - 1], oM[machSlot]);
			pJ[o] = op(oJ[job], oM[machSlot - 1]);
			sM[o] = op(oJ[job + 1], oM[machSlot]);
			sJ[o] = op(oJ[job], oM[machSlot + 1]);
		}
	}
	void resetMachSlot(vector<unsigned> &oJ, unsigned mach)
	{
		for (int job = 1; job <= nJ; job++)
		{
			unsigned o = op(oJ[job], mach);
			pM[o] = 0;
			pJ[o] = 0;
			sM[o] = 0;
			sJ[o] = 0;
		}
	}

	void initTest()
	{
		first = 1;
		nO = 6;
		nJ = 2;
		nM = 3;

		pJ = {0, 0, 1, 2, 0, 4, 5};
		sJ = {0, 2, 3, 0, 5, 6, 0};
		pM = {0, 0, 0, 0, 1, 0, 3};
		sM = {0, 4, 0, 6, 0, 2, 0};
		instance.cost = {0, 7, 3, 2, 5, 6, 7};

		/* pJ = {0, 0, 1, 2, 0, 4, 5, 0, 7, 8};
		sJ = {0, 2, 3, 0, 5, 6, 0, 8, 9, 0};
		pM = {0, 0, 0, 0, 1, 2, 3, 4, 5, 6};
		sM = {0, 4, 5, 6, 7, 8, 9, 0, 0, 0};
		instance.cost = {0, 1, 2, 3, 8, 5, 3, 1, 7, 2}; */
	}
	void fillOrd(vector<unsigned> &j_ord, vector<unsigned> &m_ord)
	{
		first = op(j_ord[1], m_ord[1]);

		for (int m = 1; m <= nM; m++)
			for (int j = 1; j <= nJ; j++)
			{
				pM[op(j_ord[j], m_ord[m])] = op(j_ord[j - 1], m_ord[m]);
				pJ[op(j_ord[j], m_ord[m])] = op(j_ord[j], m_ord[m - 1]);
				sM[op(j_ord[j], m_ord[m])] = op(j_ord[j + 1], m_ord[m]);
				sJ[op(j_ord[j], m_ord[m])] = op(j_ord[j], m_ord[m + 1]);
			}
	}
	void init(int method)
	{
		switch (method)
		{
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
	void fillVecRandom(vector<unsigned> &v)
	{
		iota(v.begin() + 1, v.end(), 1);
		srand(unsigned(time(NULL)));
		random_shuffle(v.begin() + 1, v.end());
	}

	/////////////////////////////////////////////////////////////////////////
	//////// AUXILIARES PARA CÁLCULO DE MAKESPAN ////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void findDegrees(vector<unsigned> &deg, vector<unsigned> &queue, unsigned &pushed)
	{
		if (partial)
		{
			for (unsigned i = 1; i <= nO; i++)
			{
				if (!pJ[i] && !pM[i] && i != first)
					continue;

				if (pJ[i] > 0)
					deg[i]++;
				if (pM[i] > 0)
					deg[i]++;
				if (deg[i] == 0)
					queue[pushed++] = i;
			}

			return;
		}

		for (unsigned i = 1; i <= nO; i++)
		{
			if (pJ[i] > 0)
				deg[i]++;
			if (pM[i] > 0)
				deg[i]++;
			if (deg[i] == 0)
				queue[pushed++] = i;
		}
	}

	vector<unsigned> getHeads(vector<unsigned> &deg, vector<unsigned> &queue, unsigned &pushed)
	{
		vector<unsigned> heads(nO + 1, 0), cicle;

		unsigned lookat = 0, op, newMax;

		findDegrees(deg, queue, pushed);
		//printv(deg, 1, "degrees");

		if (pushed == 0 && !partial)
			return cicle;

		while (lookat < pushed)
		{
			assert(lookat < queue.size());

			op = queue[lookat++];
			//assert(deg[op] == 0);
			assert(op != 0);
			newMax = heads[op] + instance.cost[op];

			for (unsigned succ : {sM[op], sJ[op]})
				if (succ != 0)
				{
					//assert(deg[succ] > 0 || (!deg[succ] && !partial));
					deg[succ]--;
					if (deg[succ] == 0)
					{
						if (pushed > nO && !partial)
							return cicle;
						queue[pushed++] = succ;
					}
					heads[succ] = max(heads[succ], newMax);
				}
		}
		//printl("nO",nO);printl("lookat",lookat);
		if (lookat != nO && !partial)
			return cicle;
		return heads;
	}

	vector<unsigned> getHeads()
	{
		unsigned pushed = 0;
		vector<unsigned> queue(nO), deg(nO + 1, 0);
		return getHeads(deg, queue, pushed);
	}

	/* 	vector<unsigned> getLatestPred(){
		vector<unsigned> late_pred(nO + 1, 0);
		for (int i = 1; i <= nO; i++){
			if (instance.cost[pM[i]] > instance.cost[pJ[i]])
				late_pred[i] = pM[i];
			else
				late_pred[i] = pJ[i];
		}
		return late_pred;
	} */
	vector<unsigned> getLatestPred()
	{
		vector<unsigned> late_pred(nO + 1, 0), heads = getHeads();
		for (int i = 1; i <= nO; i++)
		{
			if (heads[pM[i]] + instance.cost[pM[i]] > heads[pJ[i]] + instance.cost[pJ[i]])
				late_pred[i] = pM[i];
			else
				late_pred[i] = pJ[i];
		}
		return late_pred;
	}
	/////////////////////////////////////////////////////////////////////////
	//////// DEBUG //////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	unsigned checkSequence(unsigned op, vector<unsigned> &v)
	{
		if (op == DUMMY)
			return 0;
		return 1 + checkSequence(v[op], v);
	}
	void validSchedule()
	{
		unsigned noPredM = 0, noPredJ = 0, noSuccM = 0, noSuccJ = 0;

		// First tem predecessores?
		if (pM[first] != DUMMY || pJ[first] != DUMMY)
		{
			error("First possui predecessor(es)");
		}

		// DUMMY tem predecessor ou sucessor?
		if (pJ[0])
			error("j_pred[0] != 0");
		if (sJ[0])
			error("j_succ[0] != 0");
		if (pM[0])
			error("m_pred[0] != 0");
		if (pJ[0])
			error("m_succ[0] != 0");

		//
		for (int i = 1; i < nO + 1; i++)
		{
			if (i != first && pJ[i] == 0 && pM[i] == 0)
				error(to_string(i) + " sem predecessor!");
			//if(i!=tsirf && sJ[i]==0 && sM[i]==0) error(to_string(i) + " sem sucessor!");
			if (pJ[i] == i)
				error("j_pred[i] == i");
			if (sJ[i] == i)
				error("j_succ[i] == i");
			if (pJ[i] == 0)
			{
				if (checkSequence(i, sJ) != nJ)
				{
					printv(sJ, 1, "INCORRETO: j_succ");
					error("Sequencia comecada em " + to_string(i) + " incorreta!");
				}
				noPredJ++;
			}
			if (sJ[i] == 0)
			{
				if (checkSequence(i, pJ) != nJ)
				{
					printv(pJ, 1, "INCORRETO: j_pred");
					error("Sequencia terminada em " + to_string(i) + " incorreta!");
				}
				noSuccJ++;
			}
			if (pM[i] == i)
				error("m_pred[i] == i");
			if (sM[i] == i)
				error("m_succ[i] == i");
			if (pM[i] == 0)
			{
				if (checkSequence(i, sM) != nM)
				{
					printv(sM, 1, "INCORRETO: m_succ");
					error("Sequencia comecada em " + to_string(i) + " incorreta!");
				}
				noPredM++;
			}
			if (sM[i] == 0)
			{
				if (checkSequence(i, pM) != nJ)
				{
					printv(pM, 1, "INCORRETO: m_pred");
					error("Sequencia terminada em " + to_string(i) + " incorreta!");
				}
				noSuccM++;
			}
		}

		if (noPredJ > nJ)
		{
			printv(pJ, 1, "j_pred");
			error("Muitas ops sem predecessores em job");
		}

		if (noPredM > nM)
		{
			printv(pM, 1, "m_pred");
			error("Muitas ops sem predecessores em mach");
		}

		if (noSuccJ > nJ)
		{
			printv(sJ, 1, "j_succ");
			error("Muitas ops sem sucessores em job");
		}

		if (noSuccM > nM)
		{
			printv(sM, 1, "m_succ");
			error("Muitas ops sem sucessores em mach");
		}
	}
	void validOp(unsigned op)
	{
		if (!partial)
			assert(op <= nJ * nJ && op <= nM * nM);
		assert(op > 0);
	}
	void printG()
	{
		unsigned op = first, op1;

		cout << br;
		for (unsigned j = 1; j <= nJ; j++)
		{
			if (op == first)
				cout << "(" << op << ") ";
			else
				cout << " " << op << "  ";
			op1 = op;
			for (unsigned m = 2; m <= nM; m++)
			{
				op1 = sJ[op1];
				if (op == last)
					cout << "[" << op1 << "] ";
				else
					cout << op1 << "  ";
			}
			cout << br;
			op = sM[op];
		}
		cout << br;

		if (makespan == 0)
			calcMakespan();
		cout << br << "makespan = " << makespan << br;

		printv(instance.cost, 1, "Custo");
		printl("first:", first);
		printl("last:", last);
	}
	int calcMakespanPartial()
	{
		makespan = 0;
		unsigned temp_makespan = 0, op, lookat = 0, pushed = 0;
		vector<unsigned> queue(nO),
			deg(nO + 1, 0),
			heads = getHeads(deg, queue, pushed);
		vector<unsigned> latePred = getLatestPred();

		while (lookat < pushed)
		{
			op = queue[lookat++];
			temp_makespan = heads[op] + instance.cost[op];

			if (temp_makespan > makespan)
			{
				makespan = temp_makespan;
				last = op;
			}

			for (unsigned succ : {sJ[op], sM[op]})
				if (succ != DUMMY)
				{
					deg[succ]--;
					if (deg[succ] == 0)
						queue[pushed++] = succ;
					if (heads[succ] < temp_makespan)
					{
						heads[succ] = temp_makespan;
						latePred[succ] = op;
					}
				}
		}

		return makespan;
	}

	Solution() {}
	Solution(Instance i, int init_method)
	{
		instance = i;
		last = 0;
		makespan = 0;
		partial = false;
		nO = instance.n_ops;
		nJ = instance.n_jobs;
		nM = instance.n_machines;
		sJ.resize(nO + 1, 0);
		pJ.resize(nO + 1, 0);
		sM.resize(nO + 1, 0);
		pM.resize(nO + 1, 0);
		init(init_method);
	}
	int calcMakespan()
	{
		makespan = 0;
		unsigned temp_makespan = 0, op, lookat = 0, pushed = 0;
		vector<unsigned> queue(nO),
			deg(nO + 1, 0),
			heads = getHeads(deg, queue, pushed);
		//printv(heads, 0, "heads"); printl("size:",heads.size());

		if (heads.size() == 0)
			return 0;

		vector<unsigned> latePred = getLatestPred(); // latest predecessors AKA prev

		//printv(deg, 0, "deg"); printv(heads,1,"heads"); printv(latePred,1,"prev");
		while (lookat < pushed)
		{
			op = queue[lookat++];
			temp_makespan = heads[op] + instance.cost[op];

			if (temp_makespan > makespan)
			{
				makespan = temp_makespan;
				last = op;
			}

			for (unsigned succ : {sM[op], sJ[op]})
				if (succ != DUMMY)
				{
					deg[succ]--;
					if (deg[succ] == 0)
						queue[pushed++] = succ;
					if (heads[succ] < temp_makespan)
					{
						heads[succ] = temp_makespan;
						latePred[succ] = op;
					}
				}
		}

		return makespan;
	}
	int getMakespan() { return makespan; }
	Solution copySolution()
	{
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
	void print()
	{
		cout << br << "======================================" << br;
		printSchedule();
		printl("first", first);
		printl("last", last);
		printl("makespan", getMakespan());
		printGraph();
		cout << br << "======================================" << br;
	}
	void printPartial()
	{
		cout << br << "======================================" << br;
		printSchedule();
		printl("first", first);
		printl("last", last);
		printl("makespan", calcMakespanPartial());
		printGraph();
		//printO();
		cout << br << "======================================" << br;
	}
	void printSchedule()
	{
		int ini = 1;
		printv(sJ, ini, "Sucessores nos jobs:");
		printv(pJ, ini, "Predecessores nos jobs:");
		printv(sM, ini, "Sucessores nas maquinas:");
		printv(pM, ini, "Predecessores nas maquinas:");
		//printv(instance.cost, 1, "custo");
	}
	void printGraph()
	{
		cout << "digraph G {";
		cout << "\"" << getMakespan() << "\";";
		for (int i = 1; i <= nO; i++)
		{
			if (sJ[i])
			{
				cout << "edge[color=red];";
				cout << instance.cost[i] << "->" << instance.cost[sJ[i]] << ";";
			}
			if (sM[i])
			{
				cout << "edge[color=blue];";
				cout << instance.cost[i] << "->" << instance.cost[sM[i]] << ";";
			}
		}
		cout << "}" << br;
	}
	string edgeColor(unsigned op1, unsigned op2)
	{
		if (sameJob(op1, op2))
			return jobNode[instance.o_job[op1] - 1];
		return machNode[instance.o_machine[op1] - 1];
	}

	friend class Heuristics;
	friend class Neighbor;
	friend class Neighborhood;
};

#endif