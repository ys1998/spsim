/*
	Class definition for the two Arithmetic Logic Units (ALUs).
*/

#include "alu.hpp"

extern std::map<std::string, int> OPCODE, FUNCT;
extern int CLOCK;

ALU1::ALU1(	Latch< std::tuple<Instruction, int, int> >* in, 
			Latch< std::tuple<Instruction, int> >* out, bool* b,
			ALU2 *a, Flusher *f){
	this->in = in; this->out = out; this->b = b; this->a = a; this->f = f;
	// store latencies of operations
	latencies[FUNCT["add"]] = LATENCY_ADD;
	latencies[FUNCT["sub"]] = LATENCY_SUB;
	latencies[0] = 1;
	// set stall cycles to zero
	stall_cycles = 0;
	read = true;
}

void ALU1::operate(void){
	int funct = std::get<1>(i.type());
	int opcode = std::get<0>(i.type());
	if(funct == FUNCT["add"]){
		res = in1 + in2;
	}else if(funct == FUNCT["sub"]){
		res = in1 - in2;
	}else if(opcode != 0){
		if ((opcode == OPCODE["beq"]) || (opcode == OPCODE["bne"]))	{	
			if(opcode == OPCODE["beq"]){
				res = (in1 == in2);
			}else if(opcode == OPCODE["bne"]){
				res = (in1 != in2);
			}
			int possible_jump_address = i.get_pc() + 1 + i.get_immediate();
			/* PREDICTOR UPDATE FUNCTION */
			if (res != i.predicted || ((res == 1) && (possible_jump_address != i.jumpAddressPred))){
				//	case of misprediction
				if(res == 0) possible_jump_address = i.get_pc()+1;
				// flush second ALU
				a->flush(i.get_id());
				// flush other entities
				f->flush(i.get_id(), possible_jump_address);
			}
				
		}	
	} 
}

void ALU1::tick(void){
	if(stall_cycles == 0){
		if(i.is_valid()){	
			operate();
		}
		if(in->valid() && read){
			auto inp = in->read();
			i = std::get<0>(inp);
			in1 = std::get<1>(inp);
			in2 = std::get<2>(inp);
			i.EXEC = CLOCK;
			i.MEM = CLOCK + 1;

			if(i.is_valid() && latencies[std::get<1>(i.type())] == 1){
				operate();
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
		}
		if(in->valid() && read){
			
			auto inp = in->read();
			i = std::get<0>(inp);
			in1 = std::get<1>(inp);
			in2 = std::get<2>(inp);
			i.EXEC = CLOCK;
			i.MEM = CLOCK + 1;
			
			if(i.is_valid() && latencies[std::get<1>(i.type())] == 1){
				operate();
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
				out->write(std::make_tuple(i, lo, hi));
				Instruction reset; i = reset;
			}
		}
	}
}

void ALU2::flush(int id){
	if(i.get_id() > id){
		Instruction reset;
		i = reset;
		stall_cycles = 0;
		read = true;
	}
}

ALU3::ALU3(	Latch< std::tuple<Instruction, int, int> >* in, 
			Latch< std::tuple<Instruction, int> >* out, bool* b){
	this->in = in; this->out = out; this->b = b;
	// store latencies of operations
	latencies[FUNCT["add"]] = LATENCY_ADD;
	latencies[FUNCT["sw"]] = LATENCY_SW;
	// set stall cycles to zero
	stall_cycles = 0;
	read = true;
}

void ALU3::tick(void){
	if(in->valid() && read){

		auto inp = in->read();
		i = std::get<0>(inp);
		in1 = std::get<1>(inp);
		in2 = std::get<2>(inp);
		i.EXEC = CLOCK;
		i.RF2 = CLOCK + 1;
		
		if(i.is_valid() && latencies[std::get<1>(i.type())] == 1){
			res = in1 + in2;
		}else{
			stall_cycles = latencies[std::get<1>(i.type())] - 1;
		}
	} 
}

void ALU3::tock(void){
	if(i.is_valid()	){
		if(out->valid()){
			read = false;	// previous data has not been read; stall
		}else{
			read = true;
			out->write(std::make_tuple(i, res));
			Instruction reset; i = reset;
		}
	}
}