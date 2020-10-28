#pragma once

#define MAX_GRAPH_NUMBER 10

#include <bits/stdc++.h>
#include <string.h>

#include "solution.hpp"

bool SAVE_GRAPHS_ON_SWAP = false;
bool SAVE_GRAPHS_ON_SHIFT = false;
bool SAVE_CHARTS_ON_SWAP = false;
bool SAVE_CHARTS_ON_SHIFT = false;
unsigned CURRENT_GRAPH_NUMBER = 1;
unsigned CURRENT_CHART_NUMBER = 1;

using namespace std;

class Printer
{
    unsigned first, makespan, nO;
    vector<unsigned> sJ, sM, critical, machStarters, jobStarters, cost, heads, operationToJob, operationToMach;
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
        if (!VERBOSE && (SAVE_GRAPHS_ON_SWAP || SAVE_GRAPHS_ON_SHIFT))
        {
            graph += to_string(makespan);
            graph += "[color=yellow, style=filled, shape=box, label=\"makespan: ";
            graph += to_string(makespan);
            graph += "\"];\n";
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
    string getOperationLabel(unsigned &o)
    {
        string operationLabel = to_string(o) + " " + to_string(heads[o]) + " " + to_string(cost[o]) + " " + to_string(operationToJob[o]) + " " + to_string(operationToMach[o]);
        o = sM[o];
        if (o > 0)
            operationLabel += ",";
        return operationLabel;
    }
    string getGannt(string label)
    {
        string gantt = label + "." + to_string(makespan) + ".";
        unsigned o;
        for (unsigned o : machStarters)
        {
            do
                gantt += getOperationLabel(o);
            while (o > 0);
            gantt += ",";
        }
        gantt = gantt.substr(0, gantt.size() - 1);
        gantt = "\"" + gantt + "\"";
        return gantt;
    }
    void saveGantt()
    {
        string command = "python3 gantt.py " + getGannt(to_string(CURRENT_CHART_NUMBER++));
        //cout << command << br;
        system(command.c_str());
    }

public:
    Printer() {}
    Printer(vector<unsigned> sJobs, vector<unsigned> sMachs) : sJ(sJobs), sM(sMachs) {}
    void setStarters(vector<unsigned> jS, vector<unsigned> mS)
    {
        jobStarters = jS;
        machStarters = mS;
    }
    void setInstanceData(unsigned numberO, vector<unsigned> opCost, vector<unsigned> oJ, vector<unsigned> oM)
    {
        nO = numberO;
        cost = opCost;
        operationToJob = oJ;
        operationToMach = oM;
    }
    void setSolutionData(unsigned f, unsigned m, vector<unsigned> c, vector<unsigned> h)
    {
        first = f;
        makespan = m;
        critical = c;
        heads = h;
    }
    void printMachCluster()
    {
        if(CURRENT_GRAPH_NUMBER == MAX_GRAPH_NUMBER)
            return;
        cout << endl
             << "saving m-graph number " << CURRENT_GRAPH_NUMBER << endl;
        string graph = printClusterGraph(false, true);
        saveGraph(graph);
    }
    void printJobCluster()
    {
        if(CURRENT_GRAPH_NUMBER == MAX_GRAPH_NUMBER)
            return;
        cout << endl
             << "saving j-graph number " << CURRENT_GRAPH_NUMBER << endl;
        string graph = printClusterGraph(true, false);
        saveGraph(graph);
    }
    void printGantt()
    {
        cout << endl
             << "saving gantt chart number " << CURRENT_CHART_NUMBER << endl;
        saveGantt();
    }
};