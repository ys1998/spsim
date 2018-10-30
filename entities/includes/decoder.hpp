/*
	Class declaration for decoding unit.
*/

#ifndef _DECODER_HPP
#define _DECODER_HPP

#include "clocked_entity.hpp"
#include "globals.hpp"
#include "instruction.hpp"
#include "lists.hpp"
#include "queues.hpp"

#include <tuple>

class Decoder : public ClockedEntity {
protected:
	Buffer<Instruction> _q;								// queue that stores fetched instructions for decoding
	Buffer<Instruction> *d;								// pointer to queue of all fetched instructions
	FreeList *f;										// pointer to free list of physical registers
	ActiveList *a;										// pointer to list of active instructions
	int *r;												// pointer to logical to physical register mapping
	bool *b;											// pointer to busy bit table for physical registers
	IntegerQueue *iq;									// pointer to queue that stores decoded instructions
	bool decode_signal;									// signal indicating whether to stop decoding or not

	int decode_instr(Instruction);						// function for decoding an instruction; performs register
														// renaming and mapping of logical to physical registers
public:
	Decoder(Buffer<Instruction>*, FreeList*, ActiveList*, int*, bool*, IntegerQueue*);
	void tick();				
	void tock();
};

#endif