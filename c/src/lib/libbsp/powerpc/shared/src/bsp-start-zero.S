/**
 * @file
 *
 * @ingroup bsp_start
 *
 * @brief bsp_start_zero() implementation.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#include <rtems/asm.h>
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
	clrrwi	r11, r11, 5
	addi	r10, r11, 32
	subf	r11, r3, r10
	cmplw	cr7, r11, r4
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
	srwi	r11, r11, 5
	addi	r11, r11, 1
	mtctr	r11

	/* Main loop */
	b	main_loop_update
main_loop_begin:
#if BSP_DATA_CACHE_ENABLED
	dcbz	r0, r3
	dcbf	r0, r3
#else
	stw	r0, 0(r3)
	stw	r0, 4(r3)
	stw	r0, 8(r3)
	stw	r0, 12(r3)
	stw	r0, 16(r3)
	stw	r0, 20(r3)
	stw	r0, 24(r3)
	stw	r0, 28(r3)
#endif
	addi	r3, r3, 32
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
