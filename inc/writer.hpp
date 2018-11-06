/*
	Class declaration for the writeback unit.
*/

#ifndef _WRITER_HPP
#define _WRITER_HPP

#include "globals.hpp"
#include "clocked_entity.hpp"
#include "instruction.hpp"
#include "latch.hpp"
#include "lists.hpp"
#include "register.hpp"

#include <tuple>

class Writer : public ClockedEntity {
protected:
	std::tuple<Instruction, int> t1,t3;						// read value of latch 1
	std::tuple<Instruction, int, int> t2;					// read value of latch 2
	Latch< std::tuple<Instruction, int> >* in1,*in3;				// pointer to latch 1
	Latch< std::tuple<Instruction, int, int> >* in2;		// pointer to latch 2
	ActiveList *a;											// pointer to list of active instructions
	IntegerRegisterFile *rf;								// pointer to integer register file
	bool *b;
public:
	Writer(	Latch< std::tuple<Instruction, int> >*, 
			Latch< std::tuple<Instruction, int, int> >*, 
			Latch< std::tuple<Instruction, int> >*,
			ActiveList*, IntegerRegisterFile*, bool*);
	void tick();
	void tock();
	// void flush(int);
};

#endif