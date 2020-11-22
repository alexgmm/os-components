#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <map>

#include "instance.hpp"

using namespace std;

string instancesPath = "/home/hal/os-components/inst/";

struct InstanceFile
{
    string fileName;
    string size;
    unsigned bestKnowValue;
};

void printInstanceFile(InstanceFile i)
{
    cout << i.fileName + "\t" + i.size + "\t" << i.bestKnowValue << br;
}

class InstanceSet
{
    string setName;
    vector<InstanceFile> instanceFiles;
    vector<Instance> instances;

    Instance readInstanceFromFile(InstanceFile file)
    {
        auto path = instancesPath + setName + "/" + file.fileName;
        Instance i(path);
        return i;
    }

public:
    InstanceSet() {}
    InstanceSet(string name)
    {
        setName = name;
    }

    void addInstanceFile(InstanceFile i)
    {
        instanceFiles.push_back(i);
    }

    void printInstanceSet()
    {
        cout << wrapStringInLabel(setName) << br;

        for (unsigned i = 0; i < instanceFiles.size(); i++)
            printInstanceFile(instanceFiles[i]);
    }

    void readInstances()
    {
        for (auto instanceFile : instanceFiles)
            instances.push_back(readInstanceFromFile(instanceFile));
    }

    map<string, vector<Instance>> getInstancesPerSize()
    {
        map<string, vector<Instance>> ips;

        for (auto i : instanceFiles)
            ips[i.size].push_back(readInstanceFromFile(i));

        return ips;
    }

    friend class InstanceFileReader;
};