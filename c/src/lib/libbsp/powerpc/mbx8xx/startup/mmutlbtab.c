/*
 * mmutlbtab.c
 *
 * This file defines the  MMU_TLB_table for the MBX8xx.
 *
 * Copyright (c) 1999, National Research Council of Canada
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <mpc8xx/mmu.h>
#include <bsp/mbx.h>
/*
 * This MMU_TLB_table is used to statically initialize the Table Lookaside
 * Buffers in the MMU of the MBX8xx board.
 *
 * We initialize the entries in both the instruction and data TLBs
 * with the same values - a few bits relevant to the data TLB are unused
 * in the instruction TLB.
 *
 * An Effective Page Number (EPN), Tablewalk Control Register (TWC) and
 * Real Page Number (RPN) value are supplied in the table for each TLB entry.
 *
 * The instruction and data TLBs each can hold 32 entries, so _TLB_Table must
 * not have more than 32 lines in it!
 *
 * We set up the virtual memory map so that virtual address of a
 * location is equal to its real address.
 */
MMU_TLB_table_t MMU_TLB_table[] = {
#if    ( defined(mbx860_001b) || \
         defined(mbx860_002b) || \
         defined(mbx860_003b) || \
         defined(mbx821_001b) || \
         defined(mbx821_002b) || \
         defined(mbx821_003b) || \
         defined(mbx860_001)  || \
         defined(mbx860_002)  || \
         defined(mbx860_003)  || \
         defined(mbx821_001)  || \
         defined(mbx821_002)  || \
         defined(mbx821_003) )
  /*
	 * DRAM: CS1, Start address 0x00000000, 4M,
	 *	ASID=0x0, APG=0x0, not guarded memory, copyback data cache policy,
	 *	R/W,X for all, no ASID comparison, not cache-inhibited.
	 *	Last 512K block is cache-inhibited, but not guarded for use by EPPCBug.
	 * EPN		TWC	RPN
	 */
  { 0x00000200,	0x05,	0x000009FD },	/* DRAM - PS=512K */
  { 0x00080200,	0x05,	0x000809FD },	/* DRAM - PS=512K */
  { 0x00100200,	0x05,	0x001009FD },	/* DRAM - PS=512K */
  { 0x00180200,	0x05,	0x001809FD },	/* DRAM - PS=512K */
  { 0x00200200,	0x05,	0x002009FD },	/* DRAM - PS=512K */
  { 0x00280200,	0x05,	0x002809FD },	/* DRAM - PS=512K */
  { 0x00300200,	0x05,	0x003009FD },	/* DRAM - PS=512K */
  { 0x00380200,	0x05,	0x003809FF },	/* DRAM - PS=512K, cache-inhibited */
#elif ( defined(mbx860_004) || \
        defined(mbx860_005) || \
        defined(mbx860_004b) || \
        defined(mbx860_005b) || \
        defined(mbx860_006b) || \
        defined(mbx821_004) || \
        defined(mbx821_005) || \
        defined(mbx821_004b) || \
        defined(mbx821_005b) || \
        defined(mbx821_006b) )
  /*
	 * DRAM: CS1, Start address 0x00000000, 16M,
	 *	ASID=0x0, APG=0x0, not guarded memory, copyback data cache policy,
	 *	R/W,X for all, no ASID comparison, not cache-inhibited.
	 * EPN		TWC	RPN
	 */
	{ 0x00000200,	0x0D,	0x000009FD },	/* DRAM - PS=8M */
	{ 0x00800200,	0x0D,	0x008009FD },	/* DRAM - PS=8M */
#else
#error "MBX board not defined"
#endif
	/*
	 *
	 * NVRAM: CS4, Start address 0xFA000000, 32K,
	 *	ASID=0x0, APG=0x0, not guarded memory, copyback data cache policy,
	 *	R/W,X for all, no ASID comparison, cache-inhibited.
	 *
	 * EPN		TWC	RPN
	 */
	{ 0xFA000200,	0x01,	0xFA0009FF },	/* NVRAM - PS=16K */
	{ 0xFA004200,	0x01,	0xFA0049FF },	/* NVRAM - PS=16K */
	/*
	 *
	 * Board Control/Status Register #1/#2: CS4, Start address 0xFA100000, (4 x 8 bits?)
	 *	ASID=0x0, APG=0x0, guarded memory, write-through data cache policy,
	 *	R/W,X for all, no ASID comparison, cache-inhibited.
	 * EPN		TWC	RPN
	 */
	{ 0xFA100200,	0x13,	0xFA1009F7 },	/* BCSR - PS=4K */
	/*
	 *
	 * (IMMR-SPRs) Dual Port RAM: Start address 0xFA200000, 16K,
	 *	ASID=0x0, APG=0x0, guarded memory, write-through data cache policy,
	 *	R/W,X for all, no ASID comparison, cache-inhibited.
	 *
	 *	Note: We use the value in MBXA/PG2, which is also the value that
	 *	EPPC-Bug programmed into our boards. The alternative is the value
	 * 	in MBXA/PG1: 0xFFA00000. This value might well depend on the revision
	 *	of the firmware.
	 * EPN		TWC	RPN
	 */
	{ 0xFA200200,	0x13,	0xFA2009FF },	/* IMMR - PS=16K */
	/*
	 *
	 * Flash: CS0, Start address 0xFE000000, 4M, (BootROM-EPPCBug)
	 *	ASID=0x0, APG=0x0, not guarded memory,
	 *	R/O,X for all, no ASID comparison, not cache-inhibited.
	 * EPN		TWC	RPN
	 */
	{ 0xFE000200,	0x05,	0xFE000CFD },	/* Flash - PS=512K */
	{ 0xFE080200,	0x05,	0xFE080CFD },	/* Flash - PS=512K */
	{ 0xFE100200,	0x05,	0xFE100CFD },	/* Flash - PS=512K */
	{ 0xFE180200,	0x05,	0xFE180CFD },	/* Flash - PS=512K */
	{ 0xFE200200,	0x05,	0xFE200CFD },	/* Flash - PS=512K */
	{ 0xFE280200,	0x05,	0xFE280CFD },	/* Flash - PS=512K */
	{ 0xFE300200,	0x05,	0xFE300CFD },	/* Flash - PS=512K */
	{ 0xFE380200,	0x05,	0xFE380CFD },	/* Flash - PS=512K */
	/*
	 * BootROM: CS7, Start address 0xFC000000, 4M?, (socketed FLASH)
	 *	ASID=0x0, APG=0x0, not guarded memory,
	 *	R/O,X for all, no ASID comparison, not cache-inhibited.
	 * EPN		TWC	RPN
	 */
	{ 0xFC000200,	0x05,	0xFC000CFD },	/* BootROM - PS=512K */
	/*
	 *
	 * PCI/ISA I/O Space: CS5, Start address 0x80000000, 512M?
	 *	ASID=0x0, APG=0x0, guarded memory,
	 *	R/W,X for all, no ASID comparison, cache-inhibited.
	 * EPN		TWC	RPN
	 */
	{ 0x80000200,	0x1D,	0x800009FF },	/* PCI I/O - PS=8M */
	/*
	 *
	 * PCI/ISA Memory Space: CS5, Start address 0xC0000000, 512M?
	 *	ASID=0x0, APG=0x0, guarded memory,
	 *	R/W,X for all, no ASID comparison, cache-inhibited.
	 * EPN		TWC	RPN
	 */
	{ 0xC0000200,	0x1D,	0xC00009FF },	/* PCI Memory - PS=8M */
	/*
	 *
	 * PCI Bridge/QSPAN Registers: CS6, Start address 0xFA210000, 4K
	 *	ASID=0x0, APG=0x0, guarded memory,
	 *	R/W,X for all, no ASID comparison, cache-inhibited.
	 * EPN		TWC	RPN
	 */
	{ 0xFA210200,	0x11,	0xFA2109F7 },	/* QSPAN - PS=4K */
	/*
	 *
	 * PCMCIA Spaces: Start address 0xE0000000, 256M?
	 * For each space (MEM/DMA/ATTRIB/IO) only the first 8MB are mapped
	 *	ASID=0x0, APG=0x0, guarded memory,
	 *	R/W,X for all, no ASID comparison, cache-inhibited.
	 * EPN		                           TWC
	 * RPN
	 */
	{ (PCMCIA_MEM_ADDR & 0xfffff000) | 0x200, 0x1D,
	  (PCMCIA_MEM_ADDR & 0xfffff000) | 0x9F7 },/* PCMCIA Memory - PS=8M */

	{ (PCMCIA_DMA_ADDR & 0xfffff000) | 0x200, 0x1D,
	  (PCMCIA_DMA_ADDR & 0xfffff000) | 0x9F7 },/* PCMCIA DMA - PS=8M */

	{ (PCMCIA_ATTRB_ADDR & 0xfffff000) | 0x200, 0x1D,
	  (PCMCIA_ATTRB_ADDR & 0xfffff000) | 0x9F7 },/* PCMCIA ATTRIB-PS=8M*/

	{ (PCMCIA_IO_ADDR & 0xfffff000) | 0x200, 0x1D,
	  (PCMCIA_IO_ADDR & 0xfffff000) | 0x9F7 }     /* PCMCIA I/O - PS=8M */
};

/*
 * MMU_N_TLB_Table_Entries is defined here because the size of the
 * MMU_TLB_table is only known in this file.
 */
int MMU_N_TLB_Table_Entries = ( sizeof(MMU_TLB_table) / sizeof(MMU_TLB_table[0]) );
