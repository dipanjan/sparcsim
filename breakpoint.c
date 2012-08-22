#include <breakpoint.h>



struct breakPoint* breakPointList = NULL;



void initializeBreakPointList()
{
	if(breakPointList != NULL)
		while(deleteBreakPoint(1) != -1);		
}



int addBreakPoint(unsigned long memoryAddress, short status)
{
	struct breakPoint* curBreakPoint, *prevBreakPoint;
	
	curBreakPoint = breakPointList;
	prevBreakPoint = breakPointList;
	
	while(curBreakPoint != NULL && (curBreakPoint->memoryAddress < memoryAddress))
	{
		prevBreakPoint = curBreakPoint;
		curBreakPoint = curBreakPoint->nextBreakPoint;
	}
	
	if(curBreakPoint->memoryAddress == memoryAddress)
		return RET_SUCCESS;
	prevBreakPoint->nextBreakPoint = (struct breakPoint*)malloc(sizeof(struct breakPoint));
	if(!prevBreakPoint->nextBreakPoint)
		return BREAKPOINT_ALLOCATION_ERROR;
	
	curBreakPoint = prevBreakPoint->nextBreakPoint;
	curBreakPoint->memoryAddress = memoryAddress;
	curBreakPoint->status = status;
	curBreakPoint->nextBreakPoint = NULL;
	
	return RET_SUCCESS;
}



int deleteBreakPoint(short index)
{
	return RET_SUCCESS;
}



int main()
{
	addBreakPoint(0x40000000, 1);
	while(breakPointList != NULL)
	{
		printf("%lx\n", breakPointList->memoryAddress);
		breakPointList = breakPointList->nextBreakPoint;
	}

	return RET_SUCCESS;
}