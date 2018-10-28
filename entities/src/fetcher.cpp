/*
	Class definition for instruction fetching unit.
*/

#include "fetcher.hpp"

extern int CLOCK;

Fetcher::Fetcher(Buffer<Instruction> *cache, Buffer<Instruction> *dest){
	_PC = 0;
	this->cache = cache;
	this->dest = dest;
}

void Fetcher::tock(void){
	for(int i = 0; i < INSTR_FETCHED_PER_CYCLE; ++i){
		if(_PC < cache->size() && (*cache)[_PC].is_valid() && dest->size() < FETCH_QUEUE_SIZE){
			Instruction temp = (*cache)[_PC++];
			temp.IF = CLOCK;
			dest->push_back(temp);
		}
	}
}

void Fetcher::update_PC(int PC){
	_PC = PC;
}