# _SPARCSIM – Development Environment_ #

---


SPARCSIM grew up so fast that it became extremely difficult to manage the code base by standard text  editors. Hence, the following pieces of
software were put together to build a concrete development environment for rapid application development.

  * Operating System:
    * Microsoft Windows XP, Service Pack - III – Host OS
    * Ubuntu 10.04 Desktop (GNOME) – Guest OS

  * Compiler:
    * GNU GCC 4.4.3
    * MinGW GCC 4.6.2 – Windows port of GNU GCC

  * Assembler:
    * SPARC-ELF-AS – Cross-compiled from GCC 4.4.3 with SPARC as target architecture
    * SPARC-ELF-AS – From gnatforleon-2.3.0

  * Linker:
    * GNU LD 4.4.3
    * MinGW LD 4.6.2
    * SPARC-ELF-LD – Cross-compiled from GCC 4.4.3 with SPARC as target architecture and from gnatforleon-2.3.0

  * Integrated Development Environment (IDE):
    * Eclipse Juno (V 4.2) CDT (C Development Tool-chain) – As RAD (Rapid Application Development) environment
    * Codan – Eclipse static code analyser plugin
    * Notepad ++ 5.6.8 – A free GNU code editor with built-in code highlighter plugin

  * SPARC Tool-chain:
    * SPARC-ELF-OBJDUMP – Cross-compiled from GCC 4.4.3 with SPARC as target architecture and from gnatforleon-2.3.0
    * SPARC-ELF-READELF – Cross-compiled from GCC 4.4.3 with SPARC as target architecture and from gnatforleon-2.3.0

  * Miscellaneous:
    * Oracle VirtualBox 4.1.8 – Simulate Ubuntu as Guest  OS on top of Windows Host
    * TSIM-LEON3-EVAL – As a benchmark for detection of bugs in SPARCSIM