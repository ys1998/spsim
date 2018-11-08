/*
	Class declarations for integer and address queues.
*/

#ifndef _QUEUES_HPP
#define _QUEUES_HPP

#include "globals.hpp"
#include "instruction.hpp"
#include "lists.hpp"

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

class AddressQueue{
protected:
public:								
	Buffer<Instruction> _q;
	bool *b;							// pointer to busy bit table
	Buffer<int> _addr; 					// -2 -> not issued for addr_cal, -1 -> issued for addre_cal but address not calculated
	bool findswaddr(int);
	bool no_notcalculated_branch_above(Instruction);
	ActiveList *al;

	AddressQueue(bool*,ActiveList*);								// constructor
	int add(Instruction);								// add instruction to this queue
	std::tuple<Instruction, int, int> MEMissue();		// issue a specific instruction with address 
	std::tuple<Instruction, int>  ALUissue();			// issue a specific instrucion with index in AddressQueue of the instruction issued
};

#endif