/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief Implementation of _CPU_Context_volatile_clobber
 *
 * This file implements _CPU_Context_volatile_clobber for use in spcontext01.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

	.section	.text

FUNCTION_ENTRY(_CPU_Context_volatile_clobber)

.macro clobber_register reg
	sub	x0, x0, #1
	mov	\reg, x0
.endm

#ifdef AARCH64_MULTILIB_VFP
	mrs	x1, FPCR
	ldr	x2, =0xf000001f
	bic	x1, x1, x2
	and	x2, x2, x0
	orr	x1, x1, x2
	msr	FPCR, x1

.macro clobber_vfp_register reg
	sub	x0, x0, #1
	fmov	\reg, x0
.endm

	clobber_vfp_register	d0
	clobber_vfp_register	d1
	clobber_vfp_register	d2
	clobber_vfp_register	d3
	clobber_vfp_register	d4
	clobber_vfp_register	d5
	clobber_vfp_register	d6
	clobber_vfp_register	d7
	clobber_vfp_register	d16
	clobber_vfp_register	d17
	clobber_vfp_register	d18
	clobber_vfp_register	d19
	clobber_vfp_register	d20
	clobber_vfp_register	d21
	clobber_vfp_register	d22
	clobber_vfp_register	d23
	clobber_vfp_register	d24
	clobber_vfp_register	d25
	clobber_vfp_register	d26
	clobber_vfp_register	d27
	clobber_vfp_register	d28
	clobber_vfp_register	d29
	clobber_vfp_register	d30
	clobber_vfp_register	d31
#endif /* AARCH64_MULTILIB_VFP */

/*
 * According to the AAPCS64, X0-X18 and X29 are caller-saved registers. X0 is
 * already being clobbered.
 */
	clobber_register	x1
	clobber_register	x2
	clobber_register	x3
	clobber_register	x4
	clobber_register	x5
	clobber_register	x6
	clobber_register	x7
	clobber_register	x8
	clobber_register	x9
	clobber_register	x10
	clobber_register	x11
	clobber_register	x12
	clobber_register	x13
	clobber_register	x14
	clobber_register	x15
	clobber_register	x16
	clobber_register	x17
	clobber_register	x18
	clobber_register	x29

	ret

FUNCTION_END(_CPU_Context_volatile_clobber)
