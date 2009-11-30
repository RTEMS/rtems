/*
 * mmu.c
 *
 * This file contains routines for initializing
 * and manipulating the MMU on the MPC8xx.
 *
 * Copyright (c) 1999, National Research Council of Canada
 *
 * Trivially modified for mpc8260 21.3.2001
 * Andy Dachs <a.dachs@sstl.co.uk>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems.h>
#include <mpc8260.h>
#include <mpc8260/mmu.h>

/*
 * mmu_init
 *
 * This routine sets up the virtual memory maps on an MPC8xx.
 * The MPC8xx does not support block address translation (BATs)
 * and does not have segment registers. Thus, we must set up page
 * translation. However, its MMU supports variable size pages
 * (1-, 4-, 16-, 512-Kbyte or 8-Mbyte), which simplifies the task.
 *
 * The MPC8xx has separate data and instruction 32-entry translation
 * lookaside buffers (TLB). By mapping all of DRAM as one huge page,
 * we can preload the TLBs and not have to be concerned with taking
 * TLB miss exceptions.
 *
 * We set up the virtual memory map so that virtual address of a
 * location is equal to its real address.
 */
void mmu_init( void )
{
#if 0
/* so far we leave mmu uninitialised */

  register uint32_t   reg1, i;

  /*
   * Initialize the TLBs
   *
   * Instruction address translation and data address translation
   * must be disabled during initialization (IR=0, DR=0 in MSR).
   * We can assume the MSR has already been set this way.
   */

  /*
   * Initialize IMMU & DMMU Control Registers (MI_CTR & MD_CTR)
   * 	GPM [0]			0b0 = PowerPC mode
   *	PPM [1]			0b0 = Page resolution of protection
   *	CIDEF [2]		0b0/0b1 = Default cache-inhibit attribute =
   *							NO for IMMU, YES for DMMU!
   *	reserved/WTDEF [3]	0b0 = Default write-through attribute = not
   *	RSV4x [4]		0b0 = 4 entries not reserved
   *	reserved/TWAM [5]	0b0/0b1 = 4-Kbyte page hardware assist
   *	PPCS [6]		0b0 = Ignore user/supervisor state
   *	reserved [7-18]		0x00
   *	xTLB_INDX [19-23]	31 = 0x1F
   *	reserved [24-31]	0x00
   *
   * Note: It is important that cache-inhibit be set as the default for the
   * data cache when the DMMU is disabled in order to prevent internal memory
   * mapped registers from being cached accidentally when address translation
   * is turned off at the start of exception processing.
   */
  reg1 = M8xx_MI_CTR_ITLB_INDX(31);
  _mtspr( M8xx_MI_CTR, reg1 );
  reg1 = M8xx_MD_CTR_CIDEF | M8xx_MD_CTR_TWAM | M8xx_MD_CTR_DTLB_INDX(31);
  _mtspr( M8xx_MD_CTR, reg1 );
  _isync;

  /*
   * Invalidate all TLB entries in both TLBs.
   * Note: We rely on the RSV4 bit in MI_CTR and MD_CTR being 0b0, so
   *       all 32 entries are invalidated.
   */
  __asm__ volatile ("tlbia\n"::);
  _isync;

  /*
   * Set Current Address Space ID Register (M_CASID).
   * Supervisor: CASID = 0
   */
  reg1 = 0;
  _mtspr( M8xx_M_CASID, reg1 );

  /*
   * Initialize the MMU Access Protection Registers (MI_AP, MD_AP)
   * We ignore the Access Protection Group (APG) mechanism globally
   * by setting all of the Mx_AP fields to 0b01 : client access
   * permission is defined by page protection bits.
   */
  reg1 = 0x55555555;
  _mtspr( M8xx_MI_AP, reg1 );
  _mtspr( M8xx_MD_AP, reg1 );

  /*
   * Load both 32-entry TLBs with values from the MMU_TLB_table
   * which is defined in the BSP.
   * Note the _TLB_Table must have at most 32 entries. This code
   * makes no effort to enforce this restriction.
   */
  for( i = 0; i < MMU_N_TLB_Table_Entries; ++i ) {
    reg1 = MMU_TLB_table[i].mmu_epn;
    _mtspr( M8xx_MI_EPN, reg1 );
    _mtspr( M8xx_MD_EPN, reg1 );
    reg1 = MMU_TLB_table[i].mmu_twc;
    _mtspr( M8xx_MI_TWC, reg1 );
    _mtspr( M8xx_MD_TWC, reg1 );
    reg1 = MMU_TLB_table[i].mmu_rpn;	/* RPN must be written last! */
    _mtspr( M8xx_MI_RPN, reg1 );
    _mtspr( M8xx_MD_RPN, reg1 );
  }

  /*
   * Turn on address translation by setting MSR[IR] and MSR[DR].
   */
  _CPU_MSR_GET( reg1 );
  reg1 |= PPC_MSR_IR | PPC_MSR_DR;
  _CPU_MSR_SET( reg1 );

#endif

}
