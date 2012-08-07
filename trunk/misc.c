#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <constants.h>

long findByToken(char* token)
{
	FILE* handle = fopen("config.txt", "r");
	if(handle == NULL)
		return RET_FAILURE;
	char* buffer = (char*)malloc(20);
	char* delimiter = " =\n";
	int index;
	
	while(buffer[index] != EOF)
	{
		index = 0;
		do
			*(buffer + index++) = getc(handle);
		while(buffer[index] != '\n' && buffer[index] != EOF);
		if(!strcasecmp(token, strtok(buffer, delimiter)))
			atoi(strtok(NULL, delimiter));
	}
	
	free(buffer);
	return RET_FAILURE;
}


void main()
{
	printf("%ld", findByToken("ram"));
}