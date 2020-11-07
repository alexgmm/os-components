#include <cstdlib>
#include <fstream>
#include "src/instance.hpp"
#include "src/solution.hpp"
#include "src/tester.hpp"
#include "src/tabu_searcher.hpp"
#include "src/annealing_simulator.hpp"
#include "src/iterated_local_searcher.hpp"

using namespace std;

string t = "tests/test2";
string g = "inst/gueret-prins/GP03-02.txt";
string g7 = "inst/gueret-prins/GP07-09.txt";
string t4 = "inst/taillard/tai04_04_02.txt";
string t5 = "inst/taillard/tai05_05_01.txt";
string t7 = "inst/taillard/tai07_07_08.txt";
string t20 = "inst/taillard/tai20_20_01.txt";

void test();
int main(int argc, char **argv)
{
	if (argc > 1)
	{
		/* for (int i = 1; i < 10; i++)
			cout << i << " " << argv[i] << br; */

		/* string iterationsNumber(argv[4]), alpha(argv[6]), temperature(argv[8]), instanceFileName(argv[2]);
		Instance i(instanceFileName);
		Solution s(i, RANDOM);
		Heuristics h(s);
		h.setSimulatedAnnealingParams(stod(alpha), stod(temperature), stoi(iterationsNumber));
		unsigned r = h.solve(SA, SHIFT_CRITICAL); */

		/* string instanceFileName(argv[2]), duration(argv[4]);
		Instance i(instanceFileName);
		Solution s(i, RANDOM);
		Heuristics h(s);
		h.setTabuSearchParams(stoi(duration));
		unsigned r = h.solve(TS, SHIFT_WHOLE); */

		string instanceFileName(argv[2]), perturbationNumber(argv[4]);
		Instance i(instanceFileName);
		Solution s(i, RANDOM);
		IteratedLocalSearcher g(stoi(perturbationNumber), SWAP_CRITICAL_EDGE);
		g.setSolution(s);
		cout << g.solve();
	}
	else
	{
		VERBOSE = false;
		SAVE_GRAPHS_ON_SWAP = false;
		TRACK_SWAP_OPERATIONS = false;

		TRACK_SHIFT_OPERATIONS = true;
		SAVE_GRAPHS_ON_SHIFT = false;

		Instance i(t4);
		Solution s(i, RANDOM);
		s.printJobCluster();
		auto je = s.getJEdgesN5(), me = s.getMEdgesN5();
		for (auto e : je)
			cout << e.first << sp << e.second << br;
		for (auto e : me)
			cout << e.first << sp << e.second << br;
		/* unsigned initial = s.computeMakespan();
		//s.printJobCluster();
		IteratedLocalSearcher g(s, 8, SWAP_CRITICAL);
		cout << g.solve() << sp << initial << br; */
		//g.test();
	}

	return 0;
}

void test()
{
	//printAverageExecutionTime(SA, SWAP_CRITICAL_EDGE);
	Instance i(t20);
	Solution s(i, RANDOM);
	s.printJobCluster();
	cout << s.getHighestCostFromMachine() << br;
	/* Heuristics h(s);
	h.solve(SA, SHIFT_CRITICAL); */
}