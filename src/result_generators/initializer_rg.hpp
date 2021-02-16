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

    void printVerboseResultLine(map<string, vector<Instance>>::iterator it)
    {
        auto size = it->first;
        auto instances = it->second;

        fout << size << "," << br;

        for (auto instance : instances)
        {
            fout << instance.src << ",";
            //cout << instance.src << br;
            for (auto method : {RANDOM, GREEDY_JOBS, GREEDY_MACHINES})
            {
                for (int i = 0; i < loopsNumber; i++)
                    fout << getInitializedMakespan(instance, method) << ",";
            }
            fout << br;
        }

        fout << br;
    }

    void generateResultsForSet(InstanceSet set)
    {
        string out = resultsFolderPath + set.getSetName() + ".csv";

        fout.open(out.c_str(), fstream::out);

        fout << "size,random.dev,random.med,GM.dev,GM.med,GJ.dev,GJ.med," << br;

        auto instancesPerSize = set.getInstancesPerSize();
        map<string, vector<Instance>>::iterator ips;

        for (ips = instancesPerSize.begin(); ips != instancesPerSize.end(); ips++)
            printResultLine(ips);

        fout.close();
    }

    void printVerboseHeader()
    {
        for (string m : {"ran", "gj", "gm"})
            for (int i = 0; i < loopsNumber; i++)
                fout << m << ":" << i << ",";

        fout << br;
    }

    void generateVerboseResultsForSet(InstanceSet set)
    {
        cout << set.getSetName() << br;
        auto instancesPerSize = set.getInstancesPerSize();
        map<string, vector<Instance>>::iterator ips;

        for (ips = instancesPerSize.begin(); ips != instancesPerSize.end(); ips++)
            printVerboseResultLine(ips);
    }

public:
    InitializerResultGenerator() {}

    void generateResults()
    {
        auto instanceSets = InstanceFileReader::getInstanceSets();

        for (auto set : instanceSets)
            generateResultsForSet(set);
    }

    void generateVerboseResults()
    {

        fout.open(resultsFolderPath + "out.csv", fstream::out);
        printVerboseHeader();

        auto instanceSets = InstanceFileReader::getInstanceSets();

        for (auto set : instanceSets)
            generateVerboseResultsForSet(set);
        fout.close();
    }
};