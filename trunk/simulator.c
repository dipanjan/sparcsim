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

			sprintf(simulatorCommand, "load %s", argv[2]);
			printf("\n");
			processSimulatorCommand(simulatorCommand);
			printf("\n");

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
			if(processSimulatorCommand(simulatorCommand) == RET_QUIT)
				return RET_SUCCESS;
		}
	}

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
	char* command = NULL, *firstParametre = NULL, *secondParametre = NULL, *arguments = (char*)malloc(200);
	unsigned long firstNumericParametre = 0, secondNumericParametre = 0;
        unsigned short count = 0;
	static unsigned long lastEncounteredBreakPoint = 0;
	static short isLastEncounteredBreakPointValid = 0;
	const char delimiters[] = " \n\t";
	char* hexNumber = (char*)malloc(32);
        char* batchCommand = (char*)malloc(32);
	
        // Strip off #
        for(count = 0; count < strlen(simulatorCommand); count++)
            if(simulatorCommand[count] == '#')
                simulatorCommand[count] = '\0';
        
	strcpy(arguments, simulatorCommand);
	command = strtok(simulatorCommand, delimiters);
	if(!command)
		return RET_NOTACOMMAND;
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
		printf("\tsparcsim  -b [file_name]    |  execute a batch file of SPARCSIM commands\n");
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
                printf("\t[f]loat                     |  print the FPU registers\n");
		printf("\t[d]is [addr] [count]        |  disassemble [count] instructions at address [addr]\n");
		printf("\t[c]ont [cnt]                |  continue execution for [cnt] instructions\n");
		printf("\t[g]o <addr> [cnt]           |  start execution at <addr>\n");
		printf("\t[ru]n [cnt]                 |  reset and start execution at address zero\n");
		printf("\t[h]elp                      |  display this help\n");
		printf("\t[e]cho <string>             |  print <string> to the simulator window\n");
		printf("\t[sh]ell <cmd>               |  execute shell command\n");
		printf("\t[q]uit                      |  exit the simulator\n\n");

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
				printf("Batch file: %s does not exist\n", firstParametre);
				return RET_FAILURE;
			}
			
			char* buffer = (char*)malloc(150);
			
			while(fgets(buffer, 150, handle))
			{
				short bufferIndex = -1;
				
				// Stripping off trailing carriage return
				while(buffer[++bufferIndex] != '\0' && buffer[bufferIndex] != '\r' && buffer[bufferIndex] != '\n');
				buffer[bufferIndex] = '\0';
				
				if(!buffer || !strlen(buffer))
                                        continue;

                                strcpy(batchCommand, buffer);
				switch(processSimulatorCommand(buffer))
				{
                                        case RET_SUCCESS: printf("sparcsim>%s\n", trim(batchCommand)); break;
                                        case RET_FAILURE: printf("sparcsim>Error executing command: %s\n", trim(batchCommand)); return RET_FAILURE;
					case RET_QUIT: return RET_QUIT;
				}					
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

		switch(elfSectionCurPtr->sectionType)
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
		printf("\n");
		
		return RET_SUCCESS;
	}
	
	
	// [m]em
	if(!(strcmp(command, "mem") && strcmp(command, "m")))
	{
		if(firstParametre == NULL && secondParametre == NULL)
			return RET_FAILURE;
		else
		{
			if(secondParametre == NULL)
				displayMemoryArea(firstNumericParametre, 64);
			else
				displayMemoryArea(firstNumericParametre, secondNumericParametre);
				
			printf("\n\n");
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
			/*char* bytePointer = (char*)&secondNumericParametre;
			unsigned short count;
			for(count = 0; count < 4; count++)
			{*/
				switch(writeWord(firstNumericParametre, secondNumericParametre))
				{
					case SECOND_PAGE_TABLE_ALLOCATION_ERROR: printf("Error allocating second page table\n"); return RET_FAILURE;
					case PAGE_ALLOCATION_ERROR: printf("Error allocating page\n"); return RET_FAILURE;
				}
			//}	
		}
		return RET_SUCCESS;
	}
	
	
	// [s]tep
	if(!(strcmp(command, "step") && strcmp(command, "s")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC;

		regPC = getRegister("pc");
		cpuInstruction = readWordAsString(regPC);
		disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
		printf("\t%08lX:\t", regPC);
		displayWord(cpuInstruction, 1);
		printf("\t%s\n",disassembledInstruction);
		executeInstruction(disassembledInstruction);

		free(cpuInstruction);
		free(disassembledInstruction);
		return RET_SUCCESS;
	}
	
	
	// [c]ont
	if(!(strcmp(command, "cont") && strcmp(command, "c")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC;
		int exitCode, instructionCount;

		instructionCount = 0;
		if(!firstParametre)
		{
			do
			{
				regPC = getRegister("pc");
				if(isBreakPoint(regPC))
				{
					if((regPC == lastEncounteredBreakPoint) && isLastEncounteredBreakPointValid && !instructionCount)
						isLastEncounteredBreakPointValid = 0;
					else
					{
						lastEncounteredBreakPoint = regPC;
						isLastEncounteredBreakPointValid = 1;
						printf("Breaking at: 0x%lX after executing %d instructions\n", regPC, instructionCount);
						return RET_SUCCESS;
					}
				}
				cpuInstruction = readWordAsString(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
				free(cpuInstruction);
				free(disassembledInstruction);
				instructionCount++;
			}
			while(exitCode == RET_SUCCESS);
		}
		else
		{
			exitCode = RET_SUCCESS;
			for(instructionCount = 0; (instructionCount < firstNumericParametre) && (exitCode == RET_SUCCESS); instructionCount++)
			{
				regPC = getRegister("pc");
				if(isBreakPoint(regPC))
				{
					if((regPC == lastEncounteredBreakPoint) && isLastEncounteredBreakPointValid && !instructionCount)
						isLastEncounteredBreakPointValid = 0;
					else
					{
						lastEncounteredBreakPoint = regPC;
						isLastEncounteredBreakPointValid = 1;
						printf("Breaking at: 0x%lX after executing %d instructions\n", regPC, instructionCount);
						return RET_SUCCESS;
					}
				}
				cpuInstruction = readWordAsString(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction); //printf("%d) disassembledInstruction: %s\n", instructionCount, disassembledInstruction);
				free(cpuInstruction);
				free(disassembledInstruction);
			}
		}

		return RET_SUCCESS;
	}


	// [g]o
	if(!(strcmp(command, "go") && strcmp(command, "g")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC;
		int exitCode, instructionCount;
		unsigned short isReset;

		if(!firstParametre)
			return RET_FAILURE;
                
                // Initializing execution environment
		setRegister("pc", firstNumericParametre);
                setRegister("npc", firstNumericParametre + 4);
                setRegister("i6", 0x40400000);
                setRegister("o6", 0x403FFE80);
                setRegister("wim", 0x0000002);
                setRegister("psr", 0xF30010E0);

		if(!secondParametre)
		{
			do
			{
				regPC = getRegister("pc");
				cpuInstruction = readWordAsString(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
				free(cpuInstruction);
				free(disassembledInstruction);
			}
			while(exitCode == RET_SUCCESS);
		}
		else
		{
			exitCode = RET_SUCCESS;
			for(instructionCount = 0; (instructionCount < secondNumericParametre) && (exitCode == RET_SUCCESS); instructionCount++)
			{
				regPC = getRegister("pc");
				cpuInstruction = readWordAsString(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);
				free(cpuInstruction);
				free(disassembledInstruction);
			}
		}

		return RET_SUCCESS;
	}


	// [ru]n
	if(!(strcmp(command, "run") && strcmp(command, "ru")))
	{
		char* cpuInstruction, *disassembledInstruction;
		unsigned long regPC;
		int exitCode, instructionCount;

		setRegister("pc", 0);
                setRegister("npc", 4);
		
		if(!firstParametre)
		{
			do
			{
				regPC = getRegister("pc");
				cpuInstruction = readWordAsString(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC); //printf("%s %s\n", cpuInstruction, disassembledInstruction);
				exitCode = executeInstruction(disassembledInstruction);
				free(cpuInstruction);
				free(disassembledInstruction);
			}
			while(exitCode == RET_SUCCESS);
		}
		else
		{
			exitCode = RET_SUCCESS;
			for(instructionCount = 0; (instructionCount < firstNumericParametre) && (exitCode == RET_SUCCESS); instructionCount++)
			{
				regPC = getRegister("pc");
				cpuInstruction = readWordAsString(regPC);
				disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
				exitCode = executeInstruction(disassembledInstruction);

				free(cpuInstruction);
				free(disassembledInstruction);
			}
		}

		return RET_SUCCESS;
	}


	// [br]eak
	if(!(strcmp(command, "break") && strcmp(command, "br")))
	{
		if(firstParametre == NULL)
		{
			struct breakPoint* curBreakPoint;
			unsigned long breakPointAddress;
			
			curBreakPoint = getBreakPoint(1);
			if(!curBreakPoint)
				return RET_SUCCESS;

			do
			{
				printf("\n%d: 0x%lX", curBreakPoint->breakPointSerial, curBreakPoint->memoryAddress);
				curBreakPoint = getBreakPoint(0);
			}while(curBreakPoint);
			printf("\n\n");
		}
		
		else
			if(addBreakPoint(firstNumericParametre) == BREAKPOINT_ALLOCATION_ERROR)
			{
				printf("ERROR: Can't allocate breakpoint\n");
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
				printf("ERROR: Breakpoint does not exist\n");
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
			printf("\n");
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
			cpuInstruction = readWordAsString(regPC);
			disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
			printf("\tpc : %s\t\t", registerValue);
			displayWord(cpuInstruction, 1);
			printf("\t%s", disassembledInstruction);
			free(cpuInstruction);
			free(disassembledInstruction);
			free(registerValue);
			
			regPC = getRegister("npc");
			registerValue = displayRegister(regPC);
			cpuInstruction = readWordAsString(regPC);
			disassembledInstruction = decodeInstruction(cpuInstruction, regPC);
			printf("\n\tnpc: %s\t\t", registerValue);
			displayWord(cpuInstruction, 1);
			printf("\t%s", disassembledInstruction);
			free(cpuInstruction);
			free(disassembledInstruction);
			free(registerValue);
			
			printf("\n\n");
		}
		
		return RET_SUCCESS;
	}
	
    	
        // [f]loat
        if(!(strcmp(command, "float") && strcmp(command, "f")))
	{
		char* registerValue;
		char sparcRegister[4];
		unsigned short count;
                sparcRegister[0] = 'f';
		sparcRegister[3] = '\0';

		registerValue = displayRegister(getRegister("fsr"));
                printf("\n\tfsr: \t%s\n\n", registerValue);
                free(registerValue);
			
                for(count = 0; count < 32; count++)
                {
                        sparcRegister[1] = (count / 10) + '0';
                        sparcRegister[2] = (count % 10) + '0';
                        
                        // Hex to single precision floating-point conversion
                        convertFloat.floatToHex = getRegister(sparcRegister);
                        
                        // Hex to double precision floating-point conversion
                        if(count % 2 == 0)
                        {
                            convertDouble.doubleToHex[0] = convertFloat.floatToHex;
                            convertDouble.doubleToHex[1] = getRegister(getNextRegister(sparcRegister));
                        }
                        else
                            convertDouble.hexToDouble = 0;
                            
                        registerValue = displayRegister(convertFloat.floatToHex);
                        if(count % 2 == 0)
                            printf("\tf%02d:\t%s\t%13E\t%13lE\n", count, registerValue, convertFloat.hexToFloat, convertDouble.hexToDouble);
                        else
                            printf("\tf%02d:\t%s\t%13E\n", count, registerValue, convertFloat.hexToFloat);
                        free(registerValue);
                }
                printf("\n\n");
                
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
			cpuInstruction = readWordAsString(firstNumericParametre);
			disassembledInstruction = (char*)decodeInstruction(cpuInstruction, firstNumericParametre);
			printf("\n\t");
			sprintf(hexNumber, "%lx", firstNumericParametre);
			printf("0x%s:\t", hexNumber);
			displayWord(cpuInstruction, 1);
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
