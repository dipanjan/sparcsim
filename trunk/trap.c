#include <trap.h>



void handleTrap(short trapCode, unsigned long regPC)
{
        unsigned long tbr, tbr_tt;
        
        tbr = getRegister("tbr");
        
	switch(trapCode)
	{
                case ILLEGAL_INSTRUCTION: printf("\tTrap: illegal_instruction, PC: 0x%08lX\n", regPC); tbr_tt = ILLEGAL_INSTRUCTION; break;
		case WINDOWS_OVERFLOW: printf("\tTrap: window_overflow, PC: 0x%08lX\n", regPC); tbr_tt = WINDOWS_OVERFLOW; break;
		case WINDOWS_UNDERFLOW: printf("\tTrap: window_underflow, PC: 0x%08lX\n", regPC); tbr_tt = WINDOWS_UNDERFLOW; break;
                case MEM_ADDRESS_NOT_ALIGNED: printf("\tTrap: mem_address_not_aligned, PC: 0x%08lX\n", regPC); tbr_tt = MEM_ADDRESS_NOT_ALIGNED; break;
	}
        
        tbr = tbr | (tbr_tt << 4);
        setRegister("tbr", tbr);
}



int is_mem_address_not_aligned(unsigned long memoryAddress, int alignment)
{
    switch(alignment)
    {
        case 2: if((((unsigned long)(memoryAddress / 2)) * 2) == memoryAddress) return 0; else return 1;
        case 4: if((((unsigned long)(memoryAddress / 4)) * 4) == memoryAddress) return 0; else return 1;
        case 8: if((((unsigned long)(memoryAddress / 8)) * 8) == memoryAddress) return 0; else return 1;
    }
    return 0;
}