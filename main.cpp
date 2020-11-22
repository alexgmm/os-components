#include <cstdlib>
#include <fstream>
#include "src/result_generators/initializer_rg.hpp"
#include "src/instance/instance.hpp"
#include "src/instance/instance_file_reader.hpp"
#include "src/schedule/schedule.hpp"
#include "src/heuristics/tabu_searcher.hpp"
#include "src/heuristics/annealing_simulator.hpp"
#include "src/heuristics/iterated_local_searcher.hpp"

using namespace std;

string t = "tests/test2";
string g = "inst/gueret-prins/GP03-02.txt";
string g7 = "inst/gueret-prins/GP07-09.txt";
string t4 = "inst/taillard/tai04_04_02.txt";
string t5 = "inst/taillard/tai05_05_01.txt";
string t7 = "inst/taillard/tai07_07_08.txt";
string t20 = "inst/taillard/tai20_20_01.txt";

int main(int argc, char **argv)
{
	if (argc > 1)
	{
		/* for (int i = 1; i < 10; i++)
			cout << i << " " << argv[i] << br; */

		/* string iterationsNumber(argv[4]), alpha(argv[6]), temperature(argv[8]), instanceFileName(argv[2]);
		Instance i(instanceFileName);
		Schedule s(i, RANDOM);
		AnnealingSimulator ls(SWAP_CRITICAL_EDGE);
		ls.setParams(stoi(alpha), stod(temperature), stoi(iterationsNumber));
		ls.setSolution(s);
		cout << ls.solve() << br; */

		/* string instanceFileName(argv[2]), duration(argv[4]);
		Instance i(instanceFileName);
		Solution s(i, RANDOM);
		Heuristics h(s);
		h.setTabuSearchParams(stoi(duration));
		unsigned r = h.solve(TS, SHIFT_WHOLE); */

		string instanceFileName(argv[2]), perturbationNumber(argv[4]);
		Instance i(instanceFileName);
		Schedule s(i, RANDOM);
		IteratedLocalSearcher g(stoi(perturbationNumber), SHIFT_WHOLE);
		g.setSolution(s);
		cout << g.solve();
	}
	else
	{
		InitializerResultGenerator i;
		i.generateResults();
		/* int fim;
		PerturbationGenerator pg;
		NeighborGenerator n;
		VERBOSE = false;
		SAVE_GRAPHS_ON_SWAP = false;
		TRACK_SWAP_OPERATIONS = false;

		TRACK_SHIFT_OPERATIONS = false;
		SAVE_GRAPHS_ON_SHIFT = false;

		Instance i(t4);
		Schedule s(i, RANDOM);
		auto ini = s.getMakespan(); */
		//Printer::printJobCluster(s);
		/* pg.setSchedule(s);
		n.setSchedule(s); */

		//Printer::printSolution(s);
		//Printer::printGantt(s);

		//cout << s.getSolutionString();

		/* IteratedLocalSearcher ils(1, SWAP_CRITICAL);
		ils.setSolution(s);
		fim = ils.solve(); */

		/* AnnealingSimulator ls(SWAP_CRITICAL_EDGE);
		ls.setSolution(s);
		fim = ls.solve(); */

		/* TabuSearcher t(SHIFT_CRITICAL, 10);
		t.setSolution(s);
		fim = t.solve(); */

		//cout << ini << sp << fim << br;

		/*auto ps = p.listPossiblePerturbations(8, SWAP_CRITICAL_EDGE);
		for (auto per : ps)
		{
			printPerturbation(per);
		} */
	}

	return 0;
}
