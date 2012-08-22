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
int addBreakPoint(unsigned long memoryAddress, short status);
int deleteBreakPoint(short index);
struct breakPoint* showBreakPoint(short initializationFactor, short status);
