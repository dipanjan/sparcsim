#ifndef BREAKPOINTS_H
#define BREAKPOINTS_H

#include <stdio.h>
#include <stdlib.h>
#include <constants.h>



struct breakPoint
{
	short breakPointSerial;
        unsigned long memoryAddress;
        unsigned short breakPointType; // 1 = Breakpoint, 2 = Watchpoint
	struct breakPoint* nextBreakPoint;
};



void initializeBreakPointList();
int addBreakPoint(unsigned long memoryAddress, unsigned short breakPointType);
int deleteBreakPoint(unsigned short index);
struct breakPoint* getBreakPoint(unsigned short isReset);
int isBreakPoint(unsigned long regPC);

#endif
