# _Disassembler_ #

---


Disassembler, as its name implies, decodes SPARC instructions loaded into memory by the loader. Instead of programmatically dissecting the  instructions, one alternative could be to use sparc-elf-objdump utility with “-d” command line switch. This is the tool which is derived by the cross-compilation of GNU “binutils”  package with SPARC as target platform set. But, it has following drawbacks:

  * In the design of indigenous SPARC processor, some of unused opcodes have been remapped to some other non-standard operations useful to our on-board computing system. Those would have been incorrectly decoded by any standard utility complying SPARC ISA strictly.
  * Simulator would have to be dependent on availability of this particular utility in the system.
  * Even if sparc-elf-objdump could be found present onto the system, it has to be there either in the directory where SPARCSIM resides, or  in the system PATH. This could lead to unnecessary complication in configuration.

Disassembler first takes out the opcode by masking  the 4-byte instruction word with appropriate bit vector. Once it decides the format of the instruction, i.e. Format - I, Format – II or Format – III, it applies further masks onto t he remaining part of the instruction word.

Thus, it collects all of the individual fields as shown below:

![http://img266.imageshack.us/img266/1031/instructionformats.png](http://img266.imageshack.us/img266/1031/instructionformats.png)

<table width='1200'>
<tr>
<td width='30%'></td>
<td width='40%'><b><u>SPARC instruction formats</u></b></td>
<td width='30%'></td>
</tr>
</table>

Afterwards, disassembler forms equivalent assembly instruction as specified in “Appendix A - Suggested Assembly Language Syntax” of SPARC V8 architecture manual. It is to be noted that the disassembler is yet unable to produce synthetic form of SPARC opcodes, though it, in no way, affects the simulation.