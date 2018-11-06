/*
    Main function for the simulator.
*/

#include "globals.hpp"
#include "logging.hpp"
#include "entities.hpp"
#include "issuer.hpp"
#include "flusher.hpp"


#include <fstream>
#include <string>
#include <tuple>
#include <iomanip>

int LOG_LEVEL = 1;
int CLOCK = 0;
std::map<std::string, int> OPCODE, FUNCT;

/**********************************************************************************
							Functions for printing results
**********************************************************************************/

// Function to print the current value of all registers
void print_regs(IntegerRegisterFile &rf, int *rmap){
	std::cout << "Integer Registers\n";
	for(int j=0; j<4; ++j){
		std::cout << "\033[0;36m  Reg  Val\033[0m";
	}
	std::cout << "\n";
	for(int i=0; i < NUM_LOG_REGS/4; ++i){
		for(int j=0; j < 4; ++j){
			if(*(rmap + j*NUM_LOG_REGS/4 + i) != -1)
				std::cout << "\033[1;36m" <<
				std::setfill(' ') << std::setw(5) << j*NUM_LOG_REGS/4 + i << "\033[0m" <<
				std::setfill(' ') << std::setw(5) << rf.read(*(rmap + j*NUM_LOG_REGS/4 + i));
			else
				std::cout << "\033[1;36m" <<
				std::setfill(' ') << std::setw(5) << j*NUM_LOG_REGS/4 + i << "\033[0m" <<
				std::setfill(' ') << std::setw(5) << 0;
		}
		std::cout << std::endl;
	}
}

// Function to print the space-time diagram
void print_std(Buffer<Instruction> output_order){
	std::cout << "Space-Time Diagram\n";
	std::sort(output_order.begin(), output_order.end(), cmp);

	std::string IF =  "\033[1;43m  IF  \033[0m";
	std::string DE =  "\033[1;42m  DE  \033[0m";
	std::string RF1 = "\033[1;41m  RF1 \033[0m";
	std::string RF2 = "\033[1;46m  RF2 \033[0m";
	std::string EX =  "\033[1;44m EXEC \033[0m";
	std::string MEM = "\033[1;47;30m  MEM \033[0m";
	std::string WB =  "\033[1;45m  WB  \033[0m";
	std::string sp =  "      ";

	for(auto instr : output_order){
		int i = 1;
		std::cout << instr.text << "\t";
		while(i < instr.WB && i++ != instr.IF)
			std::cout << sp;
		std::cout << IF;
		while(i < instr.WB && i++ != instr.DE)
			std::cout << IF;
		std::cout << DE;
		while(i++ != instr.RF1)
			std::cout << DE;
		std::cout << RF1;
		while(i++ != instr.EXEC)
			std::cout << RF1;
		std::cout << EX;
		if(std::get<0>(instr.type())== OPCODE["lw"] || std::get<0>(instr.type())== OPCODE["sw"]){
			while(i++ != instr.RF2)
				std::cout << EX;
			std::cout << RF2;// << i << instr.MEM;
			while (i++ != instr.MEM)
				std::cout << RF2;
			std::cout << MEM;
			while(i++ != instr.WB)
				std::cout << MEM;
			std::cout << WB;
			std::cout << std::endl;			
		}
		else {
			while(i++ != instr.WB)
				std::cout << EX;
			std::cout << WB;
			std::cout << std::endl;
		}

	}
}

/**********************************************************************************/

int main(int argc, char const *argv[])
{	
	if(argc <= 1) {
		error_msg("main", "Program needed for execution");
	}
	if(argc == 3){
		int l = std::stoi(argv[2]);
		if(l < 0 || l > 2)
			error_msg("main", "Invalid logging level");
		LOG_LEVEL = l;
	}

	std::ifstream programFile(argv[1]);
	if(!programFile) {
    	error_msg("main", "Unable to open program file");
 	}

	initialize_ISA();

	/**********************************************************************************
						Instantiate classes and build data path
	**********************************************************************************/

	Buffer<Instruction> output_order, ICache, if_de_queue;
	int DCache[DCACHE_SIZE];
	int RegisterMapping[NUM_LOG_REGS];
	bool BusyBitTable[NUM_PHY_REGS];

	int BranchPredict[BRANCH_PREDICT_SLOTS];
	int BranchPredictAddr[BRANCH_PREDICT_SLOTS];

	Fetcher f(&ICache, &if_de_queue);

	FreeList fl;
	ActiveList al(&fl, &output_order, &RegisterMapping[0], &BusyBitTable[0]);
	IntegerQueue iq(&BusyBitTable[0]);
	AddressQueue aq(&BusyBitTable[0]);
	IntegerRegisterFile rf;

	Decoder d(&if_de_queue, &fl, &al, &RegisterMapping[0], &BusyBitTable[0], 
				&iq, &aq, &BranchPredict[0], &BranchPredictAddr[0], &f);

	Latch< std::tuple<Instruction, int, int> > in_latch_1, in_latch_2,in_latch_3,in_latch_4;

	Issuer is(&iq, &aq, &rf);
	// Issuer is_1(&iq, &aq, &rf);                                    //// after address calulation issuing to memory stage

	Latch< std::tuple<Instruction, int> > out_latch_1,out_latch_3,out_latch_4;
	Latch< std::tuple<Instruction, int, int> > out_latch_2;

	ALU2 a2(&in_latch_2, &out_latch_2, &BusyBitTable[0]);
	ALU3 a3(&in_latch_3, &out_latch_3, &BusyBitTable[0]);
	MEM  mem(&in_latch_4,&out_latch_4, &DCache[0], &BusyBitTable[0]);

	Writer w(&out_latch_1, &out_latch_2, &out_latch_4, &al, &rf, &BusyBitTable[0]);
	Flusher flsh(&is, &f, &d, &w);
	ALU1 a1(&in_latch_1, &out_latch_1, &BusyBitTable[0], &a2, &flsh, 
			&BranchPredict[0], &BranchPredictAddr[0]);
	/**********************************************************************************/

	int PC = 0;
	std::string input = "";
	while(std::getline(programFile, input)){
		if(PC > PROG_SIZE) {
			error_msg("main", std::string("Program size limit [") + std::to_string(PROG_SIZE) + "] exceeded");
		}
		Instruction instr(PC, input);
		instr.set_id();
		ICache.push_back(instr);
		PC++;
	}
	programFile.close();

	if(LOG_LEVEL >= 1)
		status_msg("main", "Program parsing complete");

	// Complete building and initializing the data path
	is.attach_latch(0, &in_latch_1,1);
	is.attach_latch(1, &in_latch_2,1);
	is.attach_latch(2,&in_latch_3,2);
	is.attach_latch(3,&in_latch_4,3);
	is.attach_cal_latch_outer(&out_latch_3);

	for(int i=0; i<NUM_LOG_REGS; ++i)
		RegisterMapping[i] = -1;
	for(int i=0; i<NUM_PHY_REGS; ++i)
		BusyBitTable[i] = false;
	for(int i=0; i<DCACHE_SIZE;++i)
		DCache[i] = i;    
	for(int i=0; i<BRANCH_PREDICT_SLOTS; ++i){
		BranchPredict[i] = STRONGLY_NOT_TAKEN;
		BranchPredictAddr[i] = i + 1;
	}                

	int t = 500; // CHANGE THIS
	while(t--){
		CLOCK++;
		f.tick();
		d.tick();
		is.tick();
		a1.tick();
		a2.tick();
		a3.tick();
		mem.tick();
		// is1.tick();
		w.tick();

		f.tock();
		d.tock();
		is.tock();
		a1.tock();
		a2.tock();
		a3.tock();
		mem.tock();
		// is1.tock();
		w.tock();
	}
	if(LOG_LEVEL >= 1)
		status_msg("main", "Program simulation complete");

	std::cout << std::endl;
	print_std(output_order);

	std::cout << std::endl;
	print_regs(rf, &RegisterMapping[0]);

	std::cout << std::endl;
	if(LOG_LEVEL >= 1)
		status_msg("main", "Space-Time Diagram and register values displayed");
    
    return 0;
}
