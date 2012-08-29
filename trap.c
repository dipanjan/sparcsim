#include <trap.h>



void handleTrap(short trapCode, unsigned long regPC)
{
	printf("\t\t");
	switch(trapCode)
	{
		case ILLEGAL_INSTRUCTION: printf("\n%08lX:   Trap: illegal_instruction\n", regPC); return;
		case WINDOWS_OVERFLOW: printf("\n%08lX:   Trap: window_overflow\n", regPC); return;
		case WINDOWS_UNDERFLOW: printf("\n%08lX:   Trap: window_underflow\n", regPC); return;
	}
}
