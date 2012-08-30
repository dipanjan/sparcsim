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
		// printf("\n\nAllocateMemory: Allocating second page table");

		if(secondPageTable == NULL)
			return SECOND_PAGE_TABLE_ALLOCATION_ERROR;

		for(counter = 0; counter < 1024; counter++)
			secondPageTable[counter++] = NULL;

		firstPageTable[firstPageTableIndex] = secondPageTable;
	}
	else
	{
		secondPageTable = firstPageTable[firstPageTableIndex];
		// printf("\n\nAllocateMemory: Second page table not allocated");
	}

	if(secondPageTable[secondPageTableIndex] == NULL)
	{
		// printf("\nAllocateMemory: Allocating required page");
		secondPageTable[secondPageTableIndex] = (char*)malloc(4 * 1024);
		if(secondPageTable[secondPageTableIndex] == NULL)
			return PAGE_ALLOCATION_ERROR;
	}
	/*else
		printf("\nAllocateMemory: Page not allocated");*/

	return RET_SUCCESS;
	//printf("Alloc: Second page table: %u\n", firstPageTable[firstPageTableIndex]);
	//printf("Alloc: Page: %u\n", secondPageTable[secondPageTableIndex]);
	//printf("First Page Table Index: %d\n", firstPageTableIndex);
	//printf("Second Page Table Index: %d\n", secondPageTableIndex);
}



int writeMemory(unsigned long memoryAddress, char byte)
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
	// printf("\nWriteMemory: FirstPageTableIndex: %lX\nSecondPageTableIndex: %lX\nOffset: %lX", firstPageTableIndex, secondPageTableIndex, offset);
	// printf("\nSecond page table: %lX\nPage: %lX\n", (unsigned long)secondPageTable,  (unsigned long)page);
	*(page + offset) = byte;
	return RET_SUCCESS;
}



char readMemory(unsigned long memoryAddress)
{
	char** secondPageTable;
	char* page;
	unsigned long firstPageTableIndex = memoryAddress >> 22;
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;
	unsigned long offset = (memoryAddress << 20) >> 20;

	if(firstPageTable[firstPageTableIndex] == NULL)
		return (char)0;
	else
		secondPageTable = firstPageTable[firstPageTableIndex]; //printf("Read: Second page table: %u\n", secondPageTable);
	if(secondPageTable[secondPageTableIndex] == NULL)
		return (char)0;
	else
		page = secondPageTable[secondPageTableIndex]; //printf("Read: Page: %u\n", page);
	return *(page + offset);
}



void displayQuadWord(char* cpuInstruction, int isInstruction)
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



char* getQuadWordFromMemory(unsigned long memoryAddress)
{
	char* cpuInstruction = (char*)malloc(4);
	cpuInstruction[0] = readMemory(memoryAddress++);
	cpuInstruction[1] = readMemory(memoryAddress++);
	cpuInstruction[2] = readMemory(memoryAddress++);
	cpuInstruction[3] = readMemory(memoryAddress);
	return cpuInstruction;
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
			quadWord = getQuadWordFromMemory(memoryAddress + 4 * counter);
			displayQuadWord(quadWord, 0);
			free(quadWord);
			count -= 4;
			printf(" ");
		}

		printf("    ");

		for(counter = 0; counter < 16; counter++)
		{
			char byte = readMemory(memoryAddress + counter);
			if(((byte >= 65) && (byte <= 90)) || ((byte >= 97) && (byte <= 122)))
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

	writeMemory(1024*4*1024-1, 'A');
	printf("Character read: %c\n",readMemory(1024*4*1024-1));

	writeMemory(1024*4*1024-2, 'B');
	printf("Character read: %c\n",readMemory(1024*4*1024-1));
	printf("Character read: %c\n",readMemory(1024*4*1024-2));

	printf("Character read: %c\n",readMemory(1024*4*1024-3)); // Should be NULL

	//printf("Character read: %c\n",readMemory(1023*4*1024-3)); Should produce segmentation fault

	getMemoryArea(1024*4*1024-16, 16);
}*/
