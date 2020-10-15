#include <cstdlib>
#include <fstream>
#include "src/instance.hpp"
#include "src/solution.hpp"
#include "src/tests.hpp"
#include "src/heuristics.hpp"
#include "src/time.hpp"

using namespace std;

string t = "tests/test2";
string g = "inst/gueret-prins/GP03-02.txt";
string t4 = "inst/taillard/tai04_04_01.txt";
string t5 = "inst/taillard/tai05_05_01.txt";
string t20 = "inst/taillard/tai20_20_01.txt";

void test();
int main(int argc, char **argv)
{
	startTimeCounting();
	if (argc > 1)
	{
		/* for (int i = 1; i < 10; i++)
			cout << i << " " << argv[i] << br; */

		string iterationsNumber(argv[4]), alpha(argv[6]), temperature(argv[8]), instanceFileName(argv[2]);
		Instance i(instanceFileName);
		Solution s(i, RANDOM);
		Heuristics h(s);
		h.setSimulatedAnnealingParams(stod(alpha), stod(temperature), stoi(iterationsNumber));
		unsigned r = h.solve(SA, SHIFT_WHOLE);
		cout << r;
	}
	else
	{
		VERBOSE = false;
		SAVE_GRAPHS_ON_SWAP = false;
		TRACK_SWAP_OPERATIONS = false;

		TRACK_SHIFT_OPERATIONS = true;
		SAVE_GRAPHS_ON_SHIFT = true;
		test();
	}

	return 0;
}

void test()
{
	//printAverageExecutionTime(SA, SWAP_CRITICAL_EDGE);
	Instance i(t4);
	Solution s(i, RANDOM);
	s.printJobCluster();
	vector<unsigned> p = s.critical(), bj, ej, bm, em;
	s.blocks(bj, ej, BLOCK_J);
	s.blocks(bm, em, BLOCK_M);
	for (int i = 0; i < bj.size(); i++)
		cout << p[bj[i]] << "--" << p[ej[i]] << br;
	cout << br;
	for (int i = 0; i < bm.size(); i++)
		cout << p[bm[i]] << "--" << p[em[i]] << br;
	//Neighborhood n(s);
	//vector<Neighbor> v = n.getNeighborhood(SHIFT_CRITICAL);
}