#include <iostream>
#include <vector>
#include "instance.hpp"
#include "solution.hpp"
#include "tests.hpp"

using namespace std;

int main(){
	/* Instance i("test"); //i.print();
	Solution s(i, GREEDY_MACHINES);
	cout <<br<< s.calc_makespan()<<br; */

	test(RANDOM);
	test(GREEDY_JOBS);
	test(GREEDY_MACHINES);
	return 0;
}
//Otimizar a ordem nas maquinas:
//-Sortear uma ordem pros jobs
//-Para cada maquina, 

/* unsigned computeMakes(TopoData & topoData, vector<unsigned> & heads, vector<unsigned> & prev, unsigned & lastOp) const {
		assert(isAlloced());
		assert(heads.size() == osinst.nO);
		assert(topoData.indeg.size() == osinst.nO);
		assert(topoData.Q.size() == osinst.nO);
		assert(prev.size() == osinst.nO);

		unsigned qInsert = 0;
		unsigned qAccess = 0;

		unsigned curOp;
		unsigned newOp;

		unsigned newMax;
		unsigned makespan = 0;

		fill(heads.begin(), heads.end(), 0);
		fill(topoData.indeg.begin(), topoData.indeg.end(), 0);
		fill(prev.begin(), prev.end(), 0);

		//initializing indeg - puting minimal elements in queue
		for (unsigned o = 1; o < osinst.nO; o++) {
			if (_job[o] != 0) topoData.indeg[o]++;
			if (_mach[o] != 0) topoData.indeg[o]++;
			if (topoData.indeg[o] == 0) topoData.Q[qInsert++] = o;
		}

		assert(qInsert > 0); //Q can only be empty if there is a cycle

		while (qAccess < qInsert) {
			assert(qAccess < topoData.Q.size()); // each operation can get into the queue once and only once

			curOp = topoData.Q[qAccess++];
			assert(topoData.indeg[curOp] == 0); //all predecessors of curOp already vizited
			assert(curOp != 0); //curOp is not dummy
			newMax = heads[curOp] + osinst.P[curOp];
			if (makespan < newMax) {
				makespan = newMax;
				lastOp = curOp;
			}
			

			//job order
			newOp = job[curOp];
			if (newOp != 0) {
				assert(topoData.indeg[newOp] > 0);
				topoData.indeg[newOp]--;
				if (topoData.indeg[newOp] == 0) {
					assert(qInsert < topoData.Q.size() - 1);
					topoData.Q[qInsert++] = newOp;
				}
				if (heads[newOp] < newMax) {
					heads[newOp] = newMax;
					prev[newOp] = curOp;
				}
			}
			//mach order
			newOp = mach[curOp];
			if (newOp != 0) {
				assert(topoData.indeg[newOp] > 0);
				topoData.indeg[newOp]--;
				if (topoData.indeg[newOp] == 0) {
					assert(qInsert < topoData.Q.size() - 1);
					topoData.Q[qInsert++] = newOp;
				}
				if (heads[newOp] < newMax) {
					heads[newOp] = newMax;
					prev[newOp] = curOp;
				}
			}
		}//while (qAccess < qInsert)
		assert(qAccess == osinst.nO - 1);
		assert(lastOp != 0);

		return makespan;
	} */