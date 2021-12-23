/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include <bspopts.h>

	.text
	.globl	microblaze_invalidate_dcache
	.ent	microblaze_invalidate_dcache
	.align	2

microblaze_invalidate_dcache:
	addik 	r5, r0, BSP_MICROBLAZE_FPGA_DCACHE_BASE & (-(4 * BSP_MICROBLAZE_FPGA_DCACHE_LINE_LEN))
	addik	r6, r5, BSP_MICROBLAZE_FPGA_DCACHE_SIZE &  (-(4 * BSP_MICROBLAZE_FPGA_DCACHE_LINE_LEN))

L_start:
	wdc	r5, r0                                          /* Invalidate the Cache */

	cmpu	r18, r5, r6                                     /* Are we at the end? */
	blei	r18, L_done

	brid	L_start                                         /* Branch to the beginning of the loop */
	addik	r5, r5, (BSP_MICROBLAZE_FPGA_DCACHE_LINE_LEN * 4)   /* Increment the address by 4 (delay slot) */
L_done:
	rtsd	r15, 8                                          /* Return */
	nop

	.end	microblaze_invalidate_dcache
