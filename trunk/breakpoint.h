#include <stdio.h>
#include <stdlib.h>
#include <constants.h>



struct breakPoint
{
	unsigned long memoryAddress;
	short status;
	struct breakPoint* nextBreakPoint;
};



void initializeBreakPointList();
int addBreakPoint(unsigned long memoryAddress);
int deleteBreakPoint(unsigned short index);
unsigned long getBreakPoint(unsigned short* isReset);
unsigned long getNextBreakPoint(unsigned long regPC, unsigned short* isReset);
