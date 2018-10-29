/*
	Class definition for registers and related entities.
*/

#ifndef _REGISTER_HPP
#define _REGISTER_HPP

#include "globals.hpp"

class IntegerRegisterFile {
protected:
	int value[NUM_PHY_REGS];		// values of physical registers
	long hi, lo; 					// special registers
public: 
	IntegerRegisterFile() {								// default constructor
		for(int i=0; i < NUM_PHY_REGS; i++) {
			value[i] = i;
		}
		hi = lo = 0;
	}

	void write(int reg, int val){ value[reg] = val; }	// functions for reading and writing to 
	int read(int reg){ return value[reg]; }				// all physical, and two special registers
	void write_hi(int val){ hi = val; }
	void write_lo(int val){ lo = val; }
	int read_hi(void){ return hi; }
	int read_lo(void){ return lo; }
};

#endif