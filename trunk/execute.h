#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdio.h>
#include <math.h>
#include <string.h>
#include <register.h>
#include <memory.h>
#include <constants.h>
#include <trap.h>
#include <limits.h>
#include <bits.h>
#include <breakpoint.h>
#include <decode.h>



#define ICC_CARRY 				20
#define ICC_OVERFLOW                            21
#define ICC_ZERO				22
#define ICC_NEGATIVE                            23
#define SIGN_BIT				31
#define FLOAT_EQUAL                              0
#define FLOAT_LESSER                             1
#define FLOAT_GREATER                            2
#define FLOAT_UNORDERED                          3



union 
{
        unsigned long floatToHex;
        float hexToFloat;
}convertFloat;



union 
{
        unsigned long doubleToHex[2];
        double hexToDouble;
}convertDouble;

       

struct watchPointInfo
{
        unsigned long memoryAddress;
        unsigned long newData;
};



struct 
{
        unsigned long regPC;
        char disassembledInstruction[50];
        char cpuInstruction[4];
}lastInstructionInfo;



int executeInstruction(char* disassembledInstruction);
unsigned long getAddressValue(char tokens[][20], unsigned short* index);
unsigned long getReg_Or_ImmValue(char* reg_or_imm);
void updateICCAdd(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD);
void updateICCSubtract(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD);
void updateICCMulLogical(unsigned long regRD);
void updateICCDiv(unsigned long regRD, short isOverflow);
void updateFCC(unsigned short fcc);
int executeNextInstruction();
unsigned short taggedAddSubtract(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD, unsigned short isTVOpcode);
void setWatchPointInfo(unsigned long memoryAddress, unsigned long newData);
struct watchPointInfo* getWatchPointInfo();

#endif
