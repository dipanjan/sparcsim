
void initializeMemory();
int allocateMemory(unsigned long memoryAddress);
void writeMemory(unsigned long memoryAddress, char byte);
char readMemory(unsigned long memoryAddress);
void displayQuadWord(char* cpuInstruction, int isInstruction);
char* getQuadWordFromMemory(unsigned long memoryAddress);
void displayMemoryArea(unsigned long memoryAddress, int count);
