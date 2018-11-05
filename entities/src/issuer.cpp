/*
	Class definition for clocked entity that issues instructions to ALUs.
*/

#include "issuer.hpp"

#include <iostream>

extern int CLOCK;
using namespace std;

Issuer::Issuer(IntegerQueue *iq, AddressQueue *aq, IntegerRegisterFile *rf){
	this->iq = iq; this->rf = rf;
	this->aq = aq;
	// n_reg_latch=1;
}

void Issuer::attach_latch(int idx, Latch< std::tuple<Instruction, int, int> > *l,int type){
	this->l[idx] = l;
	ready[idx] = true;
	if(type==2)
		addrescal_latch =idx;
	if(type==3)
		memory_latch = idx;
}
void Issuer::attach_cal_latch_outer(Latch< std::tuple<Instruction, int> > *l){
	this->outer_latch = l; 
	// outer_latch_ready = true;		
}

void Issuer::tick(void){
	// cout<<"ISSUE TICK: \n";
	if(outer_latch->valid()){
		// cout<<"OUTERLATCH  read ";
			auto temp = outer_latch->read();
			// cout<<" INST_ID:"<<(std::get<0>(temp)).get_id() <<"\n";
		if (last_index_issued >= 0 && last_index_issued < 1e8){
			aq->_addr[last_index_issued]=std::get<1>(temp);
			aq->_q[last_index_issued] = std::get<0>(temp); 
		}
	
	}

	for(int j=0; j < NUM_ALU; ++j){
		if(ready[j]){
			if(j==addrescal_latch){
				auto temp = aq->ALUissue();				
				i[j] = std::get<0>(temp);
					last_index_issued = present_index_issued;
				present_index_issued = std::get<1>(temp);
			} else if(j == memory_latch){
				auto temp=aq->MEMissue();
				i[j] = std::get<0>(temp);
				mem_address = std::get<1>(temp);
				int temp_index = std::get<2>(temp);
				if(i[j].is_valid()){
					if(temp_index<=last_index_issued)
					--last_index_issued;
					if(temp_index<=present_index_issued)
					--present_index_issued;
				}
				// cout<<"ADDRESSQUEUE: INST_ID:"<<i[j].get_id()<<"\n";
			}
			else{
				i[j] = iq->issue(j);
				// cout<<"IntegerQueue: \n";
			}
			if(i[j].is_valid()) {
				ready[j] = false;
			}
		}
		else{
			// cout<<"INNER LATCH:"<<j<<"not ready to read\n";
		}
	}
	// cout<<"\n";
}


void Issuer::tock(void){
	for(int j=0; j < NUM_ALU; ++j){
		if(!l[j]->valid() && i[j].is_valid()){		// write only when previous data has been read
			auto regs = i[j].physical_regs();
			if(j == addrescal_latch){
				int immediate=i[j].get_imm();
				l[j]->write(std::make_tuple(i[j], immediate, rf->read(std::get<1>(regs))));
			}else if(j == memory_latch){
				l[j]->write(std::make_tuple(i[j], mem_address, rf->read(std::get<2>(regs))));
			}else{
				l[j]->write(std::make_tuple(i[j], rf->read(std::get<0>(regs)), rf->read(std::get<1>(regs))));
			}
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