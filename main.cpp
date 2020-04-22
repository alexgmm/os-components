#include <iostream>
#include <vector>
#include "src/instance.hpp"
#include "src/solution.hpp"
#include "src/tests.hpp"
#include "src/heuristics.hpp"

using namespace std;

int main(){
	//test(GREEDY_JOBS);

	//Instance i("inst/taillard/tai04_04_01.txt");
	/* Instance i("test");
	Solution s(i,GREEDY_JOBS);
	cout << br << s.calc_makespan() << br; */

	test_op(RANDOM, SA, SW3J);
	test_op(RANDOM, SA, SW3M);

	/* Instance i("inst/taillard/tai10_10_01.txt");
	Solution s(i, RANDOM);
	Heuristics h(s);
	h.sa(SW2M); */
	//s.print_o();
	return 0;

}
