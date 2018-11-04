/*
	Class declaration for the Memory Unit.
*/

#ifndef _MEM_HPP
#define _MEM_HPP

#include "globals.hpp"
#include "clocked_entity.hpp"
#include "instruction.hpp"
#include "latch.hpp"

#include <tuple>
#include <map>
#include <math.h>

class MEM : public ClockedEntity{
protected:
	Instruction i;										// instruction undergoing memory stuff
	Latch< std::tuple<Instruction, int, int> >* in;		// latch from which input is read
	Latch< std::tuple<Instruction, int> >* out;			// latch where output is stored/passed
	bool *b;											// pointer to busy bit table
	int* DCache;										// DCache
	int stall_cycles;									// number of cycles for which ALU is going to be busy
	int in1, in2, res;									// inputs and result of operation
	bool read;											// boolean indicating whether ALU should read the next instr
	
public:
	MEM(Latch< std::tuple<Instruction, int, int> >* in, Latch< std::tuple<Instruction, int> >* out, int* DCache, bool *b);
	void tick();
	void tock();
};

#endif