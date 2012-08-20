#include <loader.h>



int initializeLoader(char *elfBinary)
{
	static int fileDescriptor = RET_FAILURE;
	if(elfBinary == NULL)
	{
		close(fileDescriptor);
		return -1;
	}
	else
	{
		if((fileDescriptor = open(elfBinary, O_RDWR)) == RET_FAILURE)
			return ELF_FILE_DOES_NOT_EXIST_ERROR;
		else
			return fileDescriptor;
	}
}



struct loadedSections* load_sparc_instructions(char *elfBinary)
{
	GElf_Ehdr elf_header;			// ELF header
	GElf_Shdr shdr;                 // Section Header
	Elf_Scn* scn = NULL;            // Section Descriptor
	Elf_Data* data;                 // Data Descriptor
	unsigned long instrcutionCount;
	int fileDescriptor;
	struct loadedSections* elfSections, *elfSectionsPrevPtr, *elfSectionCurPtr;

	instrcutionCount = 0;
	elfSections = (struct loadedSections*)malloc(sizeof(struct loadedSections));
	if(elfSections == NULL)
	{
		elfSections->sectionType = ELF_SECTION_LOAD_ERROR;
		return elfSections;
	}
	elfSectionCurPtr = elfSections;
	fileDescriptor = initializeLoader(elfBinary);
	if(fileDescriptor == ELF_FILE_DOES_NOT_EXIST_ERROR)
	{
		elfSections->sectionType = ELF_FILE_DOES_NOT_EXIST_ERROR;
		return elfSections;
	}
	
	// Check libelf version
	if(elf_version(EV_CURRENT) == EV_NONE)
	{
		elfSections->sectionType = ELF_BINARY_OUT_OF_DATE;
		return elfSections;
	}

	Elf* elf = elf_begin(fileDescriptor, ELF_C_READ, NULL);
	if(elf == NULL)
	{
		elfSections->sectionType = ELF_POINTER_INITIALIZATION_ERROR;
		return elfSections;
	}

	gelf_getehdr(elf, &elf_header);

	// Iterate through section headers
	while((scn = elf_nextscn(elf, scn)) != 0)
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
					writeMemory(sectionLoadAddress, *p); sectionLoadAddress++; p++;
					writeMemory(sectionLoadAddress, *p); sectionLoadAddress++; p++;
					writeMemory(sectionLoadAddress, *p); sectionLoadAddress++; p++;
					writeMemory(sectionLoadAddress, *p); sectionLoadAddress++; p++;
					n+= 4;
					instrcutionCount++;
				}
			}

			// Store section information
			strcpy(elfSectionCurPtr->sectionName, elf_strptr(elf, elf_header.e_shstrndx, shdr.sh_name));
			elfSectionCurPtr->sectionLoadAddress = shdr.sh_addr;
			elfSectionCurPtr->sectionSize = data->d_size;
			elfSectionCurPtr->instructionCount = instrcutionCount;
			if(shdr.sh_flags & SHF_EXECINSTR)
				elfSectionCurPtr->sectionType = CODE_SECTION;
			elfSectionCurPtr->nextSection = (struct loadedSections*)malloc(sizeof(struct loadedSections));
			if(elfSectionCurPtr->nextSection == NULL)
			{
				elfSections->sectionType = ELF_SECTION_LOAD_ERROR;
				return elfSections;
			}
			elfSectionsPrevPtr = elfSectionCurPtr;
			elfSectionCurPtr = elfSectionCurPtr->nextSection;
		}
	}

	free(elfSectionCurPtr);
	elfSectionsPrevPtr->nextSection = NULL;
	return elfSections;
}



/*int main(int argc, char* argv[])
{
	load_sparc_elf(argv[1]);
	int count;
	for(count = 0x10054; count <= 0x10058; count = count+= 4)
	{
		char* cpuInstruction = getQuadWordFromMemory(count);
		displayQuadWord(cpuInstruction, 0);
		free(cpuInstruction);
		printf("\n");
		return 0;
	}
}*/
