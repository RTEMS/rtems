/*
 * Cirrus EP7312 Startup code
 *
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
*/

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

.section ".bsp_start_text", "ax"
.arm

/*******************************************************
 standard exception vectors table
 *** Must be located at address 0
********************************************************/

Vector_Init_Block:
        ldr    pc, handler_addr_reset
        ldr    pc, handler_addr_undef
        ldr    pc, handler_addr_swi
        ldr    pc, handler_addr_prefetch
        ldr    pc, handler_addr_abort
        nop
        ldr    pc, handler_addr_irq
        ldr    pc, handler_addr_fiq

handler_addr_reset:
        .word  _start

handler_addr_undef:
        .word  _ARMV4_Exception_undef_default

handler_addr_swi:
        .word  _ARMV4_Exception_swi_default

handler_addr_prefetch:
        .word  _ARMV4_Exception_pref_abort_default

handler_addr_abort:
        .word  _ARMV4_Exception_data_abort_default

handler_addr_reserved:
        .word  _ARMV4_Exception_reserved_default

handler_addr_irq:
        .word  _ARMV4_Exception_interrupt

handler_addr_fiq:
        .word  _ARMV4_Exception_fiq_default

	.globl	_start
_start:
        /* Set end of interrupt stack area */
        ldr     r7, =_ISR_Stack_area_end

        /* Enter FIQ mode and set up the FIQ stack pointer */
        mov     r0, #(ARM_PSR_M_FIQ | ARM_PSR_I | ARM_PSR_F)
        msr     cpsr, r0
        ldr     r1, =bsp_stack_fiq_size
        mov     sp, r7
        sub     r7, r7, r1

        /* Enter ABT mode and set up the ABT stack pointer */
        mov     r0, #(ARM_PSR_M_ABT | ARM_PSR_I | ARM_PSR_F)
        msr     cpsr, r0
        ldr     r1, =bsp_stack_abt_size
        mov     sp, r7
        sub     r7, r7, r1

        /* Enter UND mode and set up the UND stack pointer */
        mov     r0, #(ARM_PSR_M_UND | ARM_PSR_I | ARM_PSR_F)
        msr     cpsr, r0
        ldr     r1, =bsp_stack_und_size
        mov     sp, r7
        sub     r7, r7, r1

        /* Enter IRQ mode and set up the IRQ stack pointer */
        mov     r0, #(ARM_PSR_M_IRQ | ARM_PSR_I | ARM_PSR_F)
        msr     cpsr, r0
        mov     sp, r7

        /*
         * Enter SVC mode and set up the SVC stack pointer, reuse IRQ stack
         * (interrupts are disabled).
         */
        mov     r0, #(ARM_PSR_M_SVC | ARM_PSR_I | ARM_PSR_F)
        msr     cpsr, r0
        mov     sp, r7

        /* Stay in SVC mode */
/*
 * Here is the code to initialize the low-level BSP environment
 * (Chip Select, PLL, ....?)
 */

/* zero the bss */
        LDR     r1, =bsp_section_bss_end   /* get end of ZI region */
        LDR     r0, =bsp_section_bss_begin /* load base address of ZI region */

zi_init:
        MOV     r2, #0
        CMP     r0, r1		       /* loop whilst r0 < r1 */
        STRLOT   r2, [r0], #4
        BLO     zi_init

/* --- Now we enter the C code */

	mov     r0, #0
	bl	boot_card
