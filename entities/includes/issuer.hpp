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
	AddressQueue *aq;										
	IntegerRegisterFile *rf;								// pointer to integer register file
	int addrescal_latch,memory_latch;						// number of the addresscal latch and memory latch					
	int last_index_issued,present_index_issued,mem_address;					
	Latch< std::tuple<Instruction, int> >* outer_latch;
public:
	Issuer(IntegerQueue*, AddressQueue*, IntegerRegisterFile*);					// constructor
	/* Function to connect latches to this unit:
		3 -> memory 
		2 -> memory_cal
		1 -> regular
	*/
	void attach_latch(int, Latch< std::tuple<Instruction, int, int> >*,int);	
	void attach_cal_latch_outer(Latch< std::tuple<Instruction, int> >*);	
	void tick();		
	void tock();
	void flush(int);
};

#endif