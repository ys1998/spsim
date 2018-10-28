/*
	Class definitions for integer and address queues.
*/

#include "queues.hpp"

extern std::map<std::string, int> FUNCT;

int IntegerQueue::add(Instruction instr){
	if(_q.size() < INSTR_QUEUE_SIZE){
		_q.push_back(instr);
		return 0;
	}else{
		return -1;
	}
}

Instruction IntegerQueue::issue(int idx){
	switch(idx){
		case 0:
		// send only an addition or subtraction operation
		for(size_t i=0; i < _q.size(); ++i){
			Instruction temp = _q[i];
			auto type = temp.type();
			if(temp.is_valid() && (std::get<1>(type) == FUNCT["add"] || std::get<1>(type) == FUNCT["sub"])){
				auto regs = temp.physical_regs();
				if(!*(b + std::get<0>(regs)) && !*(b + std::get<1>(regs))){
					_q.erase(_q.begin() + i);
					return temp;
				}
			}
		}
		break;

		case 1:
		// first preference to division and multiplication
		for(size_t i=0; i < _q.size(); ++i){
			Instruction temp = _q[i];
			auto type = temp.type();
			if(temp.is_valid() && (std::get<1>(type) == FUNCT["div"] || std::get<1>(type) == FUNCT["mult"])){
				auto regs = temp.physical_regs();
				if(!*(b + std::get<0>(regs)) && !*(b + std::get<1>(regs))){
					_q.erase(_q.begin() + i);
					return temp;
				}
			}
		}
		// but if neither operation is present, send addition or subtraction
		for(size_t i=0; i < _q.size(); ++i){
			Instruction temp = _q[i];
			auto type = temp.type();
			if(temp.is_valid() && (std::get<1>(type) == FUNCT["add"] || std::get<1>(type) == FUNCT["sub"])){
				auto regs = temp.physical_regs();
				if(!*(b + std::get<0>(regs)) && !*(b + std::get<1>(regs))){
					_q.erase(_q.begin() + i);
					return temp;
				}
			}
		}
		break;

		default:
		break;
	}
	// send an invalid operation/instruction if no match is found
	return Instruction();
}