#include <simulator.h>



/* Execution begins here */

int main(int argc, char* argv[])
{
	char simulatorCommand[MAX_INPUT_LENGTH];
	
	initializeMemory();
    initializeRegisters();

	printf("\nSPARC v8 Simulator\n");
	printf("******************\n");
	printf("System Configuration: RAM = 4GB\n\n");
	
	if(argc == 2)
		load_sparc_elf(argv[1]);
	
	while(1)
	{
		printf("\nsparcsim>");
		fgets(simulatorCommand, MAX_INPUT_LENGTH, stdin);
		if(processSimulatorCommand(simulatorCommand) == RET_QUIT)
			return RET_SUCCESS;
	}
}


/* Processes the commands issued to the simulator */

int processSimulatorCommand(char* simulatorCommand)
{
	char* command = NULL, *firstParametre = NULL, *secondParametre = NULL, *arguments = (char*)malloc(50);
	unsigned long firstNumericParametre = 0, secondNumericParametre = 0;
	const char delimiters[] = " \n";
	char* hexNumber = (char*)malloc(32);
	
	strcpy(arguments, simulatorCommand);
	command = strtok(simulatorCommand, delimiters); 
	firstParametre = strtok(NULL, delimiters);
	if(firstParametre != NULL)
	{
		secondParametre = strtok(NULL, delimiters);
		firstNumericParametre = strtol(firstParametre, NULL, 0);
    }
	if(secondParametre != NULL)
	{
		while(strtok(NULL, delimiters));
		secondNumericParametre = strtol(secondParametre, NULL, 0);
    }
	
	
	// Quit
	if(!(strcmp(command, "quit") && strcmp(command, "q")))
		return RET_QUIT;
	
	
	// help
	if(!(strcmp(command, "help") && strcmp(command, "h")))
	{
		printf("\n\tsparcsim  [file_name]    |  load a file into simulator memory\n");
		printf("\t[l]oad  <file_name>      |  load a file into simulator memory\n");
		printf("\t[m]em [addr] [count]     |  display memory at [addr] for [count] bytes\n");
		printf("\t[w]mem <addr> <val>      |  write memory word at <addr> with value <val>\n");
		printf("\t[r]eg [reg] [val]        |  show/set integer registers (or windows, eg 'reg w2'\n");
		printf("\t[d]is [addr] [count]     |  disassemble [count] instructions at address [addr]\n");
		printf("\t[h]elp                   |  display this help\n");
		printf("\t[e]cho <string>          |  print <string> to the simulator window\n");
		printf("\t[sh]ell <cmd>            |  execute shell command\n");
		printf("\t[q]uit                   |  exit the simulator\n\n");
		return RET_SUCCESS;
	}
	
	
	// [l]oad
	if(!(strcmp(command, "load") && strcmp(command, "l")))
	{
		load_sparc_elf(firstParametre);
		return RET_SUCCESS;
	}
	
	
	// [m]em
	if(!(strcmp(command, "mem") && strcmp(command, "m")))
	{
		if(firstParametre == NULL && secondParametre == NULL)
			return RET_FAILURE;
		else
			if(secondParametre == NULL)
			{
				displayMemoryArea(firstNumericParametre, 64);
				return RET_SUCCESS;
			}
			else
			{
				displayMemoryArea(firstNumericParametre, secondNumericParametre);
				return RET_SUCCESS;
			}
	}
	
	
	// [w]mem
	if(!(strcmp(command, "wmem") && strcmp(command, "w")))
	{
		if(firstParametre == NULL && secondParametre == NULL)
			return RET_FAILURE;
		else
		{
			char* bytePointer = (char*)&secondNumericParametre;
			unsigned short count;
			for(count = 0; count < 4; count++)
				writeMemory(firstNumericParametre + (3 - count), *(bytePointer + count));
		}
	}
	
	
	if(!(strcmp(command, "step") && strcmp(command, "s")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC;

		regPC = getRegister("pc");
		cpuInstruction = getQuadWordFromMemory(regPC);
		disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
		executeInstruction(disassembledInstruction, regPC);

		free(cpuInstruction);
		free(disassembledInstruction);
	}


	// [e]cho
	if(!(strcmp(command, "echo") && strcmp(command, "e")))
	{
		if(firstParametre == NULL)
			return RET_FAILURE;
		else
		{
			puts(firstParametre);
			return RET_SUCCESS;
		}
	}
	
	
	// [sh]ell
	if(!(strcmp(command, "shell") && strcmp(command, "sh")))
	{
		if(firstParametre == NULL)
			return RET_FAILURE;
		else
		{
			char* argumentBase = arguments;
			while(*arguments++ != ' ' );
			printf("\n");
			system(arguments);
			free(argumentBase);
			return RET_SUCCESS;
		}
	}
	
	
    //[r]eg
    if(!(strcmp(command, "reg") && strcmp(command, "r")))
	{
		char* registerValue, *cpuInstruction, *disassembledInstruction;
		char sparcRegister[3];
		unsigned long regPC;
		unsigned short count;
		sparcRegister[2] = '\0';
		
		if(firstParametre != NULL && secondParametre != NULL)
			setRegister(firstParametre, secondNumericParametre);
		else
		{
			printf("\n\t\t    INS\t\t   LOCALS\t    OUTS\t  GLOBALS\n\n");
			
			for(count = 0; count < 8; count++)
			{
				sparcRegister[1] = count + '0'; 
				
				sparcRegister[0] = 'i'; 
				registerValue = displayRegister(getRegister(sparcRegister));
				printf("\t%d:\t %s", count, registerValue);
				free(registerValue);
				
				
				sparcRegister[0] = 'l'; 
				registerValue = displayRegister(getRegister(sparcRegister));
				printf("\t %s", registerValue);
				free(registerValue);
				
				sparcRegister[0] = 'o'; 
				registerValue = displayRegister(getRegister(sparcRegister));
				printf("\t %s", registerValue);
				free(registerValue);
				
				sparcRegister[0] = 'g';
				registerValue = displayRegister(getRegister(sparcRegister));
				printf("\t %s\n", registerValue);
				free(registerValue);
			}
		
			registerValue = displayRegister(getRegister("psr"));
			printf("\n\tpsr: %s", registerValue);
			free(registerValue);
			
			registerValue = displayRegister(getRegister("wim"));
			printf("\t\twim: %s", registerValue);
			free(registerValue);
			
			registerValue = displayRegister(getRegister("tbr"));
			printf("\t\ttbr: %s", registerValue);
			free(registerValue);
			
			registerValue = displayRegister(getRegister("y"));
			printf("\t\ty: %s\n\n", registerValue);
			free(registerValue);
			
			regPC = getRegister("pc");
			registerValue = displayRegister(regPC);
			cpuInstruction = getQuadWordFromMemory(regPC);
			disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
			printf("\n\t pc:  %s\t", registerValue);
			displayQuadWord(cpuInstruction, 1);
			printf("\t%s", disassembledInstruction);			
			free(cpuInstruction);
			free(disassembledInstruction);
			free(registerValue);
			
			regPC = getRegister("npc");
			registerValue = displayRegister(regPC);
			cpuInstruction = getQuadWordFromMemory(regPC);
			disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
			printf("\n\tnpc:  %s\t", registerValue);
			displayQuadWord(cpuInstruction, 1);
			printf("\t%s", disassembledInstruction);
			free(cpuInstruction);
			free(disassembledInstruction);
			free(registerValue);
			
			printf("\n");
		}
		
		return RET_SUCCESS;
	}
	
    	
	// [d]is
	if(!(strcmp(command, "dis") && strcmp(command, "d")))
	{
		if(firstParametre == NULL && secondParametre == NULL)
			return RET_FAILURE;
		else
			if(secondParametre == NULL)
				secondNumericParametre = 16;
			
		unsigned long instructionCount;
		char* cpuInstruction = NULL;
		char* disassembledInstruction = NULL;
		
		for(instructionCount = 0; instructionCount < secondNumericParametre; instructionCount++)
		{
			cpuInstruction = getQuadWordFromMemory(firstNumericParametre);
			disassembledInstruction = (char*)decodeInstruction(cpuInstruction, firstNumericParametre);
			printf("\n\t");
			sprintf(hexNumber, "%lx", firstNumericParametre);
			printf("0x%s:\t", hexNumber);
			displayQuadWord(cpuInstruction, 1);
			printf("\t%s", disassembledInstruction);
			firstNumericParametre += 4;
		}
		
		free(cpuInstruction);
		free(disassembledInstruction);
		printf("\n");
		return RET_SUCCESS;
	}
	
	else
		return RET_FAILURE;
}
