/*
	Class declaration for instruction fetching unit.
*/

#ifndef _FETCHER_HPP
#define _FETCHER_HPP

#include "clocked_entity.hpp"
#include "globals.hpp"
#include "instruction.hpp"

class Fetcher : public ClockedEntity {
protected:
	int _PC;
	Buffer<Instruction> *cache;
	Buffer<Instruction> *dest;
public:
	Fetcher(Buffer<Instruction>*, Buffer<Instruction>*);
	void tock(void);
	void update_PC(int);
};

#endif