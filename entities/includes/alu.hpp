/*
	Class declaration for the two Arithmetic Logic Units (ALUs).
*/

#ifndef _ALU_HPP
#define _ALU_HPP

#include "globals.hpp"
#include "clocked_entity.hpp"
#include "instruction.hpp"
#include "latch.hpp"
#include "flusher.hpp"

#include <tuple>
#include <map>
#include <math.h>

class ALU2 : public ClockedEntity {
protected:
	Instruction i;										// instruction undergoing execution
	std::map<int, int> latencies;						// operation-latency mapping
	int stall_cycles;									// number of cycles for which ALU is going to be busy
	int in1, in2, hi, lo;								// inputs and results of operation
	bool read;											// boolean indicating whether ALU should read the next instr
	Latch< std::tuple<Instruction, int, int> >* in;		// latch from which input is read
	Latch< std::tuple<Instruction, int, int> >* out;	// latch where output is stored/passed
	bool *b;											// pointer to busy bit table

	void operate();										// function to perform the specified operation
public:
	ALU2(Latch< std::tuple<Instruction, int, int> >*, Latch< std::tuple<Instruction, int, int> >*, bool*);
	void tick();
	void tock();
	void flush(int);
};


class ALU1 : public ClockedEntity {
protected:
	Instruction i;										// instruction undergoing execution
	std::map<int, int> latencies;						// operation-latency mapping
	int stall_cycles;									// number of cycles for which ALU is going to be busy
	int in1, in2, res;									// inputs and result of operation
	bool read;											// boolean indicating whether ALU should read the next instr
	Latch< std::tuple<Instruction, int, int> >* in;		// latch from which input is read
	Latch< std::tuple<Instruction, int> >* out;			// latch where output is stored/passed
	bool *b;											// pointer to busy bit table
	ALU2 *a;
	Flusher *f;
	void operate();										// function to perform the specified operation
public:
	ALU1(Latch< std::tuple<Instruction, int, int> >*, Latch< std::tuple<Instruction, int> >*, bool*, ALU2*, Flusher*);
	void tick();
	void tock();
};



#endif
