#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <assert.h>
#include "../helpers/utilities.hpp"

using namespace std;

class Instance
{
	vector<unsigned> cost;
	vector<unsigned> o_job;
	vector<unsigned> o_machine;
	string src;
	int n_jobs, n_machines, n_ops;
	unsigned bestKnownValue;

public:
	Instance() {}
	Instance(string filename)
	{
		//cout << br << "Reading " << filename << " into a new instance" << br;
		src = filename;
		int i, j, c;
		fstream f(filename);
		if (f.fail())
			cout << br << "Failed opening " << filename;

		f >> i;
		n_jobs = i;
		f >> j;
		n_machines = j;
		n_ops = i * j;

		cost.resize(n_jobs * n_machines + 1, 0);
		o_job.resize(n_jobs * n_machines + 1);
		o_machine.resize(n_jobs * n_machines + 1);

		for (i = 1; i < cost.size(); i++)
		{
			f >> c;
			cost[i] = c;
			o_job[i] = (i - 1) / n_machines + 1;
			o_machine[i] = (i - 1) % n_machines + 1;
		}

		f >> bestKnownValue;

		f.close();
	}

	void print()
	{
		cout << br
			 << "Custos"
			 << br;
		for (int i = 1; i <= n_ops; i++)
			cout << i << ": " << cost[i] << br;
		cout << br
			 << "O -> jobs"
			 << br;
		for (int i = 1; i <= n_ops; i++)
			cout << o_job[i] << " ";

		cout << br
			 << "O -> machines"
			 << br;
		for (int i = 1; i <= n_ops; i++)
			cout << o_machine[i] << " ";
		cout << br;
		cout << "BKV: " << bestKnownValue << br;
	}

	friend class Schedule;
	friend class ScheduleDataRetriever;
	friend class PerturbationGenerator;
	friend class Heuristics;
	friend class NeighborGenerator;
	friend class Printer;
	friend class InitializerResultGenerator;
	friend class HeuristicResultGenerator;
};

#endif