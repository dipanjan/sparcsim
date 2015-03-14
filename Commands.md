# _SPARCSIM Commands_ #

---


SPARCSIM, as a simulator supports the following command set. End user issues these commands to operate the simulator.

| **Command** | **Action** |
|:------------|:-----------|
| sparcsim `[file_name`] | load a file into simulator memory |
| sparcsim -d `[file_name`] | disassemble SPARC ELF binary |
| sparcsim -c `[file_name`] | execute a batch file of SPARCSIM commands |
| `[ba`]tch `<file`> | execute a batch file of SPARCSIM commands |
| `[re`]set | reset simulator |
| `[l`]oad `<file_name`> | load a file into simulator memory |
| `[m`]em `[addr`] `[count`] | display memory at `[addr`] for `[count`] bytes |
| `[w`]mem `<addr`> `<val`> | write memory word at `<addr`> with value `<val`> |
| `[s`]tep | single step |
| `[br`]eak `<addr`> | add a breakpoint at `<addr`> |
| `[de`]l `<num`> | delete breakpoint `<num`> |
| `[br`]eak | print all breakpoints |
| `[wa`]tch `<addr`> | add a watchpoint at `<addr`> |
| `[t`]ra `<inst_count`> | trace `<inst_count`> instructions |
| `[f`]loat | show floating point registers |
| `[r`]eg `[reg`] `[val`] | show/set integer registers, display register windows, e.g. reg w2 |
| `[p`]sr | unpacks PSR fields |
| `[d`]is `[addr`] `[count`] | disassemble `[count`] instructions at address `[addr`] |
| `[c`]ont `[cnt`] | continue execution for `[cnt`] instructions |
| `[g`]o `<addr`> `[cnt`] | start execution at `<addr`> |
| `[ru`]n `[cnt`] | reset and start execution at address zero |
| `[h`]elp | display this help |
| `[e`]cho `<string`> | print `<string`> to the simulator window |
| `[sh`]ell `<cmd`> | execute shell command |
| `[q`]uit | exit the simulator |

This brief list of commands can be displayed on console using the “help” or “h” command of the simulator. Individual commands works exactly in the same manner as those of TSIM -LEON3 – EVAL simulator by Gaisler Research.