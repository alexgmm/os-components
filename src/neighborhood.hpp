#ifndef NEIGHBORHOOD_HPP
#define NEIGHBORHOOD_HPP

#define BLOCK_J 0
#define BLOCK_M 1

#define SWAP_J 0
#define SWAP_M 1

#define SWAP_ALL 0
#define SWAP_CRITICAL 1
#define SWAP_CRITICAL_EDGE 2

#define SWAP_PRED 0
#define SWAP_SUCC 1

#include "solution.hpp"
#include <utility>

using namespace std;

class Neighborhood {
    Solution solution;
    Solution previous;
public:
    Neighborhood(){}
    Neighborhood(Solution s):solution(s){}

    void restore(){ solution = previous; }

    /////////////////////////////////////////////////////////////////////////
	//////// FUNÇÕES AUXILIARES PARA GERAÇÃO DE VIZINHOS ////////////////////
	/////////////////////////////////////////////////////////////////////////

	vector<unsigned> critical(){
		assert(solution.last <= solution.nO);
		vector<unsigned> late_pred = solution.getLatestPred(), p;
		unsigned op = solution.last;

		while(op > 0){
			p.push_back(op);
			op = late_pred[op];
		}
		reverse(p.begin(),p.end());
		return p;	
	}

	void blocksJ(vector<unsigned> p, vector<unsigned> &b_begin, vector<unsigned> &b_end){
		int block_size = 0;
        b_begin.push_back(0);
        for(int i=0; i<p.size()-1; i++){
            if(solution.instance.o_job[p[i]] != solution.instance.o_job[p[i+1]] && block_size > 1){
                b_end.push_back(i);
                b_begin.push_back(i+1);
				block_size = 0;
            } else block_size++;
        }
        b_end.push_back(p.size()-1);
		if(b_begin[b_begin.size()-1] == b_end[b_end.size()-1]){
			b_begin.pop_back();
			b_end.pop_back();
		}		
	}

    void blocksM(vector<unsigned> p, vector<unsigned> &b_begin, vector<unsigned> &b_end){
        int block_size = 0;
        b_begin.push_back(0);
        for(int i=0; i<p.size()-1; i++){
            if((solution.instance.o_machine[p[i]] != solution.instance.o_machine[p[i+1]]) && block_size > 1){
                b_end.push_back(i);
                b_begin.push_back(i+1);
                block_size = 0;
            } else block_size++;
        }
        b_end.push_back(p.size()-1);
		if(b_begin[b_begin.size()-1] == b_end[b_end.size()-1]){
			b_begin.pop_back();
			b_end.pop_back();
		}	
	}

	/////////////////////////////////////////////////////////////////////////
	//////// OPERADORES DE VIZINHANÇA ///////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////

	void shiftForwardJ(unsigned opIndex, vector<unsigned> &p, unsigned pos){
		for(unsigned i = opIndex; i < pos; i++)
			swapJ(p[i], p[i+1]);
	}

	void shiftForwardM(unsigned opIndex, vector<unsigned> &p, unsigned pos){
		for(unsigned i = opIndex; i < pos; i++)
			swapM(p[i], p[i+1]);
	}

	void shiftBackwardJ(unsigned opIndex, vector<unsigned> &p, unsigned pos){
		for(unsigned i = opIndex; i > pos; i--)
			swapJ(p[i], p[i-1]);
	}

	void shiftBackwardM(unsigned opIndex, vector<unsigned> &p, unsigned pos){
		for(unsigned i = opIndex; i > pos; i--)
			swapM(p[i], p[i-1]);
	}

	void swapJ(unsigned op1, unsigned op2){//cout << br << "swasolution.pJ(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0);
		assert(op2 > 0);
		assert(op1 != op2);
		assert(solution.adjJob(op1,op2));

		if(op1 == solution.first) solution.first = op2;
		if(op2 == solution.last) solution.last = op1;

		if(solution.sJ[op2] == op1){
			unsigned o = op1;
			op1 = op2;
			op2 = o;
		}

		unsigned jp=solution.pJ[op1], js=solution.sJ[op2]; 
			
		if(jp) solution.sJ[jp] = op2;
		if(js) solution.pJ[js] = op1;
		
		solution.sJ[op1] = js; solution.pJ[op1] = op2;
		solution.pJ[op2] = jp; solution.sJ[op2] = op1;
		
	}

	void swapM(unsigned op1, unsigned op2){//cout << br << "swasolution.pM(" << op1 << "," << op2 << ")" << br;
		assert(op1 > 0); assert(op2 > 0);
		assert(op1 != op2);
		assert(solution.adjMach(op1,op2));

		if(op1 == solution.first) solution.first = op2;
		if(op2 == solution.last) solution.last = op1;

		if(solution.sM[op2] == op1){
			unsigned o = op1;
			op1 = op2;
			op2 = o;
		}


		unsigned mp=solution.pM[op1], ms=solution.sM[op2]; 
			
		if(mp) solution.sM[mp] = op2;
		if(ms) solution.pM[ms] = op1;
		
		solution.sM[op1] = ms; solution.pM[op1] = op2;
		solution.pM[op2] = mp; solution.sM[op2] = op1;
	}

	void swapSucc(unsigned op){//cout << br << "swapSucc(" << op << ")" << br;
		if(solution.sJ[op] != DUMMY) 
			swapJ(op, solution.sJ[op]); 
		if(solution.sM[op] != DUMMY) 
			swapM(op, solution.sM[op]); 
	}
	void swapPred(unsigned op){//cout << br << "swapPred(" << op << ")" << br;
		if(solution.pJ[op] != DUMMY) 
			swapJ(op, solution.pJ[op]); 
		if(solution.pM[op] != DUMMY) 
			swapM(op, solution.pM[op]);	
	}

	void getRandomPair(vector<unsigned> &p, vector<unsigned> &blocksB, vector<unsigned> &blocksE, unsigned &op1, unsigned &op2){
		unsigned index;
		if(randint(0,1)==0){
			index = randint(0, blocksB.size()-1);
			op1 = p[blocksB[index]];
			op2 = p[blocksB[index + 1]];
		} else {
			index = randint(0, blocksE.size()-1);
			op1 = p[blocksE[index]];
			op2 = p[blocksE[index - 1]];
		}	
	}

	void swapAll(){
		unsigned op = randint(1, solution.nO);
		if(op == solution.first){
			swapSucc(op);
		} else if(op == solution.last){
				swapPred(op);
		} else if(randint(0,1) == SWAP_PRED)
			swapPred(op);
		else swapSucc(op);
	}

	void swapCritical(){
		//blocksB = indices de começo de blocos; blocksE = ... de final ...
		vector<unsigned> blocksB, blocksE, p = critical();

		(randint(0,1)==BLOCK_J) ? blocksJ(p, blocksB, blocksE) : blocksM(p, blocksB, blocksE);
	
		if(blocksB.size() < 1) return;
		
		unsigned randomBlockIndex, randomOpIndex, randomOp;
		(blocksB.size() == 1) ? randomBlockIndex = 0 : randomBlockIndex = randint(0,blocksB.size()-1);
		randomOpIndex = randint(blocksB[randomBlockIndex], blocksE[randomBlockIndex]);
		randomOp = p[randomOpIndex];

		if(randomOp == solution.first || randomOpIndex == blocksB[randomBlockIndex]){
			swapSucc(randomOp); return;
		}

		if(randomOp == solution.last || randomOpIndex == blocksE[randomBlockIndex]){
			swapPred(randomOp); return;
		}

		if(randint(0,1) == SWAP_PRED)
			swapPred(randomOp);
		else swapSucc(randomOp);
	}
	
	void swapCriticalEdge(){
		vector<unsigned> blocksB, blocksE, p = critical();
		unsigned op1, op2;

		(randint(0,1)==BLOCK_J) ? blocksJ(p, blocksB, blocksE) : blocksM(p, blocksB, blocksE);
	
		if(blocksB.size() < 1) return;

		getRandomPair(p, blocksB, blocksE, op1, op2);

		(randint(0,1)==SWAP_J) ? swapJ(op1, op2) : swapM(op1, op2);
	}

	Solution getNeighbor(int oper){
		previous = solution.copySolution();

		switch (oper){
		case SWAP_ALL:
			swapAll();
			break;
		case SWAP_CRITICAL:
			swapCritical();
			break;
		case SWAP_CRITICAL_EDGE:
			swapCriticalEdge();
			break;
		default:
			break;
		}
		return solution;
	}

    void traverseSwapSucc(vector<pair<Solution, unsigned>> &n, unsigned op){
        unsigned makespan = -1;
        while(makespan == -1){
            swapSucc(op);
            makespan = solution.calcMakespan();
        }
        n.push_back(make_pair(solution, makespan));
        restore();
        if(solution.sJ[op] != solution.last && solution.sJ[op] != DUMMY)
            traverseSwapSucc(n, solution.sJ[op]);
        if(solution.sM[op] != solution.last && solution.sM[op] != DUMMY)
            traverseSwapSucc(n, solution.sM[op]);
    }

    void traverseSwapPred(vector<pair<Solution, unsigned>> &n, unsigned op){
        unsigned makespan = -1;
        while(makespan == -1){
            swapPred(op);
            makespan = solution.calcMakespan();
        }
        n.push_back(make_pair(solution, makespan));
        restore();
        if(solution.pJ[op] != solution.first && solution.pJ[op] != DUMMY)
            traverseSwapPred(n, solution.pJ[op]);
        if(solution.pM[op] != solution.first && solution.pM[op] != DUMMY)
            traverseSwapPred(n, solution.pM[op]);
    }


    vector<pair<Solution, unsigned>> swapAll(int oper){
        vector<pair<Solution, unsigned>> neighborhood;

        switch (oper){
        case SWAP_SUCC:
            traverseSwapSucc(neighborhood, solution.first);
            break;
        case SWAP_PRED:
            traverseSwapPred(neighborhood, solution.first);
            break;
        default:
            break;
        }
        return neighborhood;
    }

    vector<pair<Solution, unsigned>> getNeighborhood(int oper){
        vector<pair<Solution, unsigned>> neighborhood;

        switch (oper){
        case SWAP_PRED:
            return swapAll(SWAP_PRED);
            break;
        case SWAP_SUCC:
            return swapAll(SWAP_SUCC);
            break;
        default:
            break;
        }
    }

    Solution getSolution(){ return solution; }

};

#endif