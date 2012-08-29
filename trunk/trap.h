#include <stdio.h>



#define ILLEGAL_INSTRUCTION		  1
#define WINDOWS_OVERFLOW		  2
#define WINDOWS_UNDERFLOW		  3



void handleTrap(short trapCode);
