/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup mpc55xx_asm
 *
 * @brief Exception minimum prologues.
 */

/*
 * Copyright (C) 2011, 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup mpc55xx_asm Assembler files
 *
 * @ingroup RTEMSBSPsPowerPCMPC55XX
 */

#include <bspopts.h>

#include <bsp/vectors.h>

	.globl mpc55xx_exc_vector_base

	.section ".bsp_text", "ax"

#if 5510 <= MPC55XX_CHIP_TYPE && MPC55XX_CHIP_TYPE <= 5517
	.align 12
#else
	.align 16
#endif

mpc55xx_exc_vector_base:

	stw	r1, ppc_exc_lock_crit@sdarel(r13)
	stw	r4, ppc_exc_vector_register_crit@sdarel(r13)
	li	r4, -32767
	b	ppc_exc_wrap_bookE_crit
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 2
	b	ppc_exc_wrap_nopush_bookE_crit
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 3
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 4
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	stw	r3, GPR3_OFFSET(r1)
	li	r3, -32763
	b	ppc_exc_interrupt
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 6
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 7
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 8
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 12
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 24
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	stw	r3, GPR3_OFFSET(r1)
	li	r3, -32752
	b	ppc_exc_interrupt
	stwu	r1, -PPC_EXC_INTERRUPT_FRAME_SIZE(r1)
	stw	r3, GPR3_OFFSET(r1)
	li	r3, -32749
	b	ppc_exc_interrupt
	stw	r1, ppc_exc_lock_crit@sdarel(r13)
	stw	r4, ppc_exc_vector_register_crit@sdarel(r13)
	li	r4, -32748
	b	ppc_exc_wrap_bookE_crit
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 18
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 17
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 13
	b	ppc_exc_wrap_nopush_bookE_crit
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 10
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 25
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 26
	b	ppc_exc_wrap_nopush_std
	stwu	r1, -EXC_GENERIC_SIZE(r1)
	stw	r4, GPR4_OFFSET(r1)
	li	r4, 15
	b	ppc_exc_wrap_nopush_std
