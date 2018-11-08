/*
	Class declarations for free and active lists.
*/

#ifndef _LISTS_HPP
#define _LISTS_HPP

#include "globals.hpp"
#include "logging.hpp"
#include "instruction.hpp"

#include <string>
#include <tuple>
#include <utility>

class FreeList {
protected:
	bool free[NUM_PHY_REGS];		// boolean array indicating which physical registers are free
public:
	FreeList();						// default constructor
	void add(int);					// add a physical register to this free list
	void remove(int);				// remove a physical register from this free list
	int find(void);					// find the first free physical register
};

class ActiveList {
protected:
public:
	Buffer< std::pair<Instruction, bool> > _q;					// stores instructions and their completion status
	Buffer<Instruction> *o;										// pointer to buffer containing finished instructions
	FreeList *f;												// pointer to free list of physical registers
	int *r;
	bool *b;
	ActiveList(FreeList*, Buffer<Instruction>*, int*, bool*);	// instantiate active list with given pointers
	int push(Instruction);										// push an instruction if list isn't full
	void graduate(Instruction&);								// mark given instruction as completed and remove 
																// consecutive graduated instructions from front
	void flush(int);
	
};

#endif