/******************************************************************************
* Copyright (c) 2008 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

#include <bspopts.h>

	.text
	.globl	microblaze_invalidate_icache
	.ent	microblaze_invalidate_icache
	.align	2

microblaze_invalidate_icache:
    addik 	r5, r0, BSP_MICROBLAZE_FPGA_ICACHE_BASE & (-(4 * BSP_MICROBLAZE_FPGA_ICACHE_LINE_LEN))  /* Align to cache line */
	addik	r6, r5, BSP_MICROBLAZE_FPGA_ICACHE_SIZE & (-(4 * BSP_MICROBLAZE_FPGA_ICACHE_LINE_LEN))  /* Compute end */

L_start:
	wic	r5, r0                                          /* Invalidate the Cache */

	cmpu	r18, r5, r6                                     /* Are we at the end? */
	blei	r18, L_done

	brid	L_start                                         /* Branch to the beginning of the loop */
	addik	r5, r5, (BSP_MICROBLAZE_FPGA_ICACHE_LINE_LEN * 4)   /* Increment the address by 4 (delay slot) */
L_done:
	rtsd	r15, 8                                          /* Return */
    nop
	.end	microblaze_invalidate_icache