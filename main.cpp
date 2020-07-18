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

string t = "test2";
string g = "inst/gueret-prins/GP03-02.txt";
string t4 = "inst/taillard/tai04_04_01.txt";
string t20 = "inst/taillard/tai20_20_01.txt";

int main(int argc, char **argv){
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
	/* srand(0);
	Instance i("/home/hal/tuning/Instances/GP09-05.txt");
	Solution s(i, RANDOM);
	Heuristics h(s);
	cout << h.solve(SA, SWAP_ALL) << br; */
	//testSA();
	Instance i(t);
	Solution s(i, GREEDY_JOBS);
	return 0;
}