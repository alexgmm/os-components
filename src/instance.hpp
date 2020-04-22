#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <assert.h>

#define RANDOM 0
#define GREEDY_MACHINES 1
#define GREEDY_JOBS 2
#define DUMMY (0)
#define br "\n"
#define SA 0
#define IG 1
#define TS 2
#define SW1J 0
#define SW1M 1
#define SW2J 2
#define SW2M 3
#define SW3J 4
#define SW3M 5
#define SH1J 6
#define SH1M 7
#define SH2J 8
#define SH2M 9
#define SH3J 10
#define SH3M 11

using namespace std;

vector<string> name_initializer = { "ran", "grm", "grj"};
vector<string> name_heuristic = { "SA", "IG", "TS"};
vector<string> name_operator = { "SW1J", "SW1M", "SW2J", "SW2M", "SW3J", "SW3M", "SH1J", "SH1M", "SH2J", "SH2M", "SH3J", "SH3M" };

int randint(int ini, int end){
	assert(ini != end);
	assert(ini >= 0);
	assert(end > ini);
    return ini + (rand() % (end - ini + 1));
}

void printv(vector<unsigned>&v, int ini, string name){
	cout << "\n" << name << "\n";
	for(int i=ini; i<v.size(); i++) cout<<i<<" "; cout<<br;
	for(int i=ini; i<v.size()-1; i++) cout<<"--"; cout<<br;
	for(int i=ini; i<v.size(); i++) cout << v[i] << " ";
	cout << "\n";}


class Instance {
	vector<unsigned> cost;
	vector<unsigned> o_job;
	vector<unsigned> o_machine;
	int n_jobs, n_machines, n_ops;
public:
	Instance() {}
	Instance (string filename){
		int i, j, c;
		fstream f(filename);
		if(f.fail()) cout <<br<<"Erro ao abrir "<<filename;

		f >> i; n_jobs = i;
		f >> j; n_machines = j; n_ops = i*j;

		cost.resize(n_jobs*n_machines + 1, 0);
		o_job.resize(n_jobs*n_machines + 1);
		o_machine.resize(n_jobs*n_machines + 1);

		for(i = 1; i < cost.size(); i++){
			f >> c;
			cost[i] = c;
			o_job[i] = (i-1)/n_machines + 1;
			o_machine[i] = (i-1)%n_machines + 1;
		}

		f.close();
	}

	void print(){
		cout << "\n" << "Custos" << "\n";
		for(int i=1; i<=n_ops; i++)
			cout << i << ": " << cost[i] << "\n";
		cout << "\n" << "O -> jobs" << "\n";
		for(int i=1; i<=n_ops; i++)
			cout << o_job[i] << " ";

		cout << "\n" << "O -> machines" << "\n";
		for(int i=1; i<=n_ops; i++)
			cout << o_machine[i] << " ";
		cout << "\n";}
	
	friend class Solution;
	friend class Heuristics;};

#endif