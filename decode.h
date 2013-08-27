#ifndef DECODE_H
#define DECODE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>



struct instruction_fields
{
    unsigned short op, op2, op3, opf;
} sparc_instruction_fields;



char* decodeInstruction(char* cpuInstruction, unsigned long regPC);
char* getIntegerRegisterName(unsigned long registerIdentifier);
char* getFloatingRegisterName(unsigned long registerIdentifier);
char* getCoProcessorRegisterName(unsigned long registerIdentifier);
char* getAddress(unsigned long rs1, unsigned long rs2, unsigned long i, unsigned long simm13, int registerTypeIdentifier);
char* getReg_Or_Imm(unsigned long rs2, unsigned long i, unsigned long simm13, int registerTypeIdentifier);

#endif
