#include <bits.h>
#include <string.h>
#include <stdio.h>
#include <misc.h>
#include <trap.h>
#include <constants.h>



#define FORCE_CAST(var, type)			 *(type*)&var
#define SIZEOF_INTEGER_REGISTER  	 	 4
#define SIZEOF_WIM_REGISTER     		 4
#define REGISTER_WINDOW_WIDTH    		16
#define GLOBAL_REGISTERS       		 	 8



// GCC is not being able to pack this embedded structure contained in processor_status_register, hence disabled
/*struct integer_condition_codes
{
      unsigned int c:1; 
      unsigned int v:1;
      unsigned int z:1;
      unsigned int n:1;
}__attribute__ ((__packed__));*/



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
	  
	//struct integer_condition_codes icc;

	unsigned int ver:4;
	unsigned int impl:4;
}__attribute__ ((__packed__));



struct registers
{
       unsigned long* registerSet;
       unsigned long* globalRegisters;
       unsigned long* cwptr;
	   unsigned long wim, tbr, y, pc, npc;
	   unsigned short registerWindows;
       struct processor_status_register psr;
};



void initializeRegisters();
void resetSimulator();
char* displayRegister(unsigned long registerValue);
unsigned long* getWindowPointer(int direction);
unsigned long getRegister(char* sparcRegister);
void setRegister(char* sparcRegister, unsigned long registerValue);
int saveRegisters();
int restoreRegisters();
