/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2015, 2016 embedded brains GmbH & Co. KG
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

#include <libcpu/powerpc-utility.h>

	.global qoriq_l2cache_flush_invalidate

	.section ".bsp_start_text", "ax"

qoriq_l2cache_flush_invalidate:

	/* Flush L2 cache */
	lwz	r4, 0(r3)
	oris	r4, r4, FSL_EIS_L2CSR0_L2FL >> 16
	stw	r4, 0(r3)
1:
	lwz	r4, 0(r3)
	andis.	r4, r4, FSL_EIS_L2CSR0_L2FL >> 16
	bne	1b
	isync

	/* Invalidate L2 cache */
	lwz	r4, 0(r3)
	oris	r4, r4, FSL_EIS_L2CSR0_L2FI >> 16
	stw	r4, 0(r3)
1:
	lwz	r4, 0(r3)
	andis.	r4, r4, FSL_EIS_L2CSR0_L2FI >> 16
	bne	1b
	isync

	blr
