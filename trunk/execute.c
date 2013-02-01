#include <execute.h>



struct watchPointInfo* watchInfo;



int executeInstruction(char* disassembledInstruction)
{
	char tokens[10][20];
	char* token;
	short count, index, isFormatIIIOpcodeFound;
	unsigned long memoryAddress, regPC, regnPC, regPSR, regFSR, regRS1, regRS2, reg_or_imm, regRD;
        float float_regRS1, float_regRS2, float_regRD;
        double double_regRS1, double_regRS2, double_regRD;
	
	count = 0;
	isFormatIIIOpcodeFound = -1;
	regPC = getRegister("pc");
	regnPC = getRegister("npc");
	regPSR = getRegister("psr");
        regFSR = getRegister("fsr");
        struct processor_status_register psr = castUnsignedLongToPSR(regPSR);
        struct floating_point_state_register fsr = castUnsignedLongToFSR(regFSR);
        
        // Strip off %hi to differentiate it from SETHI instruction
	for(index = 0; index < strlen(disassembledInstruction) - 3; index++)
	{
		if(disassembledInstruction[index] == '%' && disassembledInstruction[index + 1] == 'h' && disassembledInstruction[index + 2] == 'i')
		{
			disassembledInstruction[index] = ' ';
			disassembledInstruction[index + 1] = ' ';
			disassembledInstruction[index + 2] = ' ';
		}
	}

	strcpy(tokens[0], strtok(disassembledInstruction, " ,+[]()"));
	do
	{
		token = strtok(NULL, " ,[]%()");
		if(token != NULL)
			strcpy(tokens[++count], token);
	}
	while(token);
        
        
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
        
	// Format - I instruction
	if(!strcmp(tokens[0], "call"))
	{
		unsigned long displacement = strtoul(tokens[1], NULL, 0);
		setRegister("pc", regnPC);
		setRegister("npc", displacement);
		setRegister("%o7", regPC);
		return RET_SUCCESS;
	}
        
        
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
	
	
	// Format - II instruction
	if(!strcmp(tokens[0], "sethi"))
	{
		setRegister(tokens[2], strtoul(tokens[1], NULL, 0));
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
		handleTrap(ILLEGAL_INSTRUCTION, regPC);
		return RET_FAILURE;
	}
	

	if(!strcmp(tokens[0], "ba"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			regPC = strtoul(tokens[2], NULL, 0);
			setRegister("pc", regPC);
			setRegister("npc", regPC + 4);
		}
		else
		{
			// Annul bit = 0
			setRegister("pc", regnPC);
			regnPC = strtoul(tokens[1], NULL, 0);
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
		strcmp(tokens[0], "bvs")))
	{
		unsigned short condition;

		if(!strcmp(tokens[0], "bne"))
			condition = !psr.z;
		else
			if(!strcmp(tokens[0], "be"))
				condition = psr.z;
			else
				if(!strcmp(tokens[0], "bg"))
					condition = !(psr.z || (psr.n ^ psr.v));
				else
					if(!strcmp(tokens[0], "ble"))
						condition = psr.z || (psr.n ^ psr.v);
					else
						if(!strcmp(tokens[0], "bge"))
							condition = !(psr.n ^ psr.v);
						else
							if(!strcmp(tokens[0], "bl"))
								condition = psr.n ^ psr.v;
							else
								if(!strcmp(tokens[0], "bgu"))
									condition = !(psr.c || psr.z);
								else
									if(!strcmp(tokens[0], "bleu"))
										condition = psr.c || psr.z;
									else
										if(!strcmp(tokens[0], "bcc"))
											condition = !psr.c;
										else
											if(!strcmp(tokens[0], "bcs"))
												condition = psr.c;
											else
												if(!strcmp(tokens[0], "bpos"))
													condition = !psr.n;
												else
													if(!strcmp(tokens[0], "bneg"))
														condition = psr.n;
													else
														if(!strcmp(tokens[0], "bvc"))
															condition = !psr.v;
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
				regnPC = strtoul(tokens[2], NULL, 0);
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
				regnPC = strtoul(tokens[1], NULL, 0);
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

        
        if(!strcmp(tokens[0], "fba"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			regPC = strtoul(tokens[2], NULL, 0);
			setRegister("pc", regPC);
			setRegister("npc", regPC + 4);
		}
		else
		{
			// Annul bit = 0
			setRegister("pc", regnPC);
			regnPC = strtoul(tokens[1], NULL, 0);
			setRegister("npc", regnPC);
		}
		return RET_SUCCESS;
	}


	if(!strcmp(tokens[0], "fbn"))
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

        
        	if(!(strcmp(tokens[0], "fbu") &&
		strcmp(tokens[0], "fbg") &&
		strcmp(tokens[0], "fbug") &&
		strcmp(tokens[0], "fbl") &&
		strcmp(tokens[0], "fbul") &&
		strcmp(tokens[0], "fblg") &&
		strcmp(tokens[0], "fbne") &&
		strcmp(tokens[0], "fbe") &&
		strcmp(tokens[0], "fbue") &&
		strcmp(tokens[0], "fbge") &&
		strcmp(tokens[0], "fbuge") &&
		strcmp(tokens[0], "fble") &&
		strcmp(tokens[0], "fbule") &&
		strcmp(tokens[0], "fbo")))
	{
		unsigned short condition;

		if(!strcmp(tokens[0], "fbu"))
			condition = (fsr.fcc == FLOAT_UNORDERED);
		else
			if(!strcmp(tokens[0], "fbg"))
				condition = (fsr.fcc == FLOAT_GREATER);
			else
				if(!strcmp(tokens[0], "fbug"))
					condition = (fsr.fcc == FLOAT_GREATER) || (fsr.fcc == FLOAT_UNORDERED);
				else
					if(!strcmp(tokens[0], "fbl"))
						condition = (fsr.fcc == FLOAT_LESSER);
					else
						if(!strcmp(tokens[0], "fbul"))
							condition = (fsr.fcc == FLOAT_LESSER) || (fsr.fcc == FLOAT_UNORDERED);
						else
							if(!strcmp(tokens[0], "fblg"))
								condition = (fsr.fcc == FLOAT_LESSER) || (fsr.fcc == FLOAT_GREATER);
							else
								if(!strcmp(tokens[0], "fbne"))
									condition = (fsr.fcc == FLOAT_LESSER) || (fsr.fcc == FLOAT_GREATER) || (fsr.fcc == FLOAT_UNORDERED);
								else
									if(!strcmp(tokens[0], "fbe"))
										condition = (fsr.fcc == FLOAT_EQUAL);
									else
										if(!strcmp(tokens[0], "fbue"))
											condition = (fsr.fcc == FLOAT_EQUAL) || (fsr.fcc == FLOAT_UNORDERED);
										else
											if(!strcmp(tokens[0], "fbge"))
												condition = (fsr.fcc == FLOAT_EQUAL) || (fsr.fcc == FLOAT_GREATER);
											else
												if(!strcmp(tokens[0], "fbuge"))
													condition = (fsr.fcc == FLOAT_EQUAL) || (fsr.fcc == FLOAT_GREATER) || (fsr.fcc == FLOAT_UNORDERED);
												else
													if(!strcmp(tokens[0], "fble"))
														condition = (fsr.fcc == FLOAT_EQUAL) || (fsr.fcc == FLOAT_LESSER);
													else
														if(!strcmp(tokens[0], "fbule"))
															condition = (fsr.fcc == FLOAT_EQUAL) || (fsr.fcc == FLOAT_LESSER) || (fsr.fcc == FLOAT_UNORDERED);
														else
															if(!strcmp(tokens[0], "fbo"))
																condition = (fsr.fcc == FLOAT_EQUAL) || (fsr.fcc == FLOAT_LESSER) || (fsr.fcc == FLOAT_GREATER);

		if(!strcmp(tokens[1], "a"))
		{
			// Annul bit = 1
			if(condition)
			{
				// Branch taken
				setRegister("pc", regnPC);
				regnPC = strtoul(tokens[2], NULL, 0);
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
				regnPC = strtoul(tokens[1], NULL, 0);
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
        
        
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
	
	// Decode Format - III operands for instruction having format: <opcode> [address], <regRD>
	index = 1;
	memoryAddress = getAddressValue(tokens, &index);
	
	
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldsb")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
                if(getBit(word, 7))
                    word = word | 0xFFFFFF00;
		setRegister(tokens[index], word);
		free(dataWord);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldsh")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
                if(getBit(word, 15))
                    word = word | 0xFFFF0000;
		setRegister(tokens[index], word);
		free(dataWord);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldub")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		setRegister(tokens[index], word);
		free(dataWord);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "lduh")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = readWordAsString(memoryAddress);
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

		dataWord = readWordAsString(memoryAddress);
		word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[2]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[3]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		setRegister(tokens[index], word);
		free(dataWord);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldd")))
	{
		char* dataWord;
		unsigned long word, hexDigit;

		dataWord = readWordAsString(memoryAddress); 
                word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[2]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[3]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		setRegister(tokens[index], word);
                
                dataWord = readWordAsString(memoryAddress + 4); 
                word = 0;
		hexDigit = dataWord[0]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[1]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[2]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
		hexDigit = dataWord[3]; hexDigit = (hexDigit << 24) >> 24; word = (word << 8) | hexDigit;
                setRegister(getNextRegister(tokens[index]), word);
		free(dataWord);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "ldstub")))
	{
                if(isWatchPoint(memoryAddress, regPC))
                {
                    setWatchPointInfo(memoryAddress, (unsigned long)0xFF);
                    return RET_WATCHPOINT;
                }
                setRegister(tokens[index], readByte(memoryAddress));
		writeByte(memoryAddress, 0xFF);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "swap")))
	{
                unsigned long registerContent;
                
                registerContent = getRegister(tokens[index]);
                if(isWatchPoint(memoryAddress, regPC))
                {
                    setWatchPointInfo(memoryAddress, registerContent);
                    return RET_WATCHPOINT;
                }
                setRegister(tokens[index], readWord(memoryAddress));
		writeWord(memoryAddress, registerContent);
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
	
	
	// Decode Format - III operands for instruction having format: <opcode> <regRD> , [address]
	index = 2;
	regRD = getRegister(tokens[1]);
	memoryAddress = getAddressValue(tokens, &index);
	
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "stb")))
	{
                char byte = regRD & 0x000000FF;
                if(isWatchPoint(memoryAddress, regPC))
                {
                    setWatchPointInfo(memoryAddress, (unsigned long)byte);
                    return RET_WATCHPOINT;
                }
                writeByte(memoryAddress, byte);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sth")))
	{
                unsigned short halfWord = regRD & 0x0000FFFF;
                if(isWatchPoint(memoryAddress, regPC))
                {
                    setWatchPointInfo(memoryAddress, regRD);
                    return RET_WATCHPOINT;
                }
                writeHalfWord(memoryAddress, halfWord);
	}
	
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "st")))
	{
                if(isWatchPoint(memoryAddress, regPC))
                {
                    setWatchPointInfo(memoryAddress, regRD);
                    return RET_WATCHPOINT;
                }
                writeWord(memoryAddress, regRD);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "std")))
	{
                unsigned long regNextRD;
                
                regNextRD = getRegister(getNextRegister(tokens[1]));
                if(isWatchPoint(memoryAddress, regPC))
                {
                    setWatchPointInfo(memoryAddress, regRD);
                    return RET_WATCHPOINT;
                }
                else
                    if(isWatchPoint(memoryAddress + 4, regPC))
                    {
                        setWatchPointInfo(memoryAddress, regNextRD);
                        return RET_WATCHPOINT;
                    }
                writeWord(memoryAddress, regRD);
                writeWord(memoryAddress + 4, regNextRD);
	}
	
	
	if(!isFormatIIIOpcodeFound)
	{
		setRegister("pc", regnPC);
		setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}
	
        
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        

	/*
         *  Decode Format - III operands for instruction having format: 
         *  1. <opcode> <regRS1>, <reg_or_imm>, <regRD> 
         *  2. <opcode> <regRS1>, <reg_or_imm>
         *  3. <opcode> <regRS1>, <regRS2>
         */
         
	regRS1 = getRegister(tokens[1]);
        regRS2 = getRegister(tokens[2]);
	reg_or_imm = getReg_Or_ImmValue(tokens[2]);
        
        // Hex to single precision floating-point conversion
        convertFloat.floatToHex = regRS1;
        float_regRS1 = convertFloat.hexToFloat;
        convertFloat.floatToHex = regRS2;
        float_regRS2 = convertFloat.hexToFloat;
        
        // Hex to double precision floating-point conversion
        convertDouble.doubleToHex[0] = regRS1;
        convertDouble.doubleToHex[1] = getRegister(getNextRegister(tokens[1]));
        double_regRS1 = convertDouble.hexToDouble;
        convertDouble.doubleToHex[0] = regRS2;
        convertDouble.doubleToHex[1] = getRegister(getNextRegister(tokens[2]));
        double_regRS2 = convertDouble.hexToDouble;

	
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "add")))
                setRegister(tokens[3], regRS1 + reg_or_imm);

        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "addcc")))
	{
		regRD = regRS1 + reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICCAddSubtract(regRS1, reg_or_imm, regRD);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "addx")))
	{
		regRD = regRS1 + reg_or_imm + psr.c;
		setRegister(tokens[3], regRD); 
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "addxcc")))
	{
		regRD = regRS1 + reg_or_imm + psr.c;
		setRegister(tokens[3], regRD);
		updateICCAddSubtract(regRS1, reg_or_imm, regRD);
	}
        
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sub")))
		setRegister(tokens[3], regRS1 - reg_or_imm);

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "subcc")))
	{
		regRD = regRS1 - reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICCAddSubtract(regRS1, reg_or_imm, regRD);
	}

        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "subx")))
	{
		regRD = regRS1 - reg_or_imm - psr.c;
		setRegister(tokens[3], regRD); 
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "subxcc")))
	{
		regRD = regRS1 - reg_or_imm - psr.c;
		setRegister(tokens[3], regRD);
		updateICCAddSubtract(regRS1, reg_or_imm, regRD);
	}
        
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "umul")))
	{
		unsigned long long extended_regRD;
		unsigned long regY;

		regY = 0, regRD = 0;
		extended_regRD = (unsigned long long)regRS1 * (unsigned long long)reg_or_imm;
		regY = regY | (unsigned long)((extended_regRD & 0xFFFFFFFF00000000ULL) >> 32);
		setRegister("y", regY);
		extended_regRD = extended_regRD & 0x00000000FFFFFFFFULL;
		regRD = regRD | (unsigned long)extended_regRD;
		setRegister(tokens[3], regRD);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "smul")))
	{
		signed long long extended_regRD;
		unsigned long regY;
                signed long signedRegRS1 = (signed long)regRS1;
                signed long signedRegOrImm = (signed long)reg_or_imm;

		regY = 0, regRD = 0;
		extended_regRD = (signed long long)signedRegRS1 * (signed long long)signedRegOrImm;
		regY = regY | (unsigned long)((extended_regRD & 0xFFFFFFFF00000000ULL) >> 32);
		setRegister("y", regY);
		extended_regRD = extended_regRD & 0x00000000FFFFFFFFULL;
		regRD = regRD | (unsigned long)extended_regRD;
		setRegister(tokens[3], regRD);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "umulcc")))
	{
		unsigned long long extended_regRD;
		unsigned long regY;

		regY = 0, regRD = 0;
		extended_regRD = (unsigned long long)regRS1 * (unsigned long long)reg_or_imm;
		regY = regY | (unsigned long)((extended_regRD & 0xFFFFFFFF00000000ULL) >> 32);
		setRegister("y", regY);
		extended_regRD = extended_regRD & 0x00000000FFFFFFFFULL;
		regRD = regRD | (unsigned long)extended_regRD;
		setRegister(tokens[3], regRD);
                updateICCMulDivLogical(regRD);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "smulcc")))
	{
		signed long long extended_regRD;
		unsigned long regY;
                signed long signed_regRS1 = (signed long)regRS1;
                signed long signed_reg_or_imm = (signed long)reg_or_imm;

		regY = 0, regRD = 0;
		extended_regRD = (signed long long)signed_regRS1 * (signed long long)signed_reg_or_imm;
		regY = regY | (unsigned long)((extended_regRD & 0xFFFFFFFF00000000ULL) >> 32);
		setRegister("y", regY);
		extended_regRD = extended_regRD & 0x00000000FFFFFFFFULL;
		regRD = regRD | (unsigned long)extended_regRD;
		setRegister(tokens[3], regRD);
                updateICCMulDivLogical(regRD);
	}

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "udiv")))
	{
		unsigned long long dividend, quotient;
		unsigned long regY;

		regY = getRegister("y");
		dividend = (dividend << 32) | regY;
		dividend = (dividend << 32) | regRS1;
		quotient = dividend / reg_or_imm;

		if(quotient > ULONG_MAX)
			setRegister(tokens[3], 0xFFFFFFFF);
		else
			setRegister(tokens[3], (unsigned long)quotient);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sdiv")))
	{
		signed long long dividend, quotient;
                signed long long signed_reg_or_imm = (signed long long)reg_or_imm;
		unsigned long regY;

		regY = getRegister("y");
		dividend = (dividend << 32) | regY;
		dividend = (dividend << 32) | regRS1; 
		quotient = dividend / signed_reg_or_imm;

		if(quotient > (signed long)0x7FFFFFFF)
			setRegister(tokens[3], 0x7FFFFFFF);    // Positive overflow
                else 
                if(quotient < (signed long)0x80000000)
                        setRegister(tokens[3], 0x80000000);    // Negative underflow
		else
			setRegister(tokens[3], (unsigned long)(quotient & 0x00000000FFFFFFFF));
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "udivcc")))
	{
		unsigned long long dividend, quotient;
		unsigned long regY;

		regY = getRegister("y");
		dividend = (dividend << 32) | regY;
		dividend = (dividend << 32) | regRS1;
		quotient = dividend / reg_or_imm;

		if(quotient > ULONG_MAX)
                {
			setRegister(tokens[3], 0xFFFFFFFF);
                        updateICCMulDivLogical(0xFFFFFFFF);
                }
		else
                {
			setRegister(tokens[3], (unsigned long)quotient);
                        updateICCMulDivLogical((unsigned long)quotient);
                }
	}

        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sdivcc")))
	{
		signed long long dividend, quotient;
                signed long long signed_reg_or_imm = (signed long long)reg_or_imm;
		unsigned long regY;

		regY = getRegister("y");
		dividend = (dividend << 32) | regY;
		dividend = (dividend << 32) | regRS1; 
		quotient = dividend / signed_reg_or_imm;

		if(quotient > 0x7FFFFFFF)
                {
			setRegister(tokens[3], 0x7FFFFFFF);    // Positive overflow
                        updateICCMulDivLogical(0x7FFFFFFF);
                }
                else 
                if(quotient < 0x80000000)
                {
                        setRegister(tokens[3], 0x80000000);    // Negative underflow
                        updateICCMulDivLogical(0x80000000);
                }
		else
                {
			setRegister(tokens[3], (unsigned long)(quotient & 0x00000000FFFFFFFF));
                        updateICCMulDivLogical((unsigned long)(quotient & 0x00000000FFFFFFFF));
                }
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fadds")))
	{
                float_regRD = float_regRS1 + float_regRS2;
                convertFloat.hexToFloat = float_regRD;
		setRegister(tokens[3], convertFloat.floatToHex); 
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "faddd")))
	{
                double_regRD = double_regRS1 + double_regRS2;
                convertDouble.hexToDouble = double_regRD;
		setRegister(tokens[3], convertDouble.doubleToHex[0]);
                setRegister(getNextRegister(tokens[3]), convertDouble.doubleToHex[1]);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fsubs")))
	{
                float_regRD = float_regRS1 - float_regRS2;
                convertFloat.hexToFloat = float_regRD;
		setRegister(tokens[3], convertFloat.floatToHex); 
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fsubd")))
	{
                double_regRD = double_regRS1 - double_regRS2;
                convertDouble.hexToDouble = double_regRD;
		setRegister(tokens[3], convertDouble.doubleToHex[0]);
                setRegister(getNextRegister(tokens[3]), convertDouble.doubleToHex[1]);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fmuls")))
	{
                float_regRD = float_regRS1 * float_regRS2;
                convertFloat.hexToFloat = float_regRD;
		setRegister(tokens[3], convertFloat.floatToHex); 
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fmuld")))
	{
                double_regRD = double_regRS1 * double_regRS2;
                convertDouble.hexToDouble = double_regRD;
		setRegister(tokens[3], convertDouble.doubleToHex[0]);
                setRegister(getNextRegister(tokens[3]), convertDouble.doubleToHex[1]);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fdivs")))
	{
                float_regRD = float_regRS1 / float_regRS2;
                convertFloat.hexToFloat = float_regRD;
		setRegister(tokens[3], convertFloat.floatToHex);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fdivd")))
	{
                double_regRD = double_regRS1 / double_regRS2;
                convertDouble.hexToDouble = double_regRD;
		setRegister(tokens[3], convertDouble.doubleToHex[0]);
                setRegister(getNextRegister(tokens[3]), convertDouble.doubleToHex[1]);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fmovs")))
                setRegister(tokens[2], regRS1);
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fnegs")))
                setRegister(tokens[2], toggleBit(regRS1, SIGN_BIT));
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fabss")))
                setRegister(tokens[2], clearBit(regRS1, SIGN_BIT));
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fcmps")))
	{
            if(float_regRS1 == float_regRS2)
                updateFCC(0);
            else
            if(float_regRS1 < float_regRS2)
                updateFCC(1);
            else
            if(float_regRS1 > float_regRS2)
                updateFCC(2);
            else
                updateFCC(3);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fcmpes")))
	{
            if(float_regRS1 == float_regRS2)
                updateFCC(0);
            else
            if(float_regRS1 < float_regRS2)
                updateFCC(1);
            else
            if(float_regRS1 > float_regRS2)
                updateFCC(2);
            else
                updateFCC(3);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fcmpd")))
	{
            if(double_regRS1 == double_regRS2)
                updateFCC(0);
            else
            if(double_regRS1 < double_regRS2)
                updateFCC(1);
            else
            if(double_regRS1 > double_regRS2)
                updateFCC(2);
            else
                updateFCC(3);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fcmped")))
	{
            if(double_regRS1 == double_regRS2)
                updateFCC(0);
            else
            if(double_regRS1 < double_regRS2)
                updateFCC(1);
            else
            if(double_regRS1 > double_regRS2)
                updateFCC(2);
            else
                updateFCC(3);
	}
        
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "and")))
		setRegister(tokens[3], regRS1 & reg_or_imm);

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "andcc")))
	{
		regRD = regRS1 & reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICCMulDivLogical(regRD);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "andncc")))
	{
		regRD = regRS1 & (~reg_or_imm);
		setRegister(tokens[3], regRD);
		updateICCMulDivLogical(regRD);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "andn")))
		setRegister(tokens[3], regRS1 & (~reg_or_imm));

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "or")))
		setRegister(tokens[3], regRS1 | reg_or_imm);
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "orcc")))
	{
		regRD = regRS1 | reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICCMulDivLogical(regRD);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "orn")))
		setRegister(tokens[3], regRS1 | (~reg_or_imm));

        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "orncc")))
	{
		regRD = regRS1 | (~reg_or_imm);
		setRegister(tokens[3], regRD);
		updateICCMulDivLogical(regRD);
	}
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xor")))
		setRegister(tokens[3], regRS1 ^ reg_or_imm);
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xorcc")))
	{
		regRD = regRS1 ^ reg_or_imm;
		setRegister(tokens[3], regRD);
		updateICCMulDivLogical(regRD);
	}
        
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xnor")))
		setRegister(tokens[3], ~(regRS1 ^ reg_or_imm));
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "xnorcc")))
	{
		regRD = ~(regRS1 ^ reg_or_imm);
		setRegister(tokens[3], regRD);
		updateICCMulDivLogical(regRD);
	}

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sll")))
		setRegister(tokens[3], regRS1 << reg_or_imm);

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "srl")))
		setRegister(tokens[3], regRS1 >> reg_or_imm);
        
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "sra")))
            setRegister(tokens[3], (regRS1 >> reg_or_imm) | (getBit(regRS1, 31) ? (0xFFFFFFFF << (32 - reg_or_imm)) : 0x00000000));

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "save")))
	{
		if(saveRegisters() == RET_FAILURE)
			return RET_FAILURE;
		else
			setRegister(tokens[3], regRS1 + reg_or_imm);
	}

	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "restore")))
	{
		if(restoreRegisters() == RET_FAILURE)
			return RET_FAILURE;
		else
			setRegister(tokens[3], regRS1 + reg_or_imm);
	}
	
        else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "rd")))
                setRegister(tokens[2], getRegister(tokens[1]));
        
	else
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "wr")))
		setRegister(tokens[3], regRS1 ^ reg_or_imm);
        
        if(!isFormatIIIOpcodeFound)
	{
		setRegister("pc", regnPC);
		setRegister("npc",regnPC + 4);
		return RET_SUCCESS;
	}
        
        
        
        ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        
        
        
        // Decode Format - III operands for instruction having format: <opcode> <regRS2>, <regRD>
        regRS2 = getRegister(tokens[1]);
        
        // Hex to single precision floating-point conversion
        convertFloat.floatToHex = regRS2;
        float_regRS2 = convertFloat.hexToFloat;
        
        // Hex to double precision floating-point conversion
        convertDouble.doubleToHex[0] = regRS2;
        convertDouble.doubleToHex[1] = getRegister(getNextRegister(tokens[1]));
        double_regRS2 = convertDouble.hexToDouble;
        
	if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fstoi")))
	{
            unsigned long roundedInteger;
            
            if(float_regRS2 > 0)
                roundedInteger = (unsigned long)floorf(float_regRS2);
            else
                roundedInteger = (unsigned long)ceilf(float_regRS2);
                
            setRegister(tokens[2], roundedInteger);
	}

        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fdtoi")))
	{
            unsigned long long roundedInteger;
            
            if(double_regRS2 > 0)
                roundedInteger = (unsigned long)floor(double_regRS2);
            else
                roundedInteger = (unsigned long)ceil(double_regRS2);
                
            setRegister(tokens[2], (unsigned long)(roundedInteger & 0x00000000FFFFFFFF));
            setRegister(getNextRegister(tokens[2]), (unsigned long)(roundedInteger >> 32));
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fstod")))
	{
            convertDouble.hexToDouble = (double)float_regRS2;
            
            setRegister(tokens[2], convertDouble.doubleToHex[0]);
            setRegister(getNextRegister(tokens[2]), convertDouble.doubleToHex[1]);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fdtos")))
	{
            convertFloat.hexToFloat = (float)double_regRS2;
            
            setRegister(tokens[2], convertFloat.floatToHex);
	}
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fitos")))
        {
            signed long signed_regRS2;
            
            signed_regRS2 = (signed long)regRS2;
            convertFloat.hexToFloat = (float)signed_regRS2;
            
            setRegister(tokens[2], convertFloat.floatToHex);
        }
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fitod")))
        {
            signed long signed_regRS2;
            
            signed_regRS2 = (signed long)regRS2;
            convertDouble.hexToDouble = (double)signed_regRS2;
            
            setRegister(tokens[2], convertDouble.doubleToHex[0]);
            setRegister(getNextRegister(tokens[2]), convertDouble.doubleToHex[1]);
        }
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fsqrts")))
        {
            convertFloat.hexToFloat = powf(float_regRS2, 0.5);
            
            setRegister(tokens[2], convertFloat.floatToHex);
        }
        
        else
        if(!(isFormatIIIOpcodeFound = strcmp(tokens[0], "fsqrtd")))
        {
            convertDouble.hexToDouble = pow(double_regRS2, 0.5);
            
            setRegister(tokens[2], convertDouble.doubleToHex[0]);
            setRegister(getNextRegister(tokens[2]), convertDouble.doubleToHex[1]);
        }
        
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



void updateICCAddSubtract(unsigned long regRS1, unsigned long reg_or_imm, unsigned long regRD)
{
	/* Disambiguation between CARRY and OVERFLOW flag:
	   1. http://teaching.idallen.com/dat2343/10f/notes/040_overflow.txt
	   2. http://www.c-jump.com/CIS77/CPU/Overflow/lecture.html 
        */

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
	regPSR = ((signBit_regRS1 && (!signBit_reg_or_imm && !signBit_regRD)) || (!signBit_regRS1 && (signBit_reg_or_imm && signBit_regRD))) ? setBit(regPSR, ICC_OVERFLOW) : clearBit(regPSR, ICC_OVERFLOW);


	// Set ICC_CARRY (c) bit: Important for UNSIGNED arithmetic
	regPSR = (!signBit_regRS1 && signBit_reg_or_imm) || (signBit_regRD && (!signBit_regRS1 || signBit_reg_or_imm)) ? setBit(regPSR, ICC_CARRY) : clearBit(regPSR, ICC_CARRY);


	// Set PSR back to modify ICC bits
	setRegister("psr", regPSR);
}



void updateICCMulDivLogical(unsigned long regRD)
{
	unsigned long regPSR;
	unsigned short signBit_regRD;
	

	regPSR = getRegister("psr");
	signBit_regRD = getBit(regRD, SIGN_BIT);

	// Set ICC_NEGATIVE (n) bit
	regPSR = getBit(regRD, SIGN_BIT) ? setBit(regPSR, ICC_NEGATIVE) : clearBit(regPSR, ICC_NEGATIVE);
	

	// Set ICC_ZERO (z) bit
	regPSR = (regRD == 0) ? setBit(regPSR, ICC_ZERO) : clearBit(regPSR, ICC_ZERO);


	// Set ICC_OVERFLOW (v) bit: Important for SIGNED arithmetic
	regPSR = clearBit(regPSR, ICC_OVERFLOW);


	// Set ICC_CARRY (c) bit: Important for UNSIGNED arithmetic
	regPSR = clearBit(regPSR, ICC_CARRY);


	// Set PSR back to modify ICC bits
	setRegister("psr", regPSR);
}



void updateFCC(unsigned short fcc)
{
	unsigned long regFSR;
	
        regFSR = getRegister("fsr");
        switch(fcc)
        {
            case 0: regFSR = clearBit(regFSR, 11); regFSR = clearBit(regFSR, 10); break;
            case 1: regFSR = clearBit(regFSR, 11); regFSR = setBit(regFSR, 10); break;
            case 2: regFSR = setBit(regFSR, 11); regFSR = clearBit(regFSR, 10); break;
            case 3: regFSR = setBit(regFSR, 11); regFSR = setBit(regFSR, 10); break;
        }

	// Set FSR back to modify FCC bits
	setRegister("fsr", regFSR);
}



int executeNextInstruction()
{
    char *cpuInstruction, *disassembledInstruction;
    unsigned long regPC;
    signed int exitCode;
    
    // Get the PC value
    regPC = getRegister("pc");
    
    // Do we need to break?
    if(isBreakPoint(regPC))
        return RET_BREAKPOINT;
    
    // Fetch the instruction word
    cpuInstruction = readWordAsString(regPC);
    
    // Disassemble the instruction
    disassembledInstruction = (char*)decodeInstruction(cpuInstruction, regPC);
    
    // Save instruction info
    lastInstructionInfo.regPC = regPC;
    strcpy(lastInstructionInfo.cpuInstruction, cpuInstruction);
    strcpy(lastInstructionInfo.disassembledInstruction, disassembledInstruction);
    
    // Execute instruction
    exitCode = executeInstruction(disassembledInstruction);
    
    // Verify the outcome
    if(exitCode == RET_WATCHPOINT)
        exitCode =  RET_WATCHPOINT;
    else
        exitCode = RET_SUCCESS;
    
    // Free up grabbed memory, prevent memory leak
    free(cpuInstruction);
    free(disassembledInstruction);
    
    return exitCode;
}



void setWatchPointInfo(unsigned long memoryAddress, unsigned long newData)
{
    watchInfo = (struct watchPointInfo*)malloc(sizeof(struct watchPointInfo));
    if(!watchInfo)
        return;
    watchInfo->memoryAddress = memoryAddress;
    watchInfo->newData = newData;
}



struct watchPointInfo* getWatchPointInfo()
{
    return watchInfo;
}



/*int main()
{
	char disassembledInstruction[] = "sethi %hi(0x40000000), %g7";
	executeInstruction(disassembledInstruction, 1);
	return RET_SUCCESS 0;
}*/
