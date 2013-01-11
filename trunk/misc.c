#include <misc.h>



long findByToken(char* token)
{
	FILE* handle = fopen("sparcsim.conf", "r");
	long tokenValue = RET_FAILURE;
	char* buffer = (char*)malloc(50);
	char* delimiter = " =\n\t";
	
	if(!handle)
		return FILE_DOES_NOT_EXIST_ERROR;
	
	while(fgets(buffer, 50, handle))
        if(!strcasecmp(token, strtok(buffer, delimiter)))
        {
                tokenValue = atol(strtok(NULL, delimiter));
                break;
        }

	free(buffer);
	fclose(handle);
	return tokenValue;
}



char* trim(char* stringToBeTrimmed)
{
    // The string to be trimmed must be writable, i.e. NOT a string literal, otherwise it will seg-fault
    
    unsigned short characterIndex = 0, leadingNonWhiteSpaceCharacterIndex = 0, isLeadingNonWhiteSpaceCharacterFound = 0;
    
    while((characterIndex < strlen(stringToBeTrimmed)) && (stringToBeTrimmed[characterIndex] != '\0') && (stringToBeTrimmed[characterIndex] != '#'))
    {
        if(((stringToBeTrimmed[characterIndex] == ' ' || stringToBeTrimmed[characterIndex] == '\t' || stringToBeTrimmed[characterIndex] == '\n')) && (isLeadingNonWhiteSpaceCharacterFound == 0))
            leadingNonWhiteSpaceCharacterIndex++;
        else
            isLeadingNonWhiteSpaceCharacterFound = 1;
        
        characterIndex++;
    }
    
    stringToBeTrimmed[characterIndex] = '\0';
    return (stringToBeTrimmed + leadingNonWhiteSpaceCharacterIndex);
}



/*int main()
{
	printf("%ld\n", findByToken("REGISTER_WINDOWS"));
	printf("%ld\n", findByToken("MEMEORY_BANKS"));
	return 0;
}*/
