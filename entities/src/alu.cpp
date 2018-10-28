/*
	Class definition for the two Arithmetic Logic Units (ALUs).
*/

#include "alu.hpp"

extern std::map<std::string, int> OPCODE, FUNCT;
extern int CLOCK;

ALU1::ALU1(	Latch< std::tuple<Instruction, int, int> >* in, 
			Latch< std::tuple<Instruction, int> >* out, bool* b){
	this->in = in; this->out = out; this->b = b;
	// store latencies of operations
	latencies[FUNCT["add"]] = LATENCY_ADD;
	latencies[FUNCT["sub"]] = LATENCY_SUB;
	// set stall cycles to zero
	stall_cycles = 0;
	read = true;
}

void ALU1::operate(void){
	int funct = std::get<1>(i.type());
	if(funct == FUNCT["add"]){
		res = in1 + in2;
	}else if(funct == FUNCT["sub"]){
		res = in1 - in2;
	} 
}

void ALU1::tick(void){
	if(stall_cycles == 0){
		if(i.is_valid()){
			operate();
			*(b + std::get<2>(i.physical_regs())) = false;
		}
		if(in->valid() && read){
			auto inp = in->read();
			i = std::get<0>(inp);
			in1 = std::get<1>(inp);
			in2 = std::get<2>(inp);
			if(i.is_valid() && latencies[std::get<1>(i.type())] == 1){
				operate();
				*(b + std::get<2>(i.physical_regs())) = false;
			}else{
				stall_cycles = latencies[std::get<1>(i.type())] - 1;	
			}
		}
	}
}

void ALU1::tock(void){
	if(stall_cycles > 0)
		stall_cycles--;
	else {
		if(i.is_valid()){
			if(out->valid()){
				read = false;	// previous data has not been read; stall
			}else{
				read = true;
				i.EXEC = CLOCK;
				out->write(std::make_tuple(i, res));
				Instruction reset; i = reset;
			}
		}
	}
}


ALU2::ALU2(	Latch< std::tuple<Instruction, int, int> >* in, 
			Latch< std::tuple<Instruction, int, int> >* out, bool* b){
	this->in = in; this->out = out; this->b = b;
	// store latencies of operations
	latencies[FUNCT["add"]] = LATENCY_ADD;
	latencies[FUNCT["sub"]] = LATENCY_SUB;
	latencies[FUNCT["mult"]] = LATENCY_MULT;
	latencies[FUNCT["div"]] = LATENCY_DIV;
	// set stall cycles to zero
	stall_cycles = 0;
	read = true;
}

void ALU2::operate(void){
	int funct = std::get<1>(i.type());
	if(funct == FUNCT["add"]){
		lo = in1 + in2; hi = 0;
	}else if(funct == FUNCT["sub"]){
		lo = in1 - in2; hi = 0;
	}else if(funct == FUNCT["mult"]){
		lo = (long)((long)in1 * (long)in2) % (long)pow(2, 32); 
		hi = (long)(((long)in1 * (long)in2) / (long)pow(2, 32)); 
	}else if(funct == FUNCT["div"]){
		lo = in1 % in2; hi = (long)(in1 / in2);
	}
}

void ALU2::tick(void){
	if(stall_cycles == 0){
		if(i.is_valid()){
			operate();
			*(b + std::get<2>(i.physical_regs())) = false;
		}
		if(in->valid() && read){
			auto inp = in->read();
			i = std::get<0>(inp);
			in1 = std::get<1>(inp);
			in2 = std::get<2>(inp);
			if(i.is_valid() && latencies[std::get<1>(i.type())] == 1){
				operate();
				*(b + std::get<2>(i.physical_regs())) = false;
			}else{
				stall_cycles = latencies[std::get<1>(i.type())] - 1;	
			}
		}
	}
}

void ALU2::tock(void){
	if(stall_cycles > 0)
		stall_cycles--;
	else {
		if(i.is_valid()){
			if(out->valid()){
				read = false;	// previous data has not been read; stall
			}else{
				read = true;
				i.EXEC = CLOCK;
				out->write(std::make_tuple(i, lo, hi));
				Instruction reset; i = reset;
			}
		}
	}
}