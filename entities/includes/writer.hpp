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
	std::tuple<Instruction, int> t1;						// read value of latch 1
	std::tuple<Instruction, int, int> t2;					// read value of latch 2
	Latch< std::tuple<Instruction, int> >* in1;				// pointer to latch 1
	Latch< std::tuple<Instruction, int, int> >* in2;		// pointer to latch 2
	ActiveList *a;											// pointer to list of active instructions
	IntegerRegisterFile *rf;								// pointer to integer register file
public:
	Writer(	Latch< std::tuple<Instruction, int> >*, 
			Latch< std::tuple<Instruction, int, int> >*, ActiveList*, IntegerRegisterFile*);
	void tick();
	void tock();
};

#endif