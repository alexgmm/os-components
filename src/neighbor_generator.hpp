#ifndef NEIGHBORHOOD_HPP
#define NEIGHBORHOOD_HPP

#include <assert.h>
#include <iostream>
#include "solution.hpp"
#include "heuristics/tabu_list.hpp"
#include "perturbation.hpp"

class NeighborGenerator
{
	SolutionPerturbator sol;
	SolutionPerturbator previous;
	vector<vector<unsigned>> tabu;

	/////////////////////////////////////////////////////////////////////////
	//////// GERAÇÃO DE VIZINHO /////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	SolutionPerturbator getRandomNeighbor_swapCritical()
	{
		auto o = sol.getOneRandomOperation_swapCritical();
		auto p = sol.listPossiblePerturbations_swapCritical(o);
		auto i = p.size() == 1 ? 0 : randint(0, p.size() - 1);
		applyPerturbation(p[i]);

		return sol;
	}
	SolutionPerturbator getRandomNeighbor_swapCriticalEdge()
	{
		auto o = sol.getOneRandomOperation_swapCriticalEdge();
		auto p = sol.listPossiblePerturbations_swapCritical(o);
		auto i = p.size() == 1 ? 0 : randint(0, p.size() - 1);
		applyPerturbation(p[i]);

		return sol;
	}
	SolutionPerturbator getRandomNeighbor_shiftCritical()
	{
		auto o = sol.getOneRandomOperation_shiftCritical();
		auto p = sol.listPossiblePerturbations_shiftCritical(o);
		auto i = p.size() == 1 ? 0 : randint(0, p.size() - 1);
		applyPerturbation(p[i]);

		return sol;
	}
	SolutionPerturbator getRandomNeighbor_shiftWhole()
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

	void applyPerturbations(vector<Perturbation> &perturbations, vector<SolutionPerturbator> &neighbors)
	{
		for (auto p : perturbations)
		{
			applyPerturbation(p);
			neighbors.push_back(sol.getCopy());
			restore();
		}
	}
	vector<SolutionPerturbator> getNeighbors_swapCritical()
	{
		vector<SolutionPerturbator> neighbors;

		auto perturbations = sol.listAllPerturbations(SWAP_CRITICAL);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}
	vector<SolutionPerturbator> getNeighbors_swapCriticalEdge()
	{
		vector<SolutionPerturbator> neighbors;

		auto perturbations = sol.listAllPerturbations(SWAP_CRITICAL_EDGE);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}
	vector<SolutionPerturbator> getNeighbors_shiftCritical()
	{
		vector<SolutionPerturbator> neighbors;

		auto perturbations = sol.listAllPerturbations(SHIFT_CRITICAL);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}
	vector<SolutionPerturbator> getNeighbors_shiftWhole()
	{
		vector<SolutionPerturbator> neighbors;

		auto perturbations = sol.listAllPerturbations(SHIFT_WHOLE);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}

public:
	NeighborGenerator() {}
	NeighborGenerator(SolutionPerturbator s)
	{
		setSolution(s);
		tabu.resize(s.nO + 1);
		for (unsigned i = 0; i <= s.nO; i++)
			tabu[i].resize(s.nO + 1, 0);
	}
	void restore() { sol = previous.getCopy(); }
	void setSolution(SolutionPerturbator s)
	{
		sol = s.getCopy();
		previous = s.getCopy();
	}
	Movement applySwap(Perturbation p)
	{
		Movement change = {0, 0, 0};
		unsigned o = p.operation, o2;

		if (p.blockType == BLOCK_J)
		{
			if (p.perturbationType == SWAP_PRED)
				o2 = sol.pJ[o];
			else
				o2 = sol.sJ[o];

			sol.swapJ(o, o2);
		}
		else
		{
			if (p.perturbationType == SWAP_PRED)
				o2 = sol.pM[o];
			else
				o2 = sol.sM[o];

			sol.swapM(o, o2);
		}
		change = {SWAP, o, o2};

		return change;
	}

	Movement applyShift(Perturbation p)
	{
		Movement change = {0, 0, 0};
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
			sol.shiftJ(block, o2Index, o1Index);
		else
			sol.shiftM(block, o2Index, o1Index);

		return change;
	}

	Movement applyPerturbation(Perturbation p)
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

	static SolutionPerturbator
	getRandomNeighbor(unsigned oper, SolutionPerturbator &s)
	{
		SolutionPerturbator copied = s.getCopy();
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
	vector<SolutionPerturbator> getNeighbors(int oper)
	{
		switch (oper)
		{
		case SHIFT_CRITICAL:
			return getNeighbors_shiftCritical();
		default:
			break;
		}
	}
	SolutionPerturbator getSolution() { return sol; }
};

#endif