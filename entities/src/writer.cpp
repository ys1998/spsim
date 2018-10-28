/*
	Class definition for the writeback unit.
*/

#include "writer.hpp"

extern int CLOCK;

Writer::Writer(	Latch< std::tuple<Instruction, int> >* in1, 
				Latch< std::tuple<Instruction, int, int> >* in2, ActiveList* a, IntegerRegisterFile* rf){
	this->in1 = in1; this->in2 = in2; this->a = a; this->rf = rf;
} 

void Writer::tick(void){
	if(in1->valid())
		t1 = in1->read();
	if(in2->valid())
		t2 = in2->read();
}

void Writer::tock(void){
	Instruction i1 = std::get<0>(t1), i2 = std::get<0>(t2);;
	if(i1.is_valid()){
		i1.WB = CLOCK;
		a->graduate(i1);
		rf->write(std::get<2>(i1.physical_regs()), std::get<1>(t1));
		Instruction reset; t1 = std::make_tuple(reset, 0);
	}
	if(i2.is_valid()){
		i2.WB = CLOCK;
		a->graduate(i2);
		rf->write_lo(std::get<1>(t2));
		rf->write_hi(std::get<2>(t2));
		Instruction reset; t2 = std::make_tuple(reset, 0, 0);
	}
}