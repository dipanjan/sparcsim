#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <loader.h>
#include <memory.h>
#include <simulator.h>
#include <constants.h>



/* Execution begins here */

int main(int argc, char* argv[])
{
	char simulatorCommand[MAX_INPUT_LENGTH];
	
	initializeMemory();

	printf("\nSPARC v8 Simulator\n");
	printf("******************\n");
	printf("System Configuration: RAM = 4GB\n\n");
	
	if(argc == 2)
		load_sparc_elf(argv[1]);
	
	while(1)
	{
		printf("sparcsim>");
		fgets(simulatorCommand, MAX_INPUT_LENGTH, stdin);
		if(processSimulatorCommand(simulatorCommand) == RET_QUIT)
			return RET_SUCCESS;
	}
}


/* Processes the commands issued to the simulator */

int processSimulatorCommand(char* simulatorCommand)
{
	char* command = NULL, *firstParametre = NULL, *secondParametre = NULL;
	long int firstNumericParametre, secondNumericParametre;
	const char delimiters[] = " \n";
	char* hexNumber = (char*)malloc(32);
	
	command = strtok(simulatorCommand, delimiters); 
	firstParametre = strtok(NULL, delimiters);
	if(firstParametre != NULL)
		secondParametre = strtok(NULL, delimiters);
	if(secondParametre != NULL)
		while(strtok(NULL, delimiters));
	
	// Quit
	if(!(strcmp(command, "quit") && strcmp(command, "q")))
		return RET_QUIT;
	
	// help
	if(!(strcmp(command, "help") && strcmp(command, "h")))
	{
		printf("\n\tsparcsim  [file_name]    |  load a file into simulator memory\n");
		printf("\t[l]oad  <file_name>      |  load a file into simulator memory\n");
		printf("\t[m]em [addr] [count]     |  display memory at [addr] for [count] bytes\n");
		printf("\t[d]is [addr] [count]     |  disassemble [count] instructions at address [addr]");
		printf("\t[h]elp                   |  display this help\n");
		printf("\t[q]uit                   |  exit the simulator\n\n");
		return RET_SUCCESS;
	}
	
	// [l]oad
	if(!(strcmp(command, "load") && strcmp(command, "l")))
	{
		load_sparc_elf(firstParametre);
		return RET_SUCCESS;
	}
	
	
	// Here starts the commands with both numeric parametres
	
	
	if(firstParametre != NULL)
		firstNumericParametre = strtol(firstParametre, NULL, 0);
	if(secondParametre != NULL)
		secondNumericParametre = strtol(secondParametre, NULL, 0);
	
	
	// [m]em
	if(!(strcmp(command, "mem") && strcmp(command, "m")))
	{
		if(firstParametre == NULL && secondParametre == NULL)
			return RET_FAILURE;
		else
			if(secondParametre == NULL)
			{
				getMemoryArea(firstNumericParametre, 64);
				return RET_SUCCESS;
			}
			else
			{
				getMemoryArea(firstNumericParametre, secondNumericParametre);
				return RET_SUCCESS;
			}
	}
	
	
	// [d]is
	if(!(strcmp(command, "dis") && strcmp(command, "d")))
	{
		if(firstParametre == NULL && secondParametre == NULL)
			return RET_FAILURE;
		else
			if(secondParametre == NULL)
			{
				secondNumericParametre = 16;
			}
			
		unsigned long instructionCount;
		char* cpuInstruction;
		char* disassembledInstruction;
		
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
		printf("\n\n");
		return RET_SUCCESS;
	}
	
	else
		return RET_FAILURE;
}
