# _Future Scope_ #

---


SPARCSIM leaves ample scope for further improvement. A few of
the possible value additions are listed below:

  1. **Implementation of unsupported commands:** SPARCSIM implements a large subset of SPARC v8 ISA.
  1. **Support for synthetic instructions:** Presently simulator’s internal disassembler can disassemble the instructions to their non-synthetic equivalent. It can further be augmented with an ability to decode the assembly opcodes to their synthetic mnemonic.
  1. **Reading symbol table:** Symbol table of ELF binary is discarded. That can be read to produce much more developer-friendly disassembled output by substituting HEX coded memory addresses by corresponding symbol names.
  1. **Support for Peripherals:** Add-on modules can be made to support by the simulator to emulate processor peripherals and timer.
  1. **Implementation of pipelining:** The simulator can be made clock cycle aware so that pipelining can be implemented to visualize processor’s performance at clock level instead of instruction level.
  1. **Implementation of cache:** The simulator can be made to support cache memory to enable generation of statistical data about cache hit and miss.