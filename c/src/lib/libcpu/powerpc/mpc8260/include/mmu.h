/*
 * mmu.h
 *
 * This include file contains definitions pertaining
 * to the MMU on the MPC8xx.
 *
 * Copyright (c) 1999, National Research Council of Canada
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _MPC8260_MMU_H
#define _MPC8260_MMU_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * The MMU_TLB_table is used to statically initialize the Table Lookaside
 * Buffers in the MMU of an MPC8260.
 */
typedef struct {
  uint32_t  	mmu_epn;	/* Effective Page Number */
  uint32_t  	mmu_twc;	/* Tablewalk Control Register */
  uint32_t  	mmu_rpn;	/* Real Page Number */
} MMU_TLB_table_t;

/*
 * The MMU_TLB_table and its size, MMU_N_TLB_Table_Entries, must be
 * supplied by the BSP.
 */
extern MMU_TLB_table_t MMU_TLB_table[];	/* MMU TLB table supplied by BSP */
extern int MMU_N_TLB_Table_Entries;	/* Number of entries in MMU TLB table */

/* Functions */

void mmu_init( void );

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
