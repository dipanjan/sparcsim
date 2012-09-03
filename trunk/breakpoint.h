#ifndef BREAKPOINTS_H
#define BREAKPOINTS_H

#include <stdio.h>
#include <stdlib.h>
#include <constants.h>



struct breakPoint
{
	unsigned long memoryAddress;
	short breakPointSerial;
	struct breakPoint* nextBreakPoint;
};



void initializeBreakPointList();
int addBreakPoint(unsigned long memoryAddress);
int deleteBreakPoint(unsigned short index);
struct breakPoint* getBreakPoint(unsigned short isReset);
int isBreakPoint(unsigned long regPC);

#endif
