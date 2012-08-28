#include <execute.h>



int executeInstruction(char* disassembledInstruction)
{
	char tokens[10][20];
	char* token;
	short count = 0, index, isFormatIIIOpcodeFound = -1;
	unsigned long memoryAddress, regPC, regnPC, regPSR, regRS1, reg_or_imm, regRD;
	
	regnPC = getRegister("npc");
	regPSR = getRegister("psr");
	struct processor_status_register psr = FORCE_CAST(regPSR, struct processor_status_register);

	// Don't include '%hi' in the delimiter string as it strips off 'hi' from 'sethi' instruction
	strcpy(tokens[0], strtok(disassembledInstruction, " ,+[]()"));
	do
	{
		// Delimiter string altered to escape %hi directive
		token = strtok(NULL, " ,[]%hi()");
		if(token != NULL)
			strcpy(tokens[++count], token);
	}
	while(token);

	int i;
	for(i = 0; i <= count; i++)
		printf("\ntokens[%d]: %s", i, tokens[i]);
	printf("\n");
		
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
	
	// Decode Format - III operands for instruction having format: <opcode> [address], <regRD>
	index = 1;
	memoryAddress = getAddressValue(tokens, &index);
	
	
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "lduh")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = getQuadWordFromMemory(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		setRegister(tokens[index], word);
		free(dataWord);
	}
	
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ld")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = getQuadWordFromMemory(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[2]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[3]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		setRegister(tokens[index], word);
		free(dataWord);
	}
	
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "jmpl")))
	{
		setRegister(tokens[index], regPC);
		setRegister("pc", regnPC);
		setRegister("npc", memoryAddress);
		return RET_SUCCESS;
	}

	
	if(!isFormatIIIOpcodeFound)
	{
		setRegister("pc", regnPC);
		setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}
	
	
	// Decode Format - III operands for instruction having format: <opcode> <regRD> , [address ]
	index = 2;
	regRD = getRegister(tokens[1]);
	memoryAddress = getAddressValue(tokens, &index);
	
	
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sth")))
	{
		char* halfWord = (char*)&regRD;
		writeMemory(memoryAddress + 1, *halfWord); halfWord++;
		writeMemory(memoryAddress, *halfWord);
	}
	
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "st")))
	{
		char* word = (char*)&regRD;
		writeMemory(memoryAddress + 3, *word); word++;
		writeMemory(memoryAddress + 2, *word); word++;
		writeMemory(memoryAddress + 1, *word); word++;
		writeMemory(memoryAddress, *word);
	}
	
	
	if(!isFormatIIIOpcodeFound)
	{
		setRegister("pc", regnPC);
		setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}
	

	// Decode Format - III operands for instruction having format: <opcode> <regRS1>, <reg_or_imm>, <regRD>
	regRS1 = getRegister(tokens[1]);
	reg_or_imm = getReg_Or_ImmValue(tokens[2]);

	
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "add")))
		setRegister(tokens[3], regRS1 + reg_or_imm);

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sub")))
		setRegister(tokens[3], regRS1 - reg_or_imm);

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "subcc")))
	{
		regRD = regRS1 - reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICC(regRS1, reg_or_imm, regRD);
	}

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "and")))
		setRegister(tokens[3], regRS1 & reg_or_imm);

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "andcc")))
	{
		regRD = regRS1 & reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICC(regRS1, reg_or_imm, regRD);
	}

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "or")))
		setRegister(tokens[3], regRS1 | reg_or_imm);

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xnor")))
		setRegister(tokens[3], ~(regRS1 ^ reg_or_imm));

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sll")))
		setRegister(tokens[3], regRS1 << reg_or_imm);

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "srl")))
		setRegister(tokens[3], regRS1 >> reg_or_imm);


	if(!isFormatIIIOpcodeFound)
	{
		setRegister("pc", regnPC);
		setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}


	printf("Unimplemented in simulator: %s\n", tokens[0]);
	return RET_FAILURE;
}



unsigned long getReg_Or_ImmValue(char* reg_or_imm)
{
	if((reg_or_imm[0] == '%') || (reg_or_imm[0] == 'g') || (reg_or_imm[0] == 'o') ||
			(reg_or_imm[0] == 'l') || (reg_or_imm[0] == 'i'))
		return getRegister(reg_or_imm);
	else
		// Note: stroul() has been used here to interpret the bit pattern
		// as signed long instead of unsigned long as translated by strtol()
		return strtoul(reg_or_imm, NULL, 0);
}



unsigned long getAddressValue(char tokens[][20], unsigned short* index)
{
	unsigned long memoryAddress;

	if(!strcmp(tokens[*index], "["))
		(*index)++;
	
	memoryAddress = getRegister(tokens[*index]);

	if(!strcmp(tokens[++(*index)], "+"))
		memoryAddress += getReg_Or_ImmValue(tokens[++(*index)]);

	(*index) += (!strcmp(tokens[(*index) + 1], "]")) ? 2 : 1;

	return memoryAddress;
}



void updateICC(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD)
{
	// Disambiguation between CARRY and OVERFLOW flag:
	// 1. http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
	// 2. http://www.c-jump.com/CIS77/CPU/Overflow/lecture.html

	unsigned long regPSR;
	unsigned short signBit_regRS1, signBit_reg_or_imm, signBit_regRD, isCarry, isOverflow;
	

	regPSR = getRegister("psr");
	signBit_regRS1 = getBit(regRS1, SIGN_BIT);
	signBit_reg_or_imm = getBit(reg_or_imm, SIGN_BIT);
	signBit_regRD = getBit(regRD, SIGN_BIT);

	// Set ICC_NEGATIVE (n) bit
	regPSR = getBit(regRD, SIGN_BIT) ? setBit(regPSR, ICC_NEGATIVE) : clearBit(regPSR, ICC_NEGATIVE);
	

	// Set ICC_ZERO (z) bit
	regPSR = (regRD == 0) ? setBit(regPSR, ICC_ZERO) : clearBit(regPSR, ICC_ZERO);


	// Set ICC_OVERFLOW (v) bit: Important for SIGNED arithmetic
	regPSR = ((signBit_regRS1 && (!signBit_reg_or_imm & !signBit_regRD)) || (!signBit_regRS1 && (signBit_reg_or_imm & signBit_regRD))) ? setBit(regPSR, ICC_OVERFLOW) : clearBit(regPSR, ICC_OVERFLOW);


	// Set ICC_CARRY (c) bit: Important for UNSIGNED arithmetic
	regPSR = (!signBit_regRS1 && signBit_reg_or_imm) || (signBit_regRD && (!signBit_regRS1 || signBit_reg_or_imm)) ? setBit(regPSR, ICC_CARRY) : clearBit(regPSR, ICC_CARRY);


	// Set PSR back to modify ICC bits
	setRegister("psr", regPSR);
}



/*int main()
{
	char disassembledInstruction[] = "sethi %hi(0x40000000), %g7";
	executeInstruction(disassembledInstruction, 1);
	return RET_SUCCESS 0;
}*/
