#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <register.h>
#include <decode.h>



char* decodeInstruction(char* cpuInstruction, unsigned long regPC)
{
	char* disassembledInstruction = (char*)malloc(50);
	unsigned long instructionWord, hexDigit, op, disp30, rd, a, cond, op2, imm22, disp22, op3, rs1, asi, i, rs2, simm13, opf;
	char* hexNumber = (char*)malloc(32);
	char* opcode = NULL;
	
	// Pack Quarword together
	instructionWord = 0;
	hexDigit = cpuInstruction[0]; hexDigit = (hexDigit << 24) >> 24; instructionWord = (instructionWord << 8) | hexDigit;
	hexDigit = cpuInstruction[1]; hexDigit = (hexDigit << 24) >> 24; instructionWord = (instructionWord << 8) | hexDigit;
	hexDigit = cpuInstruction[2]; hexDigit = (hexDigit << 24) >> 24; instructionWord = (instructionWord << 8) | hexDigit;
	hexDigit = cpuInstruction[3]; hexDigit = (hexDigit << 24) >> 24; instructionWord = (instructionWord << 8) | hexDigit;
	
	op = instructionWord >> 30;
	
	// Format - I instruction
	// CALL
	if(op == 1)
	{
		disp30 = instructionWord & 0x3FFFFFFF;
		sprintf(hexNumber, "%lx", ((disp30 << 2) + regPC));
		strcpy(disassembledInstruction, "call 0x");
		strcat(disassembledInstruction, hexNumber);
	}
	
	else
	// Format - II instruction
    	if(op == 0)
    	{
    		op2 = (instructionWord & 0x01C00000) >> 22;
    		
			if(op2 == 4)
			{				
				// SETHI
				rd = (instructionWord & 0x3E000000) >> 25; 
				imm22 = instructionWord & 0x003FFFFF;
				strcpy(disassembledInstruction, "sethi %hi(0x");
				sprintf(hexNumber, "%lx", (imm22 << 10));
				strcat(disassembledInstruction, hexNumber);
				strcat(disassembledInstruction, "), ");
				strcat(disassembledInstruction, getIntegerRegisterName(rd));
			}
			
			else
			{
				a = (instructionWord & 0x20000000) >> 29;
				cond = (instructionWord & 0x1E000000) >> 25;
				disp22 = instructionWord & 0x003FFFFF;
				
				switch(op2)
				{
					// Bicc
					case 2:
					{
						switch(cond)
						{
							case 0b1000: opcode = "ba";	break;
							case 0b0000: opcode = "bn"; break;
							case 0b1001: opcode = "bne"; break;
							case 0b0001: opcode = "be"; break;
							case 0b1010: opcode = "bg"; break;
							case 0b0010: opcode = "ble"; break;
							case 0b1011: opcode = "bge"; break;
							case 0b0011: opcode = "bl"; break;
							case 0b1100: opcode = "bgu"; break;
							case 0b0100: opcode = "bleu"; break;
							case 0b1101: opcode = "bcc"; break;
							case 0b0101: opcode = "bcs"; break;
							case 0b1110: opcode = "bpos"; break;
							case 0b0110: opcode = "bneg"; break;
							case 0b1111: opcode = "bvc"; break;
							case 0b0111: opcode = "bvs"; break;							
						}
						break;
					}
					
					// FBfcc
					case 6:
					{
						switch(cond)
						{
							case 0b1000: opcode = "fba"; break;
							case 0b0000: opcode = "fbn"; break;
							case 0b0111: opcode = "fbu"; break;
							case 0b0110: opcode = "fbg"; break;
							case 0b0101: opcode = "fbug"; break;
							case 0b0100: opcode = "fbl"; break;
							case 0b0011: opcode = "fbul"; break;
							case 0b0010: opcode = "fblg"; break;
							case 0b0001: opcode = "fbne"; break;
							case 0b1001: opcode = "fbe"; break;
							case 0b1010: opcode = "fbue"; break;
							case 0b1011: opcode = "fbge"; break;
							case 0b1100: opcode = "fbuge"; break;
							case 0b1101: opcode = "fble"; break;
							case 0b1110: opcode = "fbule"; break;							
							case 0b1111: opcode = "fbuo"; break;	
						}
						break;
					}
					
					// CBccc
					case 7:
					{
						switch(cond)
						{
							case 0b1000: opcode = "cba"; break;
							case 0b0000: opcode = "cbn"; break;
							case 0b0111: opcode = "cb3"; break;
							case 0b0110: opcode = "cb2"; break;
							case 0b0101: opcode = "cb23"; break;
							case 0b0100: opcode = "cb1"; break;
							case 0b0011: opcode = "cb13"; break;
							case 0b0010: opcode = "cb12"; break;
							case 0b0001: opcode = "cb123"; break;
							case 0b1001: opcode = "cb0"; break;
							case 0b1010: opcode = "cb03"; break;
							case 0b1011: opcode = "cb02"; break;
							case 0b1100: opcode = "cb023"; break;
							case 0b1101: opcode = "cb01"; break;
							case 0b1110: opcode = "cb013"; break;
							case 0b1111: opcode = "cb012"; break;							
						}
						break;
					}
				}
				
				if(opcode != NULL)
				{
					strcpy(disassembledInstruction, opcode);
					(a == 1) ? strcat(disassembledInstruction, ",a ") : strcat(disassembledInstruction, " ");
					sprintf(hexNumber, "0x%lx", ((disp22 << 2) + regPC));
					strcat(disassembledInstruction, hexNumber);
				}
			}
    	}
	
	// Format - III instruction
    	else
        {
            rd = (instructionWord & 0x3E000000) >> 25;
            op3 = (instructionWord & 0x01F80000) >> 19;
            rs1 = (instructionWord & 0x00007C000) >> 14;
            i = (instructionWord & 0x00002000) >> 13;
            simm13 = instructionWord & 0x00001FFF;
            rs2 = instructionWord & 0x0000201F;
            asi = (instructionWord & 0x00001FE0) >> 5;
            opf = (instructionWord & 0x00003FE0) >> 5;
			
			switch(op3)
			{
				case 0b001001: opcode = "ldsb"; break;
				case 0b001010: opcode = "ldsh"; break;
				case 0b000001: opcode = "ldub"; break;
				case 0b000010: opcode = "lduh"; break;
				case 0b000000: opcode = "ld"; break;
				case 0b000011: opcode = "ldd"; break;
				case 0b011001: opcode = "ldsba"; break;
				case 0b011010: opcode = "ldsha"; break;
				case 0b010001: opcode = "lduba"; break;
				case 0b010010: opcode = "lduha"; break;
				case 0b010000: opcode = "lda"; break;
				case 0b010011: opcode = "ldda"; break;
			}
			
			if(opcode != NULL)
			{
				strcpy(disassembledInstruction, opcode);
				strcat(disassembledInstruction, " [");
				if(rs1 != 0)
				{
					strcat(disassembledInstruction, getIntegerRegisterName(rs1));
					strcat(disassembledInstruction, " + ");
				}
				if(i == 0) 
				{
					if(rs2 != 0)
						strcat(disassembledInstruction, getIntegerRegisterName(rs2));
				}
				else
				{
					sprintf(hexNumber, "0x%lx", simm13);
					strcat(disassembledInstruction, hexNumber);
				}
				strcat(disassembledInstruction, "], ");
				strcat(disassembledInstruction, getIntegerRegisterName(rd));
				opcode = NULL;
			}
			
			
			switch(op3)
			{
				case 0b100000: opcode = "ld"; break;
				case 0b100011: opcode = "ldd"; break;
				case 0b100001: opcode = "ld"; break;
			}
			
			if(opcode != NULL)
			{
				strcpy(disassembledInstruction, opcode);
				strcat(disassembledInstruction, " [");
				if(rs1 != 0)
				{
					strcat(disassembledInstruction, getFlotingRegisterName(rs1));
					strcat(disassembledInstruction, " + ");
				}
				if(i == 0) 
				{
					if(rs2 != 0)
						strcat(disassembledInstruction, getFlotingRegisterName(rs2));
				}
				else
				{
					sprintf(hexNumber, "0x%lx", simm13);
					strcat(disassembledInstruction, hexNumber);
				}
				strcat(disassembledInstruction, "], ");
				strcat(disassembledInstruction, getFlotingRegisterName(rd));
				opcode = NULL;
			}
			
			
			switch(op3)
			{
				case 0b110000: opcode = "ld"; break;
				case 0b110011: opcode = "ldd"; break;
				case 0b110001: opcode = "ld"; break;
			}
			
			if(opcode != NULL)
			{
				strcpy(disassembledInstruction, opcode);
				strcat(disassembledInstruction, " [");
				if(rs1 != 0)
				{
					strcat(disassembledInstruction, getCoProcessorRegisterName(rs1));
					strcat(disassembledInstruction, " + ");
				}
				if(i == 0) 
				{
					if(rs2 != 0)
						strcat(disassembledInstruction, getCoProcessorRegisterName(rs2));
				}
				else
				{
					sprintf(hexNumber, "0x%lx", simm13);
					strcat(disassembledInstruction, hexNumber);
				}
				strcat(disassembledInstruction, "], ");
				strcat(disassembledInstruction, getCoProcessorRegisterName(rd));
				opcode = NULL;
			}
        }
		
	free(hexNumber);
	return disassembledInstruction;
}


/*int main()
{
	char cpuInstruction[4] = {0x10, 0x80, 0x00, 0x02};
	printf("%s\n",decodeInstruction(cpuInstruction));
}*/
