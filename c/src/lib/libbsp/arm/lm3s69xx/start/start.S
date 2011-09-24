/*
 * Copyright (c) 2011 embedded brains GmbH.  All rights reserved.
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
#include <rtems/system.h>	
#include <rtems/score/cpu.h>
	
#include <bspopts.h>
#include <bsp/irq.h>
#include <bsp/linker-symbols.h>

/* External symbols */

.extern bsp_start_hook
.extern boot_card
.extern bsp_reset

/* Global symbols */

.globl _start

.syntax unified

.section ".bsp_start_text", "ax"
.thumb
.word bsp_stack_main_end
.word _start /* Reset */
.word bsp_reset /* NMI */
.word bsp_reset /* Hard Fault */
.word bsp_reset /* MPU Fault */
.word bsp_reset /* Bus Fault */
.word bsp_reset /* Usage Fault */
.word 0 /* Reserved */
.word 0 /* Reserved */
.word 0 /* Reserved */
.word 0 /* Reserved */
.word bsp_reset /* SVC */
.word bsp_reset /* Debug Monitor */
.word 0 /* Reserved */
.word bsp_reset /* PendSV */
.word bsp_reset /* SysTick */
.rept BSP_INTERRUPT_VECTOR_MAX - 15
.word bsp_reset /* IRQ */
.endr

.thumb_func
_start:
	bl	bsp_start_hook_0
	mov	r0, #0
	bl	boot_card

twiddle:
	bl	bsp_reset
	b	twiddle
