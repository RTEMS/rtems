/*
 * Cogent CSB337 startup code
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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
.globl  _start
_start:
        /*
         * Since I don't plan to return to the bootloader,
         * I don't have to save the registers.
         *
         * I'll just set the CPSR for SVC mode, interrupts
         * off, and ARM instructions.
         */
        mov     r0, #(PSR_MODE_SVC | PSR_I | PSR_F)
        msr     cpsr, r0

        /* zero the bss */
        ldr     r1, =bsp_section_bss_end
        ldr     r0, =bsp_section_bss_begin

_bss_init:
        mov     r2, #0
        cmp     r0, r1
        strlot  r2, [r0], #4
        blo     _bss_init        /* loop while r0 < r1 */


        /* --- Initialize stack pointer registers */
        /* Enter IRQ mode and set up the IRQ stack pointer */
        mov     r0, #(PSR_MODE_IRQ | PSR_I | PSR_F)     /* No interrupts */
        msr     cpsr, r0
        ldr     r1, =bsp_stack_irq_size
        ldr     sp, =bsp_stack_irq_begin
        add     sp, sp, r1

        /* Enter FIQ mode and set up the FIQ stack pointer */
        mov     r0, #(PSR_MODE_FIQ | PSR_I | PSR_F)     /* No interrupts */
        msr     cpsr, r0
        ldr     r1, =bsp_stack_fiq_size
        ldr     sp, =bsp_stack_fiq_begin
        add     sp, sp, r1

        /* Enter ABT mode and set up the ABT stack pointer */
        mov     r0, #(PSR_MODE_ABT | PSR_I | PSR_F)     /* No interrupts */
        msr     cpsr, r0
        ldr     r1, =bsp_stack_abt_size
        ldr     sp, =bsp_stack_abt_begin
        add     sp, sp, r1

        /* Set up the SVC stack pointer last and stay in SVC mode */
        mov     r0, #(PSR_MODE_SVC | PSR_I | PSR_F)     /* No interrupts */
        msr     cpsr, r0
        ldr     r1, =bsp_stack_svc_size
        ldr     sp, =bsp_stack_svc_begin
        add     sp, sp, r1
        sub     sp, sp, #0x64

        /*
         * Initialize the MMU. After we return, the MMU is enabled,
         * and memory may be remapped. I hope we don't remap this
         * memory away.
         */
        ldr     r0, =mem_map
        bl      mmu_init

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

        /* Now we are prepared to start the BSP's C code */
        mov     r0, #0
        bl      boot_card

        /*
         * Theoretically, we could return to what started us up,
         * but we'd have to have saved the registers and stacks.
         * Instead, we'll just reset.
         */
        bl      bsp_reset

        /* We shouldn't get here. If we do, hang */
_hang:  b       _hang


/*
 * This is the exception vector table and the pointers to
 * the functions that handle the exceptions. It's a total
 * of 16 words (64 bytes)
 */
vector_block:
        ldr     pc, Reset_Handler
        ldr     pc, Undefined_Handler
        ldr     pc, SWI_Handler
        ldr     pc, Prefetch_Handler
        ldr     pc, Abort_Handler
        nop
        ldr     pc, IRQ_Handler
        ldr     pc, FIQ_Handler

Reset_Handler:          b       bsp_reset
Undefined_Handler:      b       Undefined_Handler
SWI_Handler:            b       SWI_Handler
Prefetch_Handler:       b       Prefetch_Handler
Abort_Handler:          b       Abort_Handler
                        nop
IRQ_Handler:            b       IRQ_Handler
FIQ_Handler:            b       FIQ_Handler

.globl Reset_Handler
.globl Undefined_Handler
.globl SWI_Handler
.globl Prefetch_Handler
.globl Abort_Handler
.globl IRQ_Handler
.globl FIQ_Handler
