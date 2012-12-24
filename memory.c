#include <memory.h>



char** firstPageTable[1024];



void initializeMemory()
{
	int counter;

	for(counter = 0; counter < 1024; counter++)
		firstPageTable[counter++] = NULL;
}



int allocateMemory(unsigned long memoryAddress)
{
	unsigned long firstPageTableIndex = memoryAddress >> 22;
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;
	unsigned long counter;
	char** secondPageTable;

	if(firstPageTable[firstPageTableIndex] == NULL)
	{
		secondPageTable = (char**)malloc(sizeof(char*) * 1024);

		if(secondPageTable == NULL)
			return SECOND_PAGE_TABLE_ALLOCATION_ERROR;

		for(counter = 0; counter < 1024; counter++)
			secondPageTable[counter++] = NULL;

		firstPageTable[firstPageTableIndex] = secondPageTable;
	}
	else
	{
		secondPageTable = firstPageTable[firstPageTableIndex];
	}

	if(secondPageTable[secondPageTableIndex] == NULL)
	{
		secondPageTable[secondPageTableIndex] = (char*)malloc(4 * 1024);
		if(secondPageTable[secondPageTableIndex] == NULL)
			return PAGE_ALLOCATION_ERROR;
	}

	return RET_SUCCESS;
}



char readByte(unsigned long memoryAddress)
{
	char** secondPageTable;
	char* page;
	unsigned long firstPageTableIndex = memoryAddress >> 22;
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;
	unsigned long offset = (memoryAddress << 20) >> 20;

	if(firstPageTable[firstPageTableIndex] == NULL)
		return (char)0;
	else
		secondPageTable = firstPageTable[firstPageTableIndex];
	if(secondPageTable[secondPageTableIndex] == NULL)
		return (char)0;
	else
		page = secondPageTable[secondPageTableIndex];
        
	return *(page + offset);
}



unsigned long readWord(unsigned long memoryAddress)
{        
        unsigned long word, hexDigit;
        
        hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        
        return word;
}



int writeByte(unsigned long memoryAddress, char byte)
{
	switch(allocateMemory(memoryAddress))
	{
	case SECOND_PAGE_TABLE_ALLOCATION_ERROR:
		return SECOND_PAGE_TABLE_ALLOCATION_ERROR;
	case PAGE_ALLOCATION_ERROR:
		return PAGE_ALLOCATION_ERROR;
	}

	unsigned long firstPageTableIndex = memoryAddress >> 22;
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;
	unsigned long offset = (memoryAddress << 20) >> 20;

	char** secondPageTable = firstPageTable[firstPageTableIndex];
	char* page = secondPageTable[secondPageTableIndex];
	*(page + offset) = byte;
        
	return RET_SUCCESS;
}



int writeHalfWord(unsigned long memoryAddress, unsigned short halfWord)
{
	switch(allocateMemory(memoryAddress))
	{
	case SECOND_PAGE_TABLE_ALLOCATION_ERROR:
		return SECOND_PAGE_TABLE_ALLOCATION_ERROR;
	case PAGE_ALLOCATION_ERROR:
		return PAGE_ALLOCATION_ERROR;
	}

	unsigned long firstPageTableIndex = memoryAddress >> 22;
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;
	unsigned long offset = (memoryAddress << 20) >> 20;
        char byte;

	char** secondPageTable = firstPageTable[firstPageTableIndex];
	char* page = secondPageTable[secondPageTableIndex];
        
        byte = (halfWord & 0xFF00) >> 8; *(page + offset) = byte; offset++;
        byte = halfWord & 0x00FF; *(page + offset) = byte; 
        
	return RET_SUCCESS;
}



int writeWord(unsigned long memoryAddress, unsigned long word)
{
	switch(allocateMemory(memoryAddress))
	{
	case SECOND_PAGE_TABLE_ALLOCATION_ERROR:
		return SECOND_PAGE_TABLE_ALLOCATION_ERROR;
	case PAGE_ALLOCATION_ERROR:
		return PAGE_ALLOCATION_ERROR;
	}

	unsigned long firstPageTableIndex = memoryAddress >> 22;
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;
	unsigned long offset = (memoryAddress << 20) >> 20;
        char byte;

	char** secondPageTable = firstPageTable[firstPageTableIndex];
	char* page = secondPageTable[secondPageTableIndex];
        
        byte = (word & 0xFF000000) >> 24; *(page + offset) = byte; offset++;
        byte = (word & 0x00FF0000) >> 16; *(page + offset) = byte; offset++;
        byte = (word & 0x0000FF00) >> 8; *(page + offset) = byte; offset++;
        byte = word & 0x000000FF; *(page + offset) = byte;
        
	return RET_SUCCESS;
}



char* readWordAsString(unsigned long memoryAddress)
{
	char* cpuInstruction = (char*)malloc(4);
	cpuInstruction[0] = readByte(memoryAddress++);
	cpuInstruction[1] = readByte(memoryAddress++);
	cpuInstruction[2] = readByte(memoryAddress++);
	cpuInstruction[3] = readByte(memoryAddress);
	return cpuInstruction;
}



void displayWord(char* cpuInstruction, int isInstruction)
{
	if(cpuInstruction != NULL)
	{
		int count; unsigned int hexDigit; 
		for(count = 0; count <= 3; count++)
		{
			char instructionByte = cpuInstruction[count];
			hexDigit = instructionByte;
			hexDigit = (hexDigit << 24) >> 24;
			printf("%02X", hexDigit);
			if(isInstruction)
				printf(" ");
		}
	}
}



void displayMemoryArea(unsigned long memoryAddress, int count)
{
	int counter;
	char* quadWord;

	while(count > 0)
	{
		printf("\n\t %8lX:    ", memoryAddress);

		for(counter = 0; counter < 4; counter++)
		{
			quadWord = readWordAsString(memoryAddress + 4 * counter);
			displayWord(quadWord, 0);
			free(quadWord);
			count -= 4;
			printf(" ");
		}

		printf("    ");

		for(counter = 0; counter < 16; counter++)
		{
			char byte = readByte(memoryAddress + counter);
			if((byte >= 33) && (byte <= 126))
				printf("%c", byte);
			else
				printf(".");
		}
		memoryAddress += 16;
	}
}



/*void main()
{
	initializeMemory();

	writeByte(1024*4*1024-1, 'A');
	printf("Character read: %c\n",readByte(1024*4*1024-1));

	writeByte(1024*4*1024-2, 'B');
	printf("Character read: %c\n",readByte(1024*4*1024-1));
	printf("Character read: %c\n",readByte(1024*4*1024-2));

	printf("Character read: %c\n",readByte(1024*4*1024-3)); // Should be NULL

	//printf("Character read: %c\n",readByte(1023*4*1024-3)); Should produce segmentation fault

	getMemoryArea(1024*4*1024-16, 16);
}*/
