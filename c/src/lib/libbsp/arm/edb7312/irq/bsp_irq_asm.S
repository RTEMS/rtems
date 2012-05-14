/*
 * Cirrus EP7312 Intererrupt handler
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
*/
#define __asm__
#include "irq.h"

	.extern edb7312_interrupt_dispatch

/*
 * Function to obtain, execute an IT handler and acknowledge the IT
 */

	.globl bsp_interrupt_dispatch
bsp_interrupt_dispatch :
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
  mov   r0, #BSP_DAIINT
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
  mov   r0, #BSP_EXTFIQ
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
  mov   r0, #BSP_BLINT
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
  mov   r0, #BSP_WEINT
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
  mov   r0, #BSP_MCINT
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
  mov   r0, #BSP_CSINT
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
  mov   r0, #BSP_EINT1
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
  mov   r0, #BSP_EINT2
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
  mov   r0, #BSP_EINT3
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
  mov   r0, #BSP_TC1OI
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
  mov   r0, #BSP_TC2OI
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
  mov   r0, #BSP_RTCMI
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
  mov   r0, #BSP_TINT
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
  mov   r0, #BSP_UTXINT1
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
  mov   r0, #BSP_URXINT1
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
  mov   r0, #BSP_UMSINT
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
  mov   r0, #BSP_SSEOTI
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
  mov   r0, #BSP_KBDINT
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
  mov   r0, #BSP_SS2RX
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
  mov   r0, #BSP_SS2TX
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
  mov   r0, #BSP_UTXINT2
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
  mov   r0, #BSP_URXINT2
  b     get_handler

get_handler:

  ldmia	  sp!,{r4, r5, r6}

  /*
   * re-enable interrupts at processor level as the current
   * interrupt source is now masked via VEGA logic
   */
/*
  mrs	r1, cpsr
  and	r1, r1, #0xFFFFFF3F
  msr	cpsr, r1
*/

  stmdb sp!,{lr}
  bl    edb7312_interrupt_dispatch
  ldmia sp!,{lr}

IRQ_NoInterrupt:
  /* return to the "main" interrupt handler */
  mov pc, lr
