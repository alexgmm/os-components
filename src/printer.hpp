#pragma once

#include "solution.hpp"

using namespace std;

class Printer
{
    unsigned first, makespan, nO;
    vector<unsigned> sJ, sM, critical, machStarters, jobStarters, cost;
    string label(unsigned o)
    {
        vector<unsigned> p = critical;
        string color = count(p.begin(), p.end(), o) && o != first ? "style=filled;color=chartreuse3;" : "";
        return to_string(o) + "[" + color + "label=\"" +
               to_string(o) + "(" +
               to_string(cost[o]) +
               ")\"];\n";
    }
    void saveGraph(string graph)
    {
        string filename = "/home/hal/os-components/graphs/" + to_string(CURRENT_GRAPH_NUMBER++) + ".gv";
        remove(filename.c_str());
        fstream out(filename, ios::trunc);
        out.open(filename, ios::out);
        out << graph;
        out.close();
    }
    string printClusterGraph(bool printJob, bool printMach)
    {
        unsigned o;
        string graph = "digraph G {\n";

        graph += to_string(first) + "[style=filled,color=green];\nrankdir=LR;\n";
        if (!VERBOSE && SAVE_GRAPHS)
        {
            graph += to_string(makespan) + "[color=yellow, style=filled, shape=box, label=\"makespan: " + to_string(makespan) + "\"];\n";
        }

        for (int i = 0; i < machStarters.size(); i++)
        {
            if (printMach)
                graph += "\tsubgraph cluster" + to_string(i + 1) + "{\n\t\tcolor=blue;\n\t\t";

            o = machStarters[i];
            graph += to_string(o);
            o = sM[o];
            while (o != 0)
            {
                graph += "->" + to_string(o);
                o = sM[o];
            }
            graph += ";\n\t\t";

            if (printMach)
                graph += " label = \"mach" + to_string(i) + "\";\n\t}\n";
        }

        for (int i = 0; i < jobStarters.size(); i++)
        {
            if (printJob)
                graph += "\tsubgraph cluster" + to_string(i + 1) + "{\n\t\tcolor=red;\n\t\t";

            o = jobStarters[i];
            graph += to_string(o);
            o = sJ[o];
            while (o != 0)
            {
                graph += "->" + to_string(o);
                o = sJ[o];
            }
            graph += ";\n\t\t";

            if (printJob)
                graph += " label = \"job" + to_string(i) + "\";\n\t}\n";
        }

        for (int o = 1; o <= nO; o++)
            graph += label(o);

        graph += "\n}";
        return graph;
    }

public:
    Printer() {}
    Printer(vector<unsigned> sJobs, vector<unsigned> sMachs) : sJ(sJobs), sM(sMachs) {}
    void setStarters(vector<unsigned> jS, vector<unsigned> mS)
    {
        jobStarters = jS;
        machStarters = mS;
    }
    void setInstanceData(unsigned numberO, vector<unsigned> opCost)
    {
        nO = numberO;
        cost = opCost;
    }
    void setSolutionData(unsigned f, unsigned m, vector<unsigned> c)
    {
        first = f;
        makespan = m;
        critical = c;
    }
    void printMachCluster()
    {
        //printSchedule();
        cout << br << "saving m-graph number " << CURRENT_GRAPH_NUMBER << br;
        string graph = printClusterGraph(false, true);
        saveGraph(graph);
    }
    void printJobCluster()
    {
        //printSchedule();
        cout << br << "saving j-graph number " << CURRENT_GRAPH_NUMBER << br;
        string graph = printClusterGraph(true, false);
        saveGraph(graph);
    }
};