#pragma once

#include "../schedule/schedule.hpp"

using namespace std;

class ScheduleDataRetriever : public Schedule
{
public:
    unsigned getJobAncestor(unsigned o)
    {
        while (pJ[o] != 0)
            o = pJ[o];
        return o;
    }
    unsigned getMachAncestor(unsigned o)
    {
        while (pM[o] != 0)
            o = pM[o];
        return o;
    }
    unsigned getHighestCostFromMachine()
    {
        vector<vector<unsigned>> machs = getWholeMachines();
        unsigned highestCost = 0, costSum;
        for (vector<unsigned> mach : machs)
        {
            costSum = 0;
            for (unsigned o : mach)
                costSum += instance.cost[o];
            if (costSum > highestCost)
                highestCost = costSum;
        }

        return highestCost;
    }
    unsigned getHighestCostFromJob()
    {
        vector<vector<unsigned>> jobs = getWholeJobs();
        unsigned highestCost = 0, costSum;
        for (vector<unsigned> job : jobs)
        {
            costSum = 0;
            for (unsigned o : job)
                costSum += instance.cost[o];
            if (costSum > highestCost)
                highestCost = costSum;
        }

        return highestCost;
    }
    void blocks(vector<unsigned> &begins, vector<unsigned> &ends, unsigned blockType)
    {
        vector<unsigned> p = critical(), blocksBegins, blocksEnds;

        assert(p.size() > 0);
        blocksBegins.push_back(1);
        for (unsigned i = 2; i < p.size(); i++)
        {
            bool sameCluster = (blockType == BLOCK_J) ? sameJob(p[i], p[i - 1]) : sameMach(p[i], p[i - 1]);
            if (!sameCluster)
            {
                if (p[i] == first)
                    blocksBegins.push_back(i + 1);
                else
                    blocksBegins.push_back(i);
                blocksEnds.push_back(i - 1);
            }
        }
        blocksEnds.push_back(p.size() - 1);

        for (unsigned i = 0; i < blocksBegins.size(); i++)
        {
            if (blocksEnds[i] - blocksBegins[i] > 0)
            {
                begins.push_back(blocksBegins[i]);
                ends.push_back(blocksEnds[i]);
            }
        }
    }
    vector<vector<unsigned>> getCriticalJBlocks()
    {
        vector<vector<unsigned>> jBlocks;
        vector<unsigned> p = critical(), block;
        unsigned index = 1, o;
        block.push_back(p[0]);

        while (true)
        {
            o = p[index++];
            if (!sameJob(o, p[index - 2]))
            {
                jBlocks.push_back(block);
                block.clear();
            }
            block.push_back(o);
            if (index == p.size())
            {
                jBlocks.push_back(block);
                break;
            }
        }

        return jBlocks;
    }
    vector<vector<unsigned>> getCriticalMBlocks()
    {
        vector<vector<unsigned>> mBlocks;
        vector<unsigned> p = critical(), block;
        unsigned index = 1, o;
        block.push_back(p[0]);

        while (true)
        {
            o = p[index++];
            if (!sameMach(o, p[index - 2]))
            {
                mBlocks.push_back(block);
                block.clear();
            }
            block.push_back(o);
            if (index == p.size())
            {
                mBlocks.push_back(block);
                break;
            }
        }

        return mBlocks;
    }
    void filterNonTrivialBlocks(vector<vector<unsigned>> &blocks)
    {
        vector<vector<unsigned>> filtered;

        for (auto block : blocks)
            if (block.size() > 1)
                filtered.push_back(block);

        blocks = filtered;
    }
    vector<unsigned> getRandomJBlock()
    {
        vector<unsigned> p = critical(), begins, ends, block;
        blocks(begins, ends, BLOCK_J);

        if (begins.size() <= 0)
            return begins;

        unsigned i = (begins.size() == 1) ? 0 : randint(0, begins.size() - 1);

        for (unsigned index = begins[i]; index <= ends[i]; index++)
            block.push_back(p[index]);
        return block;
    }
    vector<unsigned> getRandomMBlock()
    {
        vector<unsigned> p = critical(), begins, ends, block;
        blocks(begins, ends, BLOCK_M);

        if (begins.size() <= 0)
            return begins;

        unsigned i = (begins.size() == 1) ? 0 : randint(0, begins.size() - 1);

        for (unsigned index = begins[i]; index <= ends[i]; index++)
            block.push_back(p[index]);
        return block;
    }
    vector<unsigned> getRandomCriticalBlock()
    {
        vector<unsigned> block;
        if (randint(0, 1) == BLOCK_J)
        {
            block = getRandomJBlock();
            if (block.size() == 0)
                block = getRandomMBlock();
        }
        else
        {
            block = getRandomMBlock();
            if (block.size() == 0)
                block = getRandomJBlock();
        }
        return block;
    }
    vector<unsigned> getOperationsJBlock(unsigned o)
    {
        vector<unsigned> block;

        unsigned operation = getJobAncestor(o);

        do
        {
            block.push_back(operation);
            operation = sJ[operation];
        } while (operation != sJ[o]);

        return block;
    }
    vector<unsigned> getOperationsMBlock(unsigned o)
    {
        vector<unsigned> block;

        unsigned operation = getMachAncestor(o);

        do
        {
            block.push_back(operation);
            operation = sM[operation];
        } while (operation != sM[o]);

        return block;
    }
    vector<unsigned> getOperationsCriticalJBlock(unsigned o)
    {
        vector<unsigned> p = critical(), block;
        unsigned operation, index = findIndex(p, o);

        if (index > nO)
            return block;
        while (index <= nO)
        {
            operation = p[index];
            if (!sameJob(o, operation))
                break;
            block.push_back(operation);
            index--;
        }

        reverse(block.begin(), block.end());

        return block;
    }
    vector<unsigned> getOperationsCriticalMBlock(unsigned o)
    {
        vector<unsigned> p = critical(), block;
        unsigned operation, index = findIndex(p, o);

        if (index > nO)
            return block;
        while (index <= nO)
        {
            operation = p[index];
            if (!sameMach(o, operation))
                break;
            block.push_back(operation);
            index--;
        }

        reverse(block.begin(), block.end());

        return block;
    }
    vector<unsigned> getCriticalOperationsList()
    {
        vector<unsigned> list(nO + 1, 0);
        vector<unsigned> p = critical();

        for (unsigned o : p)
            list[o] = 1;

        return list;
    }
    vector<unsigned> getStarters(vector<unsigned> predecessors)
    {
        vector<unsigned> s;

        for (unsigned i = 1; i < predecessors.size(); i++)
            if (predecessors[i] == DUMMY)
                s.push_back(i);

        return s;
    }
    vector<vector<unsigned>> getWholeMachines()
    {
        vector<vector<unsigned>> machines;
        vector<unsigned> mStarters = getStarters(pM), block;
        //printv(mStarters, 0, "starters");
        unsigned op;

        for (unsigned starter : mStarters)
        {
            op = starter;
            block.clear();
            while (op != DUMMY)
            {
                block.push_back(op);
                op = sM[op];
            }
            if (block.size() > 0)
                machines.push_back(block);
        }
        return machines;
    }
    vector<vector<unsigned>> getWholeJobs()
    {
        vector<vector<unsigned>> jobs;
        vector<unsigned> jStarters = getStarters(pJ), block;
        //printv(jStarters, 0, "starters");
        unsigned op;

        for (unsigned starter : jStarters)
        {
            op = starter;
            block.clear();
            while (op != DUMMY)
            {
                block.push_back(op);
                op = sJ[op];
            }
            if (block.size() > 0)
                jobs.push_back(block);
        }
        return jobs;
    }
    bool isJEdgeN5(unsigned i1, unsigned i2, vector<unsigned> &p)
    {
        unsigned o1 = p[i1], o2 = p[i2];
        if (o1 == first && !adjJob(o2, p[i2 + 1]))
            return true;
        if (o1 == first || o2 == last || !sameJob(o1, o2))
            return false;
        if (!sameJob(o1, p[i1 - 1]) || !sameJob(o2, p[i2 + 1]))
            return true;
        return false;
    }
    bool isMEdgeN5(unsigned i1, unsigned i2, vector<unsigned> &p)
    {
        unsigned o1 = p[i1], o2 = p[i2];
        if (o1 == first && !adjMach(o2, p[i2 + 1]))
            return true;
        if (o1 == first || o2 == last || !sameMach(o1, o2))
            return false;
        if (!sameMach(o1, p[i1 - 1]) || !sameMach(o2, p[i2 + 1]))
            return true;
        return false;
    }
    vector<pair<unsigned, unsigned>> getJEdgesN5()
    {
        vector<pair<unsigned, unsigned>> edges;
        auto jBlocks = getCriticalJBlocks();
        filterNonTrivialBlocks(jBlocks);
        unsigned lastIdx;

        for (auto b : jBlocks)
        {
            lastIdx = b.size() - 1;

            if (b.size() == 2)
                edges.push_back(make_pair(b[0], b[1]));
            else
            {
                if (b[0] != first)
                    edges.push_back(make_pair(b[0], b[1]));
                if (b[lastIdx] != last)
                    edges.push_back(make_pair(b[lastIdx - 1], b[lastIdx]));
            }
        }

        return edges;
    }
    vector<pair<unsigned, unsigned>> getMEdgesN5()
    {
        vector<pair<unsigned, unsigned>> edges;
        auto mBlocks = getCriticalMBlocks();
        filterNonTrivialBlocks(mBlocks);
        unsigned lastIdx;

        for (auto b : mBlocks)
        {
            lastIdx = b.size() - 1;

            if (b.size() == 2)
                edges.push_back(make_pair(b[0], b[1]));
            else
            {
                if (b[0] != first)
                    edges.push_back(make_pair(b[0], b[1]));
                if (b[lastIdx] != last)
                    edges.push_back(make_pair(b[lastIdx - 1], b[lastIdx]));
            }
        }

        return edges;
    }
    bool isN7ShiftAllowed(unsigned o1, unsigned o2, unsigned blockSize)
    {
        bool areAdjacent = adjJob(o1, o2) || adjMach(o1, o2);
        if (blockSize == 2 && areAdjacent)
            return true;

        bool isFirstEdge = areAdjacent && (o1 == first || o2 == first);
        bool isLastEdge = areAdjacent && (o1 == last || o2 == last);

        return !isFirstEdge && !isLastEdge;
    }

    unsigned getOneRandomOperation_swapCritical()
    {
        vector<unsigned> p = critical();
        unsigned o, pathIndex;
        pathIndex = randint(0, p.size() - 1);
        o = p[pathIndex];

        return o;
    }
    unsigned getOneRandomOperation_swapCriticalEdge()
    {
        vector<unsigned> block;
        unsigned o = 0;

        block = getRandomCriticalBlock();

        if (block.size() <= 0)
            return 0;

        if (randint(0, 1) == BLOCK_START)
            o = block[0];
        else
            o = block[block.size() - 1];

        return o;
    }
    unsigned getOneRandomOperation_shiftWhole()
    {
        unsigned o = 0;

        do
            o = randint(1, nO);
        while (o == first);

        return o;
    }
    unsigned getOneRandomOperation_shiftCritical()
    {
        vector<unsigned> begins, ends, p = critical();

        unsigned blockType = randint(0, 1), o;
        blocks(begins, ends, blockType);
        if (ends.size() == 1)
        {
            begins.clear();
            ends.clear();
            blocks(begins, ends, (blockType + 1) % 2);
        }

        if (ends.size() == 1)
            return p[p.size() - 2];
        unsigned index = ends.size() == 2 ? 0 : randint(0, ends.size() - 2);
        o = p[ends[index]];

        return o;
    }
    unsigned getOneRandomOperation(unsigned neighborType)
    {
        switch (neighborType)
        {
        case SWAP_CRITICAL:
            return getOneRandomOperation_swapCritical();
        case SWAP_CRITICAL_EDGE:
            return getOneRandomOperation_swapCriticalEdge();
        case SHIFT_WHOLE:
            return getOneRandomOperation_shiftWhole();
        case SHIFT_CRITICAL:
            return getOneRandomOperation_shiftCritical();
        default:
            return UMAX;
        }
    }

    vector<unsigned> getAllOperations_swapCritical()
    {
        return critical();
    }
    vector<unsigned> getAllOperations_swapCriticalEdge()
    {
        vector<unsigned> ops, p = critical();

        vector<vector<unsigned>> jB = getCriticalJBlocks(), mB = getCriticalMBlocks();
        filterNonTrivialBlocks(jB);
        filterNonTrivialBlocks(mB);

        for (auto bs : {jB, mB})
            for (auto b : bs)
                ops.push_back(b[b.size() - 1]);
        if (ops.size() > 0)
            ops.pop_back();

        return ops;
    }
    vector<unsigned> getAllOperations_shiftWhole()
    {
        vector<unsigned> operations;

        for (unsigned i = 1; i < nO; i++)
            if (i != first)
                operations.push_back(i);

        return operations;
    }
    vector<unsigned> getAllOperations_shiftCritical()
    {
        return critical();
    }
    vector<unsigned> getAllOperations(unsigned neighborType)
    {
        switch (neighborType)
        {
        case SWAP_CRITICAL:
            return getAllOperations_swapCritical();
        case SWAP_CRITICAL_EDGE:
            return getAllOperations_swapCriticalEdge();
        case SHIFT_WHOLE:
            return getAllOperations_shiftWhole();
        case SHIFT_CRITICAL:
            return getAllOperations_shiftCritical();
        }
    }

    ScheduleDataRetriever() {}
    ScheduleDataRetriever(Schedule s)
    {
        setSchedule(s);
    }
    ScheduleDataRetriever(Instance i, int method) : Schedule(i, method) {}

    friend class NeighborGenerator;
};