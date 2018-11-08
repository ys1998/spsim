/*
	Global variables and constants.
*/

#ifndef _GLOBALS_HPP
#define _GLOBALS_HPP

#include <string>
#include <map>
#include <deque>

template <class T>
using Buffer = std::deque<T>;


// Hardware details
#define NUM_LOG_REGS 32				// number of logical registers
#define NUM_PHY_REGS 64				// number of physical registers
#define INSTR_FETCHED_PER_CYCLE 2	// number of instructions fetched from cache per cycle
#define INSTR_DECODED_PER_CYCLE 2	// number of instructions decoded per cycle
#define NUM_ALU 4					// number of Arithmetic Logic Units

// Bounds on sizes
#define PROG_SIZE 1000000			// maximum number of instructions in program
#define FETCH_QUEUE_SIZE 32			// size of queue holding fetched instructions
#define DECODE_QUEUE_SIZE 32		// size of queue holding decoded instructions
#define ACTIVE_LIST_SIZE 64			// maximum number of active instructions
#define INSTR_QUEUE_SIZE 16			// maximum number of queued instructions
#define ADDR_QUEUE_SIZE 16			// maximum number of memory-based queued instructions
#define DCACHE_SIZE 100             /// 100 integers can be stored (addrresses indexed by arary)

// Latencies for different operations
#define LATENCY_ADD 1
#define LATENCY_SUB 1
#define LATENCY_BEQ 1
#define LATENCY_BNE 1
#define LATENCY_MULT 6
#define LATENCY_DIV 35
#define LATENCY_SW 1

// Branch prediction
#define BRANCH_PREDICT_SLOTS 32
// States
#define STRONGLY_TAKEN 3
#define WEAKLY_TAKEN 2
#define WEAKLY_NOT_TAKEN 1
#define STRONGLY_NOT_TAKEN 0

extern std::map<std::string, int> OPCODE;
extern std::map<std::string, int> FUNCT;

/* 
	Logging level; can be specified via commandline
	0 - only error messages
	1 - errors, status messages and warnings [DEFAULT]
	2 - error, status and all log messages, and warnings
*/
extern int LOG_LEVEL;
extern int CLOCK;

#endif
