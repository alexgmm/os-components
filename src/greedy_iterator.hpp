#pragma once

#include "neighborhood.hpp"
#include "time.hpp"

using namespace std;

void printMutation(Mutation m){
    string mutationTypeLabel, blockTypeLabel;

    switch(m.mutationType){
        case SWAP_PRED:
            mutationTypeLabel = "swap_predecessor";
            break;
        case SWAP_SUCC:
            mutationTypeLabel = "swap_successor";
            break;
        case SHIFT_WHOLE:
            mutationTypeLabel = "shift " + to_string(m.factor) + " units";
            break;
        default:
            mutationTypeLabel = "shift";
            break;
    }

    switch(m.blockType){
        case BLOCK_J:
            blockTypeLabel = "job block";
            break;
        case BLOCK_M:
            blockTypeLabel = "mach block";
            break;
        default:
            blockTypeLabel = "???";
            break;
    }

    cout << mutationTypeLabel << "(" << m.operation << ") on " << blockTypeLabel << endl;
}

class GreedyIterator {
    Solution s;
    unsigned numberOps, oper;

    unsigned getOneRandomOperation_swapCritical(vector<unsigned> &used){
        vector<unsigned> p = s.critical();
        unsigned o, pathIndex;
        do {
        pathIndex = randint(0, p.size()-1);
        o = p[pathIndex];
        } while(used[o]);
        used[o] = 1;

        return o;
    }
    unsigned getOneRandomOperation_swapCriticalEdge(vector<unsigned> &used){
        vector<unsigned> block;
        unsigned o = 0;

        do {
            block = s.getRandomBlock();
            
            if(block.size()<=0)
                return 0;
                
            if(randint(0,1) == BLOCK_START)
                o = block[0];
            else
                o = block[block.size()-1];
        } while(used[o]);
        used[o] = 1;

        return o;
    }
    unsigned getOneRandomOperation_shiftWhole(vector<unsigned> &used){
        unsigned o = 0;

        do o = randint(1,s.nO);
        while(used[o] || o == s.first);
        used[o] = 1;

        return o;
    }
    unsigned getOneRandomOperation_shiftCritical(vector<unsigned> &used){
        vector<unsigned> begins, ends, p = s.critical();

        unsigned blockType = randint(0,1), o;
        s.blocks(begins, ends, blockType);

        do {
            unsigned index = ends.size() == 1? 0 : randint(0, ends.size() - 1);
            o = p[ends[index]];
        }while(used[o]);
        used[o] = 1;

        return o;
    }
    unsigned getOneRandomOperation(vector<unsigned> &used){
        switch(oper){
            case SWAP_CRITICAL:
                return getOneRandomOperation_swapCritical(used);
            case SWAP_CRITICAL_EDGE:
                return getOneRandomOperation_swapCriticalEdge(used);
            case SHIFT_WHOLE:
                return getOneRandomOperation_shiftWhole(used);
            case SHIFT_CRITICAL:
                return getOneRandomOperation_shiftCritical(used);
            default:
                return 0;
        }
    }

    vector<Mutation> listPossibleMutations_swapCritical(unsigned o){
        vector<Mutation> possibleMutations;
        vector<bool> isCriticalOperation = s.getCriticalOperationsList();
        Mutation mutation = {0, 0, 0, 0};

        if(isCriticalOperation[s.pM[o]] && s.pM[o] != s.first){
            mutation = {o, SWAP_PRED, BLOCK_M, 0};
            possibleMutations.push_back(mutation);
        }

        if(isCriticalOperation[s.sM[o]]){
            mutation = {o, SWAP_SUCC, BLOCK_M, 0};
            possibleMutations.push_back(mutation);
        }

        if(isCriticalOperation[s.pJ[o]] && s.pJ[o] != s.first){
            mutation = {o, SWAP_PRED, BLOCK_J, 0};
            possibleMutations.push_back(mutation);
        }

        if(isCriticalOperation[s.sJ[o]]){
            mutation = {o, SWAP_SUCC, BLOCK_J, 0};
            possibleMutations.push_back(mutation);
        }

        return possibleMutations;
    }
    void addPossibleMutationsOnJob(vector<Mutation> &possibleMutations, vector<unsigned> &block, unsigned o){
        if(block[0] == o || (block[o] == s.first && block[1] == o))
            return;
        
        unsigned index = 0;
        Mutation mutation = {0, 0, 0, 0};
        while(block[index] != o){
            mutation = {o, SHIFT_WHOLE, BLOCK_J, index + 1};
            possibleMutations.push_back(mutation);
            index++;
        }
    }
    void addPossibleMutationsOnMach(vector<Mutation> &possibleMutations, vector<unsigned> &block, unsigned o){
        if(block[0] == o || (block[o] == s.first && block[1] == o))
            return;
        
        unsigned index = 0;
        Mutation mutation = {0, 0, 0, 0};
        while(block[index] != o){
            mutation = {o, SHIFT_WHOLE, BLOCK_M, index + 1};
            possibleMutations.push_back(mutation);
            index++;
        }
    }
    vector<Mutation> listPossibleMutations_shiftWhole(unsigned o){
        vector<Mutation> possibleMutations;

        vector<unsigned> jobBlock = s.getOperationsJBlock(o), machBlock = s.getOperationsMBlock(o);

        addPossibleMutationsOnJob(possibleMutations, jobBlock, o);
        addPossibleMutationsOnMach(possibleMutations, machBlock, o);

        return possibleMutations;
    }
    vector<Mutation> listPossibleMutations_shiftCritical(unsigned o){
        vector<Mutation> possibleMutations;

        vector<unsigned> jobBlock = s.getOperationsCriticalJBlock(o), machBlock = s.getOperationsCriticalMBlock(o);

        if(jobBlock.size() > 1)
            addPossibleMutationsOnJob(possibleMutations, jobBlock, o);
        if(machBlock.size() > 1)
            addPossibleMutationsOnMach(possibleMutations, machBlock, o);

        return possibleMutations;
    }
    vector<Mutation> listPossibleMutations(unsigned o){
        vector<Mutation> possibleMutations;

        switch(oper){
            case SWAP_CRITICAL:
                possibleMutations = listPossibleMutations_swapCritical(o);
                break;
             case SWAP_CRITICAL_EDGE:
                possibleMutations = listPossibleMutations_swapCritical(o);
                break;
            case SHIFT_WHOLE:
                possibleMutations = listPossibleMutations_shiftWhole(o);
                break;
            case SHIFT_CRITICAL:
                possibleMutations = listPossibleMutations_shiftCritical(o);
                break;
        }

        return possibleMutations;        
    }

    void executeMutationGreedly(unsigned o){
        vector<Mutation> mutations = listPossibleMutations(o);

        if(mutations.size() == 0)
            return;

        Neighborhood n(s);

        unsigned bestValue = UMAX, value;
        Mutation bestMutation = {0,0,0,0};

        for(Mutation m:mutations){
            //printMutation(m);
            value = n.applyMutation(m);
            n.restore();

            if(value < bestValue && value > 0){
                bestValue = value;
                bestMutation = m;
            }
        }

        n.applyMutation(bestMutation);
        
        s = n.getSolution();
    }
public:
    GreedyIterator(){}
    GreedyIterator(Solution solution, unsigned number, unsigned o):s(solution),
                                                                   numberOps(number),
                                                                   oper(o){}

    void iterate(){
        vector<unsigned> used(s.nO + 1, 0);
        used[0] = 1;
        for(unsigned count = 1; count <= numberOps; count++){
            unsigned o = getOneRandomOperation(used);
            if(o==0)
                break;
            executeMutationGreedly(o);
        }
    }

    void test(){
        vector<unsigned> used(s.nO + 1, 0);
        unsigned o = getOneRandomOperation(used);
        vector<unsigned> b = s.getOperationsCriticalJBlock(o), c = s.getOperationsCriticalMBlock(o);
        printl("o",o);
        vector<Mutation> ms = listPossibleMutations_shiftCritical(o);
        for(Mutation m: ms)
            printMutation(m);
    }

    unsigned solve(){
        startTimeCounting();

        while(!isTimeOver())
            iterate();

        return s.computeMakespan();
    }

    Solution getSolution() {
        return s;
    }
};