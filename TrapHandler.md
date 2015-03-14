# _Trap Handler_ #

---


Trap handler does not handle any of the traps at this point. When a trap is detected, execution engine redirects the control to the trap  handler module. It simply prints out the faulting address and the type of the trap generated. Afterwards, the execution aborts.