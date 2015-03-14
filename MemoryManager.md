# _Memory Manager_ #

---


Memory manager is responsible to allocate, read, write and de-allocate simulator memory as and when required. Though the design objective of SPARCSIM was to develop a simulator for an embedded system not exceeding 2 MB memory limit, but its design has been made scalable to accommodate
larger memory requirement in future. It employs a two-level paging like scheme internally. 32 bit address word is divided in three sections: 10 bits for first level paging table, 10 bits for second level paging table and the remaining 12 bits for offset within specified page. Page size is 2<sup>12</sup> bytes = 4KB. Page is allocated in pages.

First level of paging table, which holds 2<sup>10</sup> = 1024 entries, is statically allocated and all its entries are set to NULL. Once any word of unallocated page is referred for the first time, the page is allocated. Any subsequent write to that page causes no more page allocation. Prior to the allocation of the page, it is checked whether the second level paging table corresponding to that page exists. If not, then a paging table containing 2<sup>10</sup> = 1024 entries is allocated dynamically. Again, all its entries are set to NULL. Then a page is created. The entry in the second level paging table corresponding to it is set to point to the newly created page in memory. All the background process is transparent to the process being simulated. All it realizes is to read and write memory seamlessly.

The mechanism of reading is much less complex sinceno memory allocation is associated with it. Once a memory word is referred, memory manager checks whether entries are set to in first level and second level paging table corresponding to it. In case it is found, memory manager adds the 12 bit offset to the beginning of  the page in memory to translate the
virtual address to its corresponding physical address. The content is returned. In response to an attempt to read from a memory location previously unallocated, the requesting module is handed over a zero.

![http://img831.imageshack.us/img831/7269/memoryg.png](http://img831.imageshack.us/img831/7269/memoryg.png)

<table width='1200'>
<tr>
<td width='30%'></td>
<td width='40%'><b><u>Memory handling in SPARCSIM</u></b></td>
<td width='30%'></td>
</tr>
</table>