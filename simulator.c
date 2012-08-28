#include <simulator.h>



/* Execution begins here */

int main(int argc, char* argv[])
{
	char simulatorCommand[MAX_INPUT_LENGTH];
	
	initializeMemory();
    initializeRegisters();
	initializeBreakPointList();
	
	if(argc == 3)
	{
		if(!(strcmp(argv[1], "-d") && strcmp(argv[1], "--dis")))
		{
			struct loadedSections *elfSectionsPrevPtr, *elfSectionCurPtr;

			elfSectionCurPtr = load_sparc_instructions(argv[2]);

			printf("\n");
			sprintf(simulatorCommand, "load %s", argv[2]);
			processSimulatorCommand(simulatorCommand);
			printf("\n\n");

			do
			{
				printf("Disassembly of Section: %s\n\n", elfSectionCurPtr->sectionName);
				sprintf(simulatorCommand, "dis %lu %lu", elfSectionCurPtr->sectionLoadAddress, elfSectionCurPtr->instructionCount);
				processSimulatorCommand(simulatorCommand);
				elfSectionsPrevPtr = elfSectionCurPtr;
				free(elfSectionsPrevPtr);
				elfSectionCurPtr = elfSectionCurPtr->nextSection;
			}
			while(elfSectionCurPtr != NULL);
			printf("\n");
			return RET_SUCCESS;
		}

	}

	printf("\nSPARC v8 Simulator\n");
	printf("******************\n");
	printf("System Configuration: RAM = 4GB\n\n");

	if(argc == 2)
	{
		sprintf(simulatorCommand, "load %s", argv[1]);
		processSimulatorCommand(simulatorCommand);
	}
	
	if(argc == 3)
	{
		if(!(strcmp(argv[1], "-b") && strcmp(argv[1], "--batch")))
		{
			sprintf(simulatorCommand, "batch %s", argv[2]);
			processSimulatorCommand(simulatorCommand);
		}
	}

	while(1)
	{
		printf("sparcsim>");
		fgets(simulatorCommand, MAX_INPUT_LENGTH, stdin);
		//printf("Buf: %d", strlen(simulatorCommand));
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
	if(!command)
		return RET_FAILURE;
	firstParametre = strtok(NULL, delimiters);
	if(firstParametre != NULL)
	{
		secondParametre = strtok(NULL, delimiters);
		firstNumericParametre = strtoul(firstParametre, NULL, 0);
    }
	if(secondParametre != NULL)
	{
		while(strtok(NULL, delimiters));
		secondNumericParametre = strtoul(secondParametre, NULL, 0);
    }
	
	
	// Quit
	if(!(strcmp(command, "quit") && strcmp(command, "q")))
		return RET_QUIT;
	
	
	// help
	if(!(strcmp(command, "help") && strcmp(command, "h")))
	{
		printf("\n\tsparcsim  [file_name]       |  load a file into simulator memory\n");
		printf("\tsparcsim  -d [file_name]    |  disassemble SPARC ELF binary\n");
		printf("\t[ba]tch <file>              |  execute a batch file of SPARCSIM commands\n");
		printf("\t[re]set                     |  reset simulator \n");
		printf("\t[l]oad  <file_name>         |  load a file into simulator memory\n");
		printf("\t[m]em [addr] [count]        |  display memory at [addr] for [count] bytes\n");
		printf("\t[w]mem <addr> <val>         |  write memory word at <addr> with value <val>\n");
		printf("\t[s]tep                      |  single step\n");
		printf("\t[br]eak <addr>              |  add a breakpoint at <addr>\n");
		printf("\t[de]l <num>                 |  delete breakpoint <num>\n");
		printf("\t[br]eak                     |  print all breakpoints\n");
		printf("\t[r]eg [reg] [val]           |  show/set integer registers (or windows, eg 'reg w2'\n");
		printf("\t[d]is [addr] [count]        |  disassemble [count] instructions at address [addr]\n");
		printf("\t[c]ont [cnt]                |  continue execution for [cnt] instructions\n");
		printf("\t[g]o <addr> [cnt]           |  start execution at <addr>\n");
		printf("\t[ru]n [cnt]                 |  reset and start execution at address zero\n");
		printf("\t[h]elp                      |  display this help\n");
		printf("\t[e]cho <string>             |  print <string> to the simulator window\n");
		printf("\t[sh]ell <cmd>               |  execute shell command\n");
		printf("\t[q]uit                      |  exit the simulator\n");

		return RET_SUCCESS;
	}
	
	
	// [re]set
	if(!(strcmp(command, "reset") && strcmp(command, "re")))
	{
		resetSimulator();
		return RET_SUCCESS;
	}


	// [ba]tch
	if(!(strcmp(command, "batch") && strcmp(command, "ba")))
	{
		if(firstParametre == NULL)
			return RET_FAILURE;
		else
		{
			FILE* handle = fopen(firstParametre, "r");
			if(!handle)
			{
				printf("\n%s does not exist", firstParametre);
				return RET_FAILURE;
			}
			
			char* buffer = (char*)malloc(150);
			
			while(fgets(buffer, 150, handle))
			{
				short bufferIndex = -1;
				
				// Stripping off trailing carriage return
				while(buffer[++bufferIndex] != '\0' && buffer[bufferIndex] != '\r' && buffer[bufferIndex] != '\n');
				buffer[bufferIndex] = '\0';
				
				if(!buffer)
					continue;

				printf("\nsparcsim>%s", buffer);
				if(processSimulatorCommand(buffer) == RET_FAILURE)
					printf("\nError executing command.");
					
			}		
			
			free(buffer);
			fclose(handle);
			return RET_SUCCESS;
		}
	}
	
	
	// [l]oad
	if(!(strcmp(command, "load") && strcmp(command, "l")))
	{
		struct loadedSections *elfSectionsPrevPtr, *elfSectionCurPtr;

		elfSectionCurPtr = load_sparc_instructions(firstParametre);
		printf("\n");

		switch(elfSectionCurPtr ->sectionType)
		{
		case ELF_FILE_DOES_NOT_EXIST_ERROR:
			printf("Couldn't open: %s\n", firstParametre);
			return RET_FAILURE;
		case ELF_BINARY_OUT_OF_DATE:
			printf("Warning: ELF library is out of date\n");
			return RET_FAILURE;
		case ELF_POINTER_INITIALIZATION_ERROR:
			printf("Couldn't initialize ELF pointer\n");
			return RET_FAILURE;
		case ELF_SECTION_LOAD_ERROR:
			printf("Couldn't load ELF sections\n");
			return RET_FAILURE;
		}

		do
		{
			printf("Loaded Section: %s, Address: 0x%lx, Size: %lu, Type: ", elfSectionCurPtr->sectionName, elfSectionCurPtr->sectionLoadAddress, elfSectionCurPtr->sectionSize);
			switch(elfSectionCurPtr->sectionType)
			{
				case CODE_SECTION: printf("Code, Instructions: %lu\n", elfSectionCurPtr->instructionCount); break;
				case DATA_SECTION: printf("Data\n"); break;
				case UNINITIALIZED_DATA_SECTION: printf("Uninitialized Data\n"); break;
			}
			elfSectionsPrevPtr = elfSectionCurPtr;
			free(elfSectionsPrevPtr);
			elfSectionCurPtr = elfSectionCurPtr->nextSection;
		}
		while(elfSectionCurPtr != NULL);
		
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
		return RET_SUCCESS;
	}
	
	
	// [s]tep
	if(!(strcmp(command, "step") && strcmp(command, "s")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC;

		regPC = getRegister("pc");
		cpuInstruction = getQuadWordFromMemory(regPC);
		disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
		executeInstruction(disassembledInstruction);

		free(cpuInstruction);
		free(disassembledInstruction);
		return RET_SUCCESS;
	}
	
	
	// [c]ont
	if(!(strcmp(command, "cont") && strcmp(command, "c")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC, nextBreakPoint;
		unsigned short isReset;
		int exitCode, instructionCount;

		regPC = getRegister("pc");
		nextBreakPoint = getNextBreakPoint(regPC, &isReset);

		if(!firstParametre)
		{
			do
			{
				regPC = getRegister("pc");
				if((isReset == 0) && (regPC == nextBreakPoint))
				{
					printf("Breaking at: 0x%lx", regPC);
					return RET_SUCCESS;
				}
				cpuInstruction = getQuadWordFromMemory(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
			}
			while(exitCode == RET_SUCCESS);
		}
		else
		{
			exitCode = RET_SUCCESS;
			for(instructionCount = 0; (instructionCount < firstNumericParametre) && (exitCode == RET_SUCCESS); instructionCount++)
			{
				regPC = getRegister("pc");
				if((isReset == 0) && (regPC == nextBreakPoint))
					return RET_SUCCESS;
				cpuInstruction = getQuadWordFromMemory(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
			}
		}

		free(cpuInstruction);
		free(disassembledInstruction);
		return RET_SUCCESS;
	}


	// [g]o
	if(!(strcmp(command, "go") && strcmp(command, "g")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC, nextBreakPoint;
		int exitCode, instructionCount;
		unsigned short isReset;

		regPC = getRegister("pc");
		nextBreakPoint = getNextBreakPoint(regPC, &isReset);

		if(!firstParametre)
			return RET_FAILURE;

		if(!secondParametre)
		{
			do
			{
				if((isReset == 0) && (regPC == nextBreakPoint))
				{
					printf("Breaking at: 0x%lx", regPC);
					return RET_SUCCESS;
				}
				setRegister("pc", secondNumericParametre);
				cpuInstruction = getQuadWordFromMemory(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
			}
			while(exitCode == RET_SUCCESS);
		}
		else
		{
			exitCode = RET_SUCCESS;
			for(instructionCount = 0; (instructionCount < secondNumericParametre) && (exitCode == RET_SUCCESS); instructionCount++)
			{
				setRegister("pc", secondNumericParametre);
				cpuInstruction = getQuadWordFromMemory(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
			}
		}

		free(cpuInstruction);
		free(disassembledInstruction);
		return RET_SUCCESS;
	}


	// [ru]n
	if(!(strcmp(command, "run") && strcmp(command, "ru")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC, nextBreakPoint;
		int exitCode, instructionCount;
		unsigned short isReset;

		regPC = getRegister("pc");
		nextBreakPoint = getNextBreakPoint(regPC, &isReset);
		setRegister("pc", 0);
		
		if(!firstParametre)
		{
			do
			{
				if((isReset == 0) && (regPC == nextBreakPoint))
				{
					printf("Breaking at: 0x%lx", regPC);
					return RET_SUCCESS;
				}
				cpuInstruction = getQuadWordFromMemory(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
			}
			while(exitCode == RET_SUCCESS);
		}
		else
		{
			exitCode = RET_SUCCESS;
			for(instructionCount = 0; (instructionCount < firstNumericParametre) && (exitCode == RET_SUCCESS); instructionCount++)
			{
				if((isReset == 0) && (regPC == nextBreakPoint))
				{
					printf("Breaking at: 0x%lx", regPC);
					return RET_SUCCESS;
				}
				cpuInstruction = getQuadWordFromMemory(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
			}
		}

		free(cpuInstruction);
		free(disassembledInstruction);
		return RET_SUCCESS;
	}


	// [br]eak
	if(!(strcmp(command, "break") && strcmp(command, "br")))
	{
		if(firstParametre == NULL)
		{
			unsigned long breakPointAddress;
			unsigned short isReset, count; 
			
			isReset = 0; 
			count = 0;
			printf("\n");
			while(1)
			{
				breakPointAddress = getBreakPoint(&isReset);
				if(isReset == 0)
					printf("%d: 0x%lx\n", ++count, breakPointAddress);
				else
					break;
			}
		}
		
		else
			if(addBreakPoint(firstNumericParametre) == BREAKPOINT_ALLOCATION_ERROR)
			{
				printf("\nERROR: Can't allocate breakpoint\n");
				return RET_FAILURE;
			}
			
		return RET_SUCCESS;
	}
	
	
	// [de]l
	if(!(strcmp(command, "del") && strcmp(command, "de")))
	{
		if(firstParametre == NULL)
			return RET_FAILURE;
		else
		{
			if(deleteBreakPoint(firstNumericParametre)  == RET_FAILURE)
				printf("\nERROR: Can't delete breakpoint");
			else			
				return RET_SUCCESS;
		}
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
			printf("\tpc : %s\t\t", registerValue);
			displayQuadWord(cpuInstruction, 1);
			printf("\t%s", disassembledInstruction);
			free(cpuInstruction);
			free(disassembledInstruction);
			free(registerValue);
			
			regPC = getRegister("npc");
			registerValue = displayRegister(regPC);
			cpuInstruction = getQuadWordFromMemory(regPC);
			disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
			printf("\n\tnpc: %s\t\t", registerValue);
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
