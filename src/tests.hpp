#ifndef TESTS_HPP
#define TESTS_HPP
#define NLOOPS 20

#include <fstream>
#include "heuristics.hpp"
#include "utilities.hpp"

using namespace std;

void printAverageExecutionTime(int heuristic, int op)
{
	float avg = 0;
	vector<string> names = fileNames();

	for (string name : names)
	{
		startTimeCounting();
		Instance i(name);
		Solution s(i, GREEDY_JOBS);
		Heuristics h(s);
		h.solve(heuristic, op);
		avg += getElapsedTime();
	}
	avg /= 1000; // transforma a média para segundos
	cout << avg / names.size() << br;
}

float testInit(string file, unsigned init)
{
	//cout << file << br;
	Instance i(file);
	float media = 0;
	for (int loop = 0; loop < NLOOPS; loop++)
	{
		Solution s(i, init);
		media += s.calcMakespan();
	}

	return media / NLOOPS;
}

void testInitMethods()
{
	fstream out("/home/hal/os-components/results/ini.csv");

	out << "random,greedyMachs,greedyJobs," << br;

	vector<string> names = fileNames();

	for (string n : names)
	{
		for (unsigned i : {RANDOM, GREEDY_MACHINES, GREEDY_JOBS})
			out << testInit(n, i) << ",";
		out << br;
	}

	out.close();
}

vector<unsigned> testInit(unsigned init)
{
	vector<unsigned> values;
	vector<string> names = fileNames();
	for (string n : names)
	{
		//cout << n << br;
		Instance i(n);
		Solution s(i, init);
		values.push_back(s.calcMakespan());
	}
	return values;
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