#ifndef TRAP_H
#define TRAP_H

#include <stdio.h>
#include <memory.h>



#define ILLEGAL_INSTRUCTION		  1
#define WINDOWS_OVERFLOW		  2
#define WINDOWS_UNDERFLOW		  3



//void handleTrap(short trapCode, unsigned long regPC);
int is_mem_address_not_aligned(unsigned long memoryAddress, int alignment);

#endif
