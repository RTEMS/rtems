/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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
#include <rtems/mips/iregdef.h>

/*
 * The caller saved registers of the ABI are v0, v1, a0 to a3 and t0 to t9.
 * a0 arrives with the pattern and is clobbered last.
 */

	.set noreorder

	.text

FRAME(_CPU_Context_volatile_clobber,sp,0,ra)

.macro clobber_register reg, inc
	addiu	\reg, a0, \inc
.endm

	clobber_register	v0, 1
	clobber_register	v1, 2
	clobber_register	a1, 3
	clobber_register	a2, 4
	clobber_register	a3, 5
	clobber_register	t0, 6
	clobber_register	t1, 7
	clobber_register	t2, 8
	clobber_register	t3, 9
	clobber_register	t4, 10
	clobber_register	t5, 11
	clobber_register	t6, 12
	clobber_register	t7, 13
	clobber_register	t8, 14
	clobber_register	t9, 15

	j	ra
	 clobber_register	a0, 16

ENDFRAME(_CPU_Context_volatile_clobber)
