#pragma once

#include "../instance/instance_file_reader.hpp"
#include "../heuristics/tabu_searcher.hpp"
#include "../heuristics/annealing_simulator.hpp"
#include "../heuristics/iterated_local_searcher.hpp"

using namespace std;

string filename = "/home/hal/os-components/results/heuristics/results.csv";

string getFileName(string path)
{
    string filename = path;
    string out = filename.substr(filename.find("/inst") + 1);
    out = out.substr(out.find("/") + 1);
    return out.substr(out.find("/") + 1);
}

struct Settings
{
    unsigned initializeMethod, heuristic, neighborhood;
};

class HeuristicResultGenerator
{
    vector<unsigned> heuristics = {SIMULATED_ANNEALING, TABU_SEARCH, ITERATED_LOCAL_SEARCH};
    vector<unsigned> neigborhoods = {SWAP_CRITICAL, SWAP_CRITICAL_EDGE, SHIFT_CRITICAL, SHIFT_WHOLE};
    unsigned loopsNumber = 5;
    fstream fout;

    void printSettings(Settings s)
    {
        cout << getHeuristicName(s.heuristic) << ":" << getNeighborhoodName(s.neighborhood) << br;
    }

    Schedule getInitializedSchedule(Instance &i, unsigned method)
    {
        Schedule s(i, method);
        return s;
    }

    unsigned getOptimizedScheduleValue(Schedule sc, Settings s)
    {
        unsigned result;
        if (s.heuristic == SIMULATED_ANNEALING)
        {
            AnnealingSimulator h(s.neighborhood);
            h.setSolution(sc);
            result = h.solve();
        }
        else if (s.heuristic == TABU_SEARCH)
        {
            TabuSearcher h(s.neighborhood);
            h.setSolution(sc);
            result = h.solve();
        }
        else if (s.heuristic == ITERATED_LOCAL_SEARCH)
        {
            IteratedLocalSearcher h(s.neighborhood);
            h.setSolution(sc);
            result = h.solve();
        }

        return result;
    }

    double getMeanExecutionValue()
    {
    }

    void writeHeader()
    {
        fout << "instance,";

        for (auto heuristic : {SIMULATED_ANNEALING, TABU_SEARCH, ITERATED_LOCAL_SEARCH})
            for (auto ini : {RANDOM, GREEDY_MACHINES, GREEDY_JOBS})
                for (auto nb : {SWAP_CRITICAL, SWAP_CRITICAL_EDGE, SHIFT_CRITICAL, SHIFT_WHOLE})
                    fout << ini + ":" << getHeuristicName(heuristic) + ":" + getNeighborhoodName(nb) + ",";

        fout << endl;
    }

    void writeVerboseHeader()
    {
        fout << "instance,";

        for (auto h : heuristics)
            for (auto nb : neigborhoods)
                for (unsigned i = 1; i <= loopsNumber; i++)
                    fout << getHeuristicName(h) + ":" + getNeighborhoodName(nb) + ":" + to_string(i) + ",";
        fout << endl;
    }

    double getResultForInstance(Instance i, Settings s)
    {
        double result;
        return result;
    }

    unsigned getSingleResultForInstance(Instance i, Settings s)
    {
        Schedule sc = getInitializedSchedule(i, s.initializeMethod);
        return getOptimizedScheduleValue(sc, s);
    }

    void writeResultsForInstance(Instance i)
    {
        Settings s;
        double value;

        for (unsigned h : {SIMULATED_ANNEALING, TABU_SEARCH, ITERATED_LOCAL_SEARCH})
            for (unsigned ini : {RANDOM, GREEDY_MACHINES, GREEDY_JOBS})
                for (unsigned nb : {SWAP_CRITICAL, SWAP_CRITICAL_EDGE, SHIFT_CRITICAL, SHIFT_WHOLE})
                {
                    s = {h, ini, nb};
                    value = getResultForInstance(i, s);
                    fout << value << ",";
                }
    }

    void writeVerboseResultsForInstance(Instance i)
    {
        Settings s;

        for (unsigned h : heuristics)
            for (unsigned nb : neigborhoods)
            {
                s = {RANDOM, h, nb};
                cout << getFileName(i.src) << " ";
                fout << getFileName(i.src) << ",";
                printSettings(s);
                for (unsigned loop = 0; loop < loopsNumber; loop++)
                    fout << getSingleResultForInstance(i, s) << ",";
            }
    }

    void writeRows()
    {
        auto instances = InstanceFileReader::getAllInstances();
        for (auto instance : instances)
        {
            writeResultsForInstance(instance);
            fout << endl;
        }
    }
    void writeVerboseRows()
    {

        auto instances = InstanceFileReader::getAllInstances();
        unsigned s = instances.size();
        for (unsigned i = 65; i <= s; i++)
        {
            cout << i << "/" << s << br;
            //fout << instances[i].src + ",";
            writeVerboseResultsForInstance(instances[i]);
            fout << endl;
        }
    }

    IteratedLocalSearcher getNewILS(unsigned oper)
    {
        IteratedLocalSearcher ils(oper);
        return ils;
    }

public:
    HeuristicResultGenerator()
    {
    }

    void generateResults()
    {
        fout.open(filename);

        writeHeader();
        writeRows();

        fout.close();
    }

    void generateVerboseResults()
    {
        fout.open(filename);
        writeVerboseHeader();
        writeVerboseRows();
        fout.close();
    }

    void generateIndividualResults()
    {
        auto instances = InstanceFileReader::getAllInstances();
        unsigned s = instances.size();
        for (unsigned i = 65; i < s; i++)
        {
            cout << i << "/" << s << endl;
            string filename = instances[i].src;
            string out = "/home/hal/os-components/results/heuristics/" + getFileName(filename) + ".csv";
            cout << out << endl;
            fout.open(out, fstream::out);

            writeVerboseResultsForInstance(instances[i]);

            fout.close();
        }
    }
};