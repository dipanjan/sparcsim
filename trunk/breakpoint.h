#ifndef BREAKPOINTS_H
#define BREAKPOINTS_H

#include <stdio.h>
#include <stdlib.h>
#include <constants.h>



#define BREAK_POINT 1
#define WATCH_POINT 2



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
short getBreakPointSerial();
int isWatchPoint(unsigned long memoryAddress, unsigned long regPC);
short getWatchPointSerial();

#endif
