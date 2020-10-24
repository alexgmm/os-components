#ifndef NEIGHBORHOOD_HPP
#define NEIGHBORHOOD_HPP

#include <iostream>
#include "solution.hpp"

using namespace std;

struct Mutation{
    unsigned operation, mutationType, blockType, factor;
};
class Neighbor
{
	Solution neighbor;
	unsigned value;
	bool legal;

public:
	Neighbor() {}
	Neighbor(Solution n, unsigned v, bool l)
	{
		if (v == 0)
		{
			neighbor.makespan = UMAX;
			value = UMAX;
		}
		else
		{
			value = v;
			neighbor.makespan = v;
		}
		neighbor = n;
		legal = l;
	}
	Solution getSolution() { return neighbor; }
	unsigned getValue() { return value; }
	bool isLegal() { return legal; }
};
class Neighborhood
{
	Solution sol;
	Solution previous;
	vector<vector<unsigned>> tabu;
	bool swapJ(unsigned op1, unsigned op2)
	{
		if (TRACK_SWAP_OPERATIONS)
			cout << br << "swapJ(" << op1 << "," << op2 << ")" << br;
		if(op1 ==0 or op2 == 0)
			return false;
		//assert(op1 > 0 && op2 > 0);
		assert(op1 != op2);
		assert(sol.adjJob(op1, op2));

		if (op1 == sol.first || op2 == sol.first)
			return false;

		if (op1 == sol.first)
			sol.first = op2;

		if (sol.sJ[op2] == op1)
		{
			unsigned o = op1;
			op1 = op2;
			op2 = o;
		}

		unsigned jp = sol.pJ[op1], js = sol.sJ[op2];

		if (jp)
			sol.sJ[jp] = op2;
		if (js)
			sol.pJ[js] = op1;

		sol.sJ[op1] = js;
		sol.pJ[op1] = op2;
		sol.pJ[op2] = jp;
		sol.sJ[op2] = op1;

		//sol.validSchedule();
		sol.computeMakespan();
		if (VERBOSE)
			sol.print();
		saveDataSwap();
		if (legal(op1, op2))
		{
			tabu[op1][op2] = CURRENT_ITER;
			return true;
		}
		return false;
	}
	bool swapM(unsigned op1, unsigned op2)
	{
		if (TRACK_SWAP_OPERATIONS)
			cout << br << "swapM(" << op1 << "," << op2 << ")" << br;
		if(op1 ==0 or op2 == 0)
			return false;
		//assert(op1 > 0 && op2 > 0);
		assert(op1 != op2);
		assert(sol.adjMach(op1, op2));

		if (op1 == sol.first || op2 == sol.first)
			return false;

		if (op1 == sol.first)
			sol.first = op2;

		if (sol.sM[op2] == op1)
		{
			unsigned o = op1;
			op1 = op2;
			op2 = o;
		}

		unsigned mp = sol.pM[op1], ms = sol.sM[op2];

		if (mp)
			sol.sM[mp] = op2;
		if (ms)
			sol.pM[ms] = op1;

		sol.sM[op1] = ms;
		sol.pM[op1] = op2;
		sol.pM[op2] = mp;
		sol.sM[op2] = op1;

		//sol.validSchedule();
		sol.computeMakespan();
		if (VERBOSE)
			sol.print();
		saveDataSwap();
		if (legal(op1, op2))
		{
			tabu[op1][op2] = CURRENT_ITER;
			return true;
		}
		return false;
	}

	/////////////////////////////////////////////////////////////////////////
	//////// GERAÇÃO DE VIZINHO /////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void swapRandomSucc()
	{
		unsigned randOp = randint(1, sol.nO);

		while (!sol.sJ[randOp] && !sol.sM[randOp])
			randOp = randint(1, sol.nO);

		if (!sol.sJ[randOp])
			swapM(randOp, sol.sM[randOp]);
		else if (!sol.sM[randOp])
			swapJ(randOp, sol.sJ[randOp]);
		else
		{
			if (randint(0, 1) == SWAP_J)
				swapJ(randOp, sol.sJ[randOp]);
			else
				swapM(randOp, sol.sM[randOp]);
		}
	}
	void swapRandomPred()
	{
		unsigned randOp = randint(1, sol.nO);

		while (!sol.pJ[randOp] && !sol.pM[randOp])
			randOp = randint(1, sol.nO);

		if (!sol.pJ[randOp])
			swapM(randOp, sol.pM[randOp]);
		else if (!sol.pM[randOp])
			swapJ(randOp, sol.pJ[randOp]);
		else
		{
			if (randint(0, 1) == SWAP_J)
				swapJ(randOp, sol.pJ[randOp]);
			else
				swapM(randOp, sol.pM[randOp]);
		}
	}
	Solution swapAllOne()
	{
		if (randint(0, 1) == SWAP_PRED)
			swapRandomPred();
		else
			swapRandomSucc();
		return sol;
	}
	Solution swapCriticalOne()
	{
		vector<unsigned> b, e, p = sol.critical(), succ, pred;
		unsigned idxEnd = 0, idxBegin = 0, chosenBlock, op;
		int chosenOperator = randint(0, 1);

		if (chosenOperator == SWAP_J)
		{
			succ = sol.sJ;
			pred = sol.pJ;
			sol.blocks(b, e, BLOCK_J);
		}
		else
		{
			succ = sol.sM;
			pred = sol.pM;
			sol.blocks(b, e, BLOCK_M);
		}

		chosenBlock = (b.size() == 1) ? 0 : randint(0, b.size() - 1);
		idxBegin = b[chosenBlock];
		idxEnd = e[chosenBlock];
		idxEnd = (idxEnd - idxBegin > 1 ? randint(idxBegin + 1, idxEnd) : idxEnd);
		op = p[randint(idxBegin, idxEnd)];

		if (chosenOperator == SWAP_J)
		{
			if (!pred[op])
				swapJ(op, succ[op]);
			else if (!succ[op])
				swapJ(op, pred[op]);
			else
			{
				if (randint(0, 1) == SWAP_PRED)
					swapJ(op, pred[op]);
				else
					swapJ(op, succ[op]);
			}
		}
		else
		{
			if (!pred[op])
				swapM(op, succ[op]);
			else if (!succ[op])
				swapM(op, pred[op]);
			else
			{
				if (randint(0, 1) == SWAP_PRED)
					swapM(op, pred[op]);
				else
					swapM(op, succ[op]);
			}
		}

		return sol;
	}
	Solution swapCriticalEdgeOne()
	{
		vector<unsigned> b, e, p = sol.critical();

		if (randint(0, 1) == SWAP_J)
		{
			sol.blocks(b, e, BLOCK_J);

			unsigned chosenBlock = (b.size() == 1) ? 0 : randint(0, b.size() - 1);
			unsigned idxBegin = b[chosenBlock], idxEnd = e[chosenBlock];
			if (idxBegin != idxEnd - 1)
				idxEnd = (idxEnd - idxBegin > 1 ? randint(idxBegin + 1, idxEnd) : idxEnd);
			if (randint(0, 1) == BLOCK_START)
				swapJ(p[idxBegin], p[idxBegin + 1]);
			else
				swapJ(p[idxEnd], p[idxEnd - 1]);
		}
		else
		{
			sol.blocks(b, e, BLOCK_M);

			unsigned chosenBlock = (b.size() == 1) ? 0 : randint(0, b.size() - 1);
			unsigned idxBegin = b[chosenBlock], idxEnd = e[chosenBlock];
			if (idxBegin != idxEnd - 1)
				idxEnd = (idxEnd - idxBegin > 1 ? randint(idxBegin + 1, idxEnd) : idxEnd);
			if (randint(0, 1) == BLOCK_START)
				swapM(p[idxBegin], p[idxBegin + 1]);
			else
				swapM(p[idxEnd], p[idxEnd - 1]);
		}

		return sol;
	}
	bool shiftJ(vector<unsigned> &p, unsigned idxBegin, unsigned idxEnd)
	{
		assert(idxBegin != idxEnd);
		if (TRACK_SHIFT_OPERATIONS)
			cout << br << "shiftJ(" << p[idxBegin] << "," << p[idxEnd] << ")" << br;
		bool legal = true;
		unsigned lastOp = p[idxEnd];
		for (unsigned i = idxEnd; i > idxBegin; i--)
			legal = legal && swapJ(lastOp, p[i - 1]);
		saveDataShift();
		return legal;
	}
	bool shiftM(vector<unsigned> &p, unsigned idxBegin, unsigned idxEnd)
	{
		assert(idxBegin != idxEnd);
		if (TRACK_SHIFT_OPERATIONS)
			cout << br << "shiftM(" << p[idxBegin] << "," << p[idxEnd] << ")" << br;
		bool legal = true;
		unsigned lastOp = p[idxEnd];
		for (unsigned i = idxEnd; i > idxBegin; i--)
			legal = legal && swapM(lastOp, p[i - 1]);
		saveDataShift();
		return legal;
	}
	Solution shiftCriticalOne()
	{
		vector<unsigned> b, e, p = sol.critical();
		unsigned typeChoice = randint(2, 3);
		sol.blocks(b, e, typeChoice);
		/* string s = typeChoice == BLOCK_J ? "BJ" : "MJ";
		cout << br << s << br;
		printv(p, 0, "path");
		printv(b, 0, "begins");
		printv(e, 0, "ends"); */
		unsigned index = b.size() == 1 ? 0 : randint(0, b.size() - 1);

		if (typeChoice == SHIFT_J)
			shiftJ(p, b[index], e[index]);
		else
			shiftM(p, b[index], e[index]);

		return sol;
	}
	Solution shiftWholeOne()
	{

		if (randint(2, 3) == SHIFT_J)
		{
			vector<vector<unsigned>> jobs = sol.wholeJobs();
			vector<unsigned> job = jobs[randint(0, jobs.size() - 1)];
			unsigned i = randint(1, job.size() - 1);
			shiftJ(job, 0, i);
		}
		else
		{
			vector<vector<unsigned>> machs = sol.wholeMachines();
			vector<unsigned> mach = machs[randint(0, machs.size() - 1)];
			unsigned i = randint(1, mach.size() - 1);
			shiftM(mach, 0, i);
		}

		return sol;
	}

	/////////////////////////////////////////////////////////////////////////
	//////// GERAÇÃO DE VIZINHANÇA COMPLETA /////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	vector<Neighbor> shiftCritical()
	{
		unsigned idxBegin, idxEnd;
		vector<Neighbor> neighbors;
		vector<unsigned> p = sol.critical(), bJ, bM, eJ, eM;
		sol.blocks(bJ, eJ, BLOCK_J);
		sol.blocks(bM, eM, BLOCK_M);
		bool legal;
		unsigned value;

		for (unsigned pos = 0; pos < bJ.size(); pos++)
		{
			idxBegin = bJ[pos];
			idxEnd = eJ[pos];
			while (idxEnd > idxBegin)
			{
				legal = shiftJ(p, idxBegin, idxEnd);
				value = sol.getMakespan();
				if (value)
				{
					Neighbor n(sol.copySolution(), value, legal);
					neighbors.push_back(n);
				}
				restore();
				idxEnd--;
			}
		}

		for (unsigned pos = 0; pos < bJ.size(); pos++)
		{
			idxBegin = bM[pos];
			idxEnd = eM[pos];
			while (idxEnd > idxBegin)
			{
				legal = shiftM(p, idxBegin, idxEnd);
				value = sol.getMakespan();
				if (value)
				{
					Neighbor n(sol.copySolution(), value, legal);
					neighbors.push_back(n);
				}
				restore();
				idxEnd--;
			}
		}

		return neighbors;
	}
	vector<Neighbor> shiftWhole()
	{
		unsigned value;
		vector<Neighbor> neighbors;
		vector<vector<unsigned>> machines = sol.wholeMachines(), jobs = sol.wholeJobs();
		bool legal;

		for (vector<unsigned> machine : machines)
		{ //printv(machine, 0, "mach");
			for (unsigned i = machine.size() - 1; i > 0; i--)
			{
				legal = shiftM(machine, 0, i);
				value = sol.getMakespan();
				if (value)
				{
					Neighbor n(sol.copySolution(), value, legal);
					neighbors.push_back(n);
				}
				restore();
			}
		}

		for (vector<unsigned> job : jobs)
		{
			for (unsigned i = job.size() - 1; i > 0; i--)
			{
				legal = shiftJ(job, 0, i);
				value = sol.getMakespan();
				if (value)
				{
					Neighbor n(sol.copySolution(), value, legal);
					neighbors.push_back(n);
				}
				restore();
			}
		}

		return neighbors;
	}
	bool legal(unsigned op1, unsigned op2)
	{
		if (CURRENT_ITER - TABU_DURATION > tabu[op1][op2] || tabu[op1][op2] == 0)
			return true;
		return false;
	}
	void traverseSwapJ(vector<Neighbor> &neighborhood, unsigned op, vector<bool> &visited)
	{
		//cout << br << "traverseSwapJ(" << op << ")" << br;
		//sol.print();
		bool l = swapJ(op, sol.sJ[op]);
		//sol.print();

		Neighbor n(sol, sol.computeMakespan(), l);
		neighborhood.push_back(n);
		restore();

		visited[op] = true;

		if (!visited[sol.sJ[op]] && !isLast(sol.sJ[op]) && sol.sJ[sol.sJ[op]])
			traverseSwapJ(neighborhood, sol.sJ[op], visited);
		if (!visited[sol.sM[op]] && !isLast(sol.sM[op]) && sol.sJ[sol.sM[op]])
			traverseSwapJ(neighborhood, sol.sM[op], visited);
	}
	void traverseSwapM(vector<Neighbor> &neighborhood, unsigned op, vector<bool> &visited)
	{
		if (op != sol.first)
		{
			//cout << br << "traverseSwapJ(" << op << ")" << br;
			//sol.print();
			bool l = swapM(op, sol.sM[op]);
			//sol.print();
			Neighbor n(sol, sol.computeMakespan(), l);
			neighborhood.push_back(n);

			restore();

			visited[op] = true;
		}

		if (!visited[sol.sJ[op]] && !isLast(sol.sJ[op]) && sol.sM[sol.sJ[op]])
			traverseSwapM(neighborhood, sol.sJ[op], visited);
		if (!visited[sol.sM[op]] && !isLast(sol.sM[op]) && sol.sM[sol.sM[op]])
			traverseSwapM(neighborhood, sol.sM[op], visited);
	}
	vector<Neighbor> swapAllNeighbors()
	{
		vector<Neighbor> neighborhood;
		vector<bool> visited(sol.nO + 1, false), visited2(sol.nO + 1, false);
		visited[0] = true;
		visited2[0] = true;

		traverseSwapJ(neighborhood, sol.first, visited);
		traverseSwapM(neighborhood, sol.first, visited2);

		return neighborhood;
	}
	void swapJAdd(unsigned op1, unsigned op2, vector<Neighbor> &neighborhood)
	{
		bool l = swapJ(op1, op2);
		Neighbor n(sol, sol.computeMakespan(), l);
		neighborhood.push_back(n);
		restore();
	}
	void swapMAdd(unsigned op1, unsigned op2, vector<Neighbor> &neighborhood)
	{
		bool l = swapM(op1, op2);
		Neighbor n(sol, sol.computeMakespan(), l);
		neighborhood.push_back(n);
		restore();
	}
	vector<Neighbor> swapCriticalNeighbors()
	{
		vector<Neighbor> neighborhood;
		vector<unsigned> p = sol.critical();

		for (unsigned i = 0; i < p.size() - 1; i++)
		{
			if (sol.sameJob(p[i], p[i + 1]))
				swapJAdd(p[i], p[i + 1], neighborhood);
			if (sol.sameMach(p[i], p[i + 1]))
				swapMAdd(p[i], p[i + 1], neighborhood);
		}

		return neighborhood;
	}
	vector<Neighbor> swapCriticalEdgeNeighbors()
	{
		vector<Neighbor> neighborhood;
		vector<unsigned> p = sol.critical();
		vector<pair<unsigned, unsigned>> jEdges = sol.getJEdgesN5(), mEdges = sol.getMEdgesN5();

		for (pair<unsigned, unsigned> e : jEdges)
			swapJAdd(e.first, e.second, neighborhood);

		for (pair<unsigned, unsigned> e : mEdges)
			swapMAdd(e.first, e.second, neighborhood);

		return neighborhood;
	}
	void saveDataShift()
	{
		if (SAVE_GRAPHS_ON_SHIFT)
			sol.printJobCluster();
		if (SAVE_CHARTS_ON_SHIFT)
			sol.printGantt();
	}
	void saveDataSwap()
	{
		if (SAVE_GRAPHS_ON_SWAP)
			sol.printJobCluster();
		if (SAVE_CHARTS_ON_SWAP)
			sol.printGantt();
	}

public:
	Neighborhood() {}
	Neighborhood(Solution s)
	{
		setSolution(s);
		tabu.resize(s.nO + 1);
		for (unsigned i = 0; i <= s.nO; i++)
			tabu[i].resize(s.nO + 1, 0);
	}
	void restore() { sol = previous.copySolution(); }
	void setSolution(Solution s)
	{
		sol = s.copySolution();
		previous = s.copySolution();
	}

	unsigned applyMutation(Mutation m){
		unsigned o = m.operation;
		if(m.blockType == BLOCK_J && m.mutationType == SWAP_PRED){
			swapJ(o, sol.pJ[o]);
			return sol.computeMakespan();
		}

		if(m.blockType == BLOCK_J && m.mutationType == SWAP_SUCC){
			swapJ(o, sol.sJ[o]);
			return sol.computeMakespan();
		}

		if(m.blockType == BLOCK_M && m.mutationType == SWAP_PRED){
			swapM(o, sol.pM[o]);
			return sol.computeMakespan();
		}

		if(m.blockType == BLOCK_M && m.mutationType == SWAP_SUCC){
			swapM(o, sol.sM[o]);
			return sol.computeMakespan();
		}

		if(m.blockType == BLOCK_J && m.mutationType == SHIFT_WHOLE){
			vector<unsigned> block = sol.getOperationsJBlock(o);
			//printv(block,0,"j block");
			shiftJ(block, 0, m.factor);
			return sol.computeMakespan();
		}

		if(m.blockType == BLOCK_M && m.mutationType == SHIFT_WHOLE){
			vector<unsigned> block = sol.getOperationsMBlock(o);
			//printv(block,0,"j block");
			shiftM(block, 0, m.factor);
			return sol.computeMakespan();
		}
	}

	/////////////////////////////////////////////////////////////////////////
	//////// OPERADORES DE VIZINHANÇA ///////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	bool isLast(unsigned op) { return sol.sJ[op] == DUMMY && sol.sM[op] == DUMMY; }
	vector<Neighbor> getNeighborhood(int oper)
	{
		switch (oper)
		{
		case SWAP_ALL:
			return swapAllNeighbors();
			break;
		case SWAP_CRITICAL:
			return swapCriticalNeighbors();
			break;
		case SWAP_CRITICAL_EDGE:
			return swapCriticalEdgeNeighbors();
			break;
		case SHIFT_CRITICAL:
			return shiftCritical();
		case SHIFT_WHOLE:
			return shiftWhole();
			break;
		default:
			break;
		}
	}
	static Solution getNeighbor(unsigned oper, Solution &s)
	{
		Solution copied = s.copySolution(), selected;
		Neighborhood n = Neighborhood(copied);
		unsigned tries = 1000;

		bool invalid = true;
		while (invalid && tries > 0)
		{
			switch (oper)
			{
			case SWAP_ALL:
				selected = n.swapAllOne();
				n.restore();
				break;
			case SWAP_CRITICAL:
				selected = n.swapCriticalOne();
				n.restore();
				break;
			case SWAP_CRITICAL_EDGE:
				selected = n.swapCriticalEdgeOne();
				n.restore();
				break;
			case SHIFT_CRITICAL:
				selected = n.shiftCriticalOne();
				n.restore();
				break;
			case SHIFT_WHOLE:
				selected = n.shiftWholeOne();
				n.restore();
				break;
			default:
				break;
			}
			//selected.print();
			invalid = (selected.getMakespan() == 0);
			tries--;
		}
		if (invalid)
			return n.getSolution();
		return selected;
	}
	Solution getSolution() { return sol; }
};

#endif