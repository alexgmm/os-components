#pragma once

#include "neighborhood.hpp"

using namespace std;

struct Mutation{
    unsigned operation, direction, blockType;
};

class GreedyIterator {
    Solution s;
    unsigned numberRandomOp, oper;

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

    unsigned getOneRandomOperation(vector<unsigned> &used){
        switch(oper){
            case SWAP_CRITICAL:
                return getOneRandomOperation_swapCritical(used);
            default:
                return 0;
        }
    }

    vector<unsigned> getRandomOperations(){
        vector<unsigned> operations, used(s.nO + 1, 0);
        used[s.first] = 1;

        for(int i = 0; i < numberRandomOp; i++)
            operations.push_back(getOneRandomOperation(used));

        return operations;
    }

    vector<Mutation> listPossibleMutations_swapCritical(unsigned o){
        vector<Mutation> possibleMutations;
        vector<bool> isCriticalOperation = s.getCriticalOperationsList();
        Mutation operation = {0, 0, 0};

        if(isCriticalOperation[s.pM[o]]){
            operation = {o, SWAP_PRED, BLOCK_M};
            possibleMutations.push_back(operation);
        }

        if(isCriticalOperation[s.sM[o]]){
            operation = {o, SWAP_SUCC, BLOCK_M};
            possibleMutations.push_back(operation);
        }

        if(isCriticalOperation[s.pJ[o]]){
            operation = {o, SWAP_PRED, BLOCK_J};
            possibleMutations.push_back(operation);
        }

        if(isCriticalOperation[s.sJ[o]]){
            operation = {o, SWAP_SUCC, BLOCK_J};
            possibleMutations.push_back(operation);
        }

        return possibleMutations;
    }

    vector<Mutation> listPossibleMutations(unsigned o){
        vector<Mutation> possibleMutations;

        switch(oper){
            case SWAP_CRITICAL:
                possibleMutations = listPossibleMutations_swapCritical(o);
        }

        return possibleMutations;        
    }

    void executeMutationGreedly(unsigned o){
        vector<Mutation> mutations = listPossibleMutations(o);
        
    }

public:
    GreedyIterator(){}
    GreedyIterator(Solution solution, unsigned number, unsigned o):s(solution),
                                                                   numberRandomOp(number),
                                                                   oper(o){}

    unsigned solve(){
        vector<unsigned> operations = getRandomOperations();

        for(unsigned o: operations)
            executeMutationGreedly(o);
        
        return 0;
    }
};