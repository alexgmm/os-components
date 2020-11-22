#pragma once

#include "instance_set.hpp"
#include "instance.hpp"

using namespace std;

class InstanceFileReader
{
public:
    static vector<InstanceSet> getInstanceSets()
    {
        vector<InstanceSet> ifs;
        fstream setsFile;
        setsFile.open("/home/hal/os-components/src/instance/instance_sets");

        unsigned nI, nJ, nM, bestKnownValue, nS;
        string setName, fileName, size, i;
        InstanceFile file;

        setsFile >> nS;

        for (int n = 0; n < nS; n++)
        {
            setsFile >> nI >> setName;
            InstanceSet set(setName);

            for (int i = 0; i < nI; i++)
            {
                setsFile >> fileName >> nJ >> nM >> bestKnownValue;

                size = to_string(nJ) + "x" + to_string(nM);
                file = {fileName, size, bestKnownValue};

                set.addInstanceFile(file);
            }

            ifs.push_back(set);
        }

        setsFile.close();
        return ifs;
    }
};