#include <execute.h>



void executeInstruction(char* disassembledInstruction)
{
	char tokens[10][20];
	char* token;
	short count = 0;
	unsigned long regPC;
	unsigned long regnPC = getRegister("npc");

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
		printf("tokens[%d]: %s\n", i, tokens[i]);
		
	// Format - I instruction
	if(!strcmp(tokens[0], "call"))
	{
		unsigned long displacement = strtol(tokens[1], NULL, 0);
		setRegister("pc", regnPC);
		setRegister("%o7", displacement);
		setRegister("npc", displacement);
		return;
	}
	
	
	// Format - II instruction
	if(!strcmp(tokens[0], "sethi"))
	{
		setRegister(tokens[2], strtol(tokens[1], NULL, 0));
		setRegister("pc", regnPC);
		setRegister("npc", regnPC + 4);
		return;
	}
	
	
	if(!strcmp(tokens[0], "nop"))
	{
		setRegister("pc", regnPC);
		setRegister("npc", regnPC + 4);
		return;
	}
	
	
	if(!strcmp(tokens[0], "unimp"))
	{
		printf("Trap: illegal_instruction\n");
		return;
	}
	

	if(!strcmp(tokens[0], "ba"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			regPC = strtol(tokens[2], NULL, 0);
			setRegister("pc", regPC);
			setRegister("npc", regPC + 4);
		}
		else
		{
			setRegister("pc", regnPC);
			regnPC = strtol(tokens[1], NULL, 0);
			setRegister("npc", regnPC);
		}
		return;
	}


	if(!strcmp(tokens[0], "bn"))
	{
		if(!strcmp(tokens[1], "a"))
		{
			setRegister("pc", regnPC + 4);
			setRegister("npc", regnPC + 8);
		}
		else
		{
			setRegister("pc", regnPC);
			setRegister("npc", regnPC + 4);
		}
		return;
	}


	// Format - III instruction
}



/*int main()
{
	char disassembledInstruction[] = "sethi %hi(0x40000000), %g7";
	executeInstruction(disassembledInstruction, 1);
	return 0;
}*/
