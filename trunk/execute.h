#include <stdio.h>
#include <string.h>
#include <register.h>
#include <constants.h>
#include <bits.h>



#define ICC_CARRY 				20
#define ICC_OVERFLOW			21
#define ICC_ZERO				22
#define ICC_NEGATIVE 			23



int executeInstruction(char* disassembledInstruction);
long getReg_Or_ImmValue(char* reg_or_imm);
void updateICC(long regRS1, long reg_or_imm, long regRD);
