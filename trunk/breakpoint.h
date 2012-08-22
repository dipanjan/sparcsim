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
unsigned long showBreakPoint(unsigned short* isReset);
