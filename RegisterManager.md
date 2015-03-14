# _Register Manager_ #

---


Register manager handles the reading and writing ofprocessor registers and also the shifting of windows in response to SAVE and RESTOREassembly commands. Till this point, only Integer registers, i.e. i0 - i7, l0 - l7, o0 -o7 g0 - g7 and PC, NPC, PSR (Processor Status Register), TBR (Trap Base Register), Y and WIM (Window Invalid Mask) are supported.

![http://img713.imageshack.us/img713/3475/registerwindowi.png](http://img713.imageshack.us/img713/3475/registerwindowi.png)

<table width='700'>
<tr>
<td width='30%'></td>
<td width='40%'><b><u>SPARC Register Window architecture </u></b></td>
<td width='30%'></td>
</tr>
</table>

Our indigenous processor contains 8 register windows. This is specified in the configurationfile from where this information is read. Required  number of bytes is then allocated dynamically and the base address of this memory chunk is stored. CWP (Current Window Pointer) field of PSR stores the index of the register window in use. Starting address of the
present register window is calculated by multiplying the value stored in CWP by 16 (i.e. width of each register window) and adding it to thebase address to memory chunk mentioned earlier. SAVE causes CWP to decrease by 1and RESTORE causes CWP to increase by 1. Before the actual increment or decrement is performed, register manager ensures that it is neither overflowing nor underflowing by testi ng the bit set in WIM register. Appropriate trap is generated, if required.

![http://img829.imageshack.us/img829/3808/registerwindowii.png](http://img829.imageshack.us/img829/3808/registerwindowii.png)

<table width='600'>
<tr>
<td width='30%'></td>
<td width='50%'><b><u>SPARCSIM Register Window implementation</u></b></td>
<td width='30%'></td>
</tr>
</table>