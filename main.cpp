#include <iostream>
#include <vector>
#include "src/instance.hpp"
#include "src/solution.hpp"
#include "src/tests.hpp"
#include "src/heuristics.hpp"

using namespace std;

string t = "test2";
string t4 = "inst/taillard/tai04_04_01.txt";
string t20 = "inst/taillard/tai20_20_01.txt";

int main(){
	srand(0);
	Instance i(t4);
	Solution s(i, GREEDY_JOBS);
	cout << s.calcMakespan() << br;
	Heuristics h(s); 
	cout << h.solve(TS, SWAP_CRITICAL_EDGE) << br;
	/* Instance i("test2");
	Solution s(i, TEST);
	Neighborhood n(s);
	n.swapJ(8,7); */
	return 0;

}
