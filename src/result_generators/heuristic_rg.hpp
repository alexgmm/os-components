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
    vector<unsigned> heuristics = {ITERATED_LOCAL_SEARCH};
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

    // WARNING: UGLY CODE
    pair<unsigned, unsigned> getMetrics(Schedule sc, Settings s)
    {
        unsigned timesImproved = 0, solutionsReached = 0;
        if (s.heuristic == SIMULATED_ANNEALING)
        {
            for (unsigned i = 0; i < loopsNumber; i++)
            {
                AnnealingSimulator h(s.neighborhood);
                h.setSolution(sc);
                h.solve();
                timesImproved += h.timesImproved;
                solutionsReached += h.solutionsReached;
            }
        }
        else if (s.heuristic == TABU_SEARCH)
        {
            for (unsigned i = 0; i < loopsNumber; i++)
            {
                TabuSearcher h(s.neighborhood);
                h.setSolution(sc);
                h.solve();
                timesImproved += h.timesImproved;
                solutionsReached += h.solutionsReached;
            }
        }
        else if (s.heuristic == ITERATED_LOCAL_SEARCH)
        {
            for (unsigned i = 0; i < loopsNumber; i++)
            {
                IteratedLocalSearcher h(s.neighborhood);
                h.setSolution(sc);
                h.solve();
                timesImproved += h.timesImproved;
                solutionsReached += h.solutionsReached;
            }
        }

        return make_pair(solutionsReached / loopsNumber, timesImproved / loopsNumber);
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

    pair<unsigned, unsigned> getMetricsForInstance(Instance i, Settings s)
    {
        Schedule sc = getInitializedSchedule(i, s.initializeMethod);
        return getMetrics(sc, s);
    }

    unsigned getILSMetricForInstance(Instance i, Settings s)
    {
        Schedule sc = getInitializedSchedule(i, s.initializeMethod);
        unsigned localSearches = 0;
        for (unsigned i = 0; i < loopsNumber; i++)
        {
            IteratedLocalSearcher h(s.neighborhood);
            h.setSolution(sc);
            h.solve();
            localSearches += h.timesLocalSearch;
        }
        return localSearches / loopsNumber;
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

    void writeMetricsForInstance(Instance i)
    {
        Settings s;

        for (unsigned h : heuristics)
            for (unsigned nb : neigborhoods)
            {
                s = {RANDOM, h, nb};
                cout << getFileName(i.src) << " ";
                fout << getFileName(i.src) << ",";
                printSettings(s);
                auto values = getMetricsForInstance(i, s);
                fout << values.first << "," << values.second << ",";
            }
    }

    void writeILSMetricForInstance(Instance i)
    {
        Settings s;

        for (unsigned h : heuristics)
            for (unsigned nb : neigborhoods)
            {
                s = {RANDOM, h, nb};
                cout << getFileName(i.src) << " ";
                fout << getFileName(i.src) << ",";
                printSettings(s);
                auto value = getILSMetricForInstance(i, s);
                fout << value << ",";
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
        for (unsigned i = 0; i <= s; i++)
        {
            cout << i << "/" << s << br;
            //fout << instances[i].src + ",";
            writeVerboseResultsForInstance(instances[i]);
            fout << endl;
        }
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
        for (unsigned i = 0; i < s; i++)
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

    void generateHeuristicMetrics()
    {
        auto instances = InstanceFileReader::getAllInstances();
        unsigned s = instances.size();
        for (unsigned i = 0; i < s; i++)
        {
            cout << i + 1 << "/" << s << endl;
            string filename = instances[i].src;
            string out = "/home/hal/os-components/results/heuristics/m_" + getFileName(filename) + ".csv";
            //cout << out.substr(42) << endl;
            fout.open(out, fstream::out);

            writeMetricsForInstance(instances[i]);

            fout.close();
        }
    }

    void generateILS_SpecificMetrics()
    {
        auto instances = InstanceFileReader::getAllInstances();
        unsigned s = instances.size();
        for (unsigned i = 178; i < s; i++)
        {
            cout << i + 1 << "/" << s << endl;
            string filename = instances[i].src;
            string out = "/home/hal/os-components/results/heuristics/ils_" + getFileName(filename) + ".csv";
            //cout << out.substr(42) << endl;
            fout.open(out, fstream::out);

            writeILSMetricForInstance(instances[i]);

            fout.close();
        }
    }
};