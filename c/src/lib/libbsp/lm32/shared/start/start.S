/*  LM32 startup code
 *
 *  This is the entry point on reset and when loading the
 *  executive from a bootloader.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#include "bspopts.h"

     .section .boot,"a",@progbits
     .align    4

     .globl  start
     .type   start,@function
     .globl  _start
     .type   _start,@function
     .globl  __start
     .type   __start,@function
     .globl  LatticeDDInit
     .type   LatticeDDInit,@function

LatticeDDInit:	
__start:	
_start:	
start:
	/* Clear r0 */
	xor	r0,r0,r0
	/* Disable interrupts */
	wcsr	IE, r0
	/* Mask all interrupts */
	wcsr	IM,r0
	/* Set exception base address */
	mvhi	r1, hi(start)
	ori	r1, r1, lo(start)
	wcsr	EBA, r1
	bi	crt0
	nop
/*
 * Unused handlers call debug handlers
 */
breakpoint_handler:
	rcsr	r7, DEBA
	addi	r7, r7, 32
	b	r7 
	nop
	nop
	nop
	nop
	nop
instruction_bus_error_handler:
	rcsr	r7, DEBA
	addi	r7, r7, 64
	b	r7 
	nop
	nop
	nop
	nop
	nop
watchpoint_handler:
	rcsr	r7, DEBA
	addi	r7, r7, 96
	b	r7 
	nop
	nop
	nop
	nop
	nop
data_bus_error_handler:
	rcsr	r7, DEBA
	addi	r7, r7, 128
	b	r7 
	nop
	nop
	nop
	nop
	nop
divide_by_zero_handler:
	rcsr	r7, DEBA
	addi	r7, r7, 160
	b	r7 
	nop
	nop
	nop
	nop
	nop
interrupt_handler:
	.extern _ISR_Handler
	bi	_ISR_Handler
	nop
	nop
	nop
	nop
	nop
	nop
	nop
system_call_handler:
	rcsr	r7, DEBA
	addi	r7, r7, 224
	b	r7 
	nop
	nop
	nop
	nop
	nop

	.text
crt0:	
	/* Flush data cache */
	addi	r1, r0, 1
	wcsr	DCC, r1
	nop
	nop
	nop
	nop
	/* Flush Instruction Cache */
	wcsr	ICC, r1
	nop
	nop
	nop
	nop
	/* Initialize stack pointer */
	mvhi	sp, hi(_fstack-4)
	ori	sp, sp, lo(_fstack-4)
	/* Initialize global pointer */
	mvhi	gp, hi(_edata)
	ori	gp, gp, lo(_edata)
	/* Clear bss */
	mvhi	r1, hi(_clear_start)
	ori	r1, r1, lo(_clear_start)
	mvhi	r3, hi(_clear_end)
	ori	r3, r3, lo(_clear_end)
.clear_bss:
	be	r1, r3, .end_clear_bss
	sw	(r1+0), r0
	addi	r1, r1, 4
	bi	.clear_bss
.end_clear_bss:
	mvi	r1, 0
	mvi	r2, 0
	mvi	r3, 0
	calli	boot_card
	# boot_card returns when RTEMS is shutdown
#if ON_SIMULATOR
	#define	SYS_exit	1

        mvi     r8, SYS_exit
        scall   
#endif

.dead_end:
	bi	.dead_end

