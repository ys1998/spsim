/*
	Class for modelling instructions.
	Function for initializing ISA.
*/

#include "instruction.hpp"
#include <math.h>

extern std::map<std::string, int> OPCODE, FUNCT;


void initialize_ISA(void){
	OPCODE["add"] = OPCODE["sub"] = OPCODE["mult"] = OPCODE["div"] =  0;
	OPCODE["beq"] = 4;
	OPCODE["bne"] = 5;
	OPCODE["lw"] = 6;
	OPCODE["sw"] = 7;


	FUNCT["add"] = 32;
	FUNCT["sub"] = 34;
	FUNCT["mult"] = 24;
	FUNCT["div"] = 26;
	FUNCT["beq"] = 0;
	FUNCT["bne"] = 0;
	FUNCT["lw"] = 0;
	FUNCT["sw"] = 0;
}

int Instruction::cnt = 0;

Instruction::Instruction(){
	PC = -1;
	ID = -1;
}

Instruction::Instruction(int _PC, std::string instr){
	ID = -1;
	PC = _PC;
	_rd = -1;
	IF = DE = RF1 = EXEC = RF2= MEM= WB = -1;
	shamt = 0;
	text = instr;
	predicted = jumpAddressPred = 55;
	// is_branch = false;

	std::istringstream iss(instr);
	std::string op;
	int cnt = 0, extra;
	while(iss){
		switch(cnt){
			case 0:
			op = ""; iss >> op;
			if(!OPCODE.count(op) || !FUNCT.count(op)){
				error_msg("parser", "Invalid operation \"" + op + "\", line " + std::to_string(_PC));
			}else{	
				opcode = OPCODE[op];
				funct = FUNCT[op];
			}
			break;
			case 1:
			if(opcode == 0 || opcode == OPCODE["lw"])
			{
				iss >> rd;
				if(rd < 0 || rd >= NUM_LOG_REGS){
					error_msg("parser", "Invalid register rd = " + std::to_string(rd) + ", line " + std::to_string(_PC));
				}
			}
			else{
				iss >> rs;
				if(rs < 0 || rs >= NUM_LOG_REGS){
					error_msg("parser", "Invalid register rd = " + std::to_string(rs) + ", line " + std::to_string(_PC));
				}
			}
			break;
			case 2:
			if(opcode == OPCODE["lw"] || opcode == OPCODE["sw"]){
				iss >> immediate;
				if(immediate>std::pow(2,15)||immediate<-std::pow(2,15))
				error_msg("parser", "Invalid immediate immediate = " + std::to_string(immediate) + ", line " + std::to_string(_PC));
				 	
			}else if(opcode == 0)
			{
				iss >> rs;
				if(rs < 0 || rs >= NUM_LOG_REGS){
					error_msg("parser", "Invalid register rs = " + std::to_string(rs) + ", line " + std::to_string(_PC));
				}
			}
			else
			{
				iss >> rt;
				if(rt < 0 || rt >= NUM_LOG_REGS){
					error_msg("parser", "Invalid register rt = " + std::to_string(rt) + ", line " + std::to_string(_PC));
				}
			}
			break;
			
			case 3:
			if(opcode == 0)
			{
				iss >> rt;
				if(rt < 0 || rt >= NUM_LOG_REGS){
					error_msg("parser", "Invalid register rt = " + std::to_string(rt) + ", line " + std::to_string(_PC));
				}
			}
			else if( opcode == 4 || opcode == 5)
			{
				iss >> immediate;
				if (immediate < 0 || immediate >= 66536)
				{
					error_msg("parser", "Invalid immediate = " + std::to_string(immediate) + ", line " + std::to_string(_PC));
				}
			}
			else{
				iss >> rt;
				if(rt < 0 || rt >= NUM_LOG_REGS){
					error_msg("parser", "Invalid register rt = " + std::to_string(rt) + ", line " + std::to_string(_PC));
				}
			}
			if(opcode == OPCODE["lw"])
				rs=rt;
			break;
			default:
			iss >> extra;
			break;
		}
		cnt++;
	}
}

void Instruction::map(std::tuple<int, int, int, int> t){
	rt_ = std::get<1>(t);
	rd_ = std::get<2>(t);
	_rd = std::get<3>(t);
	// if(opcode!=6&&opcode!=7)
	rs_ = std::get<0>(t);
	// else
		// rs_=rt_;
}

std::tuple<int, int, int, int> Instruction::physical_regs(void){
	return std::make_tuple(rs_, rt_, rd_, _rd);
}

std::tuple<int, int, int> Instruction::logical_regs(void){
	// if(opcode!=6&&opcode!=7)
		return std::make_tuple(rs, rt, rd);
	// else
		// return std::make_tuple(immediate, rt, rd);
}

int Instruction::get_imm(void){
	return immediate;
}

std::tuple<int, int> Instruction::type(void){
	return std::make_tuple(opcode, funct);
}

bool cmp(Instruction I1, Instruction I2){
    return I1.IF < I2.IF ;
}
