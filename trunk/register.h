#ifndef REGISTER_H
#define REGISTER_H

#include <bits.h>
#include <string.h>
#include <stdio.h>
#include <misc.h>
#include <trap.h>
#include <constants.h>



/* FORCE_CASTING is dangerous and highly unpredictable due to arrangement of 
 bit field fields in memory. While it works like a charm on Intel x86, 
 Ubuntu 10.10, gcc 4.4 platform, it fails for SPARC v9, Solaris 10, gcc 4.7.2.
 On Little Endian (Intel) platform, fields are arranged in order from first to 
 last, LSB to MSB. On Big Endian (SPARC) platform, gcc arranges bit field fields
 in a random fashion. Test shows setting only CWP:5 bits to all 1's, i.e. 0x1F
 gives rise to 0x000000F8 bit pattern in PSR structure. Due to C99 standard being
 silent about specifics of bit fields, using this macro is non-portable and 
 compiler dependent. Hence, appropriate bit-masking and bit-shifting operators
 will be used to explicitely disassemble bits in a bit field stream and to assign 
 to PSR structure. 
#define FORCE_CAST(var, type)			 *(type*)&var */

#define SIZEOF_INTEGER_REGISTER  	 	 4
#define SIZEOF_WIM_REGISTER     		 4
#define REGISTER_WINDOW_WIDTH    		16
#define GLOBAL_REGISTERS       		 	 8



struct processor_status_register
{
	unsigned int cwp:5;
	unsigned int et:1;
	unsigned int ps:1;
	unsigned int s:1;
	unsigned int pil:4;
	unsigned int ef:1;
	unsigned int ec:1;
	unsigned int reserved:6;
	   
	unsigned int c:1;
	unsigned int v:1;
	unsigned int z:1;
	unsigned int n:1;

	unsigned int ver:4;
	unsigned int impl:4;
}__attribute__ ((__packed__));



struct floating_pont_state_register
{
	unsigned int cexc:5;
	unsigned int aexc:5;
	unsigned int fcc:2;
	unsigned int ulow:1;
	unsigned int qne:1;
	unsigned int ftt:3;
	unsigned int ver:3;
	unsigned int res:2;
	unsigned int ns:1;
	unsigned int tem:5;
	unsigned int uhigh:2;
	unsigned int rd:2;
}__attribute__ ((__packed__));



struct registers
{
       unsigned long* registerSet;
       unsigned long* globalRegisters;
       unsigned long* cwptr;
       unsigned long wim, tbr, y, pc, npc;
       unsigned long asrRegisters[32];
       unsigned long floatingPointRegisters[32];
       unsigned short registerWindows;
       
       /* Though Intel x86 architecture allows un-aligned memory access, SPARC mandates memory accesses to be 8 byte aligned.
        Without __attribute__ ((aligned (8))) or a preceding dummy byte e.g. unsigned short dummyByte, the code below crashes 
        with a dreaded Bus error and Core dump. For more details, follow the links below:
        
        http://blog.jgc.org/2007/04/debugging-solaris-bus-error-caused-by.html
        https://groups.google.com/forum/?fromgroups=#!topic/comp.unix.solaris/8SgFiMudGL4 */
       
       struct processor_status_register __attribute__ ((aligned (8))) psr;
       struct floating_pont_state_register __attribute__ ((aligned (8))) fsr;
}__attribute__ ((__packed__));



void initializeRegisters();
void resetSimulator();
char* displayRegister(unsigned long registerValue);
unsigned long* getWindowPointer(int direction);
unsigned long getRegister(char* sparcRegister);
void setRegister(char* sparcRegister, unsigned long registerValue);
int saveRegisters();
int restoreRegisters();
unsigned long castPSRToUnsignedLong(struct processor_status_register psr);
struct processor_status_register castUnsignedLongToPSR(unsigned long registerValue);
char* getNextRegister(char* sparcRegister);

#endif
