/*
 * Cirrus EP7312 Intererrupt handler
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *	
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *
 *  http://www.rtems.com/license/LICENSE.
 *
 *
 *  $Id$
*/
#define __asm__
#include "irq.h"
	
#define VECTOR_TABLE 0x40
	
/* 
 * Function to obtain, execute an IT handler and acknowledge the IT 
 */

	.globl ExecuteITHandler
ExecuteITHandler :
/*
 * Look at interrupt status register to determine source.
 * From source, determine offset into expanded vector table
 * and load handler address into r0.
 */

  ldr   r1, =0x80000000 /* close to interrupt status/mask registers 1 */
  ldr   r2, =0x80001000 /* close to interrupt status/mask registers 2 */
  ldr   r3, =0x80002000 /* close to interrupt status/mask registers 3 */

  stmdb	  sp!,{r4, r5, r6}

/*
 * INTSR3
 */
check_dai:
  ldr   r4, [r3, #0x240]
  ldr   r5, [r3, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
  tst   r6, #0x0001
  beq   check_extfiq
  ldr	r0, =(VECTOR_TABLE + (4 * 21)) /* load the vector number */
  b     get_handler

/*
 * INTSR1
 */
check_extfiq:
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
  tst   r6, #0x0001
  beq   check_bl
  ldr	r0, =(VECTOR_TABLE + (4 * 0)) /* load the vector number */
  b     get_handler

check_bl:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0002
  beq   check_we
  ldr	r0, =(VECTOR_TABLE + (4 * 1)) /* load the vector number */
  b     get_handler

check_we:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0004
  beq   check_mc
  ldr	r0, =(VECTOR_TABLE + (4 * 2)) /* load the vector number */
  b     get_handler

check_mc:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0008
  beq   check_cs
  ldr	r0, =(VECTOR_TABLE + (4 * 3)) /* load the vector number */
  b     get_handler

check_cs:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0010
  beq   check_e1
  ldr	r0, =(VECTOR_TABLE + (4 * 4)) /* load the vector number */
  b     get_handler

check_e1:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0020
  beq   check_e2
  ldr	r0, =(VECTOR_TABLE + (4 * 5)) /* load the vector number */
  b     get_handler

check_e2:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0040
  beq   check_e3
  ldr	r0, =(VECTOR_TABLE + (4 * 6)) /* load the vector number */
  b     get_handler

check_e3:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0080
  beq   check_tc1
  ldr	r0, =(VECTOR_TABLE + (4 * 7)) /* load the vector number */
  b     get_handler

check_tc1:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0100
  beq   check_tc2
  ldr	r0, =(VECTOR_TABLE + (4 * 8)) /* load the vector number */
  b     get_handler

check_tc2:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0200
  beq   check_rtc
  ldr	r0, =(VECTOR_TABLE + (4 * 9)) /* load the vector number */
  b     get_handler

check_rtc:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0400
  beq   check_tick
  ldr	r0, =(VECTOR_TABLE + (4 * 10)) /* load the vector number */
  b     get_handler

check_tick:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0800
  beq   check_utx1
  ldr	r0, =(VECTOR_TABLE + (4 * 11)) /* load the vector number */
  b     get_handler

check_utx1:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x1000
  beq   check_urx1
  ldr	r0, =(VECTOR_TABLE + (4 * 12)) /* load the vector number */
  b     get_handler

check_urx1:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x2000
  beq   check_ums
  ldr	r0, =(VECTOR_TABLE + (4 * 13)) /* load the vector number */
  b     get_handler

check_ums:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x4000
  beq   check_sse
  ldr	r0, =(VECTOR_TABLE + (4 * 14)) /* load the vector number */
  b     get_handler

check_sse:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r1, #0x240]
  ldr   r5, [r1, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x8000
  beq   check_kbd
  ldr	r0, =(VECTOR_TABLE + (4 * 15)) /* load the vector number */
  b     get_handler

/*
 * INTSR2
 */
check_kbd:
  ldr   r4, [r2, #0x240]
  ldr   r5, [r2, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
  tst   r6, #0x0001
  beq   check_ss2rx
  ldr	r0, =(VECTOR_TABLE + (4 * 16)) /* load the vector number */
  b     get_handler

check_ss2rx:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r2, #0x240]
  ldr   r5, [r2, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0002
  beq   check_ss2tx
  ldr	r0, =(VECTOR_TABLE + (4 * 17)) /* load the vector number */
  b     get_handler

check_ss2tx:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r2, #0x240]
  ldr   r5, [r2, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x0004
  beq   check_utx2
  ldr	r0, =(VECTOR_TABLE + (4 * 18)) /* load the vector number */
  b     get_handler

check_utx2:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r2, #0x240]
  ldr   r5, [r2, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x1000
  beq   check_urx2
  ldr	r0, =(VECTOR_TABLE + (4 * 19)) /* load the vector number */
  b     get_handler

check_urx2:
#if 0
MUST REMEMBER TO UNCOMMENT IF THIS HANDLER MOVES
  ldr   r4, [r2, #0x240]
  ldr   r5, [r2, #0x280]
  and   r6, r4, r5 /* only look at interrupts which are enabled */
#endif
  tst   r6, #0x2000
  beq   IRQ_NoInterrupt
  ldr	r0, =(VECTOR_TABLE + (4 * 20)) /* load the vector number */
  b     get_handler

get_handler:

  ldmia	  sp!,{r4, r5, r6}

  ldr	r0, [r0]		/* extract the IT handler @ */

  /*
   * re-enable interrupts at processor level as the current
   * interrupt source is now masked via VEGA logic
   */
/*
  mrs	r1, cpsr
  and	r1, r1, #0xFFFFFF3F
  msr	cpsr, r1
*/

  stmdb	  sp!,{lr}
  ldr     lr, =IRQ_return         /* prepare the return from handler  */

  mov     pc, r0			/* EXECUTE INT HANDLER */

IRQ_return:
  ldmia sp!,{lr}

IRQ_NoInterrupt:
  /* return to the "main" interrupt handler */
  mov pc, lr
