#include <iostream>
#include <vector>
#include <queue>
#include <deque>
#include <bitset>
#include <string>
#include <utility>
#include <map>
#include <fstream>
#include <sstream>

#define pb push_back
#define pf push_front
#define mp make_pair
#define fi first
#define se second
using namespace std;

//long power[64]; // stores the power of 2 until 2^63
map<string,int> opcodes;
map<string,int> functs;
bool completesignal;
int numCycle;
int global_intr_cntr;
int global_timer;

void instantiateISA(){// fills opcodes and functs fields
	opcodes["add"] = opcodes["sub"] = opcodes["mult"] = opcodes["div"] =  0;
	opcodes["beq"] = 4;
	opcodes["bne"] = 5;
	functs["add"] = 32;
	functs["sub"] = 34;
	functs["mult"] = 24;
	functs["div"] = 26;
	functs["beq"] = 0;
	functs["bne"] = 0;
}

struct instruction{    // it would have to modified for store loads accordingly 
	int ID; // ID is strictly increasing determined in decode phase while adding to integer queue (// has to be manipulated carefully)
	int PC;
	int rs;
	int mappedrs;
	int rt;
	int mappedrt;
	int rd;
	int mappedrd;
	int funct;
	int opcode;
	int shamt;
	int previous_mapping;  // filled during decode , instantiated with -1
	// other control fileds like branch mask etc etc...


	bool is_branch;
	// necessary fields for outputs
	int IF,DE,RF,EXEC,WB;

	bitset<32> instr; // useless now
	// 3 different constructors according to needs
	instruction():PC(-1){} // invlaid => always instatnitate instruction with an actual instructiona and a PC
	// instruction(int pc,bitset<32> in){ // I think this constructor is useless
	// 	instr=in;
	// 	PC=pc;
	// 	// obtain rs.rt,rd,func,opcode and check!!
	// 	funct   = (in & (power[6]-1)).to_ulong;
	// 	shamt  = (((in >> 5  )) & (power[5]-1) ).to_ulong;
	// 	rd     = (((in >> 10 )) & (power[5]-1) ).to_ulong;
	// 	rt     = (((in >> 15 )) & (power[5]-1) ).to_ulong;
	// 	rd     = (((in >> 20 )) & (power[5]-1) ).to_ulong;
	// 	opcode = (((in >> 25 )) & (power[6]-1) ).to_ulong;
	// 	cout<<"Checking instantiations "<<instr<<"  opcode "<<opcode<<" rs "<<rs<<" rt "<<rt<<" rd "<<rd<<" shamt "<<shamt<<" funct "<<funct<<endl; 
	// 	(opcode == 4 || opcode == 5) ? is_branch = 1 : is_branch = 0; // set is_branch depending on opcode we only care about bew and bne
	// }

	instruction(int pc,string file_input){ // this will have to be changed for branch and load/store instructions
		PC = pc;
		previous_mapping = -1;
		IF = DE = RF = EXEC = WB = -1;
		shamt = 0;
		istringstream iss(file_input);
		int cnt = 0;
	    while(iss) {
			if(cnt == 0) {
				string operation;
				iss >> operation;
				opcode = opcodes[operation];
				funct = functs[operation];	
				cout<<operation<<" "<<opcode<<" "<<funct<<endl;		
			} else if(cnt == 2) {
				iss >> rs;
			} else if(cnt == 3) {
				iss >> rt;
				cout<<"Checking instantiations "<<file_input<<"  opcode "<<opcode<<" rs "<<rs<<" rt "<<rt<<" rd "<<rd<<" shamt "<<shamt<<" funct "<<funct<<endl; 
			} else if(cnt == 1) {
				iss >> rd;
			} else {
				int extra;
				cout<<"extra input in line?? check the command => "<<file_input<<" Oh its just the newline"<<endl;
				iss >> extra;
			}
			cnt++;
	    }
	}
};

vector<instruction> outputorder;
instruction I_cache[1000000]; // atmost 1 million instructions
queue<instruction> decodeReg; // communicating data structure (registers in hardware) for instruction fetch and decode unit
queue<instruction> decodeRegNext;
////////////////////////////////////////////////////             STAGE 1.        . /////////////////////////////////////////////////////////////////

class FetchUnit{
public:
	int PC;
	FetchUnit():PC(0){}
	void tick(){
		// always keep fetching buffering responsibility is on decoder 
		// stop condition has to be added bracnhes complicate when to stop maybe stop signal should come from 7th stage?
		// PC is being incremented 1 by 1 here however when brach occurs it would have to modified some signaling mehcanushm would be needed to change the PC
		if(I_cache[PC].PC!=-1) {
			decodeReg.push(I_cache[PC++]);
			cout<<"Pushing instruction in decodeRegNext\n";
			decodeReg.front().IF = global_timer;
		}
		if(I_cache[PC].PC!=-1) {
			decodeReg.push(I_cache[PC++]);
			cout<<"Pushing instruction in decodeReg\n";
			decodeReg.front().IF = global_timer;
		}
	}
};


int RegisterMapping[32]; // register mappings of the 32 registers
bool BusyBitTable[64]; // busy bit for the 64 physical registers

struct Freelist{
	bool free[64]; // indicates whether the ith physical register is free or not if value 1 it menas it is free else is not
	Freelist(){
		for(int i=0;i<64;i++) free[i]=1; // everyone is free
	}
	void add(int reg){ // add register reg to the freelist i.e. it is free now
		if(free[reg]==1) cout<<"Register "<<reg<<" is already free => cant add to free list\n";
		else free[reg]=1;
	}
	void remove(int reg){ // remove a free register reg from the free list
		if(free[reg]==0) cout<<"Register "<< reg <<" is not free => cant remove from free list\n";
		else free[reg]=0;
	}
	int find(){ // returns the first free register
		for(int i=0;i<64;i++) {
			if(free[i] == 1) return i;
		}
		cout<<"No free register => check \n"<<endl;
		return -1;
	}
} freelist;

struct ActiveList{
	// stores at max 32 instructions along with "necessary" information
	// will be used to determine when to commit the instructions
	// Fisrt 5 bits for tag
	// Next 6 bits for old physical destination
	// Next 5 bits from logical destination
	// Next 1 bit for done or not
	//map<>
	deque<pair<instruction,bool> > _activelist; // the bool is the done thing
	void push(instruction _ins){ // previous mapping made a part of instruction only to avoid cluttering
		_activelist.push_back({_ins,0});
	}
	void set_done_bit(instruction _ins){
		cout<<"in set_done_bit "<<_ins.IF<<" "<<_ins.DE<<" "<<_ins.RF<<" "<<_ins.EXEC<<" "<<_ins.WB<<endl;
		for(auto & p : _activelist){
			if(p.first.ID == _ins.ID) {
				p.second = 1;
				cout<<"Calling graduationceremony\n";
				graduationceremony();
				return;
			}
		}
	}

	void graduationceremony(){
		cout<<"Active list size = "<<_activelist.size()<<" "<<_activelist.front().second<<endl;
		while(_activelist.front().second == 1){
			graduate(_activelist.front().first);
			_activelist.pop_front();
		}
	}

	void graduate(instruction _ins){ // is this all there is to graduation?
		freelist.add(_ins.previous_mapping);
		cout<<"Adding to output order\n";
		outputorder.push_back(_ins);
	}
	ActiveList(){}

} activelist;

struct IntegerQueue{
	instruction array_of_instructions[32]; // at max 32 which will be never breached
	IntegerQueue(){
		for(int i=0;i<32;i++) {
			array_of_instructions[i].PC = -1; // PC -1 => that the instruction isn't there 
		}
	}
	void add(instruction I){
		for(int i=0;i<64;i++){
			if(array_of_instructions[i].PC == -1) {
				array_of_instructions[i] = I;
				return;
			}
		}
		cout<<"IntegerQueue is full which shouldnt happen check!!\n";
	}
	instruction issue1(){ // rteturn instruction with PC = -1 if cant issue else return instruction on success
		for(int i=0;i<64;i++){
			instruction x = array_of_instructions[i];
			if(x.PC != -1 && (x.funct == functs["add"] || x.funct == functs["sub"])) {
				if(!BusyBitTable[x.mappedrs] && !BusyBitTable[x.mappedrt]) { // Busy bit changed in 7th stage-first half
					array_of_instructions[i].PC=-1; // now this instruction isnt in queue
					return x;
				}
			}
		}
		instruction fail;
		fail.PC = -1;
		return fail;
	}
	instruction issue2(){ // rteturn instruction with PC = -1 if cant issue else return instruction on success
		for(int i=0;i<64;i++){
			instruction x = array_of_instructions[i];
			if(x.PC != -1 && (x.funct == functs["mult"] || x.funct == functs["div"])) {
				if(!BusyBitTable[x.mappedrs] && !BusyBitTable[x.mappedrt]) { // Busy bit changed in 7th stage-first half
					array_of_instructions[i].PC=-1; // now this instruction isnt in queue
					return x;
				}
			}
		}
		instruction fail;
		fail.PC = -1;
		return fail;
	}
} integerQueue;

struct IntegerRegisterFile{
	int value[64]; // value sof the 32 registers
	long hi; // 2 special registers
	long lo;
	void write(int reg,int val){
		value[reg] = val; 
	}
	int read(int reg){
		return value[reg];
	}
	IntegerRegisterFile() {
		for(int i=0;i<64;i++) {
			value[i] = 0;
		}
		hi = lo = 0;
	}
} integerRegisterFile;

struct Stage3_4_1{ // intermediate state needed for going from stage 3 to dtage 4 usually done using registers in hardware
	instruction instruct; // this for add/sub
	int in1,in2;
	bool is_valid;	// dont use the same struct data twice!
	void push(instruction _instruct,int _in1,int _in2){
		instruct = _instruct;
		in1 = _in1;
		in2 = _in2;
		is_valid = 1;
	}
	void pop(){
		is_valid = 0;
	}
	Stage3_4_1(){
		is_valid = false;
	}
} stage3_4_1;


struct Stage3_4_2{ // intermediate state needed for going from stage 3 to dtage 4 usually done using registers in hardware
	instruction instruct; // this struct for div/mult
	int in1,in2;
	bool is_valid;	// dont use the same struct data twice!
	void push(instruction _instruct,int _in1,int _in2){
		instruct = _instruct;
		in1 = _in1;
		in2 = _in2;
		is_valid = 1;
	}
	void pop(){
		is_valid = 0;
	}
	Stage3_4_2(){
		is_valid = false;
	}
} stage3_4_2;


struct _ALU1{ // Only for addition and subtraction
	int operate(Stage3_4_1 arg){
		return _operate(arg.in1,arg.in2,arg.instruct.opcode,arg.instruct.funct);
	}
	int _operate(int in1,int in2,int opcode,int funct){
		if(funct != functs["add"] && funct!=functs["sub"]){
			cout<<"Incorrect ALU used for opcode "<<opcode<<endl;
		}
		if(funct == functs["add"]) {
			return in1+in2;
		}
		else {
			return in1-in2;
		}
	}
	_ALU1(){}
} ALU1;


struct _ALU2{ // ALU 2 is for division and multiplicatio only
	pair<long,long> operate(Stage3_4_2 arg){
		return _operate(arg.in1,arg.in2,arg.instruct.opcode,arg.instruct.funct);
	}
	pair<long,long> _operate(int in1,int in2,int opcode,int funct){
		if(funct != functs["mult"] && funct!=functs["div"]){
			cout<<"Incorrect ALU used for opcode "<<opcode<<endl;
			return {-1,-1};
		}
		if(funct == functs["mult"]) {
			long  x(in1),y(in2);
			return make_pair((x*y),0); 
		}
		else {
			cout<<" dividing "<<opcode;
			return make_pair(in1%in2,in1/in2);
		}
	}
	_ALU2(){}
} ALU2;

////////////////////////////////////////////////////             STAGE 2        . /////////////////////////////////////////////////////////////////

class DecodeUnit {
public:
    // data members here
	queue<instruction> instructionBuffer; // Max Size  Ans: dont care
	bool decode_signal;  // if decode 0 then dont decode push into instructionbuffer else decode 
	//bool decode_signal2;   // decodesignal2
	   // Max Size 32
	DecodeUnit(){ // default constructor
		decode_signal = 1; 
		for(int i=0;i<32;i++) 
			RegisterMapping[i]=-1; // registers which havent been mapped till now are indicated through value of -1 
	}
	void tick(){
    	// first check if there are pending instructions in the buffer if so first use them and if not then we deocde the instructions in decodeReg 
    	// else we push those instructions into instruction buffer

		if(decodeReg.empty() && instructionBuffer.empty()) {
			decode_signal = 0;
		}
		if(decode_signal){
			if(instructionBuffer.size() >= 2){
				for(int i=0;i<2;i++){
					decodeInstruction(instructionBuffer.front());
    				instructionBuffer.pop();
				}
			}
			else if(instructionBuffer.size() == 1){
				decodeInstruction(instructionBuffer.front());
    			instructionBuffer.pop();
    			if(decodeReg.size()>=1){
    				instruction instr1 = decodeReg.front();
	    	 		decodeReg.pop();
	   				decodeInstruction(instr1);
    			}
    			else {
    				decode_signal = 0;
    			}
			}
			else {
				if(decodeReg.size() >= 2){
					for(int i=0;i<2;i++){
						instruction instr1 = decodeReg.front();
	    	 			decodeReg.pop();
	   					decodeInstruction(instr1);
					}
				}
				else if(decodeReg.size() == 1){
					instruction instr1 = decodeReg.front();
	    	 		decodeReg.pop();
	   				decodeInstruction(instr1);
	   				decode_signal = 0;
				}
				else {
					decode_signal = 0;
				}
			}
		}

    	// int cnt = 0;
    	// cout<<"##############################Size of decodeReg = "<<decodeReg.size()<<endl;
    	// if(decodeReg.empty()) {
    	// 	cout<<" ####################################Setting decode signal to "<<0<<endl;
    	// 	decode_signal = 0;
    	// }
    	// while(!instructionBuffer.empty() && cnt < 2){
    	// 	cnt++;
    	// 	if(decode_signal){
    	// 		decodeInstruction(instructionBuffer.front());
    	// 		instructionBuffer.pop();
    	// 	}
    	// }
    	// if(cnt == 0){
    	// 	if(decode_signal) {
	    // 		instruction instr1 = decodeReg.front();
	    // 		decodeReg.pop();
	    // 		decodeInstruction(instr1);
	    // 	}
    	// 	else if(!decodeReg.empty()){
	    // 		instruction instr1 = decodeReg.front();
	    // 		decodeReg.pop();
	    // 		instructionBuffer.push(instr1);
    	// 	}
    	// 	if(decode_signal) {
	    // 		instruction instr2 = decodeReg.front();
	    // 		decodeReg.pop();
	    // 		decodeInstruction(instr2);
	    // 	}
    	// 	else if(!decodeReg.empty()){
	    // 		instruction instr2 = decodeReg.front();
	    // 		decodeReg.pop();
	    // 		instructionBuffer.push(instr2);
    	// 	}
    	// }
    	// else if(cnt == 1){ // read first instruction from the deocdeReg and push the second instruction into the buffer
    	// 	if(!decodeReg.empty()){
		   //  	instruction instr1 = decodeReg.front();
		   //  	decodeReg.pop();
		   //  	if(decode_signal) {
		   //  		decodeInstruction(instr1);
		   //  	}
	    // 		else {
		   //  		instructionBuffer.push(instr1);
	    // 		}
	    // 	}
	    // 	if(!decodeReg.empty()){
    	// 		instruction instr2 = decodeReg.front();
	    // 		decodeReg.pop();
	    // 		instructionBuffer.push(instr2);
	    // 	}
    	// }
    	// else {// cnt = 2 push both into instructionBuffers
    	// 	instruction instr1 = decodeReg.front();
	    // 	decodeReg.pop();
	    // 	instructionBuffer.push(instr1);
	    // 	instruction instr2 = decodeReg.front();
	    // 	decodeReg.pop();
	    // 	instructionBuffer.push(instr2);
    	// }
    }

    void decodeInstruction(instruction instruct){
    	// ACtive list manipulation yet to be added
    	// there will be more if-elses as we add more instructions
    	if(instruct.is_branch){ // if its a branch different logic
    		// calculate branch address
    		// refer to branch predictor
    		// make the necessary state and push it on branch stack
    		// stall for one cycle
    		// load from that PC 
    		// maintain bitmasks fields here and there....
    	}
    	else { // fopr now else emans its an ALU operation
    		if(RegisterMapping[instruct.rs] == -1){
    			RegisterMapping[instruct.rs] = freelist.find();
    			instruct.mappedrs = RegisterMapping[instruct.rs];
    			freelist.remove(instruct.mappedrs);
    			cout<<" Logical reg "<<instruct.rs<<" Mapped to physical reg "<<instruct.mappedrs<<endl;
    		}
    		else {
    			instruct.mappedrs =  RegisterMapping[instruct.rs];
    			cout<<" Logical reg "<<instruct.rs<<" Mapped to physical reg "<<instruct.mappedrs<<endl;
    		}
    		if(RegisterMapping[instruct.rt] == -1){
    			RegisterMapping[instruct.rt] = freelist.find();
    			instruct.mappedrt = RegisterMapping[instruct.rt];
    			freelist.remove(instruct.mappedrt);
    			cout<<" Logical reg "<<instruct.rt<<" Mapped to physical reg "<<instruct.mappedrt<<endl;
    		}
    		else {
    			instruct.mappedrt =  RegisterMapping[instruct.rt];
    			cout<<" Logical reg "<<instruct.rt<<" Mapped to physical reg "<<instruct.mappedrt<<endl;
    		}
    		//  rd has to be mapped to new register
    		RegisterMapping[instruct.rd] = freelist.find();
    		instruct.mappedrd = RegisterMapping[instruct.rd];
    		freelist.remove(instruct.mappedrd);
    		cout<<" Logical reg "<<instruct.rd<<" Mapped to physical reg "<<instruct.mappedrd<<endl;
    		BusyBitTable[instruct.mappedrd] = 1; // this destination register is busy now

    		// after decoding i need to add it in integer queue
    		cout<<"Adding instruction to integer queue and active list"<<endl;
    		instruct.DE = global_timer;
    		instruct.ID = global_intr_cntr++;
    		integerQueue.add(instruct);
    		activelist.push(instruct);	
    	}
    }
};





////////////////////////////////////////////////////             STAGE 3        . /////////////////////////////////////////////////////////////////



class Stage3{
public:
	instruction I1,I2;
	void tick(){
		I1 = integerQueue.issue1();
		I2 = integerQueue.issue2();
	}
	void tock(){
		if(I1.PC!=-1){
			cout<<"Pushing instruction 1 to ALU"<<endl;
			I1.RF = global_timer;
			stage3_4_1.push(I1,integerRegisterFile.read(I1.mappedrs),integerRegisterFile.read(I1.mappedrt));
		}
		if(I2.PC!=-1){
			cout<<"Pushing instruction 2 to ALU "<<I2.PC<<" "<<I2.opcode<<" "<<I2.funct<<" "<<I2.rd<<endl;
			I2.RF = global_timer;
			stage3_4_2.push(I2,integerRegisterFile.read(I2.mappedrs),integerRegisterFile.read(I2.mappedrt));
		}
	}
} stage3;	



struct Stage4_5_1{
	instruction instruct;
	int res;
	bool is_valid;
} stage4_5_1;



struct Stage4_5_2{
	instruction instruct;
	pair<int,int> res;
	bool is_valid;
} stage4_5_2;

////////////////////////////////////////////////////             STAGE 4        . /////////////////////////////////////////////////////////////////


class Stage4{
public:
	void tick(){
		if(stage3_4_1.is_valid){
			stage4_5_1.res = ALU1.operate(stage3_4_1);
			cout<<"ALU1 operation done reseting busybit table physical register = "<<stage3_4_1.instruct.mappedrd<<"= 0 and setting stage for stage5"<<endl;
			BusyBitTable[stage3_4_1.instruct.mappedrd] = 0; // This LINE IS SUPER CRITICAL I GUESS...
			stage4_5_1.is_valid = 1;
			stage4_5_1.instruct = stage3_4_1.instruct;
			cout<<"Does RF assignment still holds?"<<stage4_5_1.instruct.RF<<endl; 
			stage4_5_1.instruct.EXEC = global_timer;
			stage3_4_1.pop();
		}
		if(stage3_4_2.is_valid){
			stage4_5_2.res = ALU2.operate(stage3_4_2);
			cout<<"ALU1 operation done reseting busybit table physical register = "<<stage3_4_2.instruct.mappedrd<<"= 0 and setting stage for stage5"<<endl;
			BusyBitTable[stage3_4_2.instruct.mappedrd] = 0;
			stage4_5_2.is_valid = 1;
			stage4_5_2.instruct = stage3_4_2.instruct;
			stage4_5_2.instruct.EXEC = global_timer;
			stage3_4_2.pop();
		}
	}	
} stage4;

////////////////////////////////////////////////////             STAGE 5         . /////////////////////////////////////////////////////////////////


class Stage5{
public:
	void tick(){
		if(stage4_5_1.is_valid){
			integerRegisterFile.write(stage4_5_1.instruct.mappedrd,stage4_5_1.res);
			stage4_5_1.is_valid = 0;
			stage4_5_1.instruct.WB = global_timer;
			activelist.set_done_bit(stage4_5_1.instruct);
		}
		if(stage4_5_2.is_valid){
			integerRegisterFile.hi = stage4_5_2.res.first;
			integerRegisterFile.lo = stage4_5_2.res.second;
			stage4_5_2.is_valid = 0;
			stage4_5_2.instruct.WB = global_timer;
			activelist.set_done_bit(stage4_5_2.instruct);
		}
	}
} stage5;

bool cmp(instruction I1,instruction I2){
	return I1.IF < I2.IF; 
}


int main(int argc, char * argv[]){
	if(argc <= 1) {
		cout<<"program.txt needed as second arguement"<<endl;
		return 0;
	}
	for(int i=0;i<1000000;i++){
		I_cache[i].PC = -1;
	}
	// input and fill i cache that will
	// input the file
	instantiateISA(); // fills the opcodes and funct maps; 
	ifstream programFile(argv[1]);
	if(!programFile) {
    	cout << "Cannot open input file. :(\n";
   	 	return 1;
 	}
	int PC=0;
	string input=" ";
	while(getline(programFile,input)){ // fill I_cache
		if(PC > 1000000) {
			cout<<"spspim  cant handle such a big program : keep less than one million instructions"<<endl;
			return 0;
		}
		cout<<input<<endl;
		instruction instruction_from_file(PC,input);
		I_cache[PC] = instruction_from_file;   // this instantiates I_cache instruction through input string and PC
		PC++;
	}

	// initiallize the required units
	FetchUnit fetchunit;
	DecodeUnit decodeunit;
	// the while loop of cycle
	int time = 8; // assume it takes 10 cycles need to figure out when to stop the pipeline
	while(time--){
		cout<<time<<endl;
		global_timer++;

		fetchunit.tick();
		decodeunit.tick();
		
		stage3.tick(); // Stage3.tock() issues instructions whose operands are available after stage 4
		stage4.tick(); // Stage4.tick() does everything fir stage 3 and stage 4
		stage5.tick(); // Stage5.tick() does everything
		stage3.tock(); // fetches operands from registers and goes into required ALU
		//completesignal = 1;
	}
	programFile.close();
	cout<<"Printing logic "<<outputorder.size()<<endl;
	// output logic now
	sort(outputorder.begin(),outputorder.end(),cmp); // sort in increasing order of time of IF
	cout<<"Printing logic "<<outputorder.size()<<endl;
	for(auto I : outputorder){
		cout<<"Printing logic "<<outputorder.size()<<" "<<I.IF<<" "<<I.RF<<endl;
		int i=0;
		while(i!=I.IF) {
			cout<<"|\t";
			i++;
		}
		cout<<"| IF ";
		while(i!=I.DE) {
			cout<<"|\t";
			i++;
		}
		cout<<"| DE ";
		while(i!=I.RF) {
			cout<<"|\t";
			i++;
		}
		cout<<"| RF ";
		while(i!=I.EXEC) {
			cout<<"|\t";
			i++;
		}
		cout<<"|EXEC";
		while(i!=I.WB) {
			cout<<"|\t";
			i++;
		}
		cout<<"| WB ";
		cout<<endl;
	}


}