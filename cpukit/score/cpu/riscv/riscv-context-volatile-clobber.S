/*
 * Copyright (c) 2018 embedded brains GmbH
 * Copyright (c) 2015 Hesham Almatary <hesham@alumni.york.ac.uk>
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>

	.section	.text, "ax", @progbits
	.align	2

PUBLIC(_CPU_Context_volatile_clobber)
SYM(_CPU_Context_volatile_clobber):

#if __riscv_flen > 0
	andi	t0, a0, 0x1f
	fsflags	t0

	addi	t0, a0, 15
	FMVYX	ft0, t0
	addi	t0, a0, 16
	FMVYX	ft1, t0
	addi	t0, a0, 17
	FMVYX	ft2, t0
	addi	t0, a0, 18
	FMVYX	ft3, t0
	addi	t0, a0, 19
	FMVYX	ft4, t0
	addi	t0, a0, 20
	FMVYX	ft5, t0
	addi	t0, a0, 21
	FMVYX	ft6, t0
	addi	t0, a0, 22
	FMVYX	ft7, t0
	addi	t0, a0, 23
	FMVYX	ft8, t0
	addi	t0, a0, 24
	FMVYX	ft9, t0
	addi	t0, a0, 25
	FMVYX	ft10, t0
	addi	t0, a0, 26
	FMVYX	ft11, t0
	addi	t0, a0, 27
	FMVYX	fa0, t0
	addi	t0, a0, 28
	FMVYX	fa1, t0
	addi	t0, a0, 29
	FMVYX	fa2, t0
	addi	t0, a0, 30
	FMVYX	fa3, t0
	addi	t0, a0, 31
	FMVYX	fa4, t0
	addi	t0, a0, 32
	FMVYX	fa5, t0
	addi	t0, a0, 33
	FMVYX	fa6, t0
	addi	t0, a0, 34
	FMVYX	fa7, t0
#endif /* __riscv_flen */

	addi	a1, a0, 1
	addi	a2, a0, 2
	addi	a3, a0, 3
	addi	a4, a0, 4
	addi	a5, a0, 5
	addi	a6, a0, 6
	addi	a7, a0, 7
	addi	t0, a0, 8
	addi	t1, a0, 9
	addi	t2, a0, 10
	addi	t3, a0, 11
	addi	t4, a0, 12
	addi	t5, a0, 13
	addi	t6, a0, 14

	ret
