/* SPDX-License-Identifier: BSD-2-Clause */

/*
 * Copyright (c) 2017 embedded brains GmbH & Co. KG
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

/* rtems_crti.S */
#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#if defined(__powerpc64__)
	.section ".init","ax"
	.align 2
	.globl	_init
	.type	_init,@function
_init:
	mflr	r0
	std	r0,16(1)
	stdu	r1,-96(1)

	.section ".fini","ax"
	.align 2
	.globl	_fini
	.type	_fini,@function
_fini:
	mflr	r0
	std	r0,16(r1)
	stdu	r1,-96(r1)
#else
	/* terminate the __init() function and create
	 * a new head '_init' for use by RTEMS to
	 * invoke C++ global constructors
	 * NOTE: it is essential that this snippet
	 * is hooked between ecrti and crtbegin
	 *
	 * ecrti has the following .init section:
	 * __init:
	 *    stwu r1,-16(r1)
	 *    mflr r0
	 *    stw  r0,20(r1)
	 *
	 * The reason for this is that we want to call
	 * __eabi() at an early stage but prevent __eabi()
	 * from branching to __init (C++ exception init
	 * and global CTORs). Hence we make __init a no-op
	 * and create a new entry point:
	 */
	.section ".init","ax"
	.align 2
	lwz		r0,r20(r1)
	mtlr	r0
	addi	r1,r1,16
	blr
	.globl	_init
	.type	_init,@function
_init:
	stwu	r1,-16(r1)
	mflr	r0
	stw		r0,20(r1)
#endif
