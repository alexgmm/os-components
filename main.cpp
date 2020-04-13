#include <iostream>
#include <vector>
#include "instance.hpp"
#include "solution.hpp"
#include "tests.hpp"

using namespace std;

int main(){
	Instance i("test"); //i.print();
	Solution s(i, GREEDY_JOBS);
	int m = s.calc_makespan();
	cout <<br<<"makespan:"<<m <<br;

	//test(RANDOM);
	//test(GREEDY_JOBS);
	//test(GREEDY_MACHINES);
	return 0;
}
