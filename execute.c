#include <execute.h>



int executeInstruction(char* disassembledInstruction)
{
	char tokens[10][20];
	char* token;
	short count = 0, isFormatIIIOpcodeFound = -1;
	unsigned long regPC, regnPC, regPSR;
	long regRS1, reg_or_imm, regRD, simm13;
	
	regnPC = getRegister("npc");
	regPSR = getRegister("psr");
	struct processor_status_register psr = FORCE_CAST(regPSR, struct processor_status_register);

	// Don't include '%hi' in the delimiter string as it strips off 'hi' from 'sethi' instruction
	strcpy(tokens[0], strtok(disassembledInstruction, " ,+[]()"));
	do
	{
		// Delimiter string altered to escape %hi directive
		token = strtok(NULL, " ,+[]%hi()");
		if(token != NULL)
			strcpy(tokens[++count], token);
	}
	while(token);

	int i;
	for(i = 0; i <= count; i++)
		printf("tokens[%d]: %s (%d)\n", i, tokens[i], strlen(tokens[i]));
		
	// Format - I instruction
	if(!strcmp(tokens[0], "call"))
	{
		unsigned long displacement = strtol(tokens[1], NULL, 0);
		setRegister("pc", regnPC);
		setRegister("%o7", displacement);
		setRegister("npc", displacement);
		return RET_SUCCESS;
	}
	
	
	// Format - II instruction
	if(!strcmp(tokens[0], "sethi"))
	{
		setRegister(tokens[2], strtol(tokens[1], NULL, 0));
		setRegister("pc", regnPC);
		setRegister("npc", regnPC + 4);
		return RET_SUCCESS;
	}
	
	
	if(!strcmp(tokens[0], "nop"))
	{
		setRegister("pc", regnPC);
		setRegister("npc", regnPC + 4);
		return RET_SUCCESS;
	}
	
	
	if(!strcmp(tokens[0], "unimp"))
	{
		printf("Program exited normally\n");
		return RET_FAILURE;
	}
	

	if(!strcmp(tokens[0], "ba"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			regPC = strtol(tokens[2], NULL, 0);
			setRegister("pc", regPC);
			setRegister("npc", regPC + 4);
		}
		else
		{
			// Annul bit = 0
			setRegister("pc", regnPC);
			regnPC = strtol(tokens[1], NULL, 0);
			setRegister("npc", regnPC);
		}
		return RET_SUCCESS;
	}


	if(!strcmp(tokens[0], "bn"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			setRegister("pc", regnPC + 4);
			setRegister("npc", regnPC + 8);
		}
		else
		{
			// Annul bit = 0
			setRegister("pc", regnPC);
			setRegister("npc", regnPC + 4);
		}
		return RET_SUCCESS;
	}
		

	if(!(strcmp(tokens[0], "bne") &&
		strcmp(tokens[0], "be") &&
		strcmp(tokens[0], "bg") &&
		strcmp(tokens[0], "ble") &&
		strcmp(tokens[0], "bge") &&
		strcmp(tokens[0], "bl") &&
		strcmp(tokens[0], "bgu") &&
		strcmp(tokens[0], "bleu") &&
		strcmp(tokens[0], "bcc") &&
		strcmp(tokens[0], "bcs") &&
		strcmp(tokens[0], "bpos") &&
		strcmp(tokens[0], "bneg") &&
		strcmp(tokens[0], "bvc") &&
		strcmp(tokens[0], "bnvs")))
	{
		short condition;

		if(!strcmp(tokens[0], "bne"))
			condition = ~psr.z;
		else
			if(!strcmp(tokens[0], "be"))
				condition = psr.z;
			else
				if(!strcmp(tokens[0], "bg"))
					condition = ~(psr.z | (psr.n ^ psr.v));
				else
					if(!strcmp(tokens[0], "ble"))
						condition = psr.z | (psr.n ^ psr.v);
					else
						if(!strcmp(tokens[0], "bge"))
							condition = ~(psr.n ^ psr.v);
						else
							if(!strcmp(tokens[0], "bl"))
								condition = psr.n ^ psr.v;
							else
								if(!strcmp(tokens[0], "bgu"))
									condition = ~(psr.c | psr.z);
								else
									if(!strcmp(tokens[0], "bleu"))
										condition = psr.c | psr.z;
									else
										if(!strcmp(tokens[0], "bcc"))
											condition = ~psr.c;
										else
											if(!strcmp(tokens[0], "bcs"))
												condition = psr.c;
											else
												if(!strcmp(tokens[0], "bpos"))
													condition = ~psr.n;
												else
													if(!strcmp(tokens[0], "bneg"))
														condition = psr.n;
													else
														if(!strcmp(tokens[0], "bvc"))
															condition = ~psr.v;
														else
															if(!strcmp(tokens[0], "bvs"))
																condition = psr.v;

		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			if(condition)
			{
				// Branch taken
				setRegister("pc", regnPC);
				regnPC = strtol(tokens[2], NULL, 0);
				setRegister("npc", regnPC);
			}
			else
			{
				// Branch NOT taken
				setRegister("pc", regnPC + 4);
				setRegister("npc", regnPC + 8);
			}
		}
		else
		{
			// Annul bit = 0
			if(condition)
			{
				// Branch taken
				setRegister("pc", regnPC);
				regnPC = strtol(tokens[1], NULL, 0);
				setRegister("npc", regnPC);
			}
			else
			{
				// Branch NOT taken
				setRegister("pc", regnPC);
				setRegister("npc", regnPC + 4);
			}
		}

		return RET_SUCCESS;
	}


	// Format - III instruction

	// Decode Format - III operands for instruction having
	// follwing format: <opcode> <regRS1>, <reg_or_imm>, <regRD>
	regRS1 = getRegister(tokens[1]);
	reg_or_imm = getReg_Or_ImmValue(tokens[2]);

	if(!strcmp(tokens[0], "add"))
		setRegister(tokens[3], regRS1 + reg_or_imm);


	if(!strcmp(tokens[0], "sub"))
		setRegister(tokens[3], regRS1 - reg_or_imm);


	if(!strcmp(tokens[0], "subcc"))
	{
		regRD = regRS1 - reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICC(regRS1, reg_or_imm, regRD);
	}


	if(!strcmp(tokens[0], "and"))
		setRegister(tokens[3], regRS1 & reg_or_imm);


	if(!strcmp(tokens[0], "andcc"))
	{
		regRD = regRS1 & reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICC(regRS1, reg_or_imm, regRD);
	}


	if(!strcmp(tokens[0], "or"))
		setRegister(tokens[3], regRS1 | reg_or_imm);


	if(!strcmp(tokens[0], "xnor"))
		setRegister(tokens[3], ~(regRS1 ^ reg_or_imm));


	if(!strcmp(tokens[0], "sll"))
		setRegister(tokens[3], regRS1 << reg_or_imm);


	if(!strcmp(tokens[0], "srl"))
		setRegister(tokens[3], regRS1 >> reg_or_imm);


	setRegister("pc", regnPC);
	setRegister("npc",regnPC + 4);
	return RET_SUCCESS;


	//return RET_FAILURE;
}



long getReg_Or_ImmValue(char* reg_or_imm)
{
	if((reg_or_imm[0] == '%') || (reg_or_imm[0] == 'g') || (reg_or_imm[0] == 'o') ||
			(reg_or_imm[0] == 'l') || (reg_or_imm[0] == 'i'))
		return getRegister(reg_or_imm);
	else
		// Note: stroul() has been used here to interpret the bit pattern
		// as sogned long instead of unsigned long as translated bt strol()
		return strtoul(reg_or_imm, NULL, 0);
}



void updateICC(long regRS1, long reg_or_imm, long regRD)
{
	// Disambiguation between CARRY and OVERFLOW flag:
	// 1. http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
	// 2. http://www.c-jump.com/CIS77/CPU/Overflow/lecture.html

	unsigned long regPSR = getRegister("psr");

	// Set ICC_ZERO bit
	if(regRD == 0)
		regPSR = setBit(regPSR, ICC_ZERO);
	else
		regPSR = clearBit(regPSR, ICC_ZERO);

	// Set ICC_NEGATIVE bit
	if(regRD < 0)
		regPSR = setBit(regPSR, ICC_NEGATIVE);
	else
		regPSR = clearBit(regPSR, ICC_NEGATIVE);

	// Set ICC_OVERFLOW bit
	if(((regRS1 < 0) && (reg_or_imm < 0) && (regRD > 0)) || ((regRS1 > 0) && (reg_or_imm > 0) && (regRD < 0)))
		regPSR = setBit(regPSR, ICC_OVERFLOW);
	else
		regPSR = clearBit(regPSR, ICC_OVERFLOW);

	// Set PSR back to modify ICC bits
	setRegister("psr", regPSR);
}



/*int main()
{
	char disassembledInstruction[] = "sethi %hi(0x40000000), %g7";
	executeInstruction(disassembledInstruction, 1);
	return RET_SUCCESS 0;
}*/
