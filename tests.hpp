#ifndef TESTS_HPP
#define TESTS_HPP
#define NLOOPS 50

#include <iostream>
#include <vector>
#include "solution.hpp"
#include "instance.hpp"
#include "heuristics.hpp"

using namespace std;

class Files {
public:
	vector<string> path = {"inst/taillard/", "inst/brucker/", "inst/gueret-prins/"};
	vector<vector<string>> names = {
		{"tai04_04_01.txt", "tai04_04_02.txt", "tai04_04_03.txt", "tai04_04_04.txt", "tai04_04_05.txt", "tai04_04_06.txt", "tai04_04_07.txt", "tai04_04_08.txt", "tai04_04_09.txt", "tai04_04_10.txt", "tai05_05_01.txt", "tai05_05_02.txt", "tai05_05_03.txt", "tai05_05_04.txt", "tai05_05_05.txt", "tai05_05_06.txt", "tai05_05_07.txt", "tai05_05_08.txt", "tai05_05_09.txt", "tai05_05_10.txt", "tai07_07_01.txt", "tai07_07_02.txt", "tai07_07_03.txt", "tai07_07_04.txt", "tai07_07_05.txt", "tai07_07_06.txt", "tai07_07_07.txt", "tai07_07_08.txt", "tai07_07_09.txt", "tai07_07_10.txt", "tai10_10_01.txt", "tai10_10_02.txt", "tai10_10_03.txt", "tai10_10_04.txt", "tai10_10_05.txt", "tai10_10_06.txt", "tai10_10_07.txt", "tai10_10_08.txt", "tai10_10_09.txt", "tai10_10_10.txt", "tai15_15_01.txt", "tai15_15_02.txt", "tai15_15_03.txt", "tai15_15_04.txt", "tai15_15_05.txt", "tai15_15_06.txt", "tai15_15_07.txt", "tai15_15_08.txt", "tai15_15_09.txt", "tai15_15_10.txt", "tai20_20_01.txt", "tai20_20_02.txt", "tai20_20_03.txt", "tai20_20_04.txt", "tai20_20_05.txt", "tai20_20_06.txt", "tai20_20_07.txt", "tai20_20_08.txt", "tai20_20_09.txt", "tai20_20_10.txt"},
		{"j3-per0-1", "j3-per0-2", "j3-per10-0", "j3-per10-1", "j3-per10-2", "j3-per20-0", "j3-per20-1", "j3-per20-2", "j4-per0-0", "j4-per0-1", "j4-per0-2", "j4-per10-0", "j4-per10-1", "j4-per10-2", "j4-per20-0", "j4-per20-1", "j4-per20-2", "j5-per0-0", "j5-per0-1", "j5-per0-2", "j5-per10-0", "j5-per10-1", "j5-per10-2", "j5-per20-0", "j5-per20-1", "j5-per20-2", "j6-per0-0", "j6-per0-1", "j6-per0-2", "j6-per10-0", "j6-per10-1", "j6-per10-2", "j6-per20-0", "j6-per20-1", "j6-per20-2", "j7-per0-0", "j7-per0-1", "j7-per0-2", "j7-per10-0", "j7-per10-1", "j7-per10-2", "j7-per20-0", "j7-per20-1", "j7-per20-2", "j8-per0-1", "j8-per0-2", "j8-per10-0", "j8-per10-1", "j8-per10-2", "j8-per20-0", "j8-per20-1", "j8-per20-2"},
		{"GP03-01.txt", "GP03-02.txt", "GP03-03.txt", "GP03-04.txt", "GP03-05.txt", "GP03-06.txt", "GP03-07.txt", "GP03-08.txt", "GP03-09.txt", "GP03-10.txt", "GP04-01.txt", "GP04-02.txt", "GP04-03.txt", "GP04-04.txt", "GP04-05.txt", "GP04-06.txt", "GP04-07.txt", "GP04-08.txt", "GP04-09.txt", "GP04-10.txt", "GP05-01.txt", "GP05-02.txt", "GP05-03.txt", "GP05-04.txt", "GP05-05.txt", "GP05-06.txt", "GP05-07.txt", "GP05-08.txt", "GP05-09.txt", "GP05-10.txt", "GP06-01.txt", "GP06-02.txt", "GP06-03.txt", "GP06-04.txt", "GP06-05.txt", "GP06-06.txt", "GP06-07.txt", "GP06-08.txt", "GP06-09.txt", "GP06-10.txt", "GP07-01.txt", "GP07-02.txt", "GP07-03.txt", "GP07-04.txt", "GP07-05.txt", "GP07-06.txt", "GP07-07.txt", "GP07-08.txt", "GP07-09.txt", "GP07-10.txt", "GP08-01.txt", "GP08-02.txt", "GP08-03.txt", "GP08-04.txt", "GP08-05.txt", "GP08-06.txt", "GP08-07.txt", "GP08-08.txt", "GP08-09.txt", "GP08-10.txt", "GP09-01.txt", "GP09-02.txt", "GP09-03.txt", "GP09-04.txt", "GP09-05.txt", "GP09-06.txt", "GP09-07.txt", "GP09-08.txt", "GP09-09.txt", "GP09-10.txt", "GP10-01.txt", "GP10-02.txt", "GP10-03.txt", "GP10-04.txt", "GP10-05.txt", "GP10-06.txt", "GP10-07.txt", "GP10-08.txt", "GP10-09.txt", "GP10-10.txt"}
	};
	Files (){}
};
void test_h(){
	Files f;   
    float media;
	ofstream out("results/r.csv");
	vector<string> ops = {"sw1j","sw1m","sw2j","sw2m","sw3j","sw3m","sh1j","sh1m","sh2j","sh2m","sh3j","sh3m"},
				   heur = {"SA"},
				   inis = {"RAN","GRM"};

	out << "file,";
	for(string i: inis){
		for(string h: heur){
			for(string o: ops){
				out<<i<<"x"<<h<<"x"<<o<<",";
			}
		}
	}

	out << br;
				

	for(int i=0; i<f.path.size(); i++){
		for(int j=0; j<f.names[i].size(); j++){
			string fp = f.path[i],
				   fn = f.names[i][j];
			out << fn << ",";

			for(int i=0; i<inis.size(); i++){
				for(int j=0; j<heur.size(); j++){
					for(int k=0; k<ops.size(); k++){
						media = 0.0;
						for(int l=0; l<NLOOPS; l++){
							Instance in(fp + fn);
							Solution s(in, i);
							Heuristics h(s);
							media += h.sa(k+1);	
						}
						out << media/NLOOPS << ",";
					}
				}
			}
			out << br;

		}
	} 
	out.close();
}

void test(int init_method){
    Files f;   
    float media = 0.0;
	string out_name;

	switch(init_method){
			case RANDOM:
				out_name = "results/init_random.csv";
				break;
			case GREEDY_JOBS:
				out_name = "results/init_greedy_jobs.csv";
				break;
			case GREEDY_MACHINES:
				out_name = "results/init_greedy_machines.csv";
				break;
			default:
				break;}

	ofstream out(out_name);

	for(int i=0; i<f.path.size(); i++){
		for(int j=0; j<f.names[i].size(); j++){
			string fp = f.path[i],
				   fn = f.names[i][j];
			media = 0.0;
            for(int l=0; l<NLOOPS; l++){
                Instance i(fp + fn);
                Solution s(i, init_method);
                media += s.calc_makespan();
            }
			//cout << media <<br;
			out << fn << "," << (int) media/NLOOPS << br;
		}
	}
	out.close();
}



#endif