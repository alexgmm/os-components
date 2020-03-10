#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

class Instance {
	vector<unsigned int> cost;
	vector<unsigned int> o_job;
	vector<unsigned int> o_machine;
	int n_jobs, n_machines;

	int array_index(int i, int j){ return i*n_machines + j + 1;}
public:
	Instance() {}
	Instance (string filename){
		int i, j, c;
		fstream f(filename);
		
		f >> i; n_jobs = i;
		f >> j; n_machines = j;

		cost.resize(n_jobs*n_machines + 1);

		for(i = 1; i < cost.size(); i++){
			f >> c;
			cost[i] = c;
		}
	}


};

class Solution {
	vetor<unsigned int> pred_job;
	vetor<unsigned int> suc_job;
	vetor<unsigned int> pred_machine;
	vetor<unsigned int> suc_machine;
public:
	Solution () {}
};

int main(){
	Instance i("test");
	return 0;
}
