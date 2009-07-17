/**
 *  @file bsp_irq_asm.S
 *
 *  Intererrupt handler for GameBoy Advance.
 */
/*
 *  RTEMS GBA BSP
 *
 *  Copyright (c) 2004  Markku Puro <markku.puro@kopteri.net>
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define   __asm__
#include  <asm_macros.h>
#include  <gba_registers.h>
#include  <arm_mode_bits.h>
/* @cond  INCLUDE_ASM */

/**
 *  Execute interrupt handler
 *  function void bsp_interrupt_dispatch(void)
 *
 * Look at interrupt status register to determine source.
 * From source, determine offset into expanded vector table
 * and load handler address into r0.
 * irq_vector_table is defined in linkcmds
 *
 */
    .align
/*  .section  .iwram  */

PUBLIC_ARM_FUNCTION(bsp_interrupt_dispatch)
  ldr   r1, =GBA_REG_IE_ADDR
  ldrh  r1, [r1]
  ldr   r2, =GBA_REG_IF_ADDR
  ldrh  r2, [r2]
  and   r3, r1, r2  /* only look at interrupts which are enabled */

check_lcdv:
  tst   r3, #0x0001
  beq   check_lcdh
  ldr   r0, =(irq_vector_table + (4 * 0)) /* load the vector number */
  ldr   r3,=0x0001
  b     get_handler

check_lcdh:
  tst   r3, #0x0002
  beq   check_lcdvc
  ldr   r0, =(irq_vector_table + (4 * 1)) /* load the vector number */
  ldr   r3,=0x0002
  b     get_handler

check_lcdvc:
  tst   r3, #0x0004
  beq   check_t0
  ldr   r0, =(irq_vector_table + (4 * 2)) /* load the vector number */
  ldr   r3,=0x0004
  b     get_handler

check_t0:
  tst   r3, #0x0008
  beq   check_t1
  ldr   r0, =(irq_vector_table + (4 * 3)) /* load the vector number */
  ldr   r3,=0x0008
  b     get_handler

check_t1:
  tst   r3, #0x0010
  beq   check_t2
  ldr   r0, =(irq_vector_table + (4 * 4)) /* load the vector number */
  ldr   r3,=0x0010
  b     get_handler

check_t2:
  tst   r3, #0x0020
  beq   check_t3
  ldr   r0, =(irq_vector_table + (4 * 5)) /* load the vector number */
  ldr   r3,=0x0020
  b     get_handler

check_t3:
  tst   r3, #0x0040
  beq   check_ser
  ldr   r0, =(irq_vector_table + (4 * 6)) /* load the vector number */
  ldr   r3,=0x0040
  b     get_handler

check_ser:
  tst   r3, #0x0080
  beq   check_dma0
  ldr   r0, =(irq_vector_table + (4 * 7)) /* load the vector number */
  ldr   r3,=0x0080
  b     get_handler

check_dma0:
  tst   r3, #0x0100
  beq   check_dma1
  ldr   r0, =(irq_vector_table + (4 * 8)) /* load the vector number */
  ldr   r3,=0x0100
  b     get_handler

check_dma1:
  tst   r3, #0x0200
  beq   check_dma2
  ldr   r0, =(irq_vector_table + (4 * 9)) /* load the vector number */
  ldr   r3,=0x0200
  b     get_handler

check_dma2:
  tst   r3, #0x0400
  beq   check_dma3
  ldr   r0, =(irq_vector_table + (4 * 10)) /* load the vector number */
  ldr   r3,=0x0400
  b     get_handler

check_dma3:
  tst   r3, #0x0800
  beq   check_keypad
  ldr   r0, =(irq_vector_table + (4 * 11)) /* load the vector number */
  ldr   r3,=0x0800
  b     get_handler

check_keypad:
  tst   r3, #0x1000
  beq   check_gamepak
  ldr   r0, =(irq_vector_table + (4 * 12)) /* load the vector number */
  ldr   r3,=0x1000
  b     get_handler

check_gamepak:
  tst   r3, #0x2000
  beq   IRQ_NoInterrupt
  ldr   r0, =(irq_vector_table + (4 * 13)) /* load the vector number */
  ldr   r3,=0x2000
  b     get_handler

unknown_irq:
  ldr   r0, =(default_int_handler)          /* Unknown Interrupt?    */
  ldr   r3,=0x0000

get_handler:
  ldr   r0, [r0]             /* extract the IT handler */

  ldr   r2, =GBA_REG_IF_ADDR /* Clear IF               */
  strh  r3, [r2]

  /*
   * re-enable interrupts at processor level
   */
  mrs	r1, cpsr
  bic	r1, r1, #Int_Bits
  msr	cpsr, r1

  stmdb sp!,{lr}
  ldr   lr, =IRQ_return    /* prepare the return from handler  */
  mov   pc, r0             /* EXECUTE INT HANDLER */

IRQ_return:
  ldmia sp!,{lr}

  /*
   * disable interrupts_again
   */
  mrs	r0, cpsr
  orr	r0, r0, #Int_Bits
  msr	cpsr, r0

IRQ_NoInterrupt:
  /* return to the "main" interrupt handler */
  mov pc, lr

LABEL_END(bsp_interrupt_dispatch)
/* @endcond */

