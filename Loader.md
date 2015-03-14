# _Loader_ #

---


Like the built-in loader of operating systems, SPARCSIM loader behaves functionally in the same fashion. An ELF binary serves as its input. The Executable and Linking Format (ELF) was originally developed and published by UNIX System Laboratories (USL) as part of the Application Binary Interface (ABI). The Tool Interface Standards committee (TIS) has
selected the evolving ELF standard as a portable object file format that works on 32-bit Intel Architecture environments for a variety of operating systems. In the present context, such a file can be produced by assembling/compiling and then linking a code written in either
assembly or some other high-level language, e.g. Ada.

An ELF file is structured as shown below:

![http://img213.imageshack.us/img213/3234/elfrq.png](http://img213.imageshack.us/img213/3234/elfrq.png)

#### ELF binary - on disk (left) and in memory (right) ####

Loader reads the ELF header to get the meta-information regarding the ELF file. From the information extracted, it gets to know the type (SPARC/Intel) and endian-ness (Big/Little). It rejects any non-SPARC binary. There are multiple sections residing in the ELF structure,
some contain executable code (.TEXT), some embeds the initialized data (.DATA) and uninitialized data (.BSS), some are there to hold string table, symbol table etc. Not all of these are relevant to simulation. There are sections which contains section names itself or symbols and their values to facilitate debugging.  Each section is preceded by a section header. Loader iterates though all section headers  to find out necessary sections, their names, sizes and entry point addresses. Also it calculates number of instructions by dividing the respective section size (in bytes) by 4 (as SPARC instructions are fixed-length). It passes
down the extracted data to memory manager to write them to appropriate addresses to be used by disassembler. Along with that, it forms a linked list of structures containing all the meta-information thus extracted. This list is handed over to UI module via dispatcher to be
displayed on screen.