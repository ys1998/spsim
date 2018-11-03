/*
	Class definition for clocked entity that issues instructions to ALUs.
*/

#ifndef _ISSUER_HPP
#define _ISSUER_HPP

#include "clocked_entity.hpp"
#include "globals.hpp"
#include "instruction.hpp"
#include "latch.hpp"
#include "queues.hpp"
#include "register.hpp"

#include <tuple>

class Issuer : public ClockedEntity {
protected:
	Instruction i[NUM_ALU];									// instructions read from input queue
	Latch< std::tuple<Instruction, int, int> >* l[NUM_ALU];	// output latches to ALU
	bool ready[NUM_ALU];									// indicates whether instruction should be read from queue for a latch
	IntegerQueue *iq;										// pointer to integer queue
	IntegerRegisterFile *rf;								// pointer to integer register file

public:
	Issuer(IntegerQueue*, IntegerRegisterFile*);							// constructor
	void attach_latch(int, Latch< std::tuple<Instruction, int, int> >*);	// function to connect latches to this unit
	void tick();		
	void tock();
	void flush(int);
};

#endif