#ifndef NEIGHBORHOOD_HPP
#define NEIGHBORHOOD_HPP

#include <assert.h>
#include <iostream>
#include "schedule/schedule.hpp"
#include "schedule/schedule_data_retriever.hpp"
#include "helpers/printer.hpp"
#include "heuristics/tabu_list.hpp"
#include "perturbation/perturbation.hpp"
#include "perturbation/perturbation_generator.hpp"

class NeighborGenerator
{
public:
	PerturbationGenerator generator;
	Schedule current, previous;
	bool initialized = false;

	Movement applySwap(Perturbation p)
	{
		Movement change = {0, 0, 0};
		unsigned o = p.operation, o2;

		if (p.blockType == BLOCK_J)
		{
			if (p.perturbationType == SWAP_PRED)
				o2 = current.pJ[o];
			else
				o2 = current.sJ[o];

			current.swapJ(o, o2);
		}
		else
		{
			if (p.perturbationType == SWAP_PRED)
				o2 = current.pM[o];
			else
				o2 = current.sM[o];

			current.swapM(o, o2);
		}
		change = {SWAP, o, o2};
		current.setFirst();

		return change;
	}
	Movement applyShift(Perturbation p)
	{
		Movement change = {0, 0, 0};
		vector<unsigned> block;
		unsigned o = p.operation;
		ScheduleDataRetriever dr(current);

		if (p.blockType == BLOCK_J)
		{
			if (p.perturbationType == SHIFT_WHOLE)
				block = dr.getOperationsJBlock(o);
			else if (p.perturbationType == SHIFT_CRITICAL)
				block = dr.getOperationsCriticalJBlock(o);
		}
		else
		{
			if (p.perturbationType == SHIFT_WHOLE)
				block = dr.getOperationsMBlock(o);
			else if (p.perturbationType == SHIFT_CRITICAL)
				block = dr.getOperationsCriticalMBlock(o);
		}

		assert(block.size() > 1);

		unsigned o1Index = block.size() - 1;
		unsigned o2Index = o1Index - p.factor;
		change = {SHIFT,
				  block[o2Index],
				  block[o1Index]};
		assertNonNullMovement(change);

		if (p.blockType == BLOCK_J)
			current.shiftJ(block, o2Index, o1Index);
		else
			current.shiftM(block, o2Index, o1Index);

		return change;
	}
	Movement applyPerturbation(Perturbation p)
	{
		if (isShift(p.perturbationType))
			return applyShift(p);
		else
			return applySwap(p);
	}
	void applyPerturbations(vector<Perturbation> &perturbations, vector<Schedule> &neighbors)
	{
		for (auto p : perturbations)
		{
			applyPerturbation(p);
			neighbors.push_back(current.getCopy());
			restore();
		}
	}

	NeighborGenerator() {}
	NeighborGenerator(Schedule s)
	{
		setSchedule(s);
	}

	void restore() { setSchedule(previous.getCopy()); }
	void setSchedule(Schedule s)
	{
		s.assertValidSchedule();
		initialized = true;
		current = s.getCopy();
		previous = s.getCopy();
		generator.setSchedule(s);
	}

	Schedule getBestNeighbor(unsigned oper)
	{
		//cout << wrapStringInLabel("searching for neighbor");
		auto ns = getNeighbors(oper);

		if (ns.size() == 0)
			return current;

		unsigned bestValue = ns[0].getMakespan(), bestIndex = 0, found;
		//cout << "\t" << bestValue << br;
		for (unsigned i = 1; i < ns.size(); i++)
		{
			found = ns[i].getMakespan();

			//cout << "\t" << found << br;
			if (found < bestValue)
			{
				bestValue = found;
				bestIndex = i;
			}
		}

		return ns[bestIndex];
	}
	vector<pair<Movement, Schedule>> getMovementsPerNeighbor(unsigned oper)
	{
		vector<pair<Movement, Schedule>> mpn;
		Movement mov;

		auto ps = generator.listAllPerturbations(oper);

		for (auto p : ps)
		{
			mov = applyPerturbation(p);
			if (current.getMakespan() > 0)
				mpn.push_back(make_pair(mov, current.getCopy()));
			restore();
		}

		return mpn;
	}
	Schedule getRandomNeighbor(unsigned oper)
	{
		assert(initialized);
		//cout << current.getSolutionString();
		//Printer::printJobCluster(current);
		auto p = generator.listAllPerturbations(oper);
		while (true)
		{
			//cout << "searching..." << br;
			//Printer::printJobCluster(current);
			auto i = p.size() == 1 ? 0 : randint(0, p.size() - 1);
			applyPerturbation(p[i]);

			if (current.getMakespan() > 0)
				break;
			else
				restore();
		}

		return current;
	}
	vector<Schedule> getNeighbors(unsigned oper)
	{
		assert(initialized);
		vector<Schedule> neighbors;

		auto perturbations = generator.listAllPerturbations(oper);
		applyPerturbations(perturbations, neighbors);

		return neighbors;
	}

	Schedule getSchedule() { return current; }
};

#endif