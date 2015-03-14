# _SPARCSIM – System Components_ #

---


SPARCSIM operates on SPARC ELF binaries. An ELF binary contains the machine code to be executed on the SPARC platform. SPARC is inherentlybig-endian, i.e. it stores the “big” end or the Most-Significant-Bit (MSB) of multi byte word to lower memory address. Other bytes follow next towards the higher memory addresses. When SPARCSIM is first invoked, it initializes its memory, registers and breakpoints.  Once an ELF file is fed to the system, it is dissected first to get the sections out of it. Relevant CODE, DATA and BSS (Uninitialized Data) sections are loaded into simulator memory following the big-endian convention. Other sections containing metadata and symbol table are discarded. Simulator memory is implemented by a two-level paging scheme to grab memory from the host OS and supply the environment as and when required. Once the loading is done, the built-in disassembler is called for. This module can produce disassembly  of either the entire binary or a selected command loaded at a specified address. Decoded command in assembly is passed on to the execution engine. It contains a mini-parser to further split the opcodes and the Format - I, Format - II and Format – III operands. Execution engine first tests whether the instruction is
located at a point where a breakpoint has already been set. If so, the execution is paused. Otherwise, it carries out the task as instructed by the opcode. During the process, memory and registers are accessed as per requirement. If an instruction tries out to carry out something not allowed in SPARC architecture, the simulator faults and the situation is
tossed into the lap of trap handler. All the modules take help from the custom libraries embedded within the simulator. To operate on ELF files, SPARCSIM makes use of an LGPL library called [LIBELF](http://www.mr511.de/software/english.html). SPARCSIM stores dynamic parameters in a configuration file to make the design flexible.
Below are the core components of SPARCSIM:

  1. User Interface
  1. Call Dispatcher
  1. Loader
  1. Disassembler
  1. Memory Manager
  1. Register Manager
  1. Execution Engine
  1. Trap Handler
  1. Custom Library
  1. Third - party Library
  1. Configuration File

![http://img6.imageshack.us/img6/8415/componentsj.png](http://img6.imageshack.us/img6/8415/componentsj.png)

<table width='1200'>
<tr>
<td width='30%'></td>
<td width='40%'><b><u>SPARCSIM - System blocks</u></b></td>
<td width='30%'></td>
</tr>
</table>