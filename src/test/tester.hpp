#ifndef TESTS_HPP
#define TESTS_HPP
#define NLOOPS 20

#include "../heuristics/tabu_searcher.hpp"
#include "../heuristics/annealing_simulator.hpp"
#include "../heuristics/iterated_local_searcher.hpp"
#include "../helpers/utilities.hpp"
#include "../helpers/time.hpp"

using namespace std;

class Tester
{
	vector<string> path = {"inst/taillard/", "inst/brucker/", "inst/gueret-prins/"};
	vector<vector<string>> names = {
		{"tai04_04_01.txt", "tai04_04_02.txt", "tai04_04_03.txt", "tai04_04_04.txt", "tai04_04_05.txt", "tai04_04_06.txt", "tai04_04_07.txt", "tai04_04_08.txt", "tai04_04_09.txt", "tai04_04_10.txt", "tai05_05_01.txt", "tai05_05_02.txt", "tai05_05_03.txt", "tai05_05_04.txt", "tai05_05_05.txt", "tai05_05_06.txt", "tai05_05_07.txt", "tai05_05_08.txt", "tai05_05_09.txt", "tai05_05_10.txt", "tai07_07_01.txt", "tai07_07_02.txt", "tai07_07_03.txt", "tai07_07_04.txt", "tai07_07_05.txt", "tai07_07_06.txt", "tai07_07_07.txt", "tai07_07_08.txt", "tai07_07_09.txt", "tai07_07_10.txt", "tai10_10_01.txt", "tai10_10_02.txt", "tai10_10_03.txt", "tai10_10_04.txt", "tai10_10_05.txt", "tai10_10_06.txt", "tai10_10_07.txt", "tai10_10_08.txt", "tai10_10_09.txt", "tai10_10_10.txt", "tai15_15_01.txt", "tai15_15_02.txt", "tai15_15_03.txt", "tai15_15_04.txt", "tai15_15_05.txt", "tai15_15_06.txt", "tai15_15_07.txt", "tai15_15_08.txt", "tai15_15_09.txt", "tai15_15_10.txt", "tai20_20_01.txt", "tai20_20_02.txt", "tai20_20_03.txt", "tai20_20_04.txt", "tai20_20_05.txt", "tai20_20_06.txt", "tai20_20_07.txt", "tai20_20_08.txt", "tai20_20_09.txt", "tai20_20_10.txt"},
		{"j3-per0-1", "j3-per0-2", "j3-per10-0", "j3-per10-1", "j3-per10-2", "j3-per20-0", "j3-per20-1", "j3-per20-2", "j4-per0-0", "j4-per0-1", "j4-per0-2", "j4-per10-0", "j4-per10-1", "j4-per10-2", "j4-per20-0", "j4-per20-1", "j4-per20-2", "j5-per0-0", "j5-per0-1", "j5-per0-2", "j5-per10-0", "j5-per10-1", "j5-per10-2", "j5-per20-0", "j5-per20-1", "j5-per20-2", "j6-per0-0", "j6-per0-1", "j6-per0-2", "j6-per10-0", "j6-per10-1", "j6-per10-2", "j6-per20-0", "j6-per20-1", "j6-per20-2", "j7-per0-0", "j7-per0-1", "j7-per0-2", "j7-per10-0", "j7-per10-1", "j7-per10-2", "j7-per20-0", "j7-per20-1", "j7-per20-2", "j8-per0-1", "j8-per0-2", "j8-per10-0", "j8-per10-1", "j8-per10-2", "j8-per20-0", "j8-per20-1", "j8-per20-2"},
		{"GP03-01.txt", "GP03-02.txt", "GP03-03.txt", "GP03-04.txt", "GP03-05.txt", "GP03-06.txt", "GP03-07.txt", "GP03-08.txt", "GP03-09.txt", "GP03-10.txt", "GP04-01.txt", "GP04-02.txt", "GP04-03.txt", "GP04-04.txt", "GP04-05.txt", "GP04-06.txt", "GP04-07.txt", "GP04-08.txt", "GP04-09.txt", "GP04-10.txt", "GP05-01.txt", "GP05-02.txt", "GP05-03.txt", "GP05-04.txt", "GP05-05.txt", "GP05-06.txt", "GP05-07.txt", "GP05-08.txt", "GP05-09.txt", "GP05-10.txt", "GP06-01.txt", "GP06-02.txt", "GP06-03.txt", "GP06-04.txt", "GP06-05.txt", "GP06-06.txt", "GP06-07.txt", "GP06-08.txt", "GP06-09.txt", "GP06-10.txt", "GP07-01.txt", "GP07-02.txt", "GP07-03.txt", "GP07-04.txt", "GP07-05.txt", "GP07-06.txt", "GP07-07.txt", "GP07-08.txt", "GP07-09.txt", "GP07-10.txt", "GP08-01.txt", "GP08-02.txt", "GP08-03.txt", "GP08-04.txt", "GP08-05.txt", "GP08-06.txt", "GP08-07.txt", "GP08-08.txt", "GP08-09.txt", "GP08-10.txt", "GP09-01.txt", "GP09-02.txt", "GP09-03.txt", "GP09-04.txt", "GP09-05.txt", "GP09-06.txt", "GP09-07.txt", "GP09-08.txt", "GP09-09.txt", "GP09-10.txt", "GP10-01.txt", "GP10-02.txt", "GP10-03.txt", "GP10-04.txt", "GP10-05.txt", "GP10-06.txt", "GP10-07.txt", "GP10-08.txt", "GP10-09.txt", "GP10-10.txt"}};

	vector<string> nameInitializators = {"random", "greedy_machines", "greedy_jobs"};
	vector<string> nameHeur = {"SA", "IG", "TS"};
	vector<string> nameOper = {"swAll", "swCritical", "swEdges", "shWhole"};

	unsigned oper;

	bool isTransposed = false;

	vector<string> fileNames()
	{

		vector<string> f;
		for (int i = 0; i < 3; i++)
			for (string s : names[i])
				f.push_back(path[i] + s);
		return f;
	}

public:
	Tester() {}

	string getInitResultFileName(unsigned init)
	{
		return "/home/hal/os-components/results/" + nameInitializators[init] + ".csv";
	}

	float executeAndPrintInitialization(string file, unsigned init, fstream &out)
	{
		out << file << ",";
		Instance i(file);
		for (int loop = 1; loop <= NLOOPS; loop++)
		{
			Schedule s(i, init);
			out << s.computeMakespan();
			if (loop != NLOOPS)
				out << ",";
		}
		out << br;
	}

	void generateInitializationResults()
	{
		vector<string> names = fileNames();
		for (unsigned init : {RANDOM, GREEDY_JOBS, GREEDY_MACHINES})
		{
			fstream out(getInitResultFileName(init));

			out << "file,";
			for (unsigned i = 1; i <= NLOOPS; i++)
			{
				out << "execution " << i;
				if (i != NLOOPS)
					out << ",";
			}
			out << br;
			for (string filename : names)
				executeAndPrintInitialization(filename, init, out);

			out.close();
		}
	}

	void transposeInstances()
	{
		isTransposed = true;
		for (unsigned i = 0; i < names.size(); i++)
			for (unsigned j = 0; j < names[i].size(); j++)
				names[i][j] = "transposed_" + names[i][j];
	}

	vector<unsigned> testInit(unsigned init)
	{
		vector<unsigned> values;
		vector<string> names = fileNames();
		for (string n : names)
		{
			//cout << n << br;
			Instance i(n);
			Schedule s(i, init);
			values.push_back(s.computeMakespan());
		}
		return values;
	}
};

#endif