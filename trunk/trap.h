#ifndef TRAP_H
#define TRAP_H

#include <stdio.h>
#include <memory.h>
#include <register.h>



#define ILLEGAL_INSTRUCTION		  0x02
#define WINDOWS_OVERFLOW		  0x05
#define WINDOWS_UNDERFLOW		  0x06
#define MEM_ADDRESS_NOT_ALIGNED		  0x07

#define HALFWORD_ALIGN                    2
#define WORD_ALIGN                        4
#define DOUBLEWORD_ALIGN                  8



void handleTrap(short trapCode, unsigned long regPC);
int is_mem_address_not_aligned(unsigned long memoryAddress, int alignment);

#endif
