#ifndef NEIGHBORHOOD_HPP
#define NEIGHBORHOOD_HPP

#include <assert.h>
#include <iostream>
#include "solution.hpp"
#include "tabu_list.hpp"
#include "perturbation.hpp"

class NeighborGenerator
{
	Solution sol;
	Solution previous;
	vector<vector<unsigned>> tabu;

	void swapJ(unsigned op1, unsigned op2)
	{
		if (TRACK_SWAP_OPERATIONS)
			cout << "swapJ(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0 && op2 > 0);
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
			cout << "swapM(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0 && op2 > 0);
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

		auto o = sol.getOneRandomOperation_shiftWhole();
		auto p = sol.listPossiblePerturbations_shiftWhole(o);
		auto i = p.size() == 1 ? 0 : randint(0, p.size() - 1);
		applyPerturbation(p[i]);

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
	ScheduleChange applySwap(Perturbation p)
	{
		ScheduleChange change = {0, 0, 0};
		unsigned o = p.operation, o2;

		if (p.blockType == BLOCK_J)
		{
			if (p.perturbationType == SWAP_PRED)
				o2 = sol.pJ[o];
			else
				o2 = sol.sJ[o];

			swapJ(o, o2);
		}
		else
		{
			if (p.perturbationType == SWAP_PRED)
				o2 = sol.pM[o];
			else
				o2 = sol.sM[o];

			swapM(o, o2);
		}
		change = {SWAP, o, o2};

		return change;
	}

	ScheduleChange applyShift(Perturbation p)
	{
		ScheduleChange change = {0, 0, 0};
		vector<unsigned> block;
		unsigned o = p.operation;

		if (p.blockType == BLOCK_J)
		{
			if (p.perturbationType == SHIFT_WHOLE)
				block = sol.getOperationsJBlock(o);
			else if (p.perturbationType == SHIFT_CRITICAL)
				block = sol.getOperationsCriticalJBlock(o);
		}
		else
		{
			if (p.perturbationType == SHIFT_WHOLE)
				block = sol.getOperationsMBlock(o);
			else if (p.perturbationType == SHIFT_CRITICAL)
				block = sol.getOperationsCriticalMBlock(o);
		}

		unsigned o1Index = block.size() - 1;
		unsigned o2Index = o1Index - p.factor;
		change = {SHIFT,
				  block[o2Index],
				  block[o1Index]};

		if (p.blockType == BLOCK_J)
			shiftJ(block, o2Index, o1Index);
		else
			shiftM(block, o2Index, o1Index);

		return change;
	}

	ScheduleChange applyPerturbation(Perturbation p)
	{
		/* cout << "on apply:\n";
		printPerturbation(p);
		cout << "+++++++++\n"; */
		sol.assertValidSwapPerturbation(p);
		if (isShift(p.perturbationType))
			return applyShift(p);
		else
			return applySwap(p);
	}
	/////////////////////////////////////////////////////////////////////////
	//////// OPERADORES DE VIZINHANÇA ///////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	static Solution
	getRandomNeighbor(unsigned oper, Solution &s)
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