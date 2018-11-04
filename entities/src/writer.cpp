/*
	Class definition for the writeback unit.
*/

#include "writer.hpp"

#include <iostream>
using namespace std;

extern std::map<std::string, int> OPCODE, FUNCT;
extern int CLOCK;

Writer::Writer(	Latch< std::tuple<Instruction, int> >* in1, 
				Latch< std::tuple<Instruction, int, int> >* in2, Latch< std::tuple<Instruction, int> >* in3,ActiveList* a, IntegerRegisterFile* rf){
	this->in1 = in1; this->in2 = in2; this->in3 = in3,this->a = a; this->rf = rf;
} 

void Writer::tick(void){
	if(!in3->valid()){
		if(in1->valid())
			t1 = in1->read();
		if(in2->valid())
			t2 = in2->read();
	}
	else{
		t3 = in3->read();
		if(in2->valid()){
			t2 = in2->read();		
		}
		else if (in1->valid())
			t1 = in1->read();
		
		else{

	}	}
}


void Writer::tock(void){
	Instruction i1 = std::get<0>(t1), i2 = std::get<0>(t2), i3 = std::get<0>(t3);
	if(i1.is_valid()){
		// cout<<"i1\n";
		i1.WB = CLOCK;
		a->graduate(i1);
		rf->write(std::get<2>(i1.physical_regs()), std::get<1>(t1));
		Instruction reset; t1 = std::make_tuple(reset, 0);
	}
	if(i2.is_valid()){
		// cout<<"i2\n";

		i2.WB = CLOCK;
		a->graduate(i2);
		rf->write_lo(std::get<1>(t2));		// TODO check instruction type and write to hi/lo or phy_reg
		rf->write_hi(std::get<2>(t2));
		Instruction reset; t2 = std::make_tuple(reset, 0, 0);
	}
	if(i3.is_valid()){
		// cout<<"i3\n";
		i3.WB = CLOCK;
		a->graduate(i3);
		// cout<<i3.EXEC<<endl;
		if(std::get<0>(i3.type())== OPCODE["lw"])
			rf->write(std::get<2>(i3.physical_regs()), std::get<1>(t1));
		Instruction reset; t3 = std::make_tuple(reset, 0);
	}
}