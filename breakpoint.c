#include <breakpoint.h>



struct breakPoint* breakPointList = NULL;



void initializeBreakPointList()
{
	if(breakPointList != NULL)
		while(deleteBreakPoint(1) == RET_SUCCESS);
}



int addBreakPoint(unsigned long memoryAddress)
{
	struct breakPoint* prevBreakPoint, *curBreakPoint, *nextBreakPoint, *newBreakPoint;

	newBreakPoint = (struct breakPoint*)malloc(sizeof(struct breakPoint));;
	
	// Allocate one node if the list is empty
	if(breakPointList == NULL)
	{
		breakPointList = newBreakPoint;
		curBreakPoint = breakPointList;
		curBreakPoint->nextBreakPoint = NULL;
	}
	else
	{
		curBreakPoint = breakPointList;
		prevBreakPoint = breakPointList;

		while(curBreakPoint != NULL && (curBreakPoint->memoryAddress <= memoryAddress))
			// Duplicate breakpoint
			if(curBreakPoint->memoryAddress == memoryAddress)
				return RET_SUCCESS;
			else
			{
				prevBreakPoint = curBreakPoint;
				curBreakPoint = curBreakPoint->nextBreakPoint;
			}
	
		if(curBreakPoint == NULL)
		{
			// We have arrived at the end of the list
			prevBreakPoint->nextBreakPoint = newBreakPoint;
			curBreakPoint = prevBreakPoint->nextBreakPoint;
			curBreakPoint->nextBreakPoint = NULL;
		}
		else
			if(breakPointList == curBreakPoint)
			{
				// We are at the beginning of the list
				newBreakPoint->nextBreakPoint = breakPointList;
				breakPointList = newBreakPoint;
				curBreakPoint= breakPointList;
			}
			else
			{
				// We are at the middle of the list
				newBreakPoint->nextBreakPoint = curBreakPoint;
				prevBreakPoint->nextBreakPoint = newBreakPoint;
				curBreakPoint = newBreakPoint;
			}
	}

	curBreakPoint->memoryAddress = memoryAddress;

	return RET_SUCCESS;
}



int deleteBreakPoint(unsigned short index)
{
	struct breakPoint* prevBreakPoint, *curBreakPoint, *nextBreakPoint;

	if((index == 1) && (breakPointList != NULL))
	{
		// Delete first breakpoint
		nextBreakPoint = breakPointList->nextBreakPoint;
		free(breakPointList);
		breakPointList = nextBreakPoint;
		return RET_SUCCESS;
	}

	else
	{
		// Delete non-first breakpoint
		unsigned short position;

		curBreakPoint = breakPointList;
		for(position = 1; position < index; position++)
			if(curBreakPoint->nextBreakPoint == NULL)
				return RET_FAILURE;
			else
			{
				prevBreakPoint = curBreakPoint;
				curBreakPoint = curBreakPoint->nextBreakPoint;
			}

		nextBreakPoint = curBreakPoint->nextBreakPoint;
		free(curBreakPoint);
		prevBreakPoint->nextBreakPoint = nextBreakPoint;
		return RET_SUCCESS;
	}
	return RET_FAILURE;
}



unsigned long showBreakPoint(unsigned short* isReset)
{
	static struct breakPoint* curBreakPoint;
	struct breakPoint* prevBreakPoint;
	static unsigned short isInitialized = 0;

	if(isInitialized == 0)
	{
		curBreakPoint = breakPointList;
		isInitialized = 1;
	}

	if(curBreakPoint != NULL)
	{
		prevBreakPoint = curBreakPoint;
		curBreakPoint = curBreakPoint->nextBreakPoint;
		*isReset = 0;
		return prevBreakPoint->memoryAddress;
	}
	else
	{
		isInitialized = 0;
		*isReset = 1;
		return RET_SUCCESS;
	}
}



/*int main()
{
	struct breakPoint* curBreakPoint;
	unsigned short isReset;
	unsigned long breakpointAddress;

	addBreakPoint(0x40000000);
	addBreakPoint(0x80000000);
	addBreakPoint(0x70000000);
	addBreakPoint(0x20000000);
	addBreakPoint(0x60000000);
	addBreakPoint(0x70000000);

	while(1)
	{
		breakpointAddress = showBreakPoint(&isReset);
		if(isReset == 0)
			printf("0x%lx\n", breakpointAddress);
		else
			break;
	}

	printf("\n----------------\n");
	deleteBreakPoint(1);
	deleteBreakPoint(3);
	deleteBreakPoint(3);
	deleteBreakPoint(4);

	while(1)
	{
		breakpointAddress = showBreakPoint(&isReset);
		if(isReset == 0)
			printf("0x%lx\n", breakpointAddress);
		else
			break;
	}

	return RET_SUCCESS;
}*/
