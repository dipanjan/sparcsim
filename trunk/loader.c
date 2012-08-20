#include <loader.h>



int initializeLoader(char *elfBinary, int isReset)
{
	static int fileDescriptor = RET_FAILURE;
	if(isReset == 0)
	{
		// Invoked from getNextInstruction()
		
		return fileDescriptor;
	}
	else
	{
		// Invoked from load_sparc_elf()
		
		if(fileDescriptor != RET_FAILURE)
			close(fileDescriptor);
		
		if((fileDescriptor = open(elfBinary, O_RDWR)) == RET_FAILURE)
		{
			printf("Couldn't open %s\n", elfBinary);
			return RET_FAILURE;
		}
		else
			return fileDescriptor;
	}
}


void load_sparc_elf(char *elfBinary)
{
	GElf_Ehdr elf_header;			// ELF header 
	GElf_Shdr shdr;                 // Section Header
	Elf* elf;               		// Elf pointer for libelf 
	Elf_Scn* scn = NULL;            // Section Descriptor 
	
 	int fileDescriptor;
	
	fileDescriptor = initializeLoader(elfBinary, 1);

	// Check libelf version first 
	if(elf_version(EV_CURRENT) == EV_NONE)
		printf("WARNING: ELF library is out of date!\n");

	elf = elf_begin(fileDescriptor, ELF_C_READ, NULL);	// Initialize 'elf' pointer to our file descriptor
	gelf_getehdr(elf, &elf_header); 					// Returns NULL on error

	// Iterate through section headers 
	while((scn = elf_nextscn(elf, scn)) != 0)
	{
		// Point shdr at this section header entry
		gelf_getshdr(scn, &shdr);

		if(shdr.sh_flags & SHF_EXECINSTR)
		{
			// The shdr name is in a string table, libelf uses elf_strptr() to find it
			// using the e_shstrndx value from the elf_header
			printf("Section: %s loaded at 0x%llx\n", elf_strptr(elf, elf_header.e_shstrndx, shdr.sh_name), shdr.sh_addr);
			load_sparc_instructions();
		}
	}
}

	

void load_sparc_instructions()
{
	//char cpuInstruction[4];
	int fileDescriptor = initializeLoader(NULL, 0);
	Elf_Scn* scn = NULL;            // Section Descriptor
	Elf_Data* data;                 // Data Descriptor 
	GElf_Shdr shdr;                 // Section Header
 	
	
	// Iterate through section headers
	Elf* elf = elf_begin(fileDescriptor, ELF_C_READ, NULL);

	while((scn = elf_nextscn(elf, scn)) != NULL)
	{
		gelf_getshdr(scn, &shdr);
		size_t n;

		if(shdr.sh_flags & SHF_EXECINSTR)
		{
			data = NULL; 
			n = 0; 
			unsigned long sectionLoadAddress = shdr.sh_addr;
			while (n < shdr.sh_size && (data = elf_getdata (scn, data)) != NULL) 
			{
				char* p = (char*)data -> d_buf; 
				while (p < (char*)data-> d_buf + data -> d_size ) 
				{
					//cpuInstruction[0] = *p;
					writeMemory(sectionLoadAddress, *p); sectionLoadAddress++; p++;
					//cpuInstruction[1] = *p;
					writeMemory(sectionLoadAddress, *p); sectionLoadAddress++; p++;
					//cpuInstruction[2] = *p;
					writeMemory(sectionLoadAddress, *p); sectionLoadAddress++; p++;
					//cpuInstruction[3] = *p;
					writeMemory(sectionLoadAddress, *p); sectionLoadAddress++; p++;
					n+= 4;
				}
			}
		}
	}
}



/*void main(int argc, char* argv[])
{
	load_sparc_elf(argv[1]);
	int count;
	for(count = 0x10054; count <= 0x10058; count = count+= 4)
	{
		char* cpuInstruction = getQuadWordFromMemory(count);
		displayQuadWord(cpuInstruction, 0);
		free(cpuInstruction);
		printf("\n");
	}
}*/
