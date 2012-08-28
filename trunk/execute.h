#include <stdio.h>
#include <string.h>
#include <register.h>
#include <memory.h>
#include <constants.h>
#include <limits.h>
#include <bits.h>



#define ICC_CARRY 				20
#define ICC_OVERFLOW			21
#define ICC_ZERO				22
#define ICC_NEGATIVE 			23
#define SIGN_BIT				31



int executeInstruction(char* disassembledInstruction);
unsigned long getAddressValue(char tokens[][20], unsigned short* index);
unsigned long getReg_Or_ImmValue(char* reg_or_imm);
void updateICC(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD);
