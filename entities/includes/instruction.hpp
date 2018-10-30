/*
	Class for modelling instructions.
	Function for initializing ISA.
*/

#ifndef _INSTRUCTION_HPP
#define _INSTRUCTION_HPP

#include "logging.hpp"
#include "globals.hpp"

#include <string>
#include <sstream>
#include <tuple>
#include <map>


class Instruction {
private:
	static int cnt;				// keeps count of all instructions, initialized with 0
protected:
	int ID;						// unique identifier for each instruction
	int PC;						// value of PC associated with this instruction
	int rs, rt, rd;				// logical register mappings
	int rs_, rt_, rd_;			// physical register mappings
	int _rd;					// previous mapping for physical destination register
	int opcode, funct, shamt;	// instruction type and data fields

	// other control fields
	// bool is_branch;

public:
	int IF, DE, RF, EXEC, WB;	// clock values when this instruction entered each stage
	std::string text;			// instruction in words
	
	Instruction();											// default constructor
	Instruction(int, std::string);							// construct instruction from string
	bool is_valid(void){ 									// check if instruction is valid
		return (PC != -1 && ID != -1);
	}
	void set_id(void){										// initialize instruction with unique identifier
		ID = cnt++;
	}
	int get_id(void){ return ID; }							// return instruction's identifier
	void map(std::tuple<int, int, int, int>);				// map logical registers to given physical registers
	std::tuple<int, int, int, int> physical_regs(void);		// return the set of physical registers
	std::tuple<int, int, int> logical_regs(void); 			// return the set of logical registers
	std::tuple<int, int> type(void);						// return the numbers defining the type of instruction
};

bool cmp(Instruction, Instruction);
void initialize_ISA(void);

#endif