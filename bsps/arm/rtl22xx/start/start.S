/*
 * Philips LPC22XX/LPC21xx Startup code
 *
 * Copyright (c) 2007 Ray Xu<rayx.cn@gmail.com>
 * Change from CSB337's code by Jay Monkman <jtm@lopingdog.com>
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
*/

#include <bsp/linker-symbols.h>

/* Some standard definitions...*/
.equ PSR_MODE_USR,       0x10
.equ PSR_MODE_FIQ,       0x11
.equ PSR_MODE_IRQ,       0x12
.equ PSR_MODE_SVC,       0x13
.equ PSR_MODE_ABT,       0x17
.equ PSR_MODE_UNDEF,     0x1B
.equ PSR_MODE_SYS,       0x1F

.equ PSR_I,              0x80
.equ PSR_F,              0x40
.equ PSR_T,              0x20

.text
.code	32
.globl  _start
_start:
        /*
         * Since I don't plan to return to the bootloader,
         * I don't have to save the registers.
         *
         * I'll just set the CPSR for SVC mode, interrupts
         * off, and ARM instructions.
         */

        /* --- Initialize stack pointer registers */
        /* Enter IRQ mode and set up the IRQ stack pointer */
        mov     r0, #(PSR_MODE_IRQ | PSR_I | PSR_F)     /* No interrupts */
        bic	r0, r0, #PSR_T
        msr     cpsr, r0
        ldr     r1, =bsp_stack_irq_size
        ldr     sp, =bsp_stack_irq_begin
        add     sp, sp, r1

        /* Enter FIQ mode and set up the FIQ stack pointer */
        mov     r0, #(PSR_MODE_FIQ | PSR_I | PSR_F)     /* No interrupts */
        bic	r0, r0, #PSR_T
        msr     cpsr, r0
        ldr     r1, =bsp_stack_fiq_size
        ldr     sp, =bsp_stack_fiq_begin
        add     sp, sp, r1

        /* Enter ABT mode and set up the ABT stack pointer */
        mov     r0, #(PSR_MODE_ABT | PSR_I | PSR_F)     /* No interrupts */
        bic	r0, r0, #PSR_T
        msr     cpsr, r0
        bic	r0, r0, #PSR_T
        ldr     r1, =bsp_stack_abt_size
        ldr     sp, =bsp_stack_abt_begin
        add     sp, sp, r1

        /* Set up the SVC stack pointer last and stay in SVC mode */
        mov     r0, #(PSR_MODE_SVC | PSR_I | PSR_F)     /* No interrupts */
        bic	r0, r0, #PSR_T
        msr     cpsr, r0
        ldr     r1, =bsp_stack_svc_size
        ldr     sp, =bsp_stack_svc_begin
        add     sp, sp, r1
        sub     sp, sp, #0x64

        /*
         * Initialize the exception vectors. This includes the
         * exceptions vectors (0x00000000-0x0000001c), and the
         * pointers to the exception handlers (0x00000020-0x0000003c).
         */
        mov     r0, #0
        adr     r1, vector_block
        ldmia   r1!, {r2-r9}
        stmia   r0!, {r2-r9}

        ldmia   r1!, {r2-r9}
        stmia   r0!, {r2-r9}


        /* zero the bss */
        ldr     r1, =bsp_section_bss_end
        ldr     r0, =bsp_section_bss_begin

_bss_init:
        mov     r2, #0
        cmp     r0, r1
        strlot  r2, [r0], #4
        blo     _bss_init        /* loop while r0 < r1 */


        /* Now we are prepared to start the BSP's C code */
        mov     r0, #0
#ifdef __thumb__
	ldr	r3, =boot_card
        bx      r3
#else
        bl      boot_card


        /*
         * Theoretically, we could return to what started us up,
         * but we'd have to have saved the registers and stacks.
         * Instead, we'll just reset.
         */
        bl      bsp_reset
#endif
	.code	32

        /* We shouldn't get here. If we do, hang */
_hang:  b       _hang


/*******************************************************
 standard exception vectors table
 *** Must be located at address 0
********************************************************/

vector_block:
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
