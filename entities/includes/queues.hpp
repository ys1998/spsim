/*
	Class declarations for integer and address queues.
*/

#ifndef _QUEUES_HPP
#define _QUEUES_HPP

#include "globals.hpp"
#include "instruction.hpp"

#include <tuple>

class IntegerQueue {
protected:
	Buffer<Instruction> _q;				// queue storing instructions for issuing to ALUs
	bool *b;							// pointer to busy bit table
public:
	IntegerQueue(bool*);				// constructor
	int add(Instruction);				// add instruction to this queue
	Instruction issue(int);				// issue a specific instruction to an ALU
	void flush(int);
};

#endif