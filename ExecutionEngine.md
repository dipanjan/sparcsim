# _Execution Engine_ #

---


This component is the heart of the simulator. It takes the disassembled output in assembly from the disassembler and parses the instruction according to SPARC assembly language syntax standard. Once again, like disassembler, it  splits the instruction segments apart. For
implementation of SPARC instructions, “Appendix B –Instruction Definition” and “Appendix C – ISP Description” sub-sections of SPARC V8 reference manual have been used. Execution engine invokes bit handling routines from application’s custom library to manipulate individual bits of the memory word, register and ICC (Integer Condition Codes) of PSR register.