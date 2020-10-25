#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "utilities.hpp"
#include "instance.hpp"
#include "printer.hpp"

using namespace std;

class Solution
{
public:
	vector<unsigned> sJ, pJ, sM, pM;
	unsigned nM, nJ, nO, last, first;
	int makespan = 0;
	Instance instance;
	bool partial = false;

	void error(string e)
	{
		cout << br << "ERRO: " << e << br;
		print();
		exit(0);
	}
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
		vector<unsigned> heads = getHeads();
		vector<unsigned> late_pred = getLatestPred(heads), p;
		unsigned op = last;

		while (op > 0)
		{
			p.push_back(op);
			op = late_pred[op];
		}
		reverse(p.begin(), p.end());
		return p;
	}
	void blocks(vector<unsigned> &begins, vector<unsigned> &ends, unsigned blockType)
	{
		vector<unsigned> p = critical(), blocksBegins, blocksEnds;
		
		assert(p.size() > 0);
		blocksBegins.push_back(1);
		for (unsigned i = 2; i < p.size(); i++)
		{
			bool sameCluster = (blockType == BLOCK_J) ? sameJob(p[i], p[i - 1]) : sameMach(p[i], p[i - 1]);
			if (!sameCluster)
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
	vector<unsigned> getRandomJBlock(){
		vector<unsigned> p = critical(), begins, ends, block;
		blocks(begins, ends, BLOCK_J);
		
		if(begins.size() <= 0)
			return begins;

		unsigned i = (begins.size()==1)? 0 : randint(0, begins.size()-1);

		for(unsigned index = begins[i]; index <= ends[i]; index++)
			block.push_back(p[index]);
		//printv(block,0,"block m");
		return block;
	}
	vector<unsigned> getRandomMBlock(){
		vector<unsigned> p = critical(), begins, ends, block;
		blocks(begins, ends, BLOCK_M);
		
		if(begins.size() <= 0)
			return begins;
		
		unsigned i = (begins.size()==1)? 0 : randint(0, begins.size()-1);

		for(unsigned index = begins[i]; index <= ends[i]; index++)
			block.push_back(p[index]);
		return block;
	}
	vector<unsigned> getRandomBlock(){
		vector<unsigned> block;
		if(randint(0,1) == BLOCK_J){
			block = getRandomJBlock();
			if(block.size()==0)
				block = getRandomMBlock();
		}
		else {
			block = getRandomMBlock();
			if(block.size()==0)
				block = getRandomJBlock();
		}
		return block;
	}
	unsigned getJobAncestor(unsigned o){
		while(pJ[o] != 0)
			o = pJ[o];
		return o;
	}
	unsigned getMachAncestor(unsigned o){
		while(pM[o] != 0)
			o = pM[o];
		return o;
	}
	vector<unsigned> getOperationsJBlock(unsigned o){
		vector<unsigned> block;

		unsigned operation = getJobAncestor(o);
		if(operation == first)
			operation = sJ[operation];

		do{
			block.push_back(operation);
			operation = sJ[operation];
		}while(operation != 0);

		return block;
	}
	vector<unsigned> getOperationsMBlock(unsigned o){
		vector<unsigned> block;

		unsigned operation = getMachAncestor(o);
		if(operation == first)
			operation = sM[operation];

		do{
			block.push_back(operation);
			operation = sM[operation];
		}while(operation != 0);

		return block;
	}
	vector<unsigned> getOperationsCriticalJBlock(unsigned o){
		vector<unsigned> p = critical(), block;
		unsigned operation, index = findIndex(p, o);

		if(index > nO) return block;

		while(index <= nO){
			operation = p[index];
				if(!sameJob(o, operation) || operation == first)
					break;
			index--;
		}

		reverse(block.begin(), block.end());

		return block;		
	}
	vector<unsigned> getOperationsCriticalMBlock(unsigned o){
		vector<unsigned> p = critical(), block;
		unsigned operation, index = findIndex(p, o);

		if(index > nO) return block;
 
		while(index <= nO){
			operation = p[index];
				if(!sameMach(o, operation) || operation == first)
					break;
			block.push_back(operation);
			index--;
		}

		reverse(block.begin(), block.end());

		return block;			
	}
	vector<bool> getCriticalOperationsList(){
		vector<bool> list(nO + 1, false);
		vector<unsigned> p = critical();

		for(unsigned o : p)
			list[o] = true;

		return list;
	}
	unsigned getHighestCostFromMachine(){
		vector<vector<unsigned>> machs = wholeMachines();
		unsigned highestCost = 0, costSum;
		for(vector<unsigned> mach: machs){
			costSum =0;
			for(unsigned o: mach)
				costSum += instance.cost[o];
			if(costSum > highestCost)
				highestCost = costSum;
		}

		return highestCost;
	}
	unsigned getHighestCostFromJob(){
		vector<vector<unsigned>> jobs = wholeJobs();
		unsigned highestCost = 0, costSum;
		for(vector<unsigned> job: jobs){
			costSum =0;
			for(unsigned o: job)
				costSum += instance.cost[o];
			if(costSum > highestCost)
				highestCost = costSum;
		}

		return highestCost;
	}
	vector<unsigned> starters(vector<unsigned> predecessors)
	{
		vector<unsigned> s;

		for (unsigned i = 1; i < predecessors.size(); i++)
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
	bool isJEdgeN5(unsigned i1, unsigned i2, vector<unsigned> &p)
	{
		if (sameJob(p[i1], p[i2]) && (!sameJob(p[i2], p[i2 + 1]) || !sameJob(p[i1 - 1], p[i1])))
			return true;
		return false;
	}
	bool isMEdgeN5(unsigned i1, unsigned i2, vector<unsigned> &p)
	{
		if (sameMach(p[i1], p[i2]) && (!sameMach(p[i2], p[i2 + 1]) || !sameMach(p[i1 - 1], p[i1])))
			return true;
		return false;
	}
	vector<pair<unsigned, unsigned>> getJEdgesN5()
	{
		vector<pair<unsigned, unsigned>> edges;
		vector<unsigned> p = critical();
		for (unsigned i = 1; i < p.size() - 2; i++)
			if (isJEdgeN5(i, i + 1, p))
				edges.push_back(make_pair(p[i], p[i + 1]));
		return edges;
	}
	vector<pair<unsigned, unsigned>> getMEdgesN5()
	{
		vector<pair<unsigned, unsigned>> edges;
		vector<unsigned> p = critical();
		for (unsigned i = 1; i < p.size() - 2; i++)
			if (isMEdgeN5(i, i + 1, p))
				edges.push_back(make_pair(p[i], p[i + 1]));
		return edges;
	}

	/////////////////////////////////////////////////////////////////////////
	//////// MÉTODOS DE INICIALIZAÇÃO E AUXILIARES //////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void initRandom()
	{
		vector<unsigned> oJ(nJ + 1, 0),
			oM(nM + 1, 0);

		fillVecRandom(oJ); //printv(oJ, 1, "Ordem randomizada - jobs");
		fillVecRandom(oM); //printv(oM, 1, "Ordem randomizada - machines");
		fillOrd(oJ, oM);
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
	void initGreedyJobs()
	{
		vector<unsigned> oJ(nJ + 2, 0), oM(nM + 1, 0), used(nJ + 1, 0);
		unsigned bestJob, bestMakespan, tempMakespan;
		fillVecRandom(oM);
		oM.push_back(0);
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
				tempMakespan = computePartialMakespan();
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
				//unsigned o1 = op(oJ[1], testMach), o2 = op(oJ[2], testMach), o3 = op(oJ[3], testMach), o4 = op(oJ[4], testMach);
				//cout << tb << tb << "order " << o1 << ":" << instance.cost[o1] << sp << o2 << ":" << instance.cost[o2] << sp << o3 << ":" << instance.cost[o3] << sp << o4 << ":" << instance.cost[o4] << br;

				first = op(oJ[1], testMach);
				//printPartial();
				tempMakespan = computePartialMakespan();
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
		//print();
	}
	void initTest()
	{
		first = 15;
		nO = 16;
		nJ = 4;
		nM = 4;

		pJ = {0, 0, 1, 2, 3, 7, 8, 6, 0, 11, 0, 12, 10, 16, 13, 0, 15};
		sJ = {0, 2, 3, 4, 0, 0, 7, 5, 6, 0, 12, 9, 11, 14, 0, 16, 13};
		pM = {0, 0, 10, 15, 12, 9, 2, 11, 0, 13, 0, 3, 16, 1, 6, 0, 8};
		sM = {0, 13, 6, 11, 0, 0, 14, 0, 16, 5, 2, 7, 4, 9, 0, 3, 12};
		instance.cost = {0, 34, 15, 38, 95, 2, 89, 19, 7, 54, 70, 28, 34, 61, 9, 87, 29};

		/* pJ = {0, 0, 1, 2, 0, 4, 5, 0, 7, 8};
		sJ = {0, 2, 3, 0, 5, 6, 0, 8, 9, 0};
		pM = {0, 0, 0, 0, 1, 2, 3, 4, 5, 6};
		sM = {0, 4, 5, 6, 7, 8, 9, 0, 0, 0};
		instance.cost = {0, 1, 2, 3, 8, 5, 3, 1, 7, 2}; */
	}
	void fillOrd(vector<unsigned> &oJ, vector<unsigned> &oM)
	{
		first = op(oJ[1], oM[1]);
		if (oJ.size() == nJ + 1)
			oJ.push_back(0);
		if (oM.size() == nM + 1)
			oM.push_back(0);

		for (int m = 1; m <= nM; m++)
			for (int j = 1; j <= nJ; j++)
			{
				unsigned o = op(oJ[j], oM[m]);
				pM[o] = op(oJ[j - 1], oM[m]);
				pJ[o] = op(oJ[j], oM[m - 1]);
				sM[o] = op(oJ[j + 1], oM[m]);
				sJ[o] = op(oJ[j], oM[m + 1]);
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
		makespan = computeMakespan();
	}

	/////////////////////////////////////////////////////////////////////////
	//////// DEBUG //////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void validOp(unsigned op)
	{
		if (!partial)
			assert(op <= nJ * nJ && op <= nM * nM);
		assert(op > 0);
	}
	void print()
	{
		cout << br << "======================================" << br;
		if (VERBOSE)
			printJobCluster();
		printSchedule();
		vector<unsigned> p = critical();
		printv(p, 0, "critical path");
		printl("first", first);
		printl("last", last);
		printl("makespan", getMakespan());
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
	Printer makePrinter()
	{
		Printer p(sJ, sM);
		p.setInstanceData(nO, instance.cost, instance.o_job, instance.o_machine);
		p.setStarters(starters(pJ), starters(pM));
		p.setSolutionData(first, computeMakespan(), critical(), getHeads());
		return p;
	}
	void printMachCluster()
	{
		makePrinter().printMachCluster();
	}
	void printJobCluster()
	{
		makePrinter().printJobCluster();
	}
	void printGantt()
	{
		makePrinter().printGantt();
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
	vector<unsigned> getLatestPred(vector<unsigned> &heads)
	{
		vector<unsigned> late_pred(nO + 1, 0);
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
	/////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	Solution() {}
	Solution(Instance i, int init_method)
	{
		instance = i;
		last = 0;
		makespan = 0;
		nO = instance.n_ops;
		nJ = instance.n_jobs;
		nM = instance.n_machines;
		sJ.resize(nO + 1, 0);
		pJ.resize(nO + 1, 0);
		sM.resize(nO + 1, 0);
		pM.resize(nO + 1, 0);
		init(init_method);
	}
	int computePartialMakespan()
	{
		makespan = 0;
		unsigned temp_makespan = 0, op, lookat = 0, pushed = 0;
		vector<unsigned> queue(nO),
			deg(nO + 1, 0),
			heads = getHeads(deg, queue, pushed);
		vector<unsigned> latePred = getLatestPred(heads);

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
	int computeMakespan()
	{
		makespan = 0;
		unsigned temp_makespan = 0, op, lookat = 0, pushed = 0;
		vector<unsigned> queue(nO),
			deg(nO + 1, 0),
			heads = getHeads(deg, queue, pushed);
		//printv(heads, 0, "heads"); printl("size:",heads.size());

		if (heads.size() == 0)
			return 0;

		vector<unsigned> latePred = getLatestPred(heads); // latest predecessors AKA prev

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
		s.makespan = computeMakespan();
		return s;
	}
	int getMakespan() { return partial ? computePartialMakespan() : computeMakespan(); }
	friend class Heuristics;
	friend class Neighbor;
	friend class Neighborhood;
	friend class GreedyIterator;
};

#endif