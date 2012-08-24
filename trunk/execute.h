#include <stdio.h>
#include <string.h>
#include <register.h>
#include <constants.h>
#include <bits.h>



#define CARRY_BIT 				20
#define OVERFLOW_BIT			21
#define ZERO_BIT 				22
#define NEGATIVE_BIT 			23



int executeInstruction(char* disassembledInstruction);
