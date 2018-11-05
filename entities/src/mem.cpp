/*
	Class definition for the Memory Unit.
*/

#include "mem.hpp"
using namespace std;
extern std::map<std::string, int> OPCODE, FUNCT;
extern int CLOCK;


MEM::MEM(Latch< std::tuple<Instruction, int, int> >* in, Latch< std::tuple<Instruction, int> >* out, int* DCache, bool *b){
	this->in = in;		this->out = out;			this->DCache = DCache;
	this->b = b;
	// latencies[FUNCT["add"]] = LATENCY_ADD;
	//latencies[FUNCT["sub"]] = LATENCY_SUB;
	// set stall cycles to zero
	// stall_cycles = 0;
	read = true;
}

void MEM::tick(){
	if(in->valid() && read){
			auto inp = in->read();
			i = std::get<0>(inp);
			in1 = std::get<1>(inp);
			in2 = std::get<2>(inp);
			i.MEM = CLOCK;
			// cout<<"MEM INSTRUCTION ID:"<<i.get_id()<<"\n";
			// cout<<i.IF<<" "<<i.DE<<" "<<i.RF1<<" "<<i.EXEC<<" "<<i.RF2<< " "<<i.MEM ;  
			// cout<<"MEM ADDRESS:"<<in1<<endl;
			if(i.is_valid()){
				//operate();
				// cout<<"MEM ADDRESS:"<<in1<<endl;
				if(std::get<0>(i.type())==OPCODE["sw"])
					DCache[in1] = in2;
				else{
					res = DCache[in1];
					// cout<<DCache[in1];
					*(b + std::get<2>(i.physical_regs())) = false;
				}
				//*(b + std::get<2>(i.physical_regs())) = false;		
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