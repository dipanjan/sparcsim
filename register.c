#include <register.h>



struct registers sparcRegisters;



/*
 * Allocates memory for Integer (i.e. in, out, global, local), Floating-point,
 * WIM, PSR, FSR, TBR, ASR, Y, PC, nPC registers and initializes with
 * default values to set up the execution environment. However, the default 
 * values, if found inappropriate for a specific implementation, can be overriden
 * from batch script using 'reg' simulator command.
 */
void initializeRegisters()
{
	unsigned short count;
	
	if(sparcRegisters.registerSet != NULL)
		free(sparcRegisters.registerSet);
	if(sparcRegisters.globalRegisters != NULL)
		free(sparcRegisters.globalRegisters);
	sparcRegisters.registerWindows = findByToken("REGISTER_WINDOWS");       // Get number of register windows from configuration file
        
        /* sparcRegisters.registerSet holds the base address of the set of register windows.
         * Each register window consists of a set of IN and LOCAL registers of its own, as well as
         * a set of OUT registers shared from its adjacent window. Hence, there are (IN + LOCAL) = (8 + 8) = 16
         * registers in a window (REGISTER_WINDOW_WIDTH), each register being 4 byte (SIZEOF_INTEGER_REGISTER)
         * wide. Therefore, 16 * 8 * <Number of register windows> bytes has been allocated.
         */
	sparcRegisters.registerSet = (unsigned long*)malloc(REGISTER_WINDOW_WIDTH * sparcRegisters.registerWindows * SIZEOF_INTEGER_REGISTER);
        
        // GLOBAL registers, not being part of of register window, are contained in memory pointed to by sparcRegisters.globalRegisters.
	sparcRegisters.globalRegisters = (unsigned long*)malloc(SIZEOF_INTEGER_REGISTER * GLOBAL_REGISTERS);
        
        /* sparcRegisters.cwptr will always point to the base address of the current register window.
         * It is initialized by the base address of set of register windows, i.e. sparcRegisters.cwptr
         */
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
	sparcRegisters.npc = 4;                 // nPC should point to the instruction ahead.

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



/*
 * Clear or initialize relevant registers on a simulator event.
 * Not all registers and even simulator memory are cleared. Hence,
 * those will contain data from previous run.
 */
void resetSimulator()
{
	sparcRegisters.psr.cwp = 0;
	sparcRegisters.psr.c = 0;
	sparcRegisters.psr.v = 0;
	sparcRegisters.psr.z = 0;
	sparcRegisters.psr.n = 0;
	sparcRegisters.pc = 0;
	sparcRegisters.npc = 4;
        
        setIUErrorMode(0);
}



/*
 * Displays 32-bit value contained in a register in HEX format, 
 * prepended with zeroes used for padding if the value is shorter than 32 bit.
 */
char* displayRegister(unsigned long registerValue)
{
    char* hexNumber = (char*)malloc(11);
    sprintf(hexNumber, "0x%08lX", registerValue);
    return hexNumber;
}



/*
 * Returns the current window pointer value contained in CWP field of PSR.
 */
unsigned short getRegisterWindow()
{
    return sparcRegisters.psr.cwp;
}



/*
 * Sets the current window pointer value contained in CWP field of PSR.
 * CWP, being a field 5 bit wide, can vary in the range [0-32]. Specific 
 * implementation can cut the maximum limit short by specifying REGISTER_WINDOWS
 * token in configuration file (sparcsim.conf).
 * Also adjusts current window pointer address (sparcRegisters.cwptr) 
 * held in memory. sparcRegisters.registerSet is the base address of register 
 * set and CWP multiplied by REGISTER_WINDOW_WIDTH gives the offset.
 */
void setRegisterWindow(unsigned short registerWindow)
{
    sparcRegisters.psr.cwp = registerWindow;
    sparcRegisters.cwptr = sparcRegisters.registerSet + sparcRegisters.psr.cwp * REGISTER_WINDOW_WIDTH;
}



/*
 * Returns the pointer to current register window forward or backward
 * as specified by direction. Positive direction indicates
 * forward move and negative direction indicates backward move.
 * Wrap around is done, if needed.
 * SAVE and RESTORE SPARC instructions require the pointer to
 * register window to be shifted appropriately.
 */
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
	unsigned long* previousWindowPointer = getWindowPointer(-1);            // Previous window pointer will be useful to access OUT registers shared from previous adjacent window. 
	
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
                return 0;                       // %g0 will always contain zero, by definition.

	if(!strcmp(sparcRegister, "psr")) 
                return castPSRToUnsignedLong(sparcRegisters.psr);       // Cast PSR structure to 4 byte long integer and return.
        
        if(!strcmp(sparcRegister, "fsr")) 
                return castFSRToUnsignedLong(sparcRegisters.fsr);       // Cast FSR structure to 4 byte long integer and return.
		
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
                asrRegister[charIndex++] = sparcRegister[3];            // If 4th character is a digit, copy it to asrRegister, i.e. 2 is taken out from %asr23.
            if(sparcRegister[4] >= '0' && sparcRegister[4] <= '9')
                asrRegister[charIndex++] = sparcRegister[4];            // If 5th character is a digit, copy it to asrRegister, i.e. 3 is taken out from %asr23.
            asrRegister[charIndex] = '\0';
            
            registerIndex = strtoul(asrRegister, NULL, 0);              // Convert asrRegister to integer index, i.e. 23 is obtained from %asr23.
            return sparcRegisters.asrRegisters[registerIndex]; 
        }
	
        
        registerType = sparcRegister[0];                // First character will indicate type of register, i.e. global(g), local(l), in(i), out(o)
	registerIndex = sparcRegister[1] - '0';         // Integer register index will be a single digit ranging between [0-7]
        
        /* If second character is non-null, calculate index as (<firstDigit> * 10 + <secondDigit>)
         * e.g. %f23, firstDigit = 2, secondDigit = 3
         * registerIndex = (2 * 10 + 3) = 23
         */
        if(sparcRegister[2] != '\0')                    
            registerIndex = registerIndex * 10 +  (sparcRegister[2] - '0');
        
	switch (registerType)
	{
	case 'o':
		return *(previousWindowPointer + registerIndex);                // OUT register are shared with IN registers from previous window.
	case 'l':
		return *(sparcRegisters.cwptr + 8 + registerIndex);             // In a register window, LOCAL registers follow IN registers.
	case 'i':
		return *(sparcRegisters.cwptr + registerIndex);                 // In a register window, IN registers precede LOCAL registers.
	case 'g':
		return *(sparcRegisters.globalRegisters + registerIndex);       // GLOBAL registers are allocated separately from register window.
        case 'f':
                return sparcRegisters.floatingPointRegisters[registerIndex];    // FP registers are stored in an array.
	}
	return 0;
}



/*
 * Stores a 32-bit value to a SPARC register.
 */
int setRegister(char* sparcRegister, unsigned long registerValue)
{
	char registerType, sparcRegisterString[6];
	unsigned short registerIndex, charIndex = 0;
	unsigned long* previousWindowPointer = getWindowPointer(-1);            // Previous window pointer will be useful to access OUT registers shared from previous adjacent window. 

        /* 
         Register name is copied to a temporary variable to avoid segmentation fault.
         Program core dump analysis shows that it faults while trying to modify a string
         literal from an invocation like setRegister("%o7", registerValue) from CALL instruction 
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
		return RET_SUCCESS;             // %g0 always contains zero, by definition.

	if(!strcmp(sparcRegister, "psr"))
	{
                if(!sparcRegisters.psr.s)
                {
                    setTrapCode(PRIVILEGED_INSTRUCTION, "Attempt to write PSR register from user mode");
                    return RET_TRAP;
                }
                
                if((registerValue & 0x0000001F /* Extract CWP field from PSR */) >= sparcRegisters.registerWindows)
                {
                    setTrapCode(ILLEGAL_INSTRUCTION, "Attempt to set CWP >= NWINDOWS");
                    return RET_TRAP;
                }
                sparcRegisters.psr = castUnsignedLongToPSR(registerValue);      // Cast 4 byte long integer to PSR structure.
                sparcRegisters.cwptr = sparcRegisters.registerSet + sparcRegisters.psr.cwp * REGISTER_WINDOW_WIDTH;     // Writing to PSR's CWP field requires pointer to current register window (cwptr) to be adjusted, too, at the same time.
                return RET_SUCCESS;
	}
        
        if(!strcmp(sparcRegister, "fsr"))
        {
                sparcRegisters.fsr = castUnsignedLongToFSR(registerValue);      // Cast 4 byte long integer to FSR structure.
                return RET_SUCCESS;;
        }
		
	if(!strcmp(sparcRegister, "wim"))
        {
                if(!sparcRegisters.psr.s)
                {
                    setTrapCode(PRIVILEGED_INSTRUCTION, "Attempt to write WIM register from user mode");
                    return RET_TRAP;
                }
                sparcRegisters.wim = registerValue;
                return RET_SUCCESS;
        }

	if(!strcmp(sparcRegister, "tbr"))
        {
		if(!sparcRegisters.psr.s)
                {
                    setTrapCode(PRIVILEGED_INSTRUCTION, "Attempt to write TBR register from user mode");
                    return RET_TRAP;
                }
                sparcRegisters.tbr = registerValue & 0xFFFFF000;                // TBR's lower 12 bits are read-only.
                return RET_SUCCESS;
        }

	if(!strcmp(sparcRegister, "y"))
        {
		sparcRegisters.y = registerValue;
                return RET_SUCCESS;
        }

	if(!strcmp(sparcRegister, "pc"))
        {
		sparcRegisters.pc = registerValue;
                return RET_SUCCESS;
        }

	if(!strcmp(sparcRegister, "npc"))
        {
		sparcRegisters.npc = registerValue;
                return RET_SUCCESS;
        }
        
        if((sparcRegister[0] == 'a') && (sparcRegister[1] == 's') && (sparcRegister[2] == 'r'))
        {
            char* asrRegister = (char*)malloc(3);
            charIndex = 0;
            if(sparcRegister[3] >= '0' && sparcRegister[3] <= '9')
                asrRegister[charIndex++] = sparcRegister[3];            // If 4th character is a digit, copy it to asrRegister, i.e. 2 is taken out from %asr23.
            if(sparcRegister[4] >= '0' && sparcRegister[4] <= '9')
                asrRegister[charIndex++] = sparcRegister[4];            // If 4th character is a digit, copy it to asrRegister, i.e. 3 is taken out from %asr23.
            asrRegister[charIndex] = '\0';
            
            registerIndex = strtoul(asrRegister, NULL, 0);              // Convert asrRegister to integer index, i.e. 23 is obtained from %asr23.
            sparcRegisters.asrRegisters[registerIndex] = registerValue; 
            return RET_SUCCESS;
        }

        
        registerType = sparcRegister[0];                // First character will indicate type of register, i.e. global(g), local(l), in(i), out(o)
	registerIndex = sparcRegister[1] - '0';         // Integer register index will be a single digit ranging between [0-7]
        
        /* If second character is non-null, calculate index as (<firstDigit> * 10 + <secondDigit>)
         * e.g. %f23, firstDigit = 2, secondDigit = 3
         * registerIndex = (2 * 10 + 3) = 23
         */
        if(sparcRegister[2] != '\0')
            registerIndex = registerIndex * 10 +  (sparcRegister[2] - '0');
	
	switch (registerType)
	{
	case 'o':
		*(previousWindowPointer + registerIndex) = registerValue;       // OUT register are shared with IN registers from previous window.
		return RET_SUCCESS;
	case 'l':
		*(sparcRegisters.cwptr + 8 + registerIndex) = registerValue;    // In a register window, LOCAL registers follow IN registers.
		return RET_SUCCESS;
	case 'i':
		*(sparcRegisters.cwptr + registerIndex) = registerValue;        // In a register window, IN registers precede LOCAL registers.
		return RET_SUCCESS;
	case 'g':
		*(sparcRegisters.globalRegisters + registerIndex) = registerValue;      // GLOBAL registers are allocated separately from register window.
		return RET_SUCCESS;
        case 'f':
		sparcRegisters.floatingPointRegisters[registerIndex]= registerValue;    // FP registers are stored in an array.
		return RET_SUCCESS;
	}
        return RET_FAILURE;
}



/*
 * Returns PSR register's content as 32-bit integer value.
 */
unsigned long getPSR()
{
    return castPSRToUnsignedLong(sparcRegisters.psr);
}



/*
 * Sets PSR register's fields as decoded from a 32-bit integer value.
 */
void setPSR(unsigned long psrValue)
{
    sparcRegisters.psr = castUnsignedLongToPSR(psrValue);
}



/*
 * Returns TBR register's content as 32-bit integer value.
 */
unsigned long getTBR()
{
    return sparcRegisters.tbr;
}



/*
 * To set tbr.tt field from trap.c: setTrapCode()for an instruction which, itself, attempts to 
 * write TBR in user mode, setTBR() has to be used instead of setRegister("tbr", registerValue)
 * Because, setRegister() checks whether the processor is in user/supervisor mode. If it is the
 * reason for the privileged_instruction trap caused by original instruction, the execution
 * comes back at the same point from where it was initiated. Hence, the result is an infinite loop.
 */

void setTBR(unsigned long tbrValue)
{
    sparcRegisters.tbr = tbrValue;
}



/*
 * Performs actions associated with SAVE instruction.
 */
int saveRegisters()
{
	unsigned long regPSR, regWIM;
	struct processor_status_register psr;
	short nextCWP;

	regPSR = getRegister("psr");
        psr = castUnsignedLongToPSR(regPSR);
	regWIM = getRegister("wim");

	if(psr.cwp == 0)
		nextCWP = sparcRegisters.registerWindows - 1;           // Wrap around when CWP = 0
	else
		nextCWP = sparcRegisters.psr.cwp - 1;                   // Decrease CWP if non-zero.

        // Test for window overflow.
	if(getBit(regWIM, nextCWP))
	{
		setTrapCode(WINDOW_OVERFLOW, "Register window overflow has occurred");
		return RET_TRAP;
	}
        
        // If window_overflow does not take place.
	else
	{
		sparcRegisters.cwptr = getWindowPointer(-1);            // Decrease pointer to current register window.
                sparcRegisters.psr.cwp = nextCWP;                       // Update CWP.

		return RET_SUCCESS;
	}
}



/*
 * Performs actions associated with RESTORE instruction.
 */
int restoreRegisters()
{
	unsigned long regPSR, regWIM;
	struct processor_status_register psr;
	short nextCWP;

	regPSR = getRegister("psr");
        psr = castUnsignedLongToPSR(regPSR);
	regWIM = getRegister("wim");

	if(psr.cwp == (sparcRegisters.registerWindows - 1))
		nextCWP = 0;                                            // Wrap around when CWP = <Number of register windows>.
	else
		nextCWP = sparcRegisters.psr.cwp + 1;                   // Increase CWP if not equal to <Number of register windows>.


        // Test for window underflow.
	if(getBit(regWIM, nextCWP))
	{
		setTrapCode(WINDOW_UNDERFLOW, "Register window underflow has occurred");
		return RET_TRAP;
	}
        
        // If window_underflow does not take place.
	else
	{
		sparcRegisters.cwptr = getWindowPointer(1);             // Increase pointer to current register window.
                sparcRegisters.psr.cwp = nextCWP;                       // Update CWP.

		return RET_SUCCESS;
	}
}



/*
 * Decreases PSR.CWP by 1, wraps around if needed.
 * Also adjusts pointer to current register window (cwptr)
 */
void nextWindow()
{
	if(sparcRegisters.psr.cwp == 0)
		sparcRegisters.psr.cwp = sparcRegisters.registerWindows - 1;
	else
		sparcRegisters.psr.cwp = sparcRegisters.psr.cwp - 1;
	
        sparcRegisters.cwptr = getWindowPointer(-1);
}



/*
 * Unpacks PSR fields from a 32-bit value and assigns 
 * to relevant members of PSR structure. Bit masking has
 * been done according to PSR field definition as
 * specified by SPARC v8 manual. Returns an instance
 * of PSR structure filled with supplied information.
 */
struct processor_status_register castUnsignedLongToPSR(unsigned long registerValue)
{
	struct processor_status_register psr;

	psr.cwp = registerValue & 0x00000001F;
	psr.et = (registerValue & 0x00000020) >> 5;
	psr.ps = (registerValue & 0x00000040) >> 6;
	psr.s = (registerValue & 0x00000080) >> 7;
	psr.pil = (registerValue & 0x00000F00) >> 8;
	psr.ef = (registerValue & 0x00001000) >> 12;
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




/*
 * Packs PSR fields to a 32-bit value. Bit shifting has
 * been done according to PSR field definition as
 * specified by SPARC v8 manual.
 */
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




/*
 * Unpacks FSR fields from a 32-bit value and assigns 
 * to relevant members of FSR structure. Bit masking has
 * been done according to FSR field definition as
 * specified by SPARC v8 manual. Returns an instance
 * of FSR structure filled with supplied information.
 */
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
        fsr.tem = (registerValue & 0x0F800000) >> 23;
        fsr.uhigh = (registerValue & 0x30000000) >> 28;
        fsr.rd = (registerValue & 0xC0000000) >> 30;

	return fsr;
}



/*
 * Packs FSR fields to a 32-bit value. Bit shifting has
 * been done according to FSR field definition as
 * specified by SPARC v8 manual.
 */
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



/*
 * Returns next register in a set of registers of similar type.
 * e.g. If %f24 is passed as argument, %f25 is returned.
 * It finds use in implementing double word (DWORD) opcodes.
 */
char* getNextRegister(char* sparcRegister)
{
    char nextRegister[15];
    /*
     *  In earlier implementation, nextRegister[6] was declared. When a garbage, i.e. a  string NOT
     *  representing a register was being passed from execute.c (convertDouble.doubleToHex[1] = getRegister(getNextRegister(tokens[1])))
     *  e.g. 0xFFFFFF90 (length = 10), it was passing out of array boundary.
     */
    unsigned short nextRegisterIndex, characterCount = 2;               // Because register name will be at least two character long, e.g. g5
    
    strcpy(nextRegister, sparcRegister);                // Copy the register name to prevent alteration of actual register
    sparcRegister = nextRegister;
    
    nextRegisterIndex = sparcRegister[1] - '0';         // First digit is taken out as integer, e.g. '2' from %f23
    
    /* If second character is non-null, calculate index as (<firstDigit> * 10 + <secondDigit>)
     * e.g. %f23, firstDigit = 2, secondDigit = 3
     * registerIndex = (2 * 10 + 3) = 23
     */
    if(sparcRegister[2] != '\0')
        nextRegisterIndex = nextRegisterIndex * 10 +  (sparcRegister[2] - '0');
    
    nextRegisterIndex++;                                // Next register is calculated, e.g. 25 for 24.
    sparcRegister[1] = nextRegisterIndex / 10 + '0';    // 1's digit converted to character.
    
    // If a 10's digit is present, it is converted to character.
    if(nextRegisterIndex % 10 != 0)
    {
        sparcRegister[characterCount] = nextRegisterIndex % 10 + '0';
        characterCount++;
    }
    sparcRegister[characterCount] = '\0';
    
    return sparcRegister;
}
