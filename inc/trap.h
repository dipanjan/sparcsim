#ifndef TRAP_H
#define TRAP_H

#include <stdio.h>
#include <fenv.h>
#include <memory.h>
#include <misc.h>
#include <register.h>



#define ILLEGAL_INSTRUCTION		  0x02
#define PRIVILEGED_INSTRUCTION		  0x03
#define FP_DISABLED                       0x04
#define WINDOW_OVERFLOW                   0x05
#define WINDOW_UNDERFLOW		  0x06
#define MEM_ADDRESS_NOT_ALIGNED		  0x07
#define FP_EXCEPTION    		  0x08
#define TAG_OVERFLOW                      0x0A
#define UNIMPLEMENTED_FLUSH		  0x25
#define DIVISION_BY_ZERO		  0x2A
#define DESIGN_UNIMP    		  0x60

#define IEEE_754_EXCEPTION    		  1
#define UNFINISHED_FPOP    		  2
#define UNIMPLEMENTED_FPOP    		  3
#define INVALID_FP_REGISTER    		  6

#define NXC                               0
#define DZC                               1
#define UFC                               2
#define OFC                               3
#define NVC                               4

#define NXA                               5
#define DZA                               6
#define UFA                               7
#define OFA                               8
#define NVA                               9

#define NXM                              23
#define DZM                              24
#define UFM                              25
#define OFM                              26
#define NVM                              27

#define HALFWORD_ALIGN                    2
#define WORD_ALIGN                        4
#define DOUBLEWORD_ALIGN                  8



int setTrapCode(short trapCode, char* reason);
int setFTTTrapCode(short fpTrapCode, short trapCode, char* reason);
int is_ieee_754_trap();
void raise_invalid_exception(char* reason);
int is_mem_address_not_aligned(unsigned long memoryAddress, int alignment);
int is_register_mis_aligned(char* sparcRegister);
void setIUErrorMode(short errorMode);
short getIUErrorMode();

#endif
