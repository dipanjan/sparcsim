#include <execute.h>



void executeInstruction(char* disassembledInstruction, unsigned long regPC)
{
	char* opcode = strtok(disassembledInstruction, " ");
		
	// Format - I instruction
	if(!strcmp(opcode, "call"))
	{
		unsigned long displacement = strtol(strtok(NULL, " "), NULL, 0);
		unsigned long pcValue = getRegister("pc") + displacement;
		setRegister("pc", pcValue);
		setRegister("%o7", pcValue);
		return;
	}
	
	
	// Format - II instruction
	if(!strcmp(opcode, "sethi"))
	{
		unsigned long constant = strtol(strtok(NULL, " %hi()"), NULL, 0);
		char* destinationRegister = strtok(NULL, ", ");
		setRegister(destinationRegister, constant);
		setRegister("pc", regPC + 4);
		setRegister("npc", regPC + 8);
		return;
	}
	
	
	if(!strcmp(opcode, "nop"))
	{
		setRegister("pc", regPC + 4);
		setRegister("npc", regPC + 8);
		return;
	}
	
	
	if(!strcmp(opcode, "unimp"))
	{
		printf("Trap: illegal_instruction\n");
		return;
	}
	
	// Format - III instruction
}



/*int main()
{
	char disassembledInstruction[] = "sethi %hi(0x40000000), %g7";
	executeInstruction(disassembledInstruction, 1);
	return 0;
}*/
