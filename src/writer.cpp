/*
	Class definition for the writeback unit.
*/

#include "writer.hpp"

extern std::map<std::string, int> OPCODE, FUNCT;
extern int CLOCK;

Writer::Writer(	Latch< std::tuple<Instruction, int> >* in1, 
				Latch< std::tuple<Instruction, int, int> >* in2, 
				Latch< std::tuple<Instruction, int> >* in3,
				ActiveList* a, IntegerRegisterFile* rf, bool *b){
	this->in1 = in1; this->in2 = in2; this->in3 = in3; this->a = a; this->rf = rf;
	this->b = b;
} 

void Writer::tick(void){
	if(in1->valid()){
		t1 = in1->read();
		Instruction i1 = std::get<0>(t1);
		if(i1.is_valid() && std::get<0>(i1.type()) == 0){
			rf->write(std::get<2>(i1.physical_regs()), std::get<1>(t1));
			*(b + std::get<2>(i1.physical_regs())) = false;
		}
	}
	if(in2->valid()){
		t2 = in2->read();
		Instruction i2 = std::get<0>(t2);
		if(i2.is_valid()){
			if(std::get<0>(i2.type()) == 0 && std::get<1>(i2.type()) != FUNCT["div"])
				rf->write(std::get<2>(i2.physical_regs()), std::get<1>(t2));
			else{
				rf->write_lo(std::get<1>(t2));
				rf->write_hi(std::get<2>(t2));
			}
			*(b + std::get<2>(i2.physical_regs())) = false;
		}
	}
	if(in3->valid()){
		t3 = in3->read();
		Instruction i3 = std::get<0>(t3);
		if(i3.is_valid()){
			if(std::get<0>(i3.type()) == OPCODE["lw"]){
				rf->write(std::get<2>(i3.physical_regs()), std::get<1>(t3));
				*(b + std::get<2>(i3.physical_regs())) = false;
			}
		}
	}

}

void Writer::tock(void){
	Instruction i1 = std::get<0>(t1), i2 = std::get<0>(t2), i3 = std::get<0>(t3);
	if(i1.is_valid()){
		i1.WB = CLOCK;
		a->graduate(i1);
		Instruction reset; t1 = std::make_tuple(reset, 0);
	}
	if(i2.is_valid()){
		i2.WB = CLOCK;
		a->graduate(i2);
		Instruction reset; t2 = std::make_tuple(reset, 0, 0);
	}
	if(i3.is_valid()){
		i3.WB = CLOCK;
		a->graduate(i3);
		Instruction reset; t3 = std::make_tuple(reset, 0);
	}
}

void Writer::flush(int id){
	if(in1->valid()){
		auto inp = in1->read();
		auto iRead = std::get<0>(inp);
		auto inp1 = std::get<1>(inp);
		if(iRead.get_id() < id){
			in1->write(std::make_tuple(iRead, inp1));
		}
	}
	if(in3->valid()){
		auto inp = in3->read();
		auto iRead = std::get<0>(inp);
		auto inp1 = std::get<1>(inp);
		if(iRead.get_id() < id){
			in3->write(std::make_tuple(iRead, inp1));
		}
	}
	if(in2->valid()){
		auto inp = in2->read();
		auto iRead = std::get<0>(inp);
		auto inp1 = std::get<1>(inp);
		auto inp2 = std::get<2>(inp);
		if(iRead.get_id() < id){
			in2->write(std::make_tuple(iRead, inp1, inp2 ));
		}
	}
}