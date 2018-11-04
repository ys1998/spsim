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
};

class AddressQueue{
protected:
public:								
	Buffer<Instruction> _q;
	bool *b;							// pointer to busy bit table
	Buffer<int> _addr; 					//_addr[i]=-2=> not issued for addr_cal,-1=> issued for addre_cal but address not calculated.
	// Instruction issued_for_addresscal  //Instruction issued for address_cal
	bool findswaddr(int);

	AddressQueue(bool*);										// constructor
	int add(Instruction);										// add instruction to this queue
	std::tuple<Instruction, int, int> MEMissue();		// issue a specific instruction with address 
	std::tuple<Instruction, int>  ALUissue();		// issue a specific instrucion with index in AddressQueue of the instruction issued
};

#endif