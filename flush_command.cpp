void flush(int ID){
	// INSTRUCTION change the static  variable back for new fetches
	// Clean every latch and other things in sunch pipes
	// Remove elements from activelist and 
	// Restore to previous mappings
	// STEP WISE
	// ALU1 -- remove instruction i < or make that instruction invalid> 
	// OR Instruction reset; i = reset;
	// ALU2 same as above, 
	// LATCH CLEANING FOR BOTH ALU'S
	// ActiveList <surely no flushable instruction has graduated>
	// So Noone in writeback stage
	// Instruction fetch <change PC and clock>
	// DECODER <MAIN>
	//

	// QUEUE 
	// Issuer Clean the latch

} 