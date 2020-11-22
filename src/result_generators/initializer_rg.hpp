#pragma once

#include "../schedule/schedule.hpp"
#include "instance_set.hpp"
#include "instance_file_reader.hpp"

using namespace std;

class InitializerResultGenerator
{
    unsigned loopsNumber = 20;
    string resultsFolderPath = "/home/hal/os-components/results/initializers/";
    fstream fileOut;

    void printResultLine(map<string, vector<Instance>>::iterator it)
    {
        float median = 0;
        auto size = it->first;
        auto instances = it->second;

        for (auto instance : instances)
            for (unsigned i = 0; i < loopsNumber; i++)
            {
            }
    }

    void generateResultsForSet(InstanceSet set)
    {
        string out = resultsFolderPath + set.getSetName() + ".csv";
        fileOut.open(out.c_str());

        fileOut << "size, mean," << br;

        auto instancesPerSize = set.getInstancesPerSize();
        map<string, vector<Instance>>::iterator ips;

        for (ips = instancesPerSize.begin(); ips != instancesPerSize.end(); ips++)
            printResultLine(ips);
        fileOut.close();
    }

public:
    void generateResults()
    {
        auto instanceSets = InstanceFileReader::getInstanceSets();

        for (auto set : instanceSets)
            generateResultsForSet(set);
    }
};