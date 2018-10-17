/*
    Class definition for DecodeUnit.
*/

#include "entities.hpp"
#include <bits/stdc++.h>

#define FOR(i, a, b) for(int i=(a); i<(b); i++)
#define REP(i,n) FOR(i, 0, n)
#define MAXACTIVELIST 32



template <size_t N1,size_t N2,size_t N>
bitset <N2-N1+1> extract(bitset <N> s,int N1, int N2){
	s=s<<(N1-1);
	s=s>>(N-N2+N1-1);
	long int temp = (long int)(s.to_ulong());
	bitset<N2-N1+1> bitset2 = temp;
	return bitset2; 
}

template <size_t N1,size_t N2>
bitset <N2+N1> concat(bitset <N1> s1, bitset<N2> s2){
	string b1=s1.to_string();
	string b2=s2.to_string();
	return bitset <N1+N2>(b1+b2);
}

// Data Structures to be global
class DecodeUnit {
protected:
    // data members here
	queue<bitset<32> > InstructionBuffer; // Max Size ???
	map<bitset<5>, bitset<6> > RAT;
	queue<bitset<6> > FreeList;
	map<bitset<6>, bitset<1> > BusyBitTable;
	queue<bitset<17> > ActiveList;   // Max Size 32
		// Fisrt 5 bits for tag
		// Next 6 bits for old physical destination
		// Next 5 bits from logical destination
		// Next 1 bit for done or not
	int tag;
	void decodeInstruction(bitset<32>);
public:
	void DecodeUnit();
	bitset<10> getfunc(bitset<12>);
    void tick(vector<bitset<32> >);
    void tock();
    // other functions declarations here
};

void DecodeUnit::DecodeUnit(){
	/*
		Initialize the RAT table (for each logical register one-one map to same phyical )
		Initialize the free list (last 32 physical registers)
		Initialize the busy bit table (all physical registers 0) 
		Initialize tag to 0
	*/
}

bitset<10> getfunc(bitset<12> opcodes){
	return ;
}

void DecodeUnit::tick(vector<bitset<32> > instructions) {
    // code here
    /*
		Accepts as arguments 4 instructions in form of vector bitset<32> and push then to Instruction Buffer
		if activeList Full or Instruction Queues Full: (second cond ifnored for now)
			Sleep
		Pop top four instruction from Instruction Buffer
		For each instruction sequentially 
			decodes instructions (only operands to be done for now) (only additions no immediate no load store)
			Branch Predicitor // ignored for now
			for other oeprands map them through RAT table
			for each destination operand get something from freelist
			store old desti rat and then update the rat for desti
			update busy bit for new desti rats
			append active listand instruction queue		
    */
	REP(i, 4) {
		InstructionBuffer.push(instructions[i]);
	}
	// activeList or Instruction Queue(to be included) full
	if (activeList.size() < MAXACTIVELIST){
		return;
	}

	vector<bitset<5+6+6+6+6+3+10+4+4> > toInstructionQueue;


	REP(i, 4) {
		bitset<32> instruction = InstructionBuffer.front(); InstructionBuffer.pop();
		
		bitset<5> rsou1 = extract(instruction,16,20);
		bitset<5> rsou2 = extract(instruction,11,15);
		bitset<5> rsou3 = extract(instruction,6,10);
		bitset<5> rdest = extract(instruction,1,5);
		bitset<6> prsou1 = RAT[rsou1];
		bitset<6> prsou2 = RAT[rsou2];
		bitset<6> prsou3 = RAT[rsou3];
		bitset<6> oldprdest = RAT[rdest];
		bitset<6> newprdest = FreeList.front(); FreeList.pop();
		RAT[rdest] = newprdest;
		BusyBitTable[newprdest] = 1;
		
		bitset<12> topushtoactive = concat((bitset<5>) tag, concat(concat(oldprdest,rdest), (bitset<1>) 0));

		thisIns = concat(
			concat(brMask, Unit),
			concat(
				concat(
					getfunc(extract(instruction, 21, 32)), 
					concat(BusyBitTable[prsou1], concat(BusyBitTable[prsou2], BusyBitTable[prsou3]))
				),
				concat(
					concat(prsou1, concat(prsou2, concat(prsou3))),
					concat(newprdest, tag++)
				)
			)
		)
		
		toInstructionQueue.PB(thisIns);
	}
	return toInstructionQueue; //// ???
}

void DecodeUnit::tock() {
    // code here
    return;
}

// other function definitions here