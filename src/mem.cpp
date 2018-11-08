/*
	Class definition for the Memory Unit.
*/

#include "mem.hpp"

extern std::map<std::string, int> OPCODE, FUNCT;
extern int CLOCK;


MEM::MEM(Latch< std::tuple<Instruction, int, int> >* in, Latch< std::tuple<Instruction, int> >* out, int* DCache, bool *b){
	this->in = in; this->out = out;	this->DCache = DCache;
	this->b = b;
	read = true;
}

void MEM::tick(){
	if(in->valid() && read){
		auto inp = in->read();
		i = std::get<0>(inp);
		in1 = std::get<1>(inp);
		in2 = std::get<2>(inp);
		i.MEM = CLOCK;
		if(i.is_valid()){
			if(std::get<0>(i.type()) == OPCODE["sw"]){
				DCache[in1] = in2;
				// std::cout<<i.text<<std::endl;
			}
			else{
				res = DCache[in1];
			}
		}
	}
}

void MEM::tock(){
	if(i.is_valid()){
		if(out->valid()){
			read = false;	// previous data has not been read; stall
		}else{
			read = true;
			out->write(std::make_tuple(i,res));
			Instruction reset; i = reset;
		}
	}
}