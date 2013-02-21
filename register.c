#include <register.h>



struct registers sparcRegisters;



void initializeRegisters()
{
	unsigned short count;
	
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
	sparcRegisters.psr.s = 1;
	sparcRegisters.psr.pil = 0;
	sparcRegisters.psr.ef = 0;
	sparcRegisters.psr.ec = 0;
	sparcRegisters.psr.reserved = 0;
	
	sparcRegisters.psr.c = 0;
	sparcRegisters.psr.v = 0;
	sparcRegisters.psr.z = 0;
	sparcRegisters.psr.n = 0;
	sparcRegisters.psr.ver = 3;
	sparcRegisters.psr.impl = 0xF;
        
        // Initialize fsr
        sparcRegisters.fsr.cexc = 0;
        sparcRegisters.fsr.aexc = 0;
        sparcRegisters.fsr.fcc = 0;
        sparcRegisters.fsr.ulow = 0;
        sparcRegisters.fsr.qne = 0;
        sparcRegisters.fsr.ftt = 0;
        sparcRegisters.fsr.ver = 0;
        sparcRegisters.fsr.res = 0;
        sparcRegisters.fsr.ns = 0;        
        sparcRegisters.fsr.tem = 0;
        sparcRegisters.fsr.uhigh = 0;
        sparcRegisters.fsr.rd = 0;

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
        
        // Initialize ASR
	for(count = 0; count < 32; count++)
	sparcRegisters.asrRegisters[count] = 0;
        
        // Initialize floating point registers
	for(count = 0; count < 32; count++)
	sparcRegisters.floatingPointRegisters[count] = 0;
}



void resetSimulator()
{
	sparcRegisters.psr.cwp = 0;
	sparcRegisters.psr.c = 0;
	sparcRegisters.psr.v = 0;
	sparcRegisters.psr.z = 0;
	sparcRegisters.psr.n = 0;
	sparcRegisters.pc = 0;
	sparcRegisters.npc = 4;
}



char* displayRegister(unsigned long registerValue)
{
    char* hexNumber = (char*)malloc(11);
    sprintf(hexNumber, "0x%08lX", registerValue);
    return hexNumber;
}



unsigned short getRegisterWindow()
{
    return sparcRegisters.psr.cwp;
}



void setRegisterWindow(unsigned short registerWindow)
{
    sparcRegisters.psr.cwp = registerWindow;
    sparcRegisters.cwptr = sparcRegisters.registerSet + sparcRegisters.psr.cwp * REGISTER_WINDOW_WIDTH;
}



unsigned long* getWindowPointer(int direction)
{
   // Move window pointer forward
   if(direction == 1)
    {
            if(sparcRegisters.psr.cwp == (sparcRegisters.registerWindows - 1))
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
	char registerType, sparcRegisterString[6];
	unsigned short registerIndex, charIndex = 0;
	unsigned long* previousWindowPointer = getWindowPointer(-1);
	
        /* 
         Register name is copied to a temporary variable to avoid segmentation fault.
         gcc core dump analysis shows that it faults while trying to modify a string literal
         from an invocation like setRegister("%o7", registerValue) from CALL type instruction 
        */
        strcpy(sparcRegisterString, sparcRegister); 
        sparcRegister = sparcRegisterString;
        
        // Strip off leading % symbol, if present
        if(sparcRegister[0] == '%')
        {
            while(sparcRegister[charIndex++])
                sparcRegister[charIndex - 1] = sparcRegister[charIndex];
            sparcRegister[charIndex - 2] = '\0'; 
        }
        
	if(!(strcmp(sparcRegister, "g0")))
                return 0;

	if(!strcmp(sparcRegister, "psr")) 
                return castPSRToUnsignedLong(sparcRegisters.psr);
        
        if(!strcmp(sparcRegister, "fsr")) 
                return castFSRToUnsignedLong(sparcRegisters.fsr);
		
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
        
        if((sparcRegister[0] == 'a') && (sparcRegister[1] == 's') && (sparcRegister[2] == 'r'))
        {
            char* asrRegister = (char*)malloc(3);
            charIndex = 0;
            if(sparcRegister[3] >= '0' && sparcRegister[3] <= '9')
                asrRegister[charIndex++] = sparcRegister[3];
            if(sparcRegister[4] >= '0' && sparcRegister[4] <= '9')
                asrRegister[charIndex++] = sparcRegister[4];
            asrRegister[charIndex] = '\0';
            
            registerIndex = strtoul(asrRegister, NULL, 0);
            return sparcRegisters.asrRegisters[registerIndex]; 
        }
	
        
        registerType = sparcRegister[0];
	registerIndex = sparcRegister[1] - '0';
        if(sparcRegister[2] != '\0')
            registerIndex = registerIndex * 10 +  (sparcRegister[2] - '0');
        
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
        case 'f':
                return sparcRegisters.floatingPointRegisters[registerIndex];
	}
	return 0;
}



void setRegister(char* sparcRegister, unsigned long registerValue)
{
	char registerType, sparcRegisterString[6];
	unsigned short registerIndex, charIndex = 0;
	unsigned long* previousWindowPointer = getWindowPointer(-1);
	
        /* 
         Register name is copied to a temporary variable to avoid segmentation fault.
         Program core dump analysis shows that it faults while trying to modify a string literal
         from an invocation like setRegister("%o7", registerValue) from CALL instruction 
        */
        strcpy(sparcRegisterString, sparcRegister); 
        sparcRegister = sparcRegisterString;
        
        // Strip off leading % symbol, if present
        if(sparcRegister[0] == '%')
        {
            while(sparcRegister[charIndex++])
                sparcRegister[charIndex - 1] = sparcRegister[charIndex];
            sparcRegister[charIndex - 2] = '\0'; 
        }
        
        if(!(strcmp(sparcRegister, "g0")))
		return;

	if(!strcmp(sparcRegister, "psr"))
	{
                sparcRegisters.psr = castUnsignedLongToPSR(registerValue);
                sparcRegisters.cwptr = sparcRegisters.registerSet + sparcRegisters.psr.cwp * REGISTER_WINDOW_WIDTH;
	}
        
        if(!strcmp(sparcRegister, "fsr"))
                sparcRegisters.fsr = castUnsignedLongToFSR(registerValue);
		
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
        
        if((sparcRegister[0] == 'a') && (sparcRegister[1] == 's') && (sparcRegister[2] == 'r'))
        {
            char* asrRegister = (char*)malloc(3);
            charIndex = 0;
            if(sparcRegister[3] >= '0' && sparcRegister[3] <= '9')
                asrRegister[charIndex++] = sparcRegister[3];
            if(sparcRegister[4] >= '0' && sparcRegister[4] <= '9')
                asrRegister[charIndex++] = sparcRegister[4];
            asrRegister[charIndex] = '\0';
            
            registerIndex = strtoul(asrRegister, NULL, 0);
            sparcRegisters.asrRegisters[registerIndex] = registerValue; 
            return;
        }

        
        registerType = sparcRegister[0];
	registerIndex = sparcRegister[1] - '0';
        if(sparcRegister[2] != '\0')
            registerIndex = registerIndex * 10 +  (sparcRegister[2] - '0');
	
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
        case 'f':
		sparcRegisters.floatingPointRegisters[registerIndex]= registerValue;
		break;
	}
}



int saveRegisters()
{
	unsigned long regPSR, regWIM;
	struct processor_status_register psr;
	short nextCWP;

	regPSR = getRegister("psr");
        psr = castUnsignedLongToPSR(regPSR);
	regWIM = getRegister("wim");

	if(psr.cwp == 0)
		nextCWP = sparcRegisters.registerWindows - 1;
	else
		nextCWP = sparcRegisters.psr.cwp - 1;

	if(getBit(regWIM, nextCWP))
	{
		handleTrap(WINDOWS_OVERFLOW, sparcRegisters.pc);
		return RET_FAILURE;
	}
	else
	{
		sparcRegisters.cwptr = getWindowPointer(-1);
                sparcRegisters.psr.cwp = nextCWP;

		return RET_SUCCESS;
	}
}



int restoreRegisters()
{
	unsigned long regPSR, regWIM;
	struct processor_status_register psr;
	short nextCWP;

	regPSR = getRegister("psr");
        psr = castUnsignedLongToPSR(regPSR);
	regWIM = getRegister("wim");

	if(psr.cwp == (sparcRegisters.registerWindows - 1))
		nextCWP = 0;
	else
		nextCWP = sparcRegisters.psr.cwp + 1;


	if(getBit(regWIM, nextCWP))
	{
		handleTrap(WINDOWS_UNDERFLOW, sparcRegisters.pc);
		return RET_FAILURE;
	}
	else
	{
		sparcRegisters.cwptr = getWindowPointer(1);
                sparcRegisters.psr.cwp = nextCWP;

		return RET_SUCCESS;
	}
}



struct processor_status_register castUnsignedLongToPSR(unsigned long registerValue)
{
	struct processor_status_register psr;

	psr.cwp = registerValue & 0x00000001F;
	psr.et = (registerValue & 0x00000020) >> 5;
	psr.ps = (registerValue & 0x00000040) >> 6;
	psr.s = (registerValue & 0x00000080) >> 7;
	psr.pil = (registerValue & 0x00000F00) >> 8;
	psr.ef = (registerValue & 0x00001000) >> 9;
	psr.ec = (registerValue & 0x00002000) >> 10;
	psr.reserved = (registerValue & 0x000FC000) >> 14;
	psr.c = (registerValue & 0x00100000) >> 20;
	psr.v = (registerValue & 0x00200000) >> 21;
	psr.z = (registerValue & 0x00400000) >> 22;
	psr.n = (registerValue & 0x00800000) >> 23;
	psr.ver = (registerValue & 0x0F000000) >> 24;
	psr.impl = (registerValue & 0xF0000000) >> 28;

	return psr;
}



unsigned long castPSRToUnsignedLong(struct processor_status_register psr)
{
	unsigned long registerValue = 0;

	registerValue = registerValue | psr.cwp;
	registerValue = registerValue | (psr.et << 5);
	registerValue = registerValue | (psr.ps << 6);
	registerValue = registerValue | (psr.s << 7);
	registerValue = registerValue | (psr.pil << 8);
	registerValue = registerValue | (psr.ef << 12);
	registerValue = registerValue | (psr.ec << 13);
	registerValue = registerValue | (psr.reserved << 14);
	registerValue = registerValue | (psr.c << 20);
	registerValue = registerValue | (psr.v << 21);
	registerValue = registerValue | (psr.z << 22);
	registerValue = registerValue | (psr.n << 23);
	registerValue = registerValue | (psr.ver << 24);
	registerValue = registerValue | (psr.impl << 28);	

	return registerValue;
}



struct floating_point_state_register castUnsignedLongToFSR(unsigned long registerValue)
{
	struct floating_point_state_register fsr;
        
        fsr.cexc = registerValue & 0x0000001F;
        fsr.aexc = (registerValue & 0x000003E0) >> 5;
        fsr.fcc = (registerValue & 0x00000C00) >> 10;
        fsr.ulow = (registerValue & 0x00001000) >> 12;
        fsr.qne = (registerValue & 0x00002000) >> 13;
        fsr.ftt = (registerValue & 0x0001C000) >> 14;
        fsr.ver = (registerValue & 0x000E0000) >> 17;
        fsr.res = (registerValue & 0x00300000) >> 20;
        fsr.ns = (registerValue & 0x00400000) >> 22;
        fsr.tem = (registerValue & 0x0F100000) >> 23;
        fsr.uhigh = (registerValue & 0x30000000) >> 28;
        fsr.rd = (registerValue & 0xC0000000) >> 30;

	return fsr;
}



unsigned long castFSRToUnsignedLong(struct floating_point_state_register fsr)
{
	unsigned long registerValue = 0;

	registerValue = registerValue | fsr.cexc;
	registerValue = registerValue | (fsr.aexc << 5);
	registerValue = registerValue | (fsr.fcc << 10);
	registerValue = registerValue | (fsr.ulow << 12);
	registerValue = registerValue | (fsr.qne << 13);
	registerValue = registerValue | (fsr.ftt << 14);
	registerValue = registerValue | (fsr.ver << 17);
	registerValue = registerValue | (fsr.res << 20);
	registerValue = registerValue | (fsr.ns << 22);
	registerValue = registerValue | (fsr.tem << 23);
	registerValue = registerValue | (fsr.uhigh << 28);
	registerValue = registerValue | (fsr.rd << 30);	

	return registerValue;
}



char* getNextRegister(char* sparcRegister)
{
    char nextRegister[15];
    /*
     * In earlier implementation, nextRegister[6] was declared. When a garbage, i.e. a  string NOT
     *  representing a register was being passed from execute.c (convertDouble.doubleToHex[1] = getRegister(getNextRegister(tokens[1])))
     *  e.g. 0xFFFFFF90 (length = 10), it was passing out of array boundary.
     */
    unsigned short nextRegisterIndex, characterCount = 2; // Because register name will be at least two character long, e.g. g5
    
    // Copy the register name to prevent alteration of actual register
    strcpy(nextRegister, sparcRegister);
    sparcRegister = nextRegister;
    
    nextRegisterIndex = sparcRegister[1] - '0';
    if(sparcRegister[2] != '\0')
        nextRegisterIndex = nextRegisterIndex * 10 +  (sparcRegister[2] - '0');
    nextRegisterIndex++;
    sparcRegister[1] = nextRegisterIndex / 10 + '0';
    if(nextRegisterIndex % 10 != 0)
    {
        sparcRegister[characterCount] = nextRegisterIndex % 10 + '0';
        characterCount++;
    }
    sparcRegister[characterCount] = '\0';
    
    return sparcRegister;
}