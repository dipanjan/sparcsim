#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <register.h>



char* getIntegerRegisterName(unsigned long registerIdentifier)
{
	char* registerName = (char*) malloc(4);
	char* registerIndex = (char*)malloc(2);
	
	if((registerIdentifier >= 0) && (registerIdentifier <=7))
		strcpy(registerName, "%g");
	else
		if((registerIdentifier >= 8) && (registerIdentifier <=15))
			strcpy(registerName, "%o");
		else
			if((registerIdentifier >= 16) && (registerIdentifier <=23))
				strcpy(registerName, "%l");
			else
				strcpy(registerName, "%i");
	
	sprintf(registerIndex, "%ld", (registerIdentifier % 8));
	strcat(registerName, registerIndex);
	
	free(registerIndex);
	return registerName;
}



char* getFlotingRegisterName(unsigned long registerIdentifier)
{
	char* registerName = (char*) malloc(4);
	char* registerIndex = (char*)malloc(3);
	
	strcpy(registerName, "%f");
	sprintf(registerIndex, "%ld", registerIdentifier);
	strcat(registerName, registerIndex);
	
	free(registerIndex);
	return registerName;
}



char* getCoProcessorRegisterName(unsigned long registerIdentifier)
{
	char* registerName = (char*) malloc(4);
	char* registerIndex = (char*)malloc(3);
	
	strcpy(registerName, "%f");
	sprintf(registerIndex, "%ld", registerIdentifier);
	strcat(registerName, registerIndex);
	
	free(registerIndex);
	return registerName;
}