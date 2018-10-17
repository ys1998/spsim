/*
    Class definition for ExectionUnit.
*/

#include "entities.hpp"

class ExectionUnit {
protected:
    // data members here
public:
    void tick();
    void tock();
    // other functions declarations here
};

void ExectionUnit::tick() {
    // code here
    /*
    	Push the instructions from the pipeline register after stage2 (returned from decodeUnit) to the instruction Queue
    	Take the first element from the queue which has its arguments ready (argument ready using busybit table) 
    	(think what to do if multiple execution entities there for float and int)
    	Load the values from the register file
    	---Execution Logic---
    	Post Execution:
			for the destination operand update the BusyBit
			
    */
}

void ExectionUnit::tock() {
    // code here
}

// other function definitions here