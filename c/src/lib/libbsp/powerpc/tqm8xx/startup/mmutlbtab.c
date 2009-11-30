/*===============================================================*\
| Project: RTEMS TQM8xx BSP                                       |
+-----------------------------------------------------------------+
| This file has been adapted to MPC8xx by                         |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
/* derived from: */
/*
 * mmutlbtab.c
 *
 * Copyright (c) 1999, National Research Council of Canada
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <mpc8xx/mmu.h>
/*
 * This MMU_TLB_table is used to statically initialize the Table Lookaside
 * Buffers in the MMU of the TQM8xx board.
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
  /*
   * DRAM: Start address 0x00000000, 128M,
   *	ASID=0x0, APG=0x0, not guarded memory, copyback data cache policy,
   *	R/W,X for all, no ASID comparison, not cache-inhibited.
   * EPN		TWC	RPN
   */
  { 0x00000200,	0x0D,	0x000009FD },	/* DRAM - PS=8M */
  { 0x00800200,	0x0D,	0x008009FD },	/* DRAM - PS=8M */
  { 0x01000200,	0x0D,	0x010009FD },	/* DRAM - PS=8M */
  { 0x01800200,	0x0D,	0x018009FD },	/* DRAM - PS=8M */
  { 0x02000200,	0x0D,	0x020009FD },	/* DRAM - PS=8M */
  { 0x02800200,	0x0D,	0x028009FD },	/* DRAM - PS=8M */
  { 0x03000200,	0x0D,	0x030009FD },	/* DRAM - PS=8M */
  { 0x03800200,	0x0D,	0x038009FD },	/* DRAM - PS=8M */
  { 0x04000200,	0x0D,	0x040009FD },	/* DRAM - PS=8M */
  { 0x04800200,	0x0D,	0x048009FD },	/* DRAM - PS=8M */
  { 0x05000200,	0x0D,	0x050009FD },	/* DRAM - PS=8M */
  { 0x05800200,	0x0D,	0x058009FD },	/* DRAM - PS=8M */
  { 0x06000200,	0x0D,	0x060009FD },	/* DRAM - PS=8M */
  { 0x06800200,	0x0D,	0x068009FD },	/* DRAM - PS=8M */
  { 0x07000200,	0x0D,	0x070009FD },	/* DRAM - PS=8M */
  { 0x07800200,	0x0D,	0x078009FD },	/* DRAM - PS=8M */
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
   * Flash: Start address 0x40000000, 8M,
   *	ASID=0x0, APG=0x0, not guarded memory,
   *	R/O,X for all, no ASID comparison, not cache-inhibited.
   * EPN		TWC	RPN
   */
  { 0x40000200,	0x0D,	0x40000CFD } 	/* Flash - PS=8M   */
};

/*
 * MMU_N_TLB_Table_Entries is defined here because the size of the
 * MMU_TLB_table is only known in this file.
 */
int MMU_N_TLB_Table_Entries = ( sizeof(MMU_TLB_table) / sizeof(MMU_TLB_table[0]) );
