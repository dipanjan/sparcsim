#include <breakpoint.h>



struct breakPoint* breakPointList = NULL;
short breakPointSerial = 0;
short watchPointSerial = 0;
static unsigned long lastBreakPointAt = 0;
static unsigned short isLastBreakPointEncountered = 0;
static unsigned long lastWatchPointAt = 0;
static unsigned short isLastWatchPointEncountered = 0;



/* 
 * Clears list of breakpoints by deleting breakpoint nodes
 * until the last one, thereby deallocating nodes and freeing 
 * up memory one by one. Sets breakPointList to NULL.
 */
void initializeBreakPointList()
{
	if(breakPointList != NULL)
		while(deleteBreakPoint(1) == RET_SUCCESS);
}



/* 
 * Adds either a breakPoint or a watchPoint depending on breakPointType to the
 * list of breakpoints. The breakPoint or watchPoint is set at memoryAddress.
 */
int addBreakPoint(unsigned long memoryAddress, unsigned short breakPointType)
{
	struct breakPoint* prevBreakPoint, *curBreakPoint, *newBreakPoint;

        // Allocate memory for a new breakpoint
	newBreakPoint = (struct breakPoint*)malloc(sizeof(struct breakPoint));
	if(!newBreakPoint)
		return BREAKPOINT_ALLOCATION_ERROR;
	
	/* If breakPointList is empty, set the newly created node
         * to be the first one of the list of breakpoints.
         */
	if(!breakPointList)
	{
		breakPointList = newBreakPoint;
		curBreakPoint = breakPointList;
	}
        
        // breakPointList is not empty
	else
	{
		/* Maintain separate pointers to previous as well as current breakpoint node.
                 * Initialize both the pointers to the head of the breakPointList.
                 */ 
                curBreakPoint = breakPointList;
		prevBreakPoint = breakPointList;

                /* Traverse breakPointList either till end or memoryAddress
                 * matches to one which breakpoint is already set on.
                 */
		while(curBreakPoint && (prevBreakPoint->memoryAddress != memoryAddress))
		{
			prevBreakPoint = curBreakPoint;
			curBreakPoint = curBreakPoint->nextBreakPoint;
		}
	
                // If a breakpoint is already set, nothing else needs to be done.
		if(prevBreakPoint->memoryAddress == memoryAddress)
			return RET_SUCCESS;
                
                /* Otherwise, we are at the end of the list.
                 * Set the newly created breakpoint to be the last one.
                 */
		else
		{
			prevBreakPoint->nextBreakPoint = newBreakPoint;
			curBreakPoint = prevBreakPoint->nextBreakPoint;
		}
	}

	breakPointSerial++;                                     // Increase serial as a new node has been added.
        curBreakPoint->breakPointSerial = breakPointSerial;     // Set serial of breakpoint.
	curBreakPoint->memoryAddress = memoryAddress;           // Set memoryAddress which breakpoint/watchpoint is set at.
        curBreakPoint->breakPointType = breakPointType;         // Breakpoint or Watchpoint?
	curBreakPoint->nextBreakPoint = NULL;                   // Terminate the list.

	return RET_SUCCESS;
}



/* 
 * Given the serial of a breakPoint, delete the node associated.
 */
int deleteBreakPoint(unsigned short index)
{
	struct breakPoint* prevBreakPoint, *curBreakPoint, *nextBreakPoint;

        // Deletion can take place only if breakPointList is non-empty.
	if(breakPointList)
	{
		// Is it the first breakPoint we are attempting to delete?
                if(breakPointList->breakPointSerial == index)
		{
			nextBreakPoint = breakPointList->nextBreakPoint;
			free(breakPointList);
			breakPointList = nextBreakPoint;
			return RET_SUCCESS;
		}
                
                // A non-first breakPoint has to be deleted.
		else
		{
			// Set current breakPoint to be the head of the list.
                        curBreakPoint = breakPointList;
                        
                        // Start traversing until the specified breakPoint is found or the end is reached.
			while((curBreakPoint->breakPointSerial != index) && curBreakPoint->nextBreakPoint)
			{
				prevBreakPoint = curBreakPoint;
				curBreakPoint = curBreakPoint->nextBreakPoint;
			}

                        // If we have found the specified breakPoint, delete it.
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



/*
 * Returns the first breakPoint, if isReset = 1
 * Returns the next breakPoint, if isReset = 0
 */
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



/* 
 * Returns 1, if a breakPoint is set at address 
 * contained in regPC, otherwise returns 0.
 */
int isBreakPoint(unsigned long regPC)
{
	struct breakPoint *curBreakPoint;

	curBreakPoint = breakPointList;

        // Iterate over list of breakPoints
	while(curBreakPoint)
	{
            // Check for breakPointType as it can be a watchPoint, too
            if(curBreakPoint->breakPointType == BREAK_POINT)
            {
                if(curBreakPoint->memoryAddress == regPC) 
                {
                    if(lastBreakPointAt == regPC)
                    {
                        if(isLastBreakPointEncountered)
                            isLastBreakPointEncountered = 0;
                        else
                        {
                            lastBreakPointAt = regPC;
                            isLastBreakPointEncountered = 1; 
                            breakPointSerial = curBreakPoint->breakPointSerial;
                            return 1;
                        }
                    }
                    else
                    {
                        lastBreakPointAt = regPC;
                        isLastBreakPointEncountered = 1;
                        breakPointSerial = curBreakPoint->breakPointSerial;
                        return 1;
                    }
                }
            }
            curBreakPoint = curBreakPoint->nextBreakPoint;
	}

	return 0;
}



/* 
 * Returns the serial number of last breakPoint added
 */
short getBreakPointSerial()
{
    return breakPointSerial;
}



/* 
 * Returns 1, if a watchPoint is set at address 
 * contained in regPC, otherwise returns 0.
 */
int isWatchPoint(unsigned long memoryAddress, unsigned long regPC)
{
	struct breakPoint *curBreakPoint; 

	curBreakPoint = breakPointList;
        memoryAddress = wordAlign(memoryAddress);

	while(curBreakPoint)
	{
            if(curBreakPoint->breakPointType == WATCH_POINT)
            {
                if(curBreakPoint->memoryAddress == memoryAddress) 
                {
                    if(lastWatchPointAt == regPC)
                    {
                        if(isLastWatchPointEncountered)
                            isLastWatchPointEncountered = 0;
                        else
                        {
                            lastWatchPointAt = regPC;
                            isLastWatchPointEncountered = 1;
                            watchPointSerial = curBreakPoint->breakPointSerial;
                            return 1;
                        }
                    }
                    else
                    {
                        lastWatchPointAt = regPC;
                        isLastWatchPointEncountered = 1;
                        watchPointSerial = curBreakPoint->breakPointSerial;
                        return 1;
                    }
                }
            }
            curBreakPoint = curBreakPoint->nextBreakPoint;
	}

	return 0;
}



/* 
 * Returns the serial number of last watchPoint added
 */
short getWatchPointSerial()
{
    return watchPointSerial;
}
