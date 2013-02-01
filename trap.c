#include <trap.h>



void handleTrap(short trapCode, unsigned long regPC)
{
	printf("\t\t");
	switch(trapCode)
	{
		case ILLEGAL_INSTRUCTION: printf("%08lX:   Trap: illegal_instruction\n", regPC); return;
		case WINDOWS_OVERFLOW: printf("%08lX:   Trap: window_overflow\n", regPC); return;
		case WINDOWS_UNDERFLOW: printf("%08lX:   Trap: window_underflow\n", regPC); return;
	}
}



int is_mem_address_not_aligned(unsigned long memoryAddress, int alignment)
{
    switch(alignment)
    {
        case 2: if((((unsigned long)(memoryAddress / 2)) * 2) == memoryAddress) return 0; else return 1;
        case 4: if((((unsigned long)(memoryAddress / 4)) * 4) == memoryAddress) return 0; else return 1;
        case 8: if((((unsigned long)(memoryAddress / 8)) * 8) == memoryAddress) return 0; else return 1;
    }
}