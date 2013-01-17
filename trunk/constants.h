#ifndef CONSTANTS_H
#define CONSTANTS_H

// Generic
#define RET_QUIT     						 0
#define RET_SUCCESS  						 1
#define RET_FAILURE 						-1
#define RET_NOTACOMMAND 					 2
#define RET_BREAKPOINT   					 3



// simulator.c
#define MAX_INPUT_LENGTH 					50



// loader.c
#define ELF_FILE_DOES_NOT_EXIST_ERROR           	-1
#define ELF_BINARY_OUT_OF_DATE				-2
#define ELF_POINTER_INITIALIZATION_ERROR                -3
#define ELF_SECTION_LOAD_ERROR				-4



// memory.c
#define SECOND_PAGE_TABLE_ALLOCATION_ERROR              -1
#define PAGE_ALLOCATION_ERROR				-2



// misc.c
#define FILE_DOES_NOT_EXIST_ERROR			-2



// breakpoint.c
#define BREAKPOINT_ALLOCATION_ERROR                     -1

#endif
