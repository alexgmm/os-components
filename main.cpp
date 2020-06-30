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
	unsigned v, v2;
	srand(0);
	Instance i(t20);
	Solution s(i, RANDOM);
	v = s.getMakespan();
	Heuristics h(s); 
	h.solve(TS, SHIFT_CRITICAL);
	v2 = h.getSolution().getMakespan();
	cout << v << " " << v2 << br;
	return 0;

}
