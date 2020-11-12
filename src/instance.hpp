#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <assert.h>
#include "helpers/utilities.hpp"

using namespace std;

class Instance
{
	vector<unsigned> cost;
	vector<unsigned> o_job;
	vector<unsigned> o_machine;
	string src;
	int n_jobs, n_machines, n_ops;

public:
	Instance() {}
	Instance(string filename)
	{
		//cout << br << "Reading " << filename << " into a new instance" << br;
		src = filename;
		int i, j, c;
		fstream f(filename);
		if (f.fail())
			cout << br << "Erro ao abrir " << filename;

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

		f.close();
	}

	void print()
	{
		cout << "\n"
			 << "Custos"
			 << "\n";
		for (int i = 1; i <= n_ops; i++)
			cout << i << ": " << cost[i] << "\n";
		cout << "\n"
			 << "O -> jobs"
			 << "\n";
		for (int i = 1; i <= n_ops; i++)
			cout << o_job[i] << " ";

		cout << "\n"
			 << "O -> machines"
			 << "\n";
		for (int i = 1; i <= n_ops; i++)
			cout << o_machine[i] << " ";
		cout << "\n";
	}

	friend class Schedule;
	friend class ScheduleDataRetriever;
	friend class PerturbationGenerator;
	friend class Heuristics;
	friend class NeighborGenerator;
	friend class Printer;
};

#endif