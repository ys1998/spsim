/*
    Main function for the simulator.
*/

#include "globals.hpp"
#include "logging.hpp"
#include "entities.hpp"
#include "issuer.hpp"

#include <fstream>
#include <string>
#include <tuple>

int LOG_LEVEL = 1;
int CLOCK = 0;
std::map<std::string, int> OPCODE, FUNCT;

/**********************************************************************************
					Instantiate classes and build data path
**********************************************************************************/

Buffer<Instruction> output_order, ICache, if_de_queue;
int RegisterMapping[NUM_LOG_REGS];
bool BusyBitTable[NUM_PHY_REGS]; 

Fetcher f(&ICache, &if_de_queue);

FreeList fl;
ActiveList al(&fl, &output_order);
IntegerQueue iq;
IntegerRegisterFile rf;

Decoder d(&if_de_queue, &fl, &al, &RegisterMapping[0], &BusyBitTable[0], &iq);

Latch< std::tuple<Instruction, int, int> > in_latch_1, in_latch_2;

Issuer is(&iq, &rf);

Latch< std::tuple<Instruction, int> > out_latch_1;
Latch< std::tuple<Instruction, int, int> > out_latch_2;

ALU1 a1(&in_latch_1, &out_latch_1, &BusyBitTable[0]);
ALU2 a2(&in_latch_2, &out_latch_2, &BusyBitTable[0]);

Writer w(&out_latch_1, &out_latch_2, &al, &rf);

/**********************************************************************************
							Functions for printing results
**********************************************************************************/

// Function to print the current value of all registers
void print_regs(IntegerRegisterFile &rf, int *rmap){
	std::cout << "Integer Registers\n";
	std::cout << "\033[1;36mRegister\tValue\033[0m\n";
	for(int i=0; i < NUM_LOG_REGS; ++i){
		if(*(rmap + i) != -1)
			std::cout << i << "\t" << rf.read(*(rmap + i)) << std::endl;
		else
			std::cout << i << "\t" << 0 << std::endl;
	}
}

// Function to print the space-time diagram
void print_std(Buffer<Instruction> &output_order){
	std::cout << "Space-Time Diagram\n"<<output_order.size()<<std::endl;
	std::sort(output_order.begin(), output_order.end(), cmp);

	std::string IF = "\033[1;33m IF   \033[0m";
	std::string DE = "\033[1;32m DE   \033[0m";
	std::string RF = "\033[1;31m RF   \033[0m";
	std::string EX = "\033[1;34m EXEC \033[0m";
	std::string WB = "\033[1;35m WB   \033[0m";
	std::string sp = "      ";

	for(auto instr : output_order){
		int i = 1;
		while(i++ != instr.IF)
			std::cout << sp;
		std::cout << IF; i++;
		while(i++ != instr.DE)
			std::cout << IF;
		std::cout << DE; i++;
		while(i++ != instr.RF)
			std::cout << DE;
		std::cout << RF; i++;
		while(i++ != instr.EXEC)
			std::cout << RF;
		std::cout << EX; i++;
		while(i++ != instr.WB)
			std::cout << EX;
		std::cout << WB;
		std::cout << std::endl;
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

	initialize_ISA();
	std::ifstream programFile(argv[1]);
	if(!programFile) {
    	error_msg("main", "Unable to open program file");
 	}

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
	is.attach_latch(0, &in_latch_1);
	is.attach_latch(1, &in_latch_2);
	for(int i=0; i<NUM_LOG_REGS; ++i)
		RegisterMapping[i] = -1;
	for(int i=0; i<NUM_PHY_REGS; ++i)
		BusyBitTable[i] = false;

	int t = 20; // CHANGE THIS
	while(t--){
		std::cout<<"cl"<<CLOCK++<<std::endl;
		f.tick();
		std::cout<<"1\n";
		d.tick();
		std::cout<<"2\n";
		is.tick();
		std::cout<<"3\n";
		a1.tick();
		std::cout<<"4\n";
		a2.tick();
		std::cout<<"5\n";
		w.tick();
		std::cout<<"6\n";

		f.tock();
		std::cout<<"7\n";
		d.tock();
		std::cout<<"8\n";
		is.tock();
		std::cout<<"9\n";
		a1.tock();
		std::cout<<"10\n";
		a2.tock();
		std::cout<<"11\n";
		w.tock();
		std::cout<<"12\n";
	}
	if(LOG_LEVEL >= 1)
		status_msg("main", "Program simulation complete");

	print_std(output_order);
	print_regs(rf, &RegisterMapping[0]);

	if(LOG_LEVEL >= 1)
		status_msg("main", "Space-Time Diagram and register values displayed");
    
    return 0;
}
