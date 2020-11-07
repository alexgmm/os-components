#ifndef NEIGHBORHOOD_HPP
#define NEIGHBORHOOD_HPP

#include <iostream>
#include "solution.hpp"
#include "tabu_list.hpp"
#include "perturbation.hpp"

using namespace std;

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
class NeighborGenerator
{
	Solution sol;
	Solution previous;
	vector<vector<unsigned>> tabu;
	void swapJ(unsigned op1, unsigned op2)
	{
		if (TRACK_SWAP_OPERATIONS)
			cout << br << "swapJ(" << op1 << "," << op2 << ")" << br;
		//assert(op1 > 0 && op2 > 0);
		assert(op1 != op2);
		assert(sol.adjJob(op1, op2));

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
	}
	void swapM(unsigned op1, unsigned op2)
	{
		if (TRACK_SWAP_OPERATIONS)
			cout << br << "swapM(" << op1 << "," << op2 << ")" << br;
		//assert(op1 > 0 && op2 > 0);
		assert(op1 != op2);
		assert(sol.adjMach(op1, op2));

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
	}

	/////////////////////////////////////////////////////////////////////////
	//////// GERAÇÃO DE VIZINHO /////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	Solution getRandomNeighbor_swapCritical()
	{
		auto o = sol.getOneRandomOperation_swapCritical();
		auto p = sol.listPossiblePerturbations_swapCritical(o);
		auto i = p.size() == 1 ? 0 : randint(0, p.size() - 1);
		applyPerturbation(p[i]);

		return sol;
	}
	Solution getRandomNeighbor_swapCriticalEdge()
	{
		auto o = sol.getOneRandomOperation_swapCriticalEdge();
		auto p = sol.listPossiblePerturbations_swapCritical(o);
		auto i = p.size() == 1 ? 0 : randint(0, p.size() - 1);
		applyPerturbation(p[i]);

		return sol;
	}
	void shiftJ(vector<unsigned> &p, unsigned idxBegin, unsigned idxEnd)
	{
		assert(idxBegin != idxEnd);
		if (TRACK_SHIFT_OPERATIONS)
			cout << br << "shiftJ(" << p[idxBegin] << "," << p[idxEnd] << ")" << br;
		unsigned lastOp = p[idxEnd];
		for (unsigned i = idxEnd; i > idxBegin; i--)
			swapJ(lastOp, p[i - 1]);
		saveDataShift();
	}
	void shiftM(vector<unsigned> &p, unsigned idxBegin, unsigned idxEnd)
	{
		assert(idxBegin != idxEnd);
		if (TRACK_SHIFT_OPERATIONS)
			cout << br << "shiftM(" << p[idxBegin] << "," << p[idxEnd] << ")" << br;
		unsigned lastOp = p[idxEnd];
		for (unsigned i = idxEnd; i > idxBegin; i--)
			swapM(lastOp, p[i - 1]);
		saveDataShift();
	}
	Solution getRandomNeighbor_shiftCritical()
	{
		auto o = sol.getOneRandomOperation_shiftCritical();
		auto p = sol.listPossiblePerturbations_shiftCritical(o);
		auto i = p.size() == 1 ? 0 : randint(0, p.size() - 1);
		applyPerturbation(p[i]);

		return sol;
	}
	Solution getRandomNeighbor_shiftWhole()
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

	void applyPerturbations(vector<Perturbation> &perturbations, vector<Solution> &neighbors)
	{
		for (auto p : perturbations)
		{
			applyPerturbation(p);
			neighbors.push_back(sol.copySolution());
			restore();
		}
	}
	vector<Solution> getNeighbors_swapCritical()
	{
		vector<Solution> neighbors;

		auto perturbations = sol.listAllPerturbations(SWAP_CRITICAL);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}
	vector<Solution> getNeighbors_swapCriticalEdge()
	{
		vector<Solution> neighbors;

		auto perturbations = sol.listAllPerturbations(SWAP_CRITICAL_EDGE);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}
	vector<Solution> getNeighbors_shiftCritical()
	{
		vector<Solution> neighbors;

		auto perturbations = sol.listAllPerturbations(SHIFT_CRITICAL);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}
	vector<Solution> getNeighbors_shiftWhole()
	{
		vector<Solution> neighbors;

		auto perturbations = sol.listAllPerturbations(SHIFT_WHOLE);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}

public:
	NeighborGenerator() {}
	NeighborGenerator(Solution s)
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

	ScheduleChange applyPerturbation(Perturbation m)
	{
		ScheduleChange change = {0, 0, 0};

		unsigned o = m.operation;
		if (m.blockType == BLOCK_J && m.perturbationType == SWAP_PRED)
		{
			swapJ(o, sol.pJ[o]);
			change = {SWAP,
					  o,
					  sol.pJ[o]};
		}

		if (m.blockType == BLOCK_J && m.perturbationType == SWAP_SUCC)
		{
			swapJ(o, sol.sJ[o]);
			change = {SWAP,
					  o,
					  sol.sJ[o]};
		}

		if (m.blockType == BLOCK_M && m.perturbationType == SWAP_PRED)
		{
			swapM(o, sol.pM[o]);
			change = {SWAP,
					  o,
					  sol.pM[o]};
		}

		if (m.blockType == BLOCK_M && m.perturbationType == SWAP_SUCC)
		{
			swapM(o, sol.sM[o]);
			change = {SWAP,
					  o,
					  sol.sM[o]};
		}

		if (m.blockType == BLOCK_J && m.perturbationType == SHIFT_WHOLE)
		{
			vector<unsigned> block = sol.getOperationsJBlock(o);
			unsigned opIndex = findIndex(block, o);
			shiftJ(block, opIndex - m.factor, opIndex);
			change = {SHIFT,
					  block[opIndex - m.factor],
					  block[opIndex]};
		}

		if (m.blockType == BLOCK_M && m.perturbationType == SHIFT_WHOLE)
		{
			vector<unsigned> block = sol.getOperationsMBlock(o);
			unsigned opIndex = findIndex(block, o);
			shiftM(block, opIndex - m.factor, findIndex(block, o));
			change = {SHIFT,
					  block[opIndex - m.factor],
					  block[findIndex(block, o)]};
		}

		return change;
	}

	/////////////////////////////////////////////////////////////////////////
	//////// OPERADORES DE VIZINHANÇA ///////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	static Solution getRandomNeighbor(unsigned oper, Solution &s)
	{
		Solution copied = s.copySolution();
		NeighborGenerator n = NeighborGenerator(copied);

		switch (oper)
		{
		case SWAP_CRITICAL:
			return n.getRandomNeighbor_swapCritical();
		case SWAP_CRITICAL_EDGE:
			return n.getRandomNeighbor_swapCriticalEdge();
		case SHIFT_CRITICAL:
			return n.getRandomNeighbor_shiftCritical();
		case SHIFT_WHOLE:
			return n.getRandomNeighbor_shiftWhole();
		default:
			break;
		}
	}
	vector<Solution> getNeighbors(int oper)
	{
		switch (oper)
		{
		case SHIFT_CRITICAL:
			return getNeighbors_shiftCritical();
		default:
			break;
		}
	}
	Solution getSolution() { return sol; }
};

#endif