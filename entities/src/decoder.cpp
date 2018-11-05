/*
	Class definition for decoding unit.
*/

#include "decoder.hpp"

extern int CLOCK;
extern std::map<std::string, int> OPCODE, FUNCT;

Decoder::Decoder(Buffer<Instruction>* d, FreeList *f, ActiveList *a, int *r, bool *b, IntegerQueue *iq,AddressQueue *aq){
	this->d = d; this->f = f; this->a = a;
	this->r = r; this->b = b; this->iq = iq;
	this->aq = aq;
	decode_signal = true;
}

int Decoder::decode_instr(Instruction instr){
	std::tuple<int, int, int> reg = instr.logical_regs();
	std::tuple<int,int> instruction_type = instr.type();
	int opcode = std::get<0>(instruction_type);
	
	int rs = std::get<0>(reg);
	int rt = std::get<1>(reg);
	int rd = std::get<2>(reg);
	
	int temp, rs_, rt_, rd_;
	bool found[3] = {false, false, false};

	if(*(r + rs) == -1){
		if((temp = f->find()) == -1)
			return -1;
		*(r + rs) = rs_ = temp;
		f->remove(rs_);
		found[0] = true;
	} else {
		rs_ = *(r + rs);
	}
	if(*(r + rt) == -1){
		if((temp = f->find()) == -1){
			if(found[0]){
				f->add(rs_);
				*(r + rs) = -1;	
			} 	// add already removed register

			return -1;
		}
		*(r + rt) = rt_ = temp;
		f->remove(rt_);
		found[1] = true;
	} else {
		rt_ = *(r + rt);
	}
	if(opcode == OPCODE["lw"] || opcode == 0){
		if((temp = f->find()) == -1){
			if(found[0]) {
				f->add(rs_);
				*(r + rs) = -1;
			}	// add already removed register
			if(found[1]) {
				f->add(rt_);
				*(r + rt) = -1;
			}
			return -1;
		} 
		rd_ = temp;
		f->remove(rd_);	
	}
	instr.map(std::make_tuple(rs_, rt_, rd_, *(r + rd)));
	
	if(opcode == OPCODE["lw"] || opcode == 0){
		*(r + rd) = temp;
		*(b + rd_) = true;
	}
	instr.DE = CLOCK;
	instr.RF1 = CLOCK+1;
	instr.set_id();

	if(!(opcode == OPCODE["lw"] || opcode == OPCODE["sw"]))
		iq->add(instr); // TODO check return value
	else
		aq->add(instr);
	a->push(instr); // TODO check return value
	return 0;
}

void Decoder::tick(void){
	while(!d->empty()) {
		_q.push_back(d->front()); d->pop_front();
	}
}

void Decoder::tock(void){
	int cnt = 0;
	size_t i = 0; 
	while(!_q.empty() && cnt < INSTR_DECODED_PER_CYCLE && i < _q.size()){
		if(decode_instr(_q[i]) < 0){
			++i;
		}else{
			_q.erase(_q.begin() + i);
			cnt++;
		}
	}
}

void Decoder::flush(int id){
	// clean eveerything, no check needed
	_q.clear();
	d->clear();
	a->flush(id);
}
