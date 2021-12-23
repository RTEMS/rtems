/******************************************************************************
* Copyright (c) 2004 - 2020 Xilinx, Inc.  All rights reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/

	.text
	.globl	microblaze_enable_dcache
	.ent	microblaze_enable_dcache
	.align	2
microblaze_enable_dcache:
    /* Read the MSR register */
	mfs	r8, rmsr
	/* Set the interrupt enable bit */
	ori	r8, r8, 0x80
	/* Save the MSR register */
	mts	rmsr, r8
	/* Return */
	rtsd	r15, 8
	nop
    .end	microblaze_enable_dcache
