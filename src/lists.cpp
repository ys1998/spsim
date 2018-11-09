/*
	Class definitions for free and active lists.
*/

#include "lists.hpp"

#include <string>

extern bool terminate;

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

ActiveList::ActiveList(FreeList *f, Buffer<Instruction> *o, int *r, bool *b){
	this->f = f; this->o = o; this->r = r; this->b = b;
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
	for(auto& p : _q){
		if(p.first.get_id() == instr.get_id()){
			p.second = true;
			// update the clock times
			p.first.IF = instr.IF;
			p.first.DE = instr.DE;
			p.first.RF1 = instr.RF1;
			p.first.EXEC = instr.EXEC;
			p.first.RF2 = instr.RF2;
			p.first.MEM = instr.MEM;
			p.first.WB = instr.WB;
			break;
		}
	}

	while(!_q.empty() && _q.front().second){
		Instruction temp = _q.front().first;
		std::tuple<int, int, int, int> t = temp.physical_regs();
		if(std::get<3>(t) != -1){
			f->add(std::get<3>(t));
		}
		o->push_back(temp);
		_q.pop_front();
		if(_q.empty())
			terminate = true;
	}
}

void ActiveList::flush(int id){
	size_t i = _q.size() - 1;
	while(i != 0){
		if(_q[i].first.get_id() > id){
			auto regs = _q[i].first.physical_regs();
			auto log_regs = _q[i].first.logical_regs();
			
			if(std::get<2>(regs) >= 0 && std::get<2>(regs) < NUM_PHY_REGS)
				f->add(std::get<2>(regs));
			
			if(std::get<2>(log_regs) != -1)
				*(r + std::get<2>(log_regs)) = std::get<3>(regs);
			if(std::get<2>(regs) != -1)
				*(b + std::get<2>(regs)) = false;
			
			_q.erase(_q.begin() + i);
		}
		i--;
	}
}