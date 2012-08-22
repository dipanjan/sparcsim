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



/*int main()
{
	printf("%ld\n", findByToken("REGISTER_WINDOWS"));
	printf("%ld\n", findByToken("MEMEORY_BANKS"));
	return 0;
}*/
