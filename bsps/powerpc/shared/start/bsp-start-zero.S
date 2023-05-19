/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup bsp_start
 *
 * @brief bsp_start_zero() implementation.
 */

/*
 * Copyright (C) 2010, 2014 embedded brains GmbH & Co. KG
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

#include <rtems/asm.h>
#include <libcpu/powerpc-utility.h>
#include <bspopts.h>

	.globl bsp_start_zero
	.globl bsp_start_zero_begin
	.globl bsp_start_zero_end
	.globl bsp_start_zero_size

	.section ".bsp_start_text", "ax"
	.type bsp_start_zero, @function
bsp_start_zero:
bsp_start_zero_begin:
	li	r0, 0
	subi	r11, r3, 1
	CLEAR_RIGHT_IMMEDIATE	r11, r11, PPC_CACHE_ALIGN_POWER
	addi	r10, r11, PPC_CACHE_ALIGNMENT
	subf	r11, r3, r10
	COMPARE_LOGICAL	cr7, r11, r4
	add	r9, r3, r4
	ble-	cr7, head_end_done
	mr	r10, r9
head_end_done:
	subf	r11, r3, r10
	addi	r11, r11, 1
	mtctr	r11

	/* Head loop */
	b	head_loop_update
head_loop_begin:
	stb	r0, 0(r3)
	addi	r3, r3, 1
head_loop_update:
	bdnz+	head_loop_begin

	subf	r11, r3, r9
	SHIFT_RIGHT_IMMEDIATE	r11, r11, PPC_CACHE_ALIGN_POWER
	addi	r11, r11, 1
	mtctr	r11

	/* Main loop */
	b	main_loop_update
main_loop_begin:
#if BSP_DATA_CACHE_ENABLED
	dcbz	r0, r3
	dcbf	r0, r3
#else
  #if PPC_CACHE_ALIGNMENT == 32 || PPC_CACHE_ALIGNMENT == 64
	stw	r0, 0(r3)
	stw	r0, 4(r3)
	stw	r0, 8(r3)
	stw	r0, 12(r3)
	stw	r0, 16(r3)
	stw	r0, 20(r3)
	stw	r0, 24(r3)
	stw	r0, 28(r3)
    #if PPC_CACHE_ALIGNMENT == 64
	stw	r0, 32(r3)
	stw	r0, 36(r3)
	stw	r0, 40(r3)
	stw	r0, 44(r3)
	stw	r0, 48(r3)
	stw	r0, 52(r3)
	stw	r0, 56(r3)
	stw	r0, 60(r3)
    #endif
  #else
    #error "unsupported cache alignment"
  #endif
#endif
	addi	r3, r3, PPC_CACHE_ALIGNMENT
main_loop_update:
	bdnz+	main_loop_begin

	subf	r9, r3, r9
	addi	r9, r9, 1
	mtctr	r9

	/* Tail loop */
	b	tail_loop_update
tail_loop_begin:
	stb	r0, 0(r3)
	addi	r3, r3, 1
tail_loop_update:
	bdnz+	tail_loop_begin

	/* Return */
	sync
	isync
	blr

bsp_start_zero_end:
	.set bsp_start_zero_size, bsp_start_zero_end - bsp_start_zero_begin
