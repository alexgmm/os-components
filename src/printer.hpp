#pragma once

#define MAX_GRAPH_NUMBER 600

#include <bits/stdc++.h>
#include <string.h>

#include "schedule.hpp"
#include "perturbation.hpp"

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
    string getMakespanValueNode()
    {
        string node = to_string(makespan);
        node += "[color=yellow, style=filled, shape=box, label=\"makespan: ";
        node += to_string(makespan);
        node += "\"];\n";
        return node;
    }
    string getFirstOperationNode()
    {
        return to_string(first) + "[style=filled,color=green];\nrankdir=LR;\n";
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
    string getMachSequence(unsigned index)
    {
        string sequence = "";
        unsigned o;
        o = machStarters[index];
        sequence += to_string(o);
        o = sM[o];
        while (o != 0)
        {
            sequence += "->" + to_string(o);
            o = sM[o];
        }
        return sequence + ";\n\t\t";
    }
    string getJobSequence(unsigned index)
    {
        string sequence = "";
        unsigned o;
        o = jobStarters[index];
        sequence += to_string(o);
        o = sJ[o];
        while (o != 0)
        {
            sequence += "->" + to_string(o);
            o = sJ[o];
        }
        return sequence + ";\n\t\t";
    }
    string getJobClusterDeclaration(unsigned index)
    {
        return "\tsubgraph cluster" + to_string(index + 1) + "{\n\t\tcolor=red;\n\t\t";
    }
    string getJobClusterLabel(unsigned index)
    {
        return " label = \"job" + to_string(index) + "\";\n\t}\n";
    }
    string getMachClusterDeclaration(unsigned index)
    {
        return "\tsubgraph cluster" + to_string(index + 1) + "{\n\t\tcolor=blue;\n\t\t";
    }
    string getMachClusterLabel(unsigned index)
    {
        return " label = \"mach" + to_string(index) + "\";\n\t}\n";
    }
    string printClusterGraph(bool printJob, bool printMach)
    {
        string graph = "digraph G {\n";

        graph += getFirstOperationNode();
        graph += getMakespanValueNode();

        for (int i = 0; i < machStarters.size(); i++)
        {
            graph += printMach ? getMachClusterDeclaration(i) : "";
            graph += getMachSequence(i);
            graph += printMach ? getMachClusterLabel(i) : "";
        }

        for (int i = 0; i < jobStarters.size(); i++)
        {
            graph += printJob ? getJobClusterDeclaration(i) : "";
            graph += getJobSequence(i);
            graph += printJob ? getJobClusterLabel(i) : "";
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
    string getSaveLabel()
    {
        return wrapStringInLabel("saving graph number " + to_string(CURRENT_GRAPH_NUMBER));
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
        if (CURRENT_GRAPH_NUMBER == MAX_GRAPH_NUMBER)
            return;
        cout << getSaveLabel() << endl;
        string graph = printClusterGraph(false, true);
        saveGraph(graph);
    }
    void printJobCluster()
    {
        if (CURRENT_GRAPH_NUMBER == MAX_GRAPH_NUMBER)
            return;
        cout << getSaveLabel() << endl;
        string graph = printClusterGraph(true, false);
        saveGraph(graph);
    }
    static void printJobCluster(Schedule s)
    {
        ScheduleDataRetriever dr(s);
        Printer p(dr.sJ, dr.sM);
        p.setInstanceData(dr.nO, dr.instance.cost, dr.instance.o_job, dr.instance.o_machine);
        p.setStarters(dr.getStarters(dr.pJ), dr.getStarters(dr.pM));
        p.setSolutionData(dr.first, dr.computeMakespan(), dr.critical(), dr.getHeads());
        p.printJobCluster();
    }
    static void printSolution(Schedule s)
    {
        cout << s.getSolutionString();
    }
    void printGantt()
    {
        cout << endl
             << "saving gantt chart number " << CURRENT_CHART_NUMBER << endl;
        saveGantt();
    }
    static void printBlockSet(vector<vector<unsigned>> &set, string label)
    {
        cout << br << label << br;
        for (auto block : set)
        {
            for (auto o : block)
                cout << o << sp;
            cout << br;
        }
    }
    static void appendToFile(string data, string filename)
    {
        fstream out(filename, ios_base::app);
        out << data;
        out.close();
    }
};