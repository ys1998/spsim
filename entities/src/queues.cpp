/*
	Class definitions for integer and address queues.
*/

#include "queues.hpp"

extern std::map<std::string, int> OPCODE, FUNCT;

IntegerQueue::IntegerQueue(bool *b){
	this->b = b;
}

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
		// prefer branch
		for(size_t i=0; i < _q.size(); ++i){
			Instruction temp = _q[i];
			auto type = temp.type();
			if(temp.is_valid() && (std::get<0>(type) == OPCODE["beq"] || std::get<0>(type) == OPCODE["beq"])){
				auto regs = temp.physical_regs();
				if(!*(b + std::get<0>(regs)) && !*(b + std::get<1>(regs))){
					_q.erase(_q.begin() + i);
					return temp;
				}
			}
		}

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

void IntegerQueue::flush(int id){
	size_t i=0;
	while(i < _q.size()){
		if(_q[i].get_id() > id){
			_q.erase(_q.begin() + i);
		}else{
			i++;
		}
	}
}

AddressQueue::AddressQueue(bool *b){
	this->b = b;
}

int AddressQueue::add(Instruction instr){
	if(_q.size() < INSTR_QUEUE_SIZE){
		_q.push_back(instr);
		_addr.push_back(-2);
		return 0;
	}else{
		return -1;
	}
}

std::tuple<Instruction, int ,int> AddressQueue::MEMissue(){
	for(size_t i=0; i < _q.size(); ++i){
		Instruction temp = _q[i];
		auto type = temp.type();
	
		if(temp.is_valid() && (std::get<0>(type) == OPCODE["lw"])){
			auto regs = temp.physical_regs();
			if(!*(b + std::get<1>(regs)) && (_addr[i] != -2) && (_addr[i] != -1) && findswaddr(i)) {
				_q.erase(_q.begin() + i);
				int addr = _addr[i];
				_addr.erase(_addr.begin() + i);
				return std::make_tuple(temp, addr , i);
			}
		}	
		if(temp.is_valid() && (std::get<0>(type) == OPCODE["sw"])  && findswaddr(i)){
			auto regs = temp.physical_regs();
			if(!*(b + std::get<1>(regs)) && (_addr[i] != -2) && (_addr[i] != -1)) {
				if (!*(b + std::get<2>(regs))){
					_q.erase(_q.begin() + i);
					int addr = _addr[i];
					_addr.erase(_addr.begin() + i);
					return std::make_tuple(temp, addr , i);
				}
			}
		}
	}
	return std::make_tuple(Instruction(), -1, -1);
}

bool AddressQueue::findswaddr(int index){
	int myaddr = _addr[index];
	for(size_t i=0; i < _q.size() && i < (unsigned int)index; ++i){
		if ((myaddr == _addr[i] && ((std::get<0>(_q[i].type()))) == OPCODE["sw"])){
			return 0;
		}
	}
	return 1;
}

std::tuple<Instruction, int> AddressQueue::ALUissue(){
	for(size_t i=0; i < _q.size(); ++i){
		if (_addr[i]!=-2||_addr[i]==-1)
			continue;
		Instruction temp = _q[i];
		if(temp.is_valid()){
			auto regs = temp.physical_regs();
			if(!*(b + std::get<1>(regs))){
				_addr[i] = -1;
				return std::make_tuple(temp, i);
			}
		}
	}
	return std::make_tuple(Instruction(), -1);
}
