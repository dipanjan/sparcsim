#include <breakpoint.h>



struct breakPoint* breakPointList = NULL;
short breakPointSerial = 0;


void initializeBreakPointList()
{
	if(breakPointList != NULL)
		while(deleteBreakPoint(1) == RET_SUCCESS);
}



int addBreakPoint(unsigned long memoryAddress)
{
	struct breakPoint* prevBreakPoint, *curBreakPoint, *nextBreakPoint, *newBreakPoint;

	newBreakPoint = (struct breakPoint*)malloc(sizeof(struct breakPoint));
	if(!newBreakPoint)
		return BREAKPOINT_ALLOCATION_ERROR;
	
	// Allocate one node if the list is empty
	if(!breakPointList)
	{
		breakPointList = newBreakPoint;
		curBreakPoint = breakPointList;
	}
	else
	{
		curBreakPoint = breakPointList;
		prevBreakPoint = breakPointList;

		while(curBreakPoint && (prevBreakPoint->memoryAddress != memoryAddress))
		{
			prevBreakPoint = curBreakPoint;
			curBreakPoint = curBreakPoint->nextBreakPoint;
		}
	
		if(prevBreakPoint->memoryAddress == memoryAddress)
			return RET_SUCCESS;
		else
		{
			// We are at the end of the list
			prevBreakPoint->nextBreakPoint = newBreakPoint;
			curBreakPoint = prevBreakPoint->nextBreakPoint;
		}
	}

	breakPointSerial++;
	curBreakPoint->memoryAddress = memoryAddress;
	curBreakPoint->breakPointSerial = breakPointSerial;
	curBreakPoint->nextBreakPoint = NULL;

	return RET_SUCCESS;
}



int deleteBreakPoint(unsigned short index)
{
	struct breakPoint* prevBreakPoint, *curBreakPoint, *nextBreakPoint;

	if(breakPointList)
	{
		if(breakPointList->breakPointSerial == index)
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
			while((curBreakPoint->breakPointSerial != index) && curBreakPoint->nextBreakPoint)
			{
				prevBreakPoint = curBreakPoint;
				curBreakPoint = curBreakPoint->nextBreakPoint;
			}

			if(curBreakPoint->breakPointSerial ==index)
			{
				nextBreakPoint = curBreakPoint->nextBreakPoint;
				free(curBreakPoint);
				prevBreakPoint->nextBreakPoint = nextBreakPoint;
				return RET_SUCCESS;
			}
		}
	}

	return RET_FAILURE;
}



struct breakPoint* getBreakPoint(unsigned short isReset)
{
	static struct breakPoint* curBreakPoint;
	struct breakPoint* prevBreakPoint;
	static unsigned short isInitialized = 0;

	if(isInitialized == 0 || isReset == 1)
	{
		curBreakPoint = breakPointList;
		isInitialized = 1;
	}

	if(curBreakPoint != NULL)
	{
		prevBreakPoint = curBreakPoint;
		curBreakPoint = curBreakPoint->nextBreakPoint;
		return prevBreakPoint;
	}
	else
	{
		isInitialized = 0;
		return NULL;
	}
}


int isBreakPoint(unsigned long regPC)
{
	struct breakPoint* prevBreakPoint, *curBreakPoint;

	prevBreakPoint = breakPointList;
	curBreakPoint = breakPointList;

	while(curBreakPoint)
	{
		if(prevBreakPoint->memoryAddress == regPC)
			return 1;
		prevBreakPoint = curBreakPoint;
		curBreakPoint = curBreakPoint->nextBreakPoint;
	}

	return 0;
}



/*int main()
{
	struct breakPoint* curBreakPoint;
	unsigned short isReset;
	unsigned long breakPointAddress;

	addBreakPoint(0x40000000);
	addBreakPoint(0x80000000);
	addBreakPoint(0x70000000);
	addBreakPoint(0x20000000);
	addBreakPoint(0x60000000);
	addBreakPoint(0x70000000);

	isReset = 0;
	while(1)
	{
		breakPointAddress = getBreakPoint(&isReset);
		if(isReset == 0)
			printf("0x%lx\n", breakPointAddress);
		else
			break;
	}

	printf("\n----------------\n");
	deleteBreakPoint(1);
	deleteBreakPoint(3);
	deleteBreakPoint(3);
	deleteBreakPoint(4);

	isReset = 0;
	while(1)
	{
		breakPointAddress = getBreakPoint(&isReset);
		if(isReset == 0)
			printf("0x%lx\n", breakPointAddress);
		else
			break;
	}
	
	printf("\n----------------\n");
	addBreakPoint(0x50000000);
	addBreakPoint(0x80000000);
	addBreakPoint(0x70000000);
	
	isReset = 0;
	breakPointAddress = getBreakPoint(&isReset);
	if(isReset == 0)
		printf("0x%lx\n", breakPointAddress);
	breakPointAddress = getBreakPoint(&isReset);
	if(isReset == 0)
		printf("0x%lx\n", breakPointAddress);
		
	isReset = 1;
	breakPointAddress = getBreakPoint(&isReset);
	if(isReset == 0)
		printf("0x%lx\n", breakPointAddress);

	printf("\n----------------\n");
	isReset = 1;
	breakPointAddress = getNextBreakPoint(0x45000000, &isReset);
	if(isReset == 0)
		printf("0x%lx\n", breakPointAddress);
	else
		printf("No such breakpoint exixts\n");

	isReset = 0;
	breakPointAddress = getNextBreakPoint(0x55000000, &isReset);
		if(isReset == 0)
			printf("0x%lx\n", breakPointAddress);
		else
			printf("No such breakpoint exixts\n");

	isReset = 0;
	breakPointAddress = getNextBreakPoint(0x85000000, &isReset);
	if(isReset == 0)
		printf("0x%lx\n", breakPointAddress);
	else
		printf("No such breakpoint exists\n");


	return RET_SUCCESS;
}*/
