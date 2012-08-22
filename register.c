#include <register.h>



struct registers sparcRegisters;



void initializeRegisters()
{
	unsigned short count;
	unsigned short registerWindows;
	
	if(sparcRegisters.registerSet != NULL)
		free(sparcRegisters.registerSet);
	if(sparcRegisters.globalRegisters != NULL)
		free(sparcRegisters.globalRegisters);
	sparcRegisters.registerWindows = findByToken("REGISTER_WINDOWS");
	sparcRegisters.registerSet = (unsigned long*)malloc(REGISTER_WINDOW_WIDTH * sparcRegisters.registerWindows * SIZEOF_INTEGER_REGISTER);
	sparcRegisters.globalRegisters = (unsigned long*)malloc(SIZEOF_INTEGER_REGISTER * GLOBAL_REGISTERS);
	sparcRegisters.cwptr = sparcRegisters.registerSet;

	// Initialize psr
	sparcRegisters.psr.cwp = 0;
	sparcRegisters.psr.et = 0;
	sparcRegisters.psr.ps = 0;
	sparcRegisters.psr.s = 0;
	sparcRegisters.psr.pil = 0;
	sparcRegisters.psr.ef = 0;
	sparcRegisters.psr.ec = 0;
	sparcRegisters.psr.reserved = 0;
	/*sparcRegisters.psr.icc.c = 0;
	sparcRegisters.psr.icc.v = 0;
	sparcRegisters.psr.icc.z = 0;
	sparcRegisters.psr.icc.n = 0;*/
	sparcRegisters.psr.c = 0;
	sparcRegisters.psr.v = 0;
	sparcRegisters.psr.z = 0;
	sparcRegisters.psr.n = 0;
	sparcRegisters.psr.ver = 0;
	sparcRegisters.psr.impl = 0;

	// Initialize wim, tbr, y, pc, npc
	sparcRegisters.wim = 0;
	sparcRegisters.tbr = 0;
	sparcRegisters.y = 0;
	sparcRegisters.pc = 0;
	sparcRegisters.npc = 4;

	// Initialize global registers
	for(count = 0; count < GLOBAL_REGISTERS; count++)
	sparcRegisters.globalRegisters[count] = 0;

	// Initialize out, local, in registers
	for(count = 0; count < REGISTER_WINDOW_WIDTH * sparcRegisters.registerWindows; count++)
	sparcRegisters.registerSet[count] = 0;
}



char* displayRegister(unsigned long registerValue)
{
    char* hexNumber = (char*)malloc(11);
    sprintf(hexNumber, "0x%08lx", registerValue);
    return hexNumber;
}



unsigned long* getWindowPointer(int direction)
{
   // Move window pointer forward
   if(direction == 1)
    {
		if(sparcRegisters.psr.cwp == sparcRegisters.registerWindows)
			return sparcRegisters.registerSet;
		else
			return sparcRegisters.cwptr + REGISTER_WINDOW_WIDTH;
    }
	
	// Move window pointer backward
    else
    {
		if(sparcRegisters.psr.cwp == 0)
			return sparcRegisters.registerSet + (REGISTER_WINDOW_WIDTH * (sparcRegisters.registerWindows - 1));
		else
			return sparcRegisters.cwptr - REGISTER_WINDOW_WIDTH;
    }
}



unsigned long getRegister(char* sparcRegister)
{
	char registerType;
	unsigned short registerIndex;
	unsigned long* previousWindowPointer = getWindowPointer(-1);
	
	if(!strcmp(sparcRegister, "psr"))
		return FORCE_CAST (sparcRegisters.psr, unsigned long);
		
	if(!strcmp(sparcRegister, "wim"))
		return sparcRegisters.wim;
	
	if(!strcmp(sparcRegister, "tbr"))
		return sparcRegisters.tbr;
	
	if(!strcmp(sparcRegister, "y"))
		return sparcRegisters.y;
		
	if(!strcmp(sparcRegister, "pc"))
		return sparcRegisters.pc;
		
	if(!strcmp(sparcRegister, "npc"))
		return sparcRegisters.npc;
	
	// Preceding % sign is present
	if(strlen(sparcRegister) == 3)
	{
		registerType = sparcRegister[1];
		registerIndex = sparcRegister[2] - '0';
	}
	else
	{
		registerType = sparcRegister[0];
		registerIndex = sparcRegister[1] - '0';
	}
	
	switch (registerType)
	{
	case 'o':
		return *(previousWindowPointer + registerIndex);
	case 'l':
		return *(sparcRegisters.cwptr + 8 + registerIndex);
	case 'i':
		return *(sparcRegisters.cwptr + registerIndex);
	case 'g':
		return *(sparcRegisters.globalRegisters + registerIndex);
	}
	return 0;
}



void setRegister(char* sparcRegister, unsigned long registerValue)
{
	char registerType;
	unsigned short registerIndex;
	unsigned long* previousWindowPointer = getWindowPointer(-1);
	
	if(!strcmp(sparcRegister, "wim"))
		sparcRegisters.wim = registerValue;

	if(!strcmp(sparcRegister, "tbr"))
		sparcRegisters.tbr = registerValue;

	if(!strcmp(sparcRegister, "y"))
		sparcRegisters.y = registerValue;

	if(!strcmp(sparcRegister, "pc"))
		sparcRegisters.pc = registerValue;

	if(!strcmp(sparcRegister, "npc"))
		sparcRegisters.npc = registerValue;

	// Preceding % sign is present
	if(strlen(sparcRegister) == 3)
	{
		registerType = sparcRegister[1];
		registerIndex = sparcRegister[2] - '0';
	}
	else
	{
		registerType = sparcRegister[0];
		registerIndex = sparcRegister[1] - '0';
	}
	
	
	switch (registerType)
	{
	case 'o':
		*(previousWindowPointer + registerIndex) = registerValue;
		break;
	case 'l':
		*(sparcRegisters.cwptr + 8 + registerIndex) = registerValue;
		break;
	case 'i':
		*(sparcRegisters.cwptr + registerIndex) = registerValue;
		break;
	case 'g':
		*(sparcRegisters.globalRegisters + registerIndex) = registerValue;
		break;
	}
}



void saveRegisters()
{
     sparcRegisters.cwptr = getWindowPointer(-1);
     sparcRegisters.psr.cwp--;
}



void restoreRegisters()
{
     sparcRegisters.cwptr = getWindowPointer(1);
     sparcRegisters.psr.cwp++;
}
