#include <memory.h>



char** firstPageTable[1024];



/*
 * Initializes all the entries of first page table to NULL.
 * Simulator starts with no memory occupied, allocating
 * memory on demand basis thereafter. 
 */
void initializeMemory()
{
	int counter;

	for(counter = 0; counter < 1024; counter++)
		firstPageTable[counter++] = NULL;
}



/*
 * Allocates the page containing <memoryAddress>, if not
 * allocated yet. Also updates first and second level page tables.
 * 
 * It employs a two-level paging like scheme internally. 32 bit address word
 * is divided in three sections: 10 bits for first level paging table, 
 * 10 bits for second level paging table and the remaining 12 bits for offset 
 * within specified page. Page size is 2^12 bytes = 4KB. Memory is allocated in pages.
 * First level of paging table, which holds 2^10 = 1024 entries, is statically
 * allocated and all its entries are set to NULL. Once any word of unallocated
 * page is referred for the first time, the page is allocated. Any subsequent
 * write to that page causes no more page allocation. Prior to the allocation of
 * the page, it is checked whether the second level paging table corresponding
 * to that page exists. If not, then a paging table containing 2^10 = 1024
 * entries is allocated dynamically. Again, all its entries are set to NULL. Then
 * a page is created. The entry in the second level paging table
 * to it is set to point to the newly created page in memory. All the background
 * process is transparent to the process being simulated. All it realizes is to
 * read and write memory seamlessly.
 * 
 * The mechanism of reading is much less complex since no memory allocation
 * is associated with it. Once a memory word is referred, memory manager
 * checks whether entries are set in the first level and second level paging table
 * corresponding to it. In case it is found, memory manager adds the 12 bit
 * offset to the beginning of the page in memory to translate the virtual address
 * to its corresponding physical address. The content is returned. In response
 * to an attempt to read from a memory location previously unallocated, the
 * requesting module is handed over a zero.
 */
int allocateMemory(unsigned long memoryAddress)
{
	unsigned long firstPageTableIndex = memoryAddress >> 22;                // Extracting higher order 10 bits to serve as an index into first level page table.
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;       // Extracting middle order 10 bits to serve as an index into second level page table.
	unsigned long counter;
	char** secondPageTable;

        // Check whether first page table entry already exists.
	if(firstPageTable[firstPageTableIndex] == NULL)
	{
		secondPageTable = (char**)malloc(sizeof(char*) * 1024);         // Allocate memory for second level page table.

		if(secondPageTable == NULL)
			return SECOND_PAGE_TABLE_ALLOCATION_ERROR;

		for(counter = 0; counter < 1024; counter++)
			secondPageTable[counter++] = NULL;                      // Initialize all entries in second level page table to NULL as corresponding pages do not exist yet.

		firstPageTable[firstPageTableIndex] = secondPageTable;          // Update first level page table entry to point to corresponding second level page table.
	}
        
        // Second level page table corresponding to this page already exists.
	else
	{
		secondPageTable = firstPageTable[firstPageTableIndex];
	}

        // Check whether the page exists.
	if(secondPageTable[secondPageTableIndex] == NULL)
	{
		secondPageTable[secondPageTableIndex] = (char*)malloc(4 * 1024);        // Allocate the page and update second level page table.
		if(secondPageTable[secondPageTableIndex] == NULL)
			return PAGE_ALLOCATION_ERROR;
	}

	return RET_SUCCESS;
}



/*
 * Returns the byte located at <memoryAddress> in memory.
 */
char readByte(unsigned long memoryAddress)
{
	char** secondPageTable;
	char* page;
	unsigned long firstPageTableIndex = memoryAddress >> 22;                // Extracting higher order 10 bits to serve as an index into first level page table.
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;       // Extracting middle order 10 bits to serve as an index into second level page table.
	unsigned long offset = (memoryAddress << 20) >> 20;                     // Extracting lower order 12 bits to serve as offset into the page containing the byte.

        // Check whether a valid first level page table entry exists.
	if(firstPageTable[firstPageTableIndex] == NULL)
		return (char)0;
	else
		secondPageTable = firstPageTable[firstPageTableIndex];
        
        // Check whether a valid second level page table entry exists.
	if(secondPageTable[secondPageTableIndex] == NULL)
		return (char)0;
	else
		page = secondPageTable[secondPageTableIndex];
        
	return *(page + offset);
}



/*
 * Returns the word located at <memoryAddress> in memory.
 */
unsigned long readWord(unsigned long memoryAddress)
{        
        unsigned long word, hexDigit;
        
        /* Reads four bytes one by one starting from lowest to highest. Once a byte is read, it is left shifted
         * by 24 bits followed by right shifted by 24 bits to clear higher order 24 bits, if set by sign extension 
         * caused by widening of data during auto-casting. Casting takes place because of hexDigit being an
         * unsigned long (32 bits) while readByte() returns data of type char (8 bits). All four bytes are
         * packed together to form a 32 bit word.
         */
        hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = hexDigit;
        hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        hexDigit = readByte(memoryAddress); memoryAddress++; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
        
        return word;
}



/*
 * Writes the byte located at <memoryAddress> in memory with <byte>.
 */
int writeByte(unsigned long memoryAddress, char byte)
{        
        // Allocate memory for the page, if doesn't exist already.
        switch(allocateMemory(memoryAddress))
	{
	case SECOND_PAGE_TABLE_ALLOCATION_ERROR:
		return SECOND_PAGE_TABLE_ALLOCATION_ERROR;
	case PAGE_ALLOCATION_ERROR:
		return PAGE_ALLOCATION_ERROR;
	}

	unsigned long firstPageTableIndex = memoryAddress >> 22;                // Extracting higher order 10 bits to serve as an index into first level page table.
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;       // Extracting middle order 10 bits to serve as an index into second level page table.
	unsigned long offset = (memoryAddress << 20) >> 20;                     // Extracting lower order 12 bits to serve as offset into the page containing the byte.

	char** secondPageTable = firstPageTable[firstPageTableIndex];           // Get a pointer to second level page table.
	char* page = secondPageTable[secondPageTableIndex];                     // Get a pointer to the page containing the byte referred.
	*(page + offset) = byte;                                                // Write the byte.
        
	return RET_SUCCESS;
}



/*
 * Writes the Half Word located at <memoryAddress> in memory with <halfWord>.
 */
int writeHalfWord(unsigned long memoryAddress, unsigned short halfWord)
{
        // Allocate memory for the page, if doesn't exist already.
        switch(allocateMemory(memoryAddress))
	{
	case SECOND_PAGE_TABLE_ALLOCATION_ERROR:
		return SECOND_PAGE_TABLE_ALLOCATION_ERROR;
	case PAGE_ALLOCATION_ERROR:
		return PAGE_ALLOCATION_ERROR;
	}

	unsigned long firstPageTableIndex = memoryAddress >> 22;                // Extracting higher order 10 bits to serve as an index into first level page table.
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;       // Extracting middle order 10 bits to serve as an index into second level page table.
	unsigned long offset = (memoryAddress << 20) >> 20;                     // Extracting lower order 12 bits to serve as offset into the page containing the byte.
        char byte;

	char** secondPageTable = firstPageTable[firstPageTableIndex];           // Get a pointer to second level page table.
	char* page = secondPageTable[secondPageTableIndex];                     // Get a pointer to the page containing the half word referred.
        
        byte = (halfWord & 0xFF00) >> 8; *(page + offset) = byte; offset++;     // Write the first byte.
        byte = halfWord & 0x00FF; *(page + offset) = byte;                      // Write the second byte.
        
	return RET_SUCCESS;
}



/*
 * Writes the Word located at <memoryAddress> in memory with <word>.
 */
int writeWord(unsigned long memoryAddress, unsigned long word)
{
        switch(allocateMemory(memoryAddress))
	{
	case SECOND_PAGE_TABLE_ALLOCATION_ERROR:
		return SECOND_PAGE_TABLE_ALLOCATION_ERROR;
	case PAGE_ALLOCATION_ERROR:
		return PAGE_ALLOCATION_ERROR;
	}

	unsigned long firstPageTableIndex = memoryAddress >> 22;                // Extracting higher order 10 bits to serve as an index into first level page table.           
	unsigned long secondPageTableIndex = (memoryAddress << 10) >> 22;       // Extracting middle order 10 bits to serve as an index into second level page table.
	unsigned long offset = (memoryAddress << 20) >> 20;                     // Extracting lower order 12 bits to serve as offset into the page containing the byte.
        char byte;

	char** secondPageTable = firstPageTable[firstPageTableIndex];           // Get a pointer to second level page table.
	char* page = secondPageTable[secondPageTableIndex];                     // Get a pointer to the page containing the half word referred.
        
        byte = (word & 0xFF000000) >> 24; *(page + offset) = byte; offset++;    // Write the first byte.
        byte = (word & 0x00FF0000) >> 16; *(page + offset) = byte; offset++;    // Write the second byte.
        byte = (word & 0x0000FF00) >> 8; *(page + offset) = byte; offset++;     // Write the third byte.
        byte = word & 0x000000FF; *(page + offset) = byte;                      // Write the fourth byte.
        
	return RET_SUCCESS;
}



/*
 * Returns the word at <memoryAddress> as string.
 */
char* readWordAsString(unsigned long memoryAddress)
{
	char* cpuInstruction = (char*)malloc(4);
	cpuInstruction[0] = readByte(memoryAddress++);          // Read the first byte.
	cpuInstruction[1] = readByte(memoryAddress++);          // Read the second byte.
	cpuInstruction[2] = readByte(memoryAddress++);          // Read the third byte.
	cpuInstruction[3] = readByte(memoryAddress);            // Read the fourth byte.
	return cpuInstruction;
}



/*
 * Displays the word sized string <cpuInstruction> in HEX,
 * either as instruction (if isInstruction = 1),
 * or as memory dump (isInstruction = 0).
 */
void displayWord(char* cpuInstruction, int isInstruction)
{
	if(cpuInstruction != NULL)
	{
		int count; unsigned int hexDigit; 
		for(count = 0; count <= 3; count++)
		{
			char instructionByte = cpuInstruction[count];
			hexDigit = instructionByte;
                        
                       /* hexDigit is left shifted by 24 bits followed by right shifted by 24 bits 
                        * to clear higher order 24 bits, if set by sign extension caused by widening 
                        * of data during auto-casting. Casting takes place because of hexDigit being an
                        * unsigned long (32 bits) while cpuInstruction is an array of type char (8 bits).
                        */
			hexDigit = (hexDigit << 24) >> 24;
			printf("%02X", hexDigit);
			if(isInstruction)
				printf(" ");
		}
	}
}



/*
 * Displays <count> number of bytes starting from <memoryAddress>
 * in HEX. Also displays the ASCII interpretation side by side.
 */
void displayMemoryArea(unsigned long memoryAddress, int count)
{
	int counter;
	char* quadWord;

	while(count > 0)
	{
		printf("\n\t %8lX:    ", memoryAddress);

                // Display raw bytes in HEX.
		for(counter = 0; counter < 4; counter++)
		{
			quadWord = readWordAsString(memoryAddress + 4 * counter);
			displayWord(quadWord, 0);
			free(quadWord);
			count -= 4;
			printf(" ");
		}

		printf("    ");

                // Display the ASCII interpretation of the bytes read.
		for(counter = 0; counter < 16; counter++)
		{
			char byte = readByte(memoryAddress + counter);
                        
                        // Is it a printable character?
			if((byte >= 33) && (byte <= 126))
				printf("%c", byte);
                        
                        // Otherwise display a dot (.) as placeholder character.
			else
				printf(".");
		}
		memoryAddress += 16;
	}
}



/*
 * Aligns a <memoryAddress> to its nearest lower word-sized boundary.
 * e.g. If memoryAddress = 15, return value is 12.
 */
unsigned long wordAlign(unsigned long memoryAddress)
{
    return ((unsigned long)(memoryAddress / 4)) * 4;
}
