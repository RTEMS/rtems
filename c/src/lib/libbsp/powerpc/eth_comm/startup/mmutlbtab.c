/* 
 * mmutlbtab.c
 * 
 * This file defines the MMU_TLB_table for the eth_comm board. 
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <mpc8xx/mmu.h>

/*
 * This MMU_TLB_table is used to statically initialize the Table Lookaside
 * Buffers in the MMU of the MPC860 processor.
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
	 * DRAM: CS1, Start address 0x00000000, 8M, 
	 *	ASID=0x0, APG=0x0, not guarded memory, copyback data cache policy, 
	 *	R/W,X for supervisor, no ASID comparison, not cache-inhibited.
	 * EPN	        TWC     RPN
	 */
	{ 0x00000200,	0x0D,	0x000001FD }	/* DRAM - PS=PS=8M */
};

/* 
 * MMU_N_TLB_Table_Entries is defined here because the size of the
 * MMU_TLB_table is only known in this file.
 */
int MMU_N_TLB_Table_Entries = ( sizeof(MMU_TLB_table) / sizeof(MMU_TLB_table[0]) );
