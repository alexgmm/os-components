#pragma once

#include "../helpers/utilities.hpp"
#include "../instance/instance.hpp"
#include "../perturbation/perturbation.hpp"

using namespace std;

void printEdge(pair<unsigned, unsigned> &e)
{
	cout << "[" << e.first << "," << e.second << "]" << br;
}

class Schedule
{
public:
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
	vector<unsigned> critical()
	{
		vector<unsigned> heads = getHeads(), cicle;
		if (heads.size() == 0)
			return cicle;
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
	void setFirst()
	{
		auto p = critical();
		if (p.size() > 0)
			first = p[0];
	}
	void assertValidSchedule()
	{
		assert(pJ.size() == nO + 1);
		assert(pM.size() == nO + 1);
		assert(sJ.size() == nO + 1);
		assert(sM.size() == nO + 1);
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
		setFirst();
	}
	void swapM(unsigned op1, unsigned op2)
	{
		if (TRACK_SWAP_OPERATIONS)
			cout << "swapM(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0 && op2 > 0);
		assert(op1 != op2);
		assert(adjMach(op1, op2));

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
		setFirst();
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
		first = 8;
		nO = 16;
		nJ = 4;
		nM = 4;

		pJ = {0, 2, 4, 1, 0, 6, 8, 5, 0, 10, 12, 9, 0, 14, 16, 13, 0};

		sJ = {0, 3, 1, 0, 2, 7, 5, 0, 6, 11, 9, 0, 10, 15, 13, 0, 14};

		pM = {0, 9, 10, 11, 12, 0, 0, 0, 0, 13, 14, 15, 16, 5, 6, 7, 8};

		sM = {0, 0, 0, 0, 0, 13, 14, 15, 16, 1, 2, 3, 4, 9, 10, 11, 12};

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
		vector<unsigned> late_pred(nO + 1, 0), cicle;
		if (heads.size() == 0)
			return cicle;

		for (int i = 1; i <= nO; i++)
		{
			if (heads[pM[i]] + instance.cost[pM[i]] > heads[pJ[i]] + instance.cost[pJ[i]])
				late_pred[i] = pM[i];
			else
				late_pred[i] = pJ[i];
		}
		return late_pred;
	}

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
		assertValidSchedule();
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

	void printSchedule()
	{
		int ini = 0;
		printv(sJ, ini, "Sucessores nos jobs:");
		printv(pJ, ini, "Predecessores nos jobs:");
		printv(sM, ini, "Sucessores nas maquinas:");
		printv(pM, ini, "Predecessores nas maquinas:");
		//printv(instance.cost, 1, "custo");
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
		s.assertValidSchedule();
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
	void printCriticalPath()
	{
		auto p = critical();
		printv(p, 0, "critical path:");
	}
	friend class Printer;
	friend class Heuristics;
	friend class TabuSearcher;
	friend class NeighborGenerator;
	friend class PerturbationGenerator;
	friend class IteratedLocalSearcher;
	friend class ScheduleDataRetriever;
};
