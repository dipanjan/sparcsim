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



int initializeLoader(char *elfBinary, int isReset);
void load_sparc_elf(char *elfBinary);
void load_sparc_instructions();
