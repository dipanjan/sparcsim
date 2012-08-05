#include <stdio.h>
#include <stdlib.h>
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
	int firstPageTableIndex = memoryAddress >> 22;
	int secondPageTableIndex = (memoryAddress << 10) >> 22;
	int counter;
	char** secondPageTable;

	if(firstPageTable[firstPageTableIndex] == NULL)
	{
		secondPageTable = (char**)malloc(sizeof(char*) * 1024);
		
		if(secondPageTable == NULL)
		{
			printf("Couldn't allocate Second Page Table\n");
			return -1;
		}
			
		for(counter = 0; counter < 1024; counter++)
			secondPageTable[counter++] = NULL;
			
		firstPageTable[firstPageTableIndex] = secondPageTable;
		if(secondPageTable[secondPageTableIndex] == NULL)
		{
			secondPageTable[secondPageTableIndex] = (char*)malloc(4 * 1024);
			if(secondPageTable[secondPageTableIndex] == NULL)
			{
				printf("Couldn't allocate Page\n");
				return -1;
			}
		}
		
		return 0;
	}

	//printf("Alloc: Second page table: %u\n", firstPageTable[firstPageTableIndex]);
	//printf("Alloc: Page: %u\n", secondPageTable[secondPageTableIndex]);
	//printf("First Page Table Index: %d\n", firstPageTableIndex);
	//printf("Second Page Table Index: %d\n", secondPageTableIndex);
}


void writeMemory(unsigned long memoryAddress, char byte)
{
	allocateMemory(memoryAddress);
	
	int firstPageTableIndex = memoryAddress >> 22;
	int secondPageTableIndex = (memoryAddress << 10) >> 22;
	int offset = (memoryAddress << 20) >> 20;
	//printf("Write: Offset within page: %d\n", offset);

	char** secondPageTable = firstPageTable[firstPageTableIndex]; //printf("Write: Second page table: %u\n", secondPageTable);
	char* page = secondPageTable[secondPageTableIndex]; //printf("Write: Page: %u\n", page);
	*(page + offset) = byte;
}


char readMemory(unsigned long memoryAddress)
{
	int firstPageTableIndex = memoryAddress >> 22;
	int secondPageTableIndex = (memoryAddress << 10) >> 22;
	int offset = (memoryAddress << 20) >> 20;

	char** secondPageTable = firstPageTable[firstPageTableIndex]; //printf("Read: Second page table: %u\n", secondPageTable);
	char* page = secondPageTable[secondPageTableIndex]; //printf("Read: Page: %u\n", page);
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
			printf("%02x", hexDigit);
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


void getMemoryArea(unsigned long memoryAddress, int count)
{
	int counter;
	char* quadWord;
	
	while(count > 0)
	{
		printf("\n\t %8lx:    ", memoryAddress);
		
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
	printf("\n\n");
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
