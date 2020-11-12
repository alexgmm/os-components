#pragma once

#include "../schedule/schedule_data_retriever.hpp"

using namespace std;

class PerturbationGenerator : public ScheduleDataRetriever
{
public:
    PerturbationGenerator() {}
    PerturbationGenerator(Schedule s)
    {
        setSchedule(s);
    }
    Schedule getSchedule()
    {
        return getCopy();
    }

    vector<Perturbation> getCriticalJBlockShifts(vector<unsigned> &block, unsigned o)
    {
        vector<Perturbation> possiblePerturbations;

        int factorIndex = block.size() - 2;
        unsigned opIndex = factorIndex + 1;
        Perturbation perturbation;

        while (factorIndex != -1)
        {
            if (isN7ShiftAllowed(o, block[factorIndex], block.size()))
            {
                perturbation = {o, SHIFT_CRITICAL, BLOCK_J, opIndex - factorIndex};
                possiblePerturbations.push_back(perturbation);
            }
            factorIndex--;
        }

        return possiblePerturbations;
    }
    vector<Perturbation> getCriticalMBlockShifts(vector<unsigned> &block, unsigned o)
    {
        vector<Perturbation> possiblePerturbations;

        int factorIndex = block.size() - 2;
        unsigned opIndex = factorIndex + 1;
        Perturbation perturbation;

        while (factorIndex != -1)
        {
            if (isN7ShiftAllowed(o, block[factorIndex], block.size()))
            {
                perturbation = {o, SHIFT_CRITICAL, BLOCK_M, opIndex - factorIndex};
                possiblePerturbations.push_back(perturbation);
            }
            factorIndex--;
        }

        return possiblePerturbations;
    }
    vector<Perturbation> getCriticalJBlockShifts(vector<unsigned> &block)
    {
        vector<Perturbation> ps;
        Perturbation p;
        unsigned factor;

        for (unsigned opIndex = block.size() - 1; opIndex > 0; opIndex--)
        {
            factor = opIndex;
            while (factor > 0)
            {
                if (isN7ShiftAllowed(block[opIndex], block[opIndex - factor], block.size()))
                {
                    p = {block[opIndex], SHIFT_CRITICAL, BLOCK_J, factor};
                    ps.push_back(p);
                }
                factor--;
            }
        }

        return ps;
    }
    vector<Perturbation> getCriticalMBlockShifts(vector<unsigned> &block)
    {
        vector<Perturbation> ps;
        Perturbation p;
        unsigned factor;

        for (unsigned opIndex = block.size() - 1; opIndex > 0; opIndex--)
        {
            factor = opIndex;
            while (factor > 0)
            {
                if (isN7ShiftAllowed(block[opIndex], block[opIndex - factor], block.size()))
                {
                    p = {block[opIndex], SHIFT_CRITICAL, BLOCK_M, factor};
                    ps.push_back(p);
                }
                factor--;
            }
        }

        return ps;
    }
    vector<Perturbation> getJobShifts(vector<unsigned> &job)
    {
        vector<Perturbation> ps;
        Perturbation p;
        unsigned factor;

        for (unsigned opIndex = job.size() - 1; opIndex > 0; opIndex--)
        {
            factor = opIndex;
            while (factor > 0)
            {
                p = {job[opIndex], SHIFT_WHOLE, BLOCK_J, factor--};
                ps.push_back(p);
            }
        }

        return ps;
    }
    vector<Perturbation> getMachineShifts(vector<unsigned> &mach)
    {
        vector<Perturbation> ps;
        Perturbation p;
        unsigned factor;

        for (unsigned opIndex = mach.size() - 1; opIndex > 0; opIndex--)
        {
            factor = opIndex;
            while (factor > 0)
            {

                p = {mach[opIndex], SHIFT_WHOLE, BLOCK_M, factor--};
                ps.push_back(p);
            }
        }

        return ps;
    }

    unsigned getSwapOperationFromPerturbation(Perturbation p)
    {
        vector<unsigned> block;
        if (p.blockType == BLOCK_J)
            block = p.perturbationType == SWAP_PRED ? pJ : sJ;
        else
            block = p.perturbationType == SWAP_PRED ? pM : sM;
        return block[p.operation];
    }
    void assertValidSwapPerturbation(Perturbation p)
    {
        auto o1 = p.operation;
        auto o2 = getSwapOperationFromPerturbation(p);
        auto isCriticalOperation = getCriticalOperationsList();
        if (o1 == 0 || o2 == 0 || isCriticalOperation[o1] == 0 || isCriticalOperation[o2] == 0)
        {
            cout << "(!) Invalid perturbation: " << getPerturbationString(p);
            exit(1);
        }
    }

    vector<Perturbation> listAllPerturbations_swapCritical()
    {
        vector<Perturbation> perturbations;
        Perturbation p;

        auto jB = getCriticalJBlocks(), mB = getCriticalMBlocks();
        filterNonTrivialBlocks(jB);
        filterNonTrivialBlocks(mB);

        for (auto b : jB)
            for (int i = 0; i < b.size() - 1; i++)
            {
                p = {b[i], SWAP_SUCC, BLOCK_J, 0};
                perturbations.push_back(p);
            }

        for (auto b : mB)
            for (int i = 0; i < b.size() - 1; i++)
            {
                p = {b[i], SWAP_SUCC, BLOCK_M, 0};
                perturbations.push_back(p);
            }

        return perturbations;
    }
    vector<Perturbation> listAllPerturbations_swapCriticalEdge()
    {
        vector<Perturbation> perturbations;
        Perturbation p;
        auto jE = getJEdgesN5(), mE = getMEdgesN5();

        for (auto e : jE)
        {
            p = {e.first, SWAP_SUCC, BLOCK_J, 0};
            perturbations.push_back(p);
        }

        for (auto e : mE)
        {
            p = {e.first, SWAP_SUCC, BLOCK_M, 0};
            perturbations.push_back(p);
        }

        return perturbations;
    }
    vector<Perturbation> listAllPerturbations_shiftCritical()
    {
        vector<Perturbation> perturbations;

        auto jB = getCriticalJBlocks(), mB = getCriticalMBlocks();

        filterNonTrivialBlocks(jB);
        filterNonTrivialBlocks(mB);

        for (auto b : jB)
        {
            auto p = getCriticalJBlockShifts(b);
            perturbations.insert(perturbations.end(), p.begin(), p.end());
        }

        for (auto b : mB)
        {
            auto p = getCriticalMBlockShifts(b);
            perturbations.insert(perturbations.end(), p.begin(), p.end());
        }

        return perturbations;
    }
    vector<Perturbation> listAllPerturbations_shiftWhole()
    {
        vector<Perturbation> ps;

        auto jobs = getWholeJobs(), machs = getWholeMachines();

        for (auto job : jobs)
        {
            auto p = getJobShifts(job);
            ps.insert(ps.end(), p.begin(), p.end());
        }

        for (auto mach : machs)
        {
            auto p = getMachineShifts(mach);
            ps.insert(ps.end(), p.begin(), p.end());
        }

        return ps;
    }

    vector<Perturbation> listPossiblePerturbations_swapCritical(unsigned o)
    {
        vector<Perturbation> possiblePerturbations;
        auto isCriticalOperation = getCriticalOperationsList();
        Perturbation perturbation;

        if (isCriticalOperation[pM[o]])
        {
            perturbation = {o, SWAP_PRED, BLOCK_M, 0};
            possiblePerturbations.push_back(perturbation);
        }

        if (isCriticalOperation[sM[o]])
        {
            perturbation = {o, SWAP_SUCC, BLOCK_M, 0};
            possiblePerturbations.push_back(perturbation);
        }

        if (isCriticalOperation[pJ[o]])
        {
            perturbation = {o, SWAP_PRED, BLOCK_J, 0};
            possiblePerturbations.push_back(perturbation);
        }

        if (isCriticalOperation[sJ[o]])
        {
            perturbation = {o, SWAP_SUCC, BLOCK_J, 0};
            possiblePerturbations.push_back(perturbation);
        }

        return possiblePerturbations;
    }
    vector<Perturbation> listPossiblePerturbations_swapCriticalEdge(unsigned o)
    {
        vector<Perturbation> possiblePerturbations;
        Perturbation p;
        unsigned pType;

        auto jEdges = getJEdgesN5(), mEdges = getMEdgesN5();

        for (auto e : jEdges)
        {
            p = {e.first, SWAP_SUCC, BLOCK_J, 0};
            possiblePerturbations.push_back(p);
        }

        for (auto e : mEdges)
        {
            p = {e.first, SWAP_SUCC, BLOCK_M, 0};
            possiblePerturbations.push_back(p);
        }

        return possiblePerturbations;
    }
    vector<Perturbation> listPossiblePerturbations_shiftWhole(unsigned o)
    {
        vector<Perturbation> possiblePerturbations;
        Perturbation m;
        unsigned pred = pJ[o], factor = 1;

        while (pred != 0)
        {
            m = {o, SHIFT_WHOLE, BLOCK_J, factor++};
            possiblePerturbations.push_back(m);
            pred = pJ[pred];
        }
        pred = pM[o];
        factor = 1;
        while (pred != 0)
        {
            m = {o, SHIFT_WHOLE, BLOCK_M, factor++};
            possiblePerturbations.push_back(m);
            pred = pM[pred];
        }

        return possiblePerturbations;
    }
    vector<Perturbation> listPossiblePerturbations_shiftCritical(unsigned o)
    {
        vector<Perturbation> possiblePerturbations;

        auto j = getOperationsCriticalJBlock(o);
        if (j.size() > 1)
        {
            auto p = getCriticalJBlockShifts(j, o);
            possiblePerturbations.insert(possiblePerturbations.end(), p.begin(), p.end());
        }

        auto m = getOperationsCriticalMBlock(o);
        if (m.size() > 1)
        {
            auto p = getCriticalMBlockShifts(m, o);
            possiblePerturbations.insert(possiblePerturbations.end(), p.begin(), p.end());
        }

        return possiblePerturbations;
    }

    vector<Perturbation> listPossiblePerturbations(unsigned o, unsigned neighborType)
    {
        switch (neighborType)
        {
        case SWAP_CRITICAL:
            return listPossiblePerturbations_swapCritical(o);
        case SWAP_CRITICAL_EDGE:
            return listPossiblePerturbations_swapCriticalEdge(o);
        case SHIFT_WHOLE:
            return listPossiblePerturbations_shiftWhole(o);
        case SHIFT_CRITICAL:
            return listPossiblePerturbations_shiftCritical(o);
        }
    }
    vector<Perturbation> listAllPerturbations(unsigned neighborType)
    {
        switch (neighborType)
        {
        case SWAP_CRITICAL:
            return listAllPerturbations_swapCritical();
        case SWAP_CRITICAL_EDGE:
            return listAllPerturbations_swapCriticalEdge();
        case SHIFT_WHOLE:
            return listAllPerturbations_shiftWhole();
        case SHIFT_CRITICAL:
            return listAllPerturbations_shiftCritical();
        }
    }

    void printSchedule()
    {
        int ini = 1;
        printv(sJ, ini, "Sucessores nos jobs:");
        printv(pJ, ini, "Predecessores nos jobs:");
        printv(sM, ini, "Sucessores nas maquinas:");
        printv(pM, ini, "Predecessores nas maquinas:");
        //printv(instance.cost, 1, "custo");
    }
};
