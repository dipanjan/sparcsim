#ifndef MEMORY_H
#define MEMORY_H

#include <stdio.h>
#include <stdlib.h>
#include <constants.h>



void initializeMemory();
int allocateMemory(unsigned long memoryAddress);
char readByte(unsigned long memoryAddress);
char* readWordAsString(unsigned long memoryAddress);
int writeByte(unsigned long memoryAddress, char byte);
int writeHalfWord(unsigned long memoryAddress, unsigned short halfWord);
int writeWord(unsigned long memoryAddress, unsigned long word);
void displayWord(char* cpuInstruction, int isInstruction);
void displayMemoryArea(unsigned long memoryAddress, int count);

#endif
