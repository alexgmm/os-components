#ifndef TESTS_HPP
#define TESTS_HPP
#define NLOOPS 20

#include <iostream>
#include <vector>
#include <fstream>
#include "heuristics.hpp"
#include "utilities.hpp"

using namespace std;

float testInit(string file, unsigned init)
{
	Instance i(file);
	float media = 0;
	for (int loop = 0; loop < NLOOPS; loop++)
	{
		Solution s(i, init);
		media += s.calcMakespan();
	}

	return media / NLOOPS;
}

void testInit(unsigned init)
{
	fstream out("/home/hal/os-components/results/sa.csv");

	unsigned media;

	vector<string> names = fileNames();

	for (string n : names)
		cout << testInit(n, RANDOM) << " x " << testInit(n, GREEDY_MACHINES) << br;

	out.close();
}

void testSA()
{
	fstream out("/home/hal/os-components/results/sa.csv");

	for (string i : nameInit)
		for (string o : nameOper)
			out << i + " x " + o << ",";
	out << br;

	unsigned media;

	vector<string> names = fileNames();

	for (string n : names)
	{
		out << n + ",";
		for (int i = 0; i < nameInit.size(); i++)
		{
			for (int j = 0; j < nameOper.size(); j++)
			{
				cout << n + " x " + nameInit[i] + " x " + nameOper[j] << br;
				media = 0;
				for (int loop = 0; loop < NLOOPS; loop++)
				{
					Instance instance(n);
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

void testTS()
{
	fstream out("/home/hal/os-components/results/ts.csv");

	for (string i : nameInit)
		for (string o : nameOper)
			out << i + " x " + o << ",";
	out << br;

	unsigned media;

	vector<string> names = fileNames();

	for (string n : names)
	{
		out << n + ",";
		for (int i = 0; i < nameInit.size(); i++)
		{
			for (int j = 0; j < nameOper.size(); j++)
			{
				cout << n + " x " + nameInit[i] + " x " + nameOper[j] << br;
				media = 0;
				for (int loop = 0; loop < 1; loop++)
				{
					Instance instance(n);
					Solution s(instance, i);
					Heuristics h(s);
					media += h.solve(TS, j);
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