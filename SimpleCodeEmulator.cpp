
#include <iostream>
#include <fstream>
#include <Vector>
#include <sstream>
#include <cstdint>
#include <string>
#include <map>

//numbers represent the register # that you need to place the value for the operation to read from

enum OpCode : uint32_t {
	//codes for writing to registers	
	RAX = 1,
	RBX = 2,
	RCX = 3,
	RDX = 4,
	REX = 5,
	RFX = 6,
	RGX = 7,
	RHX = 8,

	DRF = 9, // derefrencs **MemorySegment | **Register to *MemoryPointedTo
	MOV = 10,
	SET = 11,




};

std::map<std::string, uint8_t> InstructionSet = { 
	{"NULL", NULL}, // Value Type
	{"RAX", RAX},
	{"RBX", RBX},
	{"RCX", RCX},
	{"RDX", RDX},
	{"REX", REX},
	{"RFX", RFX},
	{"RGX", RGX},
	{"RHX", RHX},

	{"DRF", DRF},
	{"MOV", MOV},
	{"SET", SET}
};

struct memory {
	uint8_t Type;
	uint32_t MemorySegment;
};

std::vector<memory> ProgramMemory;
uint32_t ProgramCounter;

void InitRegisters(int Amount) {
	memory Register = {0,0};

	for (int i = 0; i < Amount; i++)
	{
		ProgramMemory.emplace_back(Register);
	}
	ProgramCounter = Amount;
}

inline uint64_t Combine32Bits(uint32_t Lower, uint32_t Higher) {
	return static_cast<uint64_t>(Higher) << 32 | Lower;
}

uint32_t LoadFileIntoProgramMemory(char* FileName) {
	std::ifstream File;

	uint32_t CurrentSize = ProgramMemory.size();

	File.open(FileName);

	std::string line;

	if (File.is_open()) {
		int LineNumber = 1;
		while (std::getline(File, line)) {
			memory Current;
			
			std::stringstream ss(line);

			std::string CurrentInstruction;
			std::string CurrentMemory;

			ss >> CurrentInstruction;
			ss >> CurrentMemory;

			if (InstructionSet.find(CurrentInstruction) != InstructionSet.end()) {
				Current.Type = InstructionSet[CurrentInstruction];
			}
			else {
				//the user has entred a non existant instruciton
				std::cerr << "File " << FileName << " ON LINE " << LineNumber << " UNIDENTIFIED TYPE -" << CurrentInstruction << "-\n";
			}

			if (InstructionSet.find(CurrentMemory) != InstructionSet.end()) {
				// the user has used a opcode for the memory
				Current.MemorySegment = InstructionSet[CurrentMemory];
			}
			else {
				Current.MemorySegment = std::stoull(CurrentMemory);
			}

			ProgramMemory.emplace_back(Current);


			LineNumber++;
		}
		File.close();
	}
	else {
		std::cerr << "File " << FileName << " does not exist. You need to be in the same file dir as your file";
	}

	return CurrentSize;
}

void ReadCurrentMemory() {
	memory Current = ProgramMemory[ProgramCounter];
	ProgramCounter += 1;

	//implementations of all instructions
	
	if (Current.Type == NULL) {
		return;
	}
	else if (RAX <= Current.Type && Current.Type <= RHX) {
		ProgramMemory[Current.Type - 1].MemorySegment = Current.MemorySegment;
	}
	else if (Current.Type == DRF) {
		ProgramMemory[Current.MemorySegment - 1].MemorySegment = ProgramMemory[ ProgramMemory[Current.MemorySegment - 1].MemorySegment - 1].MemorySegment;
	}
	else if (Current.Type == MOV) {
		ProgramMemory[ProgramMemory[0].MemorySegment - 1] = ProgramMemory[ProgramMemory[1].MemorySegment - 1];
	}
	else if (Current.Type == SET){
		ProgramMemory[ProgramMemory[0].MemorySegment - 1].MemorySegment = ProgramMemory[1].MemorySegment;
	}


}

int main(int argc, char* argv[])
{
	if (argc != 2) {
		std::cout << "Incorrect Input Format\n Usage:\n SimpleCodeEmulator.exe MyProgram.e\n";
		return -1;
	}
	
	InitRegisters(16);
	LoadFileIntoProgramMemory(argv[1]);

	return 0;
}