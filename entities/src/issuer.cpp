/*
	Class definition for clocked entity that issues instructions to ALUs.
*/

#include "issuer.hpp"

#include <iostream>

extern int CLOCK;

Issuer::Issuer(IntegerQueue *iq, IntegerRegisterFile *rf){
	this->iq = iq; this->rf = rf;
}

void Issuer::attach_latch(int idx, Latch< std::tuple<Instruction, int, int> > *l){
	this->l[idx] = l;
	ready[idx] = true;
}

void Issuer::tick(void){
	for(int j=0; j < NUM_ALU; ++j){
		if(ready[j]){
			i[j] = iq->issue(j);
			if(i[j].is_valid()) {
				ready[j] = false;
			}
		}
	}
}

void Issuer::tock(void){
	for(int j=0; j < NUM_ALU; ++j){
		if(!l[j]->valid() && i[j].is_valid()){		// write only when previous data has been read
			auto regs = i[j].physical_regs();
			// i[j].RF = CLOCK;
			l[j]->write(std::make_tuple(i[j], rf->read(std::get<0>(regs)), rf->read(std::get<1>(regs))));
			ready[j] = true;
		}
	}
}

void Issuer::flush(int id){
	iq->flush(id);
	for(int j=0; j < NUM_ALU; ++j){
		if(l[j]->valid()){
			auto inp = l[j]->read();
			auto iRead = std::get<0>(inp);
			auto in1 = std::get<1>(inp);
			auto in2 = std::get<2>(inp);
			if(iRead.get_id() < id){
				l[j]->write(std::make_tuple(iRead, in1, in2 ));
			}
		}
	}

}