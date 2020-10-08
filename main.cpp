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
		N_ITER_SA = stoi(iterationsNumber);
		ALPHA = stod(alpha);
		T_MIN = stod(temperature);
		//cout << it << " " << a << " " << t << " " << f << br;
		Instance i(instanceFileName);
		Solution s(i, RANDOM);
		Heuristics h(s);
		unsigned r = h.solve(SA, SWAP_CRITICAL_EDGE);
		to_string(6);
		cout << r;
	}
	else
	{
		VERBOSE = false;
		SAVE_GRAPHS = true;
		TRACK_OPERATIONS = false;
		test();
	}

	return 0;
}

void test()
{
	//printAverageExecutionTime(SA, SWAP_CRITICAL_EDGE);
	Instance i(t4);
	Solution s(i, RANDOM);
	int initialMakespan = s.calcMakespan();
	cout << initialMakespan;
	s.printJobCluster();
	s.printGantt();
	Heuristics h(s);
	cout << " " << h.solve(SA, SWAP_CRITICAL_EDGE) << br;
}