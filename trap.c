#include <trap.h>



short isIUInErrorMode =0;



/*
 * Sets appropriate trap code of TBR, PC and nPC on
 * occurrence of a trap. Displays the type of trap taken place and the reason behind.
 * Also sets PSR.S, PSR.ET, PSR.PS fields and advances register window.
 * Actions associated with a trap can be referred from 'Traps' section of SPARC v8 manual.
 */
int setTrapCode(short trapCode, char* reason)
{
        unsigned long regPSR, regTBR, tbr_tt;
        struct processor_status_register psr;

        regPSR = getPSR();
        regTBR = getTBR();
        psr = castUnsignedLongToPSR(regPSR);
        
        // If trap occurs while trap being disabled, Integer Unit goes to error mode.
        if(!psr.et)
        {
            isIUInErrorMode = 1;
            return RET_FAILURE;
        }
        
        psr.et = 0;             // Disable trap.
        psr.ps = psr.s;         // Save supervisor bit.
        psr.s = 1;              // Switch to supervisor mode.
        setPSR(castPSRToUnsignedLong(psr));
        nextWindow();           // Advance to next window.
        setRegister("l1", getRegister("pc"));           // Save PC to be used on return from trap handler.
        setRegister("l2", getRegister("npc"));          // Save nPC to be used on return from trap handler.
        
        // Display trap occurred along with the reason.
	switch(trapCode)
	{
                case ILLEGAL_INSTRUCTION: color("\tTrap: illegal_instruction, ", 0); tbr_tt = ILLEGAL_INSTRUCTION; break;
                case PRIVILEGED_INSTRUCTION: color("\tTrap: privileged_instruction, ", 0); tbr_tt = PRIVILEGED_INSTRUCTION; break;
                case FP_DISABLED: color("\tTrap: fp_disabled, ", 0); tbr_tt = FP_DISABLED; break;
		case WINDOW_OVERFLOW: color("\tTrap: window_overflow, ", 0); tbr_tt = WINDOW_OVERFLOW; break;
		case WINDOW_UNDERFLOW: color("\tTrap: window_underflow, ", 0); tbr_tt = WINDOW_UNDERFLOW; break;
                case MEM_ADDRESS_NOT_ALIGNED: color("\tTrap: mem_address_not_aligned, ", 0); tbr_tt = MEM_ADDRESS_NOT_ALIGNED; break;
                case FP_EXCEPTION: color("\tTrap: fp_exception, ", 0); tbr_tt = FP_EXCEPTION; break;
                case TAG_OVERFLOW: color("\tTrap: tag_overflow, ", 0); tbr_tt = TAG_OVERFLOW; break;
                case UNIMPLEMENTED_FLUSH: color("\tTrap: unimplemented_flush, ", 0); tbr_tt = UNIMPLEMENTED_FLUSH; break;
                case DIVISION_BY_ZERO: color("\tTrap: division_by_zero, ", 0); tbr_tt = DIVISION_BY_ZERO; break;
                case DESIGN_UNIMP: color("\tTrap: design_unimp_exception, ", 0); tbr_tt = DESIGN_UNIMP; break;
	}
        
        color(reason, 0); printf("\n");
        regTBR = regTBR & 0xFFFFF00F;      // TBR.TT field is cleared.
        regTBR = regTBR | (tbr_tt << 4);   // TBR.TT field is set.
        setTBR(regTBR);                    // Set TBR.
        setRegister("pc", regTBR);         // Set PC to the entry point of trap handler.
        setRegister("npc", regTBR + 4);    // Set nPC
        
        return RET_SUCCESS;
}



/*
 * Sets appropriate floating-point trap code of FSR on
 * occurrence of a floating-point trap. Displays the type 
 * of trap taken place and the reason behind. Actions 
 * associated with a trap can be referred from 'Traps' 
 * section of SPARC v8 manual.
 */
int setFTTTrapCode(short fpTrapCode, short trapCode, char* reason)
{
        unsigned long fsr, fsr_ftt;
        
        fsr = getRegister("fsr");
        
        if(setTrapCode(trapCode, reason) == RET_FAILURE)
            return RET_FAILURE;
        
        // Display trap occurred along with the reason.
	switch(fpTrapCode)
	{
                case IEEE_754_EXCEPTION: fsr_ftt = IEEE_754_EXCEPTION; break;
                case INVALID_FP_REGISTER: fsr_ftt = INVALID_FP_REGISTER; break;
	}
        
        fsr = fsr & 0xFFFE3FFF;         // FSR.FTT field is cleared.
        fsr = fsr | (fsr_ftt << 14);    // FSR.FTT field is set.
        setRegister("fsr", fsr);        // Set FSR.
        
        return RET_SUCCESS;
}



/*
 * Detects and raises ieee_754_exception in
 * floating-point operation.
 */
int is_ieee_754_trap()
{
    unsigned long fsr;
    struct floating_point_state_register fsr_reg;
    
    fsr = getRegister("fsr");
    
    if (fetestexcept(FE_INEXACT))
      fsr = setBit(fsr, NXC);
    
    if (fetestexcept(FE_DIVBYZERO))
      fsr = setBit(fsr, DZC);

    if (fetestexcept(FE_UNDERFLOW))
      fsr = setBit(fsr, UFC);

    if (fetestexcept(FE_OVERFLOW))
      fsr = setBit(fsr, OFC);
    
    if (fetestexcept(FE_INVALID))
      fsr = setBit(fsr, NVC);
    
    setRegister("fsr", fsr);                                    // Set fsr.cexc field
    fsr_reg = castUnsignedLongToFSR(fsr);
    
    // Check whether specific exception is masked.
    if(fsr_reg.tem & fsr_reg.cexc)
    {
        setFTTTrapCode(IEEE_754_EXCEPTION, FP_EXCEPTION, "IEEE 754 trap detected, check fsr.cexc");       // Raise ieee_754_exception trap
        return RET_TRAP;
    }
    
    // Accumulate the exception, if masked.
    else
    {
        fsr_reg.aexc = fsr_reg.aexc | fsr_reg.cexc;
        setRegister("fsr", castFSRToUnsignedLong(fsr_reg));     // Set fsr.aexc field
        return RET_SUCCESS;
    }
}



/*
 * Sets FSR.NVC bit if invalid_exception occurs.
 * Also raises corresponding floating-point exception.
 */
void raise_invalid_exception(char* reason)
{
    unsigned long fsr;
    
    fsr = getRegister("fsr");   // get FSR value.
    fsr = setBit(fsr, NVC);     // Set FSR.NVC bit.
    setRegister("fsr", fsr);    // Set FSR.
    setFTTTrapCode(IEEE_754_EXCEPTION, FP_EXCEPTION, reason);           // Raise invalid_exception.
}



/*
 * Returns 1, if memoryAddress is not aligned on <alignment> boundary.
 * Returns 0, if memoryAddress is aligned on <alignment> boundary.
 * It's useful to prevent loading mis-aligned memoryAddress
 * in double word (DWORD) instructions by raising
 * memory_address_not_aligned traps.
 */
int is_mem_address_not_aligned(unsigned long memoryAddress, int alignment)
{
    switch(alignment)
    {
        case 2: if((((unsigned long)(memoryAddress / 2)) * 2) == memoryAddress) return 0; else return 1;
        case 4: if((((unsigned long)(memoryAddress / 4)) * 4) == memoryAddress) return 0; else return 1;
        case 8: if((((unsigned long)(memoryAddress / 8)) * 8) == memoryAddress) return 0; else return 1;
    }
    return 0;
}



/*
 * Returns 1, if sparcRegister is mis-aligned (Odd).
 * Returns 0, if sparcRegister is not mis-aligned (Even).
 * It's useful to prevent loading mis-aligned registers
 * in double word (DWORD) instructions by raising
 * illegal_instruction and invalid_fp_register traps.
 */
int is_register_mis_aligned(char* sparcRegister)
{
    unsigned short registerIndex;
    
    registerIndex = sparcRegister[1] - '0';             // First digit is taken out as integer, e.g. '2' from %f23
    
    /* If second character is non-null, calculate index as (<firstDigit> * 10 + <secondDigit>)
     * e.g. %f23, firstDigit = 2, secondDigit = 3
     * registerIndex = (2 * 10 + 3) = 23
     */
    if(sparcRegister[2] != '\0')
        registerIndex = registerIndex * 10 +  (sparcRegister[2] - '0');
    
    // Is register mis-aligned?
    if(registerIndex % 2)
        return 1;       // Odd register
    else
        return 0;       // Even register
}



/*
 * Sets 1, if Integer Unit (IU) is in error mode.
 * Sets 0, otherwise.
 */
void setIUErrorMode(short errorMode)
{
    isIUInErrorMode = errorMode;
}



/*
 * Returns 1, if Integer Unit (IU) is in error mode.
 * Returns 0, otherwise.
 */
short getIUErrorMode()
{
    return isIUInErrorMode;
}
