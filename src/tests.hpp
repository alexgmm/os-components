#ifndef TESTS_HPP
#define TESTS_HPP
#define NLOOPS 20

#include <iostream>
#include <vector>
#include <fstream>
#include "heuristics.hpp"
#include "utilities.hpp"

using namespace std;

void testSA(){
	fstream out("/home/hal/os-components/results/sa.csv");

	for(string i: nameInit)
		for(string o: nameOper)
			out << i + " x " + o << ",";
	out << br;

	unsigned media;

	vector<string> names = fileNames();

	for(string n: names){
		out << n + ",";
		for(int i = 0; i<nameInit.size(); i++){
			for(int j=0; j<nameOper.size(); i++){
				media = 0;
				Instance instance(n);
				for(int loop=0; loop<NLOOPS; loop++){
					Solution s(instance, i);
					Heuristics h(s);
					media += h.solve(SA, j);
				}
				media /= NLOOPS;
				out << media << ",";
			}
		}
		out << br;
	}

	out.close();
}

#endif