/*
	Class definitions for free and active lists.
*/

#include "lists.hpp"

#include <iostream>

FreeList::FreeList(){
	for(int i = 0 ; i < NUM_PHY_REGS ; i++)
		free[i] = true;
}

void FreeList::add(int reg){
	if(reg < 0 || reg >= NUM_PHY_REGS)
		error_msg("free_list", std::string("Index ") + std::to_string(reg) + " greater than maximum size (" + std::to_string(NUM_PHY_REGS) + ")");
	if(free[reg]){
		if(LOG_LEVEL >= 1)
			warning_msg("free_list", std::string("Register ") + std::to_string(reg) + " is already free; can't add to free list");
	}else{
		free[reg] = true;	
	}
}

void FreeList::remove(int reg){
	if(reg < 0 || reg >= NUM_PHY_REGS)
		error_msg("free_list", std::string("Index ") + std::to_string(reg) + " greater than maximum size (" + std::to_string(NUM_PHY_REGS) + ")");
	if(!free[reg]){
		if(LOG_LEVEL >= 1)
			warning_msg("free_list", std::string("Register ") + std::to_string(reg) + " is not free; can't remove from free list");
	}else{
		free[reg] = false;	
	}
}

int FreeList::find(){
	for(int i = 0; i < NUM_PHY_REGS; i++) {
		if(free[i]) 
			return i;
	}
	return -1;
}

ActiveList::ActiveList(FreeList *f, Buffer<Instruction> *o){
	this->f = f; this->o = o;
}

int ActiveList::push(Instruction instr){
	if(_q.size() < ACTIVE_LIST_SIZE){
		_q.push_back({instr, false});
		return 0;
	}else{
		return -1;
	}
}

void ActiveList::graduate(Instruction& instr){
	std::cout<<"gr\n";
	for(auto& p : _q){
		if(p.first.get_id() == instr.get_id()){
			p.second = true;
			// update the clock times
			p.first.IF = instr.IF;
			p.first.DE = instr.DE;
			p.first.RF = instr.RF;
			p.first.EXEC = instr.EXEC;
			p.first.WB = instr.WB;
			break;
		}
	}

	while(_q.front().second){
		Instruction temp = _q.front().first;
		std::tuple<int, int, int, int> t = temp.physical_regs();
		if(std::get<3>(t) != -1){
			f->add(std::get<3>(t));
		}
		o->push_back(temp);
		_q.pop_front();
	}
}