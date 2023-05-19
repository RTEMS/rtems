/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2013, 2017 embedded brains GmbH & Co. KG
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

#ifdef __thumb__
FUNCTION_THUMB_ENTRY(_CPU_Context_volatile_clobber)
#else
FUNCTION_ENTRY(_CPU_Context_volatile_clobber)
#endif

.macro clobber_register reg
	sub	r0, r0, #1
	mov	\reg, r0
.endm

#ifdef ARM_MULTILIB_VFP
	vmrs	r1, FPSCR
	ldr	r2, =0xf000001f
	bic	r1, r1, r2
	and	r2, r2, r0
	orr	r1, r1, r2
	vmsr	FPSCR, r1

.macro clobber_vfp_register reg
	sub	r0, r0, #1
	vmov	\reg, r0, r0
.endm

	clobber_vfp_register	d0
	clobber_vfp_register	d1
	clobber_vfp_register	d2
	clobber_vfp_register	d3
	clobber_vfp_register	d4
	clobber_vfp_register	d5
	clobber_vfp_register	d6
	clobber_vfp_register	d7
#ifdef ARM_MULTILIB_VFP_D32
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
#endif /* ARM_MULTILIB_VFP_D32 */
#endif /* ARM_MULTILIB_VFP */

	clobber_register	r1
	clobber_register	r2
	clobber_register	r3
	clobber_register	r12

	bx	lr

FUNCTION_END(_CPU_Context_volatile_clobber)
