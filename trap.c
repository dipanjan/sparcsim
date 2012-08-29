#include <trap.h>



void handleTrap(short trapCode)
{
	switch(trapCode)
	{
		case ILLEGAL_INSTRUCTION: printf("\nTrap: illegal_instruction\n"); return;
		case WINDOWS_OVERFLOW: printf("\nTrap: window_overflow\n"); return;
		case WINDOWS_UNDERFLOW: printf("\nTrap: window_underflow\n"); return;
	}
}
