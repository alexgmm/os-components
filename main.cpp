#include <iostream>
#include <vector>
#include "instance.hpp"
#include "solution.hpp"
#include "tests.hpp"
#include "heuristics.hpp"

using namespace std;

int main(){
	/* Instance i("test"); //i.print();
	Solution s(i, GREEDY_JOBS);
	int m = s.calc_makespan();
	cout <<br<<"makespan:"<<m <<br; */

	//test(RANDOM);
	//test(GREEDY_JOBS);
	test(GREEDY_MACHINES);

	/* Instance i("inst/taillard/tai10_10_01.txt");
	Solution s(i,RANDOM);
	Heuristics h(s);
	h.sa(3); */

	/* Instance i("test");
	Solution s(i, RANDOM);
	s.print();
	s.swap(1,4);
	s.print(); */
	//s.print_o();
	return 0;
}
