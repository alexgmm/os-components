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

    bool isMutationPossible(Mutation m){
        vector<bool> isCriticalOperation = s.getCriticalOperationsList();

        if(m.mutationType == SWAP_PRED && m.blockType == BLOCK_M)
            return isCriticalOperation[s.pM[m.operation]] && s.pM[m.operation] != s.first;

        if(m.mutationType == SWAP_PRED && m.blockType == BLOCK_J)
            return isCriticalOperation[s.pJ[m.operation]] && s.pJ[m.operation] != s.first;

        if(m.mutationType == SWAP_SUCC && m.blockType == BLOCK_J)
            return isCriticalOperation[s.sJ[m.operation]];

        if(m.mutationType == SWAP_SUCC && m.blockType == BLOCK_M)
            return isCriticalOperation[s.sM[m.operation]];    
    }

    vector<Mutation> listPossibleMutations_swapCritical(unsigned o){
        vector<Mutation> possibleMutations;
        vector<bool> isCriticalOperation = s.getCriticalOperationsList();
        Mutation mutation = {0, 0, 0};

        if(isCriticalOperation[s.pM[o]] && s.pM[o] != s.first){
            mutation = {o, SWAP_PRED, BLOCK_M};
            possibleMutations.push_back(mutation);
        }

        if(isCriticalOperation[s.sM[o]]){
            mutation = {o, SWAP_SUCC, BLOCK_M};
            possibleMutations.push_back(mutation);
        }

        if(isCriticalOperation[s.pJ[o]] && s.pJ[o] != s.first){
            mutation = {o, SWAP_PRED, BLOCK_J};
            possibleMutations.push_back(mutation);
        }

        if(isCriticalOperation[s.sJ[o]]){
            mutation = {o, SWAP_SUCC, BLOCK_J};
            possibleMutations.push_back(mutation);
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

        if(mutations.size() == 0)
            return;

        Neighborhood n(s);

        unsigned bestValue = UMAX, value;
        Mutation bestMutation = {0,0,0};

        for(Mutation m:mutations){
            value = n.executeMutation(m);
            n.restore();

            if(value < bestValue && value > 0){
                bestValue = value;
                bestMutation = m;
            }
        }

        n.executeMutation(bestMutation);
        s = n.getSolution();
    }

public:
    GreedyIterator(){}
    GreedyIterator(Solution solution, unsigned number, unsigned o):s(solution),
                                                                   numberOps(number),
                                                                   oper(o){}

    void iterate(){
        vector<unsigned> used(s.nO + 1, 0);

        for(unsigned count = 1; count <= numberOps; count++){
            unsigned o = getOneRandomOperation(used);
            executeMutationGreedly(o);
        }
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