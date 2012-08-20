#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <libelf/libelf.h>
#include <libelf/gelf.h>
#include <memory.h>
#include <constants.h>



#define CODE_SECTION				1
#define DATA_SECTION				2
#define UNINITIALIZED_DATA_SECTION	3



struct loadedSections
{
	char sectionName[15];
	unsigned long sectionLoadAddress;
	short sectionType;
	unsigned long sectionSize;
	unsigned long instructionCount;
	struct loadedSections* nextSection;
};



int initializeLoader(char *elfBinary);
struct loadedSections* load_sparc_instructions(char *elfBinary);
