#ifndef SOLUTION_HPP
#define SOLUTION_HPP

#include "utilities.hpp"
#include "instance.hpp"
#include "perturbation.hpp"

using namespace std;

void printEdge(pair<unsigned, unsigned> &e)
{
	cout << "[" << e.first << "," << e.second << "]" << br;
}

class Schedule
{
	vector<unsigned> sJ, pJ, sM, pM;
	unsigned nM, nJ, nO, last, first;
	int makespan = 0;
	Instance instance;
	bool partial = false;

	unsigned op(unsigned j, unsigned m)
	{
		if (j == 0 || m == 0)
			return 0;
		assert(j >= 0);
		assert(m >= 0);
		return (j - 1) * nM + m;
	}
	void isOpValid(unsigned op)
	{
		if (!partial)
			assert(op <= nJ * nJ && op <= nM * nM);
		assert(op > 0);
	}

	/////////////////////////////////////////////////////////////////////////
	/////////////////////////// MOVEMENT APPLIERS ///////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void swapJ(unsigned op1, unsigned op2)
	{
		if (TRACK_SWAP_OPERATIONS)
			cout << "swapJ(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0 && op2 > 0);
		assert(op1 != op2);
		assert(adjJob(op1, op2));

		if (op1 == first)
			first = op2;

		if (sJ[op2] == op1)
		{
			unsigned o = op1;
			op1 = op2;
			op2 = o;
		}

		unsigned jp = pJ[op1], js = sJ[op2];

		if (jp)
			sJ[jp] = op2;
		if (js)
			pJ[js] = op1;

		sJ[op1] = js;
		pJ[op1] = op2;
		pJ[op2] = jp;
		sJ[op2] = op1;

		computeMakespan();
	}
	void swapM(unsigned op1, unsigned op2)
	{
		if (TRACK_SWAP_OPERATIONS)
			cout << "swapM(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0 && op2 > 0);
		assert(op1 != op2);
		assert(adjMach(op1, op2));

		if (op1 == first)
			first = op2;

		if (sM[op2] == op1)
		{
			unsigned o = op1;
			op1 = op2;
			op2 = o;
		}

		unsigned mp = pM[op1], ms = sM[op2];

		if (mp)
			sM[mp] = op2;
		if (ms)
			pM[ms] = op1;

		sM[op1] = ms;
		pM[op1] = op2;
		pM[op2] = mp;
		sM[op2] = op1;

		computeMakespan();
	}
	void shiftJ(vector<unsigned> &p, unsigned idxBegin, unsigned idxEnd)
	{
		assert(idxBegin != idxEnd);
		if (TRACK_SHIFT_OPERATIONS)
			cout << br << "shiftJ(" << p[idxBegin] << "," << p[idxEnd] << ")" << br;
		unsigned lastOp = p[idxEnd];
		for (unsigned i = idxEnd; i > idxBegin; i--)
			swapJ(lastOp, p[i - 1]);
	}
	void shiftM(vector<unsigned> &p, unsigned idxBegin, unsigned idxEnd)
	{
		assert(idxBegin != idxEnd);
		if (TRACK_SHIFT_OPERATIONS)
			cout << br << "shiftM(" << p[idxBegin] << "," << p[idxEnd] << ")" << br;
		unsigned lastOp = p[idxEnd];
		for (unsigned i = idxEnd; i > idxBegin; i--)
			swapM(lastOp, p[i - 1]);
	}

	bool sameJob(unsigned op1, unsigned op2) { return instance.o_job[op1] == instance.o_job[op2]; }
	bool sameMach(unsigned op1, unsigned op2) { return instance.o_machine[op1] == instance.o_machine[op2]; }
	bool adjJob(unsigned op1, unsigned op2) { return pJ[op1] == op2 || pJ[op2] == op1; }
	bool adjMach(unsigned op1, unsigned op2) { return pM[op1] == op2 || pM[op2] == op1; }

	/////////////////////////////////////////////////////////////////////////
	///////////////////////////// INITIALIZERS //////////////////////////////
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
	{
		if (tM < bM)
		{
			bV = tV;
			bM = tM;
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

		partial = true;
		for (int jobSlot = nJ; jobSlot > 0; jobSlot--)
		{

			bestMakespan = UMAX;
			for (unsigned testJob = 1; testJob <= nJ; testJob++)
			{
				if (used[testJob])
					continue;

				oJ[jobSlot] = testJob;
				fillJobSlot(oJ, oM, testJob, jobSlot);
				first = op(testJob, oM[1]);

				tempMakespan = computePartialMakespan();

				evaluateResult(tempMakespan, bestMakespan, testJob, bestJob);
				resetJobSlot(oM, testJob);
			}
			oJ[jobSlot] = bestJob;
			used[bestJob]++;
			first = op(bestJob, oM[1]);
			fillJobSlot(oJ, oM, bestJob, jobSlot);
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

		partial = true;
		for (int machSlot = nM; machSlot > 0; machSlot--)
		{

			bestMakespan = UMAX;
			for (unsigned testMach = 1; testMach <= nM; testMach++)
			{
				if (used[testMach])
					continue;

				oM[machSlot] = testMach;
				fillMachSlot(oJ, oM, testMach, machSlot);

				first = op(oJ[1], testMach);

				tempMakespan = computePartialMakespan();

				evaluateResult(tempMakespan, bestMakespan, testMach, bestMach);
				resetMachSlot(oJ, testMach);
			}
			oM[machSlot] = bestMach;
			used[bestMach]++;
			first = op(oJ[1], bestMach);
			fillMachSlot(oJ, oM, bestMach, machSlot);
		}
		partial = false;
		makespan = bestMakespan;
		fillOrd(oJ, oM);
	}
	void initTest()
	{
		first = 2;
		nO = 16;
		nJ = 4;
		nM = 4;

		pJ = {0, 2, 0, 1, 3, 6, 0, 5, 7, 10, 0, 9, 11, 14, 0, 13, 15};

		sJ = {0, 3, 1, 4, 0, 7, 5, 8, 0, 11, 9, 12, 0, 15, 13, 16, 0};

		pM = {0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

		sM = {0, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0, 0, 0, 0};

		instance.cost = {0, 45, 80, 29, 74, 83, 45, 56, 45, 70, 58, 29, 64, 5, 24, 61, 43};
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
	//////////////////////////  MAKESPAN HELPERS ////////////////////////////
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
		if (heads.size() == 0)
		{
			cout << "(!) Invalid heads (probable cicle)\n";
			assert(heads.size() > 0);
		}
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

public:
	Schedule() {}
	Schedule(Instance i, int init_method)
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

		if (heads.size() == 0)
			return -1;

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
	int getMakespan() { return partial ? computePartialMakespan() : computeMakespan(); }
	unsigned getLowerBound()
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

	string getSolutionString()
	{
		string info = "first = " + to_string(first) + ";" + br;
		info += "nO = " + to_string(nO) + ";" + br;
		info += "nJ = " + to_string(nJ) + ";" + br;
		info += "nM = " + to_string(nM) + ";" + br + br;
		string pJStr = getVectorString(pJ, "pJ");
		string sJStr = getVectorString(sJ, "sJ");
		string pMStr = getVectorString(pM, "pM");
		string sMStr = getVectorString(sM, "sM");
		string costStr = getVectorString(instance.cost, "instance.cost");
		return info + pJStr + sJStr + pMStr + sMStr + costStr;
	}
	void setSchedule(Schedule s)
	{
		nO = s.nO;
		nJ = s.nJ;
		nM = s.nM;
		pJ = s.pJ;
		sJ = s.sJ;
		pM = s.pM;
		sM = s.sM;
		first = s.first;
		last = s.last;
		instance = s.instance;
		makespan = s.computeMakespan();
	}
	Schedule getCopy()
	{
		Schedule s;
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

	friend class Printer;
	friend class Heuristics;
	friend class TabuSearcher;
	friend class NeighborGenerator;
	friend class PerturbationGenerator;
	friend class IteratedLocalSearcher;
	friend class ScheduleDataRetriever;
};
class ScheduleDataRetriever : public Schedule
{
public:
	unsigned getJobAncestor(unsigned o)
	{
		while (pJ[o] != 0)
			o = pJ[o];
		return o;
	}
	unsigned getMachAncestor(unsigned o)
	{
		while (pM[o] != 0)
			o = pM[o];
		return o;
	}
	unsigned getHighestCostFromMachine()
	{
		vector<vector<unsigned>> machs = getWholeMachines();
		unsigned highestCost = 0, costSum;
		for (vector<unsigned> mach : machs)
		{
			costSum = 0;
			for (unsigned o : mach)
				costSum += instance.cost[o];
			if (costSum > highestCost)
				highestCost = costSum;
		}

		return highestCost;
	}
	unsigned getHighestCostFromJob()
	{
		vector<vector<unsigned>> jobs = getWholeJobs();
		unsigned highestCost = 0, costSum;
		for (vector<unsigned> job : jobs)
		{
			costSum = 0;
			for (unsigned o : job)
				costSum += instance.cost[o];
			if (costSum > highestCost)
				highestCost = costSum;
		}

		return highestCost;
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
	vector<vector<unsigned>> getCriticalJBlocks()
	{
		vector<vector<unsigned>> jBlocks;
		vector<unsigned> p = critical(), block;
		unsigned index = 1, o;
		block.push_back(p[0]);

		while (true)
		{
			o = p[index++];
			if (!sameJob(o, p[index - 2]))
			{
				jBlocks.push_back(block);
				block.clear();
			}
			block.push_back(o);
			if (index == p.size())
			{
				jBlocks.push_back(block);
				break;
			}
		}

		return jBlocks;
	}
	vector<vector<unsigned>> getCriticalMBlocks()
	{
		vector<vector<unsigned>> mBlocks;
		vector<unsigned> p = critical(), block;
		unsigned index = 1, o;
		block.push_back(p[0]);

		while (true)
		{
			o = p[index++];
			if (!sameMach(o, p[index - 2]))
			{
				mBlocks.push_back(block);
				block.clear();
			}
			block.push_back(o);
			if (index == p.size())
			{
				mBlocks.push_back(block);
				break;
			}
		}

		return mBlocks;
	}
	void filterNonTrivialBlocks(vector<vector<unsigned>> &blocks)
	{
		vector<vector<unsigned>> filtered;

		for (auto block : blocks)
			if (block.size() > 1)
				filtered.push_back(block);

		blocks = filtered;
	}
	vector<unsigned> getRandomJBlock()
	{
		vector<unsigned> p = critical(), begins, ends, block;
		blocks(begins, ends, BLOCK_J);

		if (begins.size() <= 0)
			return begins;

		unsigned i = (begins.size() == 1) ? 0 : randint(0, begins.size() - 1);

		for (unsigned index = begins[i]; index <= ends[i]; index++)
			block.push_back(p[index]);
		return block;
	}
	vector<unsigned> getRandomMBlock()
	{
		vector<unsigned> p = critical(), begins, ends, block;
		blocks(begins, ends, BLOCK_M);

		if (begins.size() <= 0)
			return begins;

		unsigned i = (begins.size() == 1) ? 0 : randint(0, begins.size() - 1);

		for (unsigned index = begins[i]; index <= ends[i]; index++)
			block.push_back(p[index]);
		return block;
	}
	vector<unsigned> getRandomCriticalBlock()
	{
		vector<unsigned> block;
		if (randint(0, 1) == BLOCK_J)
		{
			block = getRandomJBlock();
			if (block.size() == 0)
				block = getRandomMBlock();
		}
		else
		{
			block = getRandomMBlock();
			if (block.size() == 0)
				block = getRandomJBlock();
		}
		return block;
	}
	vector<unsigned> getOperationsJBlock(unsigned o)
	{
		vector<unsigned> block;

		unsigned operation = getJobAncestor(o);

		do
		{
			block.push_back(operation);
			operation = sJ[operation];
		} while (operation != sJ[o]);

		return block;
	}
	vector<unsigned> getOperationsMBlock(unsigned o)
	{
		vector<unsigned> block;

		unsigned operation = getMachAncestor(o);

		do
		{
			block.push_back(operation);
			operation = sM[operation];
		} while (operation != sM[o]);

		return block;
	}
	vector<unsigned> getOperationsCriticalJBlock(unsigned o)
	{
		vector<unsigned> p = critical(), block;
		unsigned operation, index = findIndex(p, o);

		if (index > nO)
			return block;
		while (index <= nO)
		{
			operation = p[index];
			if (!sameJob(o, operation))
				break;
			block.push_back(operation);
			index--;
		}

		reverse(block.begin(), block.end());

		return block;
	}
	vector<unsigned> getOperationsCriticalMBlock(unsigned o)
	{
		vector<unsigned> p = critical(), block;
		unsigned operation, index = findIndex(p, o);

		if (index > nO)
			return block;
		while (index <= nO)
		{
			operation = p[index];
			if (!sameMach(o, operation))
				break;
			block.push_back(operation);
			index--;
		}

		reverse(block.begin(), block.end());

		return block;
	}
	vector<unsigned> getCriticalOperationsList()
	{
		vector<unsigned> list(nO + 1, 0);
		vector<unsigned> p = critical();

		for (unsigned o : p)
			list[o] = 1;

		return list;
	}
	vector<unsigned> getStarters(vector<unsigned> predecessors)
	{
		vector<unsigned> s;

		for (unsigned i = 1; i < predecessors.size(); i++)
			if (predecessors[i] == DUMMY)
				s.push_back(i);

		return s;
	}
	vector<vector<unsigned>> getWholeMachines()
	{
		vector<vector<unsigned>> machines;
		vector<unsigned> mStarters = getStarters(pM), block;
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
	vector<vector<unsigned>> getWholeJobs()
	{
		vector<vector<unsigned>> jobs;
		vector<unsigned> jStarters = getStarters(pJ), block;
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
		unsigned o1 = p[i1], o2 = p[i2];
		if (o1 == first && !adjJob(o2, p[i2 + 1]))
			return true;
		if (o1 == first || o2 == last || !sameJob(o1, o2))
			return false;
		if (!sameJob(o1, p[i1 - 1]) || !sameJob(o2, p[i2 + 1]))
			return true;
		return false;
	}
	bool isMEdgeN5(unsigned i1, unsigned i2, vector<unsigned> &p)
	{
		unsigned o1 = p[i1], o2 = p[i2];
		if (o1 == first && !adjMach(o2, p[i2 + 1]))
			return true;
		if (o1 == first || o2 == last || !sameMach(o1, o2))
			return false;
		if (!sameMach(o1, p[i1 - 1]) || !sameMach(o2, p[i2 + 1]))
			return true;
		return false;
	}
	vector<pair<unsigned, unsigned>> getJEdgesN5()
	{
		vector<pair<unsigned, unsigned>> edges;
		auto jBlocks = getCriticalJBlocks();
		filterNonTrivialBlocks(jBlocks);
		unsigned lastIdx;

		for (auto b : jBlocks)
		{
			lastIdx = b.size() - 1;

			if (b.size() == 2)
				edges.push_back(make_pair(b[0], b[1]));
			else
			{
				if (b[0] != first)
					edges.push_back(make_pair(b[0], b[1]));
				if (b[lastIdx] != last)
					edges.push_back(make_pair(b[lastIdx - 1], b[lastIdx]));
			}
		}

		return edges;
	}
	vector<pair<unsigned, unsigned>> getMEdgesN5()
	{
		vector<pair<unsigned, unsigned>> edges;
		auto mBlocks = getCriticalMBlocks();
		filterNonTrivialBlocks(mBlocks);
		unsigned lastIdx;

		for (auto b : mBlocks)
		{
			lastIdx = b.size() - 1;

			if (b.size() == 2)
				edges.push_back(make_pair(b[0], b[1]));
			else
			{
				if (b[0] != first)
					edges.push_back(make_pair(b[0], b[1]));
				if (b[lastIdx] != last)
					edges.push_back(make_pair(b[lastIdx - 1], b[lastIdx]));
			}
		}

		return edges;
	}
	bool isN7ShiftAllowed(unsigned o1, unsigned o2, unsigned blockSize)
	{
		bool areAdjacent = adjJob(o1, o2) || adjMach(o1, o2);
		if (blockSize == 2 && areAdjacent)
			return true;

		bool isFirstEdge = areAdjacent && (o1 == first || o2 == first);
		bool isLastEdge = areAdjacent && (o1 == last || o2 == last);

		return !isFirstEdge && !isLastEdge;
	}

	unsigned getOneRandomOperation_swapCritical()
	{
		vector<unsigned> p = critical();
		unsigned o, pathIndex;
		pathIndex = randint(0, p.size() - 1);
		o = p[pathIndex];

		return o;
	}
	unsigned getOneRandomOperation_swapCriticalEdge()
	{
		vector<unsigned> block;
		unsigned o = 0;

		block = getRandomCriticalBlock();

		if (block.size() <= 0)
			return 0;

		if (randint(0, 1) == BLOCK_START)
			o = block[0];
		else
			o = block[block.size() - 1];

		return o;
	}
	unsigned getOneRandomOperation_shiftWhole()
	{
		unsigned o = 0;

		do
			o = randint(1, nO);
		while (o == first);

		return o;
	}
	unsigned getOneRandomOperation_shiftCritical()
	{
		vector<unsigned> begins, ends, p = critical();

		unsigned blockType = randint(0, 1), o;
		blocks(begins, ends, blockType);
		if (ends.size() == 1)
		{
			begins.clear();
			ends.clear();
			blocks(begins, ends, (blockType + 1) % 2);
		}

		if (ends.size() == 1)
			return p[p.size() - 2];
		unsigned index = ends.size() == 2 ? 0 : randint(0, ends.size() - 2);
		o = p[ends[index]];

		return o;
	}
	unsigned getOneRandomOperation(unsigned neighborType)
	{
		switch (neighborType)
		{
		case SWAP_CRITICAL:
			return getOneRandomOperation_swapCritical();
		case SWAP_CRITICAL_EDGE:
			return getOneRandomOperation_swapCriticalEdge();
		case SHIFT_WHOLE:
			return getOneRandomOperation_shiftWhole();
		case SHIFT_CRITICAL:
			return getOneRandomOperation_shiftCritical();
		default:
			return UMAX;
		}
	}

	vector<unsigned> getAllOperations_swapCritical()
	{
		return critical();
	}
	vector<unsigned> getAllOperations_swapCriticalEdge()
	{
		vector<unsigned> ops, p = critical();

		vector<vector<unsigned>> jB = getCriticalJBlocks(), mB = getCriticalMBlocks();
		filterNonTrivialBlocks(jB);
		filterNonTrivialBlocks(mB);

		for (auto bs : {jB, mB})
			for (auto b : bs)
				ops.push_back(b[b.size() - 1]);
		if (ops.size() > 0)
			ops.pop_back();

		return ops;
	}
	vector<unsigned> getAllOperations_shiftWhole()
	{
		vector<unsigned> operations;

		for (unsigned i = 1; i < nO; i++)
			if (i != first)
				operations.push_back(i);

		return operations;
	}
	vector<unsigned> getAllOperations_shiftCritical()
	{
		return critical();
	}
	vector<unsigned> getAllOperations(unsigned neighborType)
	{
		switch (neighborType)
		{
		case SWAP_CRITICAL:
			return getAllOperations_swapCritical();
		case SWAP_CRITICAL_EDGE:
			return getAllOperations_swapCriticalEdge();
		case SHIFT_WHOLE:
			return getAllOperations_shiftWhole();
		case SHIFT_CRITICAL:
			return getAllOperations_shiftCritical();
		}
	}

	ScheduleDataRetriever() {}
	ScheduleDataRetriever(Schedule s)
	{
		setSchedule(s);
	}
	ScheduleDataRetriever(Instance i, int method) : Schedule(i, method) {}

	friend class NeighborGenerator;
};
class PerturbationGenerator : public ScheduleDataRetriever
{
public:
	PerturbationGenerator() {}
	PerturbationGenerator(Schedule s)
	{
		setSchedule(s);
	}
	Schedule getSchedule()
	{
		return getCopy();
	}

	vector<Perturbation> getCriticalJBlockShifts(vector<unsigned> &block, unsigned o)
	{
		vector<Perturbation> possiblePerturbations;

		int factorIndex = block.size() - 2;
		unsigned opIndex = factorIndex + 1;
		Perturbation perturbation;

		while (factorIndex != -1)
		{
			if (isN7ShiftAllowed(o, block[factorIndex], block.size()))
			{
				perturbation = {o, SHIFT_CRITICAL, BLOCK_J, opIndex - factorIndex};
				possiblePerturbations.push_back(perturbation);
			}
			factorIndex--;
		}

		return possiblePerturbations;
	}
	vector<Perturbation> getCriticalMBlockShifts(vector<unsigned> &block, unsigned o)
	{
		vector<Perturbation> possiblePerturbations;

		int factorIndex = block.size() - 2;
		unsigned opIndex = factorIndex + 1;
		Perturbation perturbation;

		while (factorIndex != -1)
		{
			if (isN7ShiftAllowed(o, block[factorIndex], block.size()))
			{
				perturbation = {o, SHIFT_CRITICAL, BLOCK_M, opIndex - factorIndex};
				possiblePerturbations.push_back(perturbation);
			}
			factorIndex--;
		}

		return possiblePerturbations;
	}
	vector<Perturbation> getCriticalJBlockShifts(vector<unsigned> &block)
	{
		vector<Perturbation> ps;
		Perturbation p;
		unsigned factor;

		for (unsigned opIndex = block.size() - 1; opIndex > 0; opIndex--)
		{
			factor = opIndex;
			while (factor > 0)
			{
				if (isN7ShiftAllowed(block[opIndex], block[opIndex - factor], block.size()))
				{
					p = {block[opIndex], SHIFT_CRITICAL, BLOCK_J, factor};
					ps.push_back(p);
				}
				factor--;
			}
		}

		return ps;
	}
	vector<Perturbation> getCriticalMBlockShifts(vector<unsigned> &block)
	{
		vector<Perturbation> ps;
		Perturbation p;
		unsigned factor;

		for (unsigned opIndex = block.size() - 1; opIndex > 0; opIndex--)
		{
			factor = opIndex;
			while (factor > 0)
			{
				if (isN7ShiftAllowed(block[opIndex], block[opIndex - factor], block.size()))
				{
					p = {block[opIndex], SHIFT_CRITICAL, BLOCK_M, factor};
					ps.push_back(p);
				}
				factor--;
			}
		}

		return ps;
	}
	vector<Perturbation> getJobShifts(vector<unsigned> &job)
	{
		vector<Perturbation> ps;
		Perturbation p;
		unsigned factor;

		for (unsigned opIndex = job.size() - 1; opIndex > 0; opIndex--)
		{
			factor = opIndex;
			while (factor > 0)
			{
				p = {job[opIndex], SHIFT_WHOLE, BLOCK_J, factor--};
				ps.push_back(p);
			}
		}

		return ps;
	}
	vector<Perturbation> getMachineShifts(vector<unsigned> &mach)
	{
		vector<Perturbation> ps;
		Perturbation p;
		unsigned factor;

		for (unsigned opIndex = mach.size() - 1; opIndex > 0; opIndex--)
		{
			factor = opIndex;
			while (factor > 0)
			{

				p = {mach[opIndex], SHIFT_WHOLE, BLOCK_M, factor--};
				ps.push_back(p);
			}
		}

		return ps;
	}

	unsigned getSwapOperationFromPerturbation(Perturbation p)
	{
		vector<unsigned> block;
		if (p.blockType == BLOCK_J)
			block = p.perturbationType == SWAP_PRED ? pJ : sJ;
		else
			block = p.perturbationType == SWAP_PRED ? pM : sM;
		return block[p.operation];
	}
	void assertValidSwapPerturbation(Perturbation p)
	{
		auto o1 = p.operation;
		auto o2 = getSwapOperationFromPerturbation(p);
		auto isCriticalOperation = getCriticalOperationsList();
		if (o1 == 0 || o2 == 0 || isCriticalOperation[o1] == 0 || isCriticalOperation[o2] == 0)
		{
			cout << "(!) Invalid perturbation: " << getPerturbationString(p);
			exit(1);
		}
	}

	vector<Perturbation> listAllPerturbations_swapCritical()
	{
		vector<Perturbation> perturbations;
		Perturbation p;

		auto jB = getCriticalJBlocks(), mB = getCriticalMBlocks();
		filterNonTrivialBlocks(jB);
		filterNonTrivialBlocks(mB);

		for (auto b : jB)
			for (int i = 0; i < b.size() - 1; i++)
			{
				p = {b[i], SWAP_SUCC, BLOCK_J, 0};
				perturbations.push_back(p);
			}

		for (auto b : mB)
			for (int i = 0; i < b.size() - 1; i++)
			{
				p = {b[i], SWAP_SUCC, BLOCK_M, 0};
				perturbations.push_back(p);
			}

		return perturbations;
	}
	vector<Perturbation> listAllPerturbations_swapCriticalEdge()
	{
		vector<Perturbation> perturbations;
		Perturbation p;
		auto jE = getJEdgesN5(), mE = getMEdgesN5();

		for (auto e : jE)
		{
			p = {e.first, SWAP_SUCC, BLOCK_J, 0};
			perturbations.push_back(p);
		}

		for (auto e : mE)
		{
			p = {e.first, SWAP_SUCC, BLOCK_M, 0};
			perturbations.push_back(p);
		}

		return perturbations;
	}
	vector<Perturbation> listAllPerturbations_shiftCritical()
	{
		vector<Perturbation> perturbations;

		auto jB = getCriticalJBlocks(), mB = getCriticalMBlocks();

		filterNonTrivialBlocks(jB);
		filterNonTrivialBlocks(mB);

		for (auto b : jB)
		{
			auto p = getCriticalJBlockShifts(b);
			perturbations.insert(perturbations.end(), p.begin(), p.end());
		}

		for (auto b : mB)
		{
			auto p = getCriticalMBlockShifts(b);
			perturbations.insert(perturbations.end(), p.begin(), p.end());
		}

		return perturbations;
	}
	vector<Perturbation> listAllPerturbations_shiftWhole()
	{
		vector<Perturbation> ps;

		auto jobs = getWholeJobs(), machs = getWholeMachines();

		for (auto job : jobs)
		{
			auto p = getJobShifts(job);
			ps.insert(ps.end(), p.begin(), p.end());
		}

		for (auto mach : machs)
		{
			auto p = getMachineShifts(mach);
			ps.insert(ps.end(), p.begin(), p.end());
		}

		return ps;
	}

	vector<Perturbation> listPossiblePerturbations_swapCritical(unsigned o)
	{
		vector<Perturbation> possiblePerturbations;
		auto isCriticalOperation = getCriticalOperationsList();
		Perturbation perturbation;

		if (isCriticalOperation[pM[o]])
		{
			perturbation = {o, SWAP_PRED, BLOCK_M, 0};
			possiblePerturbations.push_back(perturbation);
		}

		if (isCriticalOperation[sM[o]])
		{
			perturbation = {o, SWAP_SUCC, BLOCK_M, 0};
			possiblePerturbations.push_back(perturbation);
		}

		if (isCriticalOperation[pJ[o]])
		{
			perturbation = {o, SWAP_PRED, BLOCK_J, 0};
			possiblePerturbations.push_back(perturbation);
		}

		if (isCriticalOperation[sJ[o]])
		{
			perturbation = {o, SWAP_SUCC, BLOCK_J, 0};
			possiblePerturbations.push_back(perturbation);
		}

		return possiblePerturbations;
	}
	vector<Perturbation> listPossiblePerturbations_swapCriticalEdge(unsigned o)
	{
		vector<Perturbation> possiblePerturbations;
		Perturbation p;
		unsigned pType;

		auto jEdges = getJEdgesN5(), mEdges = getMEdgesN5();

		for (auto e : jEdges)
		{
			p = {e.first, SWAP_SUCC, BLOCK_J, 0};
			possiblePerturbations.push_back(p);
		}

		for (auto e : mEdges)
		{
			p = {e.first, SWAP_SUCC, BLOCK_M, 0};
			possiblePerturbations.push_back(p);
		}

		return possiblePerturbations;
	}
	vector<Perturbation> listPossiblePerturbations_shiftWhole(unsigned o)
	{
		vector<Perturbation> possiblePerturbations;
		Perturbation m;
		unsigned pred = pJ[o], factor = 1;

		while (pred != 0)
		{
			m = {o, SHIFT_WHOLE, BLOCK_J, factor++};
			possiblePerturbations.push_back(m);
			pred = pJ[pred];
		}
		pred = pM[o];
		factor = 1;
		while (pred != 0)
		{
			m = {o, SHIFT_WHOLE, BLOCK_M, factor++};
			possiblePerturbations.push_back(m);
			pred = pM[pred];
		}

		return possiblePerturbations;
	}
	vector<Perturbation> listPossiblePerturbations_shiftCritical(unsigned o)
	{
		vector<Perturbation> possiblePerturbations;

		auto j = getOperationsCriticalJBlock(o);
		if (j.size() > 1)
		{
			auto p = getCriticalJBlockShifts(j, o);
			possiblePerturbations.insert(possiblePerturbations.end(), p.begin(), p.end());
		}

		auto m = getOperationsCriticalMBlock(o);
		if (m.size() > 1)
		{
			auto p = getCriticalMBlockShifts(m, o);
			possiblePerturbations.insert(possiblePerturbations.end(), p.begin(), p.end());
		}

		return possiblePerturbations;
	}

	vector<Perturbation> listPossiblePerturbations(unsigned o, unsigned neighborType)
	{
		switch (neighborType)
		{
		case SWAP_CRITICAL:
			return listPossiblePerturbations_swapCritical(o);
		case SWAP_CRITICAL_EDGE:
			return listPossiblePerturbations_swapCriticalEdge(o);
		case SHIFT_WHOLE:
			return listPossiblePerturbations_shiftWhole(o);
		case SHIFT_CRITICAL:
			return listPossiblePerturbations_shiftCritical(o);
		}
	}
	vector<Perturbation> listAllPerturbations(unsigned neighborType)
	{
		switch (neighborType)
		{
		case SWAP_CRITICAL:
			return listAllPerturbations_swapCritical();
		case SWAP_CRITICAL_EDGE:
			return listAllPerturbations_swapCriticalEdge();
		case SHIFT_WHOLE:
			return listAllPerturbations_shiftWhole();
		case SHIFT_CRITICAL:
			return listAllPerturbations_shiftCritical();
		}
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
};

#endif