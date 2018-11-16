/*
 * Copyright (c) 2015 University of York.
 * Hesham ALMatary <hmka501@york.ac.uk>
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
#include <bsp/linker-symbols.h>
#include <rtems/asm.h>

EXTERN(bsp_section_bss_begin)
EXTERN(bsp_section_bss_end)
EXTERN(_ISR_Handler)
EXTERN(bsp_start_vector_table_end)
EXTERN(bsp_start_vector_table_size)
EXTERN(bsp_vector_table_size)
EXTERN(_ISR_Stack_area_end)

PUBLIC(_EPIPHANY_Exception_default)
PUBLIC(bsp_start_vector_table_begin)
PUBLIC(_start)

.section .vector, "wax"
TYPE_FUNC(_start)
SYM(_start):
  .balign 4 ;
    b .normal_start

  .balign 4 ; 0x4
    b .sw_exception

  .balign 4 ; 0x8
    b .normal_start

  .balign 4 ; 0xc
    b .clock_isr

  .balign 4 ; 0x10
    b .timer1_isr

  .balign 4 ; 0x14
    b _EPIPHANY_Exception_default

  .balign 4 ; 0x18
    b _EPIPHANY_Exception_default

  .balign 4 ; 0x1c
    b _EPIPHANY_Exception_default

  .balign 4 ; 0x20
    b _EPIPHANY_Exception_default

  .balign 4 ; 0x24
    b _EPIPHANY_Exception_default

bsp_start_vector_table_begin:
  .word .normal_start /* Reset */
  .word _EPIPHANY_Exception_default /* SW exception */
  .word _EPIPHANY_Exception_default /* Data Page Fault */
  .word _EPIPHANY_Exception_default /* Timer 0 */
  .word _EPIPHANY_Exception_default /* Timer 1 */
  .word _EPIPHANY_Exception_default /* Message int */
  .word _EPIPHANY_Exception_default /* DMA0 int */
  .word _EPIPHANY_Exception_default /* DMA1 int */
  .word _EPIPHANY_Exception_default /* WAND */
  .word _EPIPHANY_Exception_default /* User interrupt */

_bsp_start_vector_table_end:

.size  _start, .-_start

.section .start,"ax"
.align  4
.type   _external_start, %function
.normal_start:
  /* Initialize the stack and frame pointers */
  mov  sp, %low(_ISR_Stack_area_end)
  movt sp, %high(_ISR_Stack_area_end)
  mov  fp, sp

cpu0:
  /* Zero .bss section */
  mov  r0, %low(bsp_section_bss_begin)
  movt r0, %high(bsp_section_bss_begin)
  mov  r1, sp
  mov  r2,#0
  mov  r3,#0

_bss_clear_loop:
  strd  r2, [r0], +#1
  sub   r5, r1, r0
  bne   _bss_clear_loop

  /* Clear the reset interrupt flag */
  mov   r0, %low(_jump_to_c)
  movt  r0, %high(_jump_to_c)
  movts iret, r0
  rti

_jump_to_c:
  /* Jump to bootcard */
  mov  r3, %low(boot_card)
  movt r3, %high(boot_card)
  jalr r3

 /* Should never reach here */
 idle

.size  .normal_start, .-.normal_start

.balign 4
.type   .sw_exception, %function
.sw_exception:
  idle

.balign 4
.type   .clock_isr, %function
.clock_isr:
  /*
   * r62 and r63 are saved here, and restored from _ISR_Handler, they
   * and hold vector number and _ISR_Handler address repsectively.
   */
  add  sp, sp, #-8
  str  r62, [sp, #0]
  str  r63, [sp, #4]
  mov  r62, 3
  mov  r63, %low(_ISR_Handler)
  movt r63, %high(_ISR_Handler)
  jr   r6

.balign 4
.type   .timer1_isr, %function
.timer1_isr:
  /*
   * r62 and r63 are saved here, and restored from _ISR_Handler, they
   * and hold vector number and _ISR_Handler address repsectively.
   */
  add  sp, sp, #-8
  str  r62, [sp, 0]
  str  r63, [sp, 4]
  mov  r62, 4
  mov  r63, %low(_ISR_Handler)
  movt r63, %high(_ISR_Handler)
  jr   r63

.balign 4
TYPE_FUNC(_EPIPHANY_Exception_default)
SYM(_EPIPHANY_Exception_default):
  idle
