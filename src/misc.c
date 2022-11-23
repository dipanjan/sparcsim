#include <misc.h>



/* 
 * Reads the value associated with <token> as specified
 * in sparcsim.conf configuration file. Token value must
 * have to be in <token> = <value> format, separated
 * by newlines (\n) from each other. Returns the
 * value read as 32-bit integer. Token name is case-sensitive.
 */
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



/*
 * Strips off the leading whitespace characters (blank space, newline and tab) from a string.
 * Also removes the trailing in-line comment marked by '#'.
 * The string to be trimmed must be writable, i.e. not a
 * string literal (constant), otherwise it will seg-fault.
 */
char* trim(char* stringToBeTrimmed)
{
    unsigned short characterIndex = 0, leadingNonWhiteSpaceCharacterIndex = 0, isLeadingNonWhiteSpaceCharacterFound = 0;
    
    // Iterate over the length of the string until the string terminates or an in-line comment marked by '#' begins. 
    while((characterIndex < strlen(stringToBeTrimmed)) && (stringToBeTrimmed[characterIndex] != '\0') && (stringToBeTrimmed[characterIndex] != '#'))
    {
        // Check for whitespace character unless already found one.
        if(((stringToBeTrimmed[characterIndex] == ' ' || stringToBeTrimmed[characterIndex] == '\t' || stringToBeTrimmed[characterIndex] == '\n')) && (isLeadingNonWhiteSpaceCharacterFound == 0))
            leadingNonWhiteSpaceCharacterIndex++;       // Advance the index until the first non-whitespace character.
        else
            isLeadingNonWhiteSpaceCharacterFound = 1;   // Indicates a leading whitespace character has already been found.
        
        characterIndex++;
    }
    
    stringToBeTrimmed[characterIndex] = '\0';                           // Terminate the string before the comment begins.
    return (stringToBeTrimmed + leadingNonWhiteSpaceCharacterIndex);    // Return a pointer to first non-whitespace character.
}



/*
 * Prints a string with specific ANSI terminal colorCode
 */
void colorize(const char *str, const char *colorCode) {
  printf("%s%s\e[0m", colorCode, str);
}



/*
 * Formats the string containing the text to be printed prepended by ANSI color code 
 */
void color(const char *str, int colorIndex) {
  char chr[32];

  sprintf(chr, "\33[7m\33[1m\e[%dm", 31 + colorIndex % 7);
  colorize(str, chr);
}



/*
 * Returns 1, if the 32-bit floating point number is Sounding NaN(SNaN), 
 * Returns 0, otherwise.
 */
int isSNaN32(float float32)
{
	convertFloat.hexToFloat = float32;
	int exponent = (convertFloat.floatToHex & 0x7F800000) >> 23;
	int mantissa = convertFloat.floatToHex & 0x007FFFFF;
	int msb_mantissa = (convertFloat.floatToHex & 0x00400000) >> 22;
	if((exponent == 255 ) && mantissa && !msb_mantissa)
		return 1;
	else
		return 0;
}



/*
 * Returns 1, if the 32-bit floating point number is Quiet NaN(QNaN), 
 * Returns 0, otherwise.
 */
int isQNaN32(float float32)
{
	if(isnan(float32) && !isSNaN32(float32))
		return 1;
	else
		return 0;
}



/*
 * Returns 1, if the 64-bit floating point number is Sounding NaN(SNaN), 
 * Returns 0, otherwise.
 */
int isSNaN64(double float64)
{
	convertDouble.hexToDouble = float64;
	int exponent = (convertDouble.doubleToHex[0] & 0x7FF00000) >> 20;
	int mantissa_high = convertDouble.doubleToHex[0] & 0x000FFFFF;
	int mantissa_low = convertDouble.doubleToHex[1];
	int msb_mantissa = (convertDouble.doubleToHex[0] & 0x00080000) >> 19;
	if((exponent == 2047) && (mantissa_high || mantissa_low) && !msb_mantissa)
		return 1;
	else
		return 0;
}



/*
 * Returns 1, if the 64-bit floating point number is Quiet NaN(QNaN), 
 * Returns 0, otherwise.
 */
int isQNaN64(double float64)
{
	if(isnan(float64) && !isSNaN64(float64))
		return 1;
	else
		return 0;
}