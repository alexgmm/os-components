#include <iostream>
#include <vector>
#include <cstdlib>
#include <string>
#include "src/instance.hpp"
#include "src/solution.hpp"
#include "src/tests.hpp"
#include "src/heuristics.hpp"
#include <fstream>

using namespace std;

unsigned op(unsigned j, unsigned m)
{
	if (j == 0 || m == 0)
		return 0;
	assert(j >= 0);
	assert(m >= 0);
	return (j - 1) * 4 + m;
}

string t = "test2";
string g = "inst/gueret-prins/GP03-02.txt";
string t4 = "inst/taillard/tai04_04_01.txt";
string t5 = "inst/taillard/tai05_05_01.txt";
string t20 = "inst/taillard/tai20_20_01.txt";

void test();
int main(int argc, char **argv)
{
	/* if(argc > 1){
		ofstream file("/home/hal/out");
		for(int i=1; i< 10; i++)
			file << i << " " << argv[i] << br;
		file.close();
		string it(argv[4]), a(argv[6]), t(argv[8]), f(argv[2]);
		N_ITER_SA = stoi(it);
		ALPHA = stod(a);
		T_MIN = stod(t);
		//cout << it << " " << a << " " << t << " " << f << br;
		srand(0);
		Instance i(f);
		Solution s(i, RANDOM);
		Heuristics h(s);
		cout << h.solve(SA, SWAP_CRITICAL) << br;
	} */
	/* 	if(argc > 1){
		srand(0);
		string it(argv[4]), a(argv[6]),  f(argv[2]);
		N_ITER_TS = stoi(it);
		TABU_DURATION = stod(a);
		Instance i(f);
		Solution s(i, GREEDY_JOBS);
		Heuristics h(s);
		cout << h.solve(TS, SWAP_CRITICAL) << br;
	}*/
	srand(0);

	test();
	return 0;
}

void test()
{
	/* for (int i = 1; i < 5; i++)
	{
		for (int j = 1; j < 5; j++)
		{
			cout << op(i, j) << " ";
		}
		cout << br;
	} */
	/* Instance i(t5);
	i.print();
	Solution s(i, RANDOM);
	cout << br << s.getMakespan() << br; */
	vector<unsigned> v1 = testInit(RANDOM), v2 = testInit(GREEDY_MACHINES);
	int random = 0, grm = 0;
	for (int i = 0; i < v1.size(); i++)
	{
		if (v1[i] > v2[i])
			random++;
		if (v1[i] < v2[i])
			grm++;
	}
	cout << random << " " << grm << br;
}