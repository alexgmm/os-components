#pragma once

#include "../schedule/schedule.hpp"
#include "../instance/instance_set.hpp"
#include "../instance/instance_file_reader.hpp"

using namespace std;

class InitializerResultGenerator
{
    unsigned loopsNumber = 20;
    string resultsFolderPath = "/home/hal/os-components/results/initializers/";
    fstream fout;
    vector<unsigned> results;

    unsigned getMedian()
    {
        mergeSort(results);
        return results[results.size() / 2];
    }

    unsigned getInitializedMakespan(Instance &i, unsigned method)
    {
        Schedule s(i, method);
        return s.getMakespan();
    }

    double getDevianceForMethodOnInstance(Instance &instance, unsigned method)
    {
        double deviance;
        double bkv = instance.bestKnownValue * 1.0;
        bkv = bkv == 0 ? 0.1 : bkv;

        for (unsigned i = 0; i < loopsNumber; i++)
        {
            auto m = getInitializedMakespan(instance, method);
            results.push_back(m);
            auto v = m / bkv;
            deviance += v;
        }

        return deviance / loopsNumber;
    }

    double getDevianceForMethodOnInstances(vector<Instance> &instances, unsigned method)
    {
        double deviance = 0.0;

        for (auto instance : instances)
            deviance += getDevianceForMethodOnInstance(instance, method);

        return deviance / instances.size();
    }

    void printResultLine(map<string, vector<Instance>>::iterator it)
    {
        auto size = it->first;
        auto instances = it->second;

        fout << size << ",";

        for (auto method : {RANDOM, GREEDY_MACHINES, GREEDY_JOBS})
        {
            results.resize(0);
            fout << getDevianceForMethodOnInstances(instances, method) << ",";
            fout << getMedian() << ",";
        }

        fout << br;
    }

    void generateResultsForSet(InstanceSet set)
    {
        string out = resultsFolderPath + set.getSetName() + ".csv";

        fout.open(out.c_str());

        fout << "size,random.dev,random.med,GM.dev,GM.med,GJ.dev,GJ.med," << br;

        auto instancesPerSize = set.getInstancesPerSize();
        map<string, vector<Instance>>::iterator ips;

        for (ips = instancesPerSize.begin(); ips != instancesPerSize.end(); ips++)
            printResultLine(ips);

        fout.close();
    }

public:
    InitializerResultGenerator() {}

    void generateResults()
    {
        auto instanceSets = InstanceFileReader::getInstanceSets();

        for (auto set : instanceSets)
            generateResultsForSet(set);
    }
};