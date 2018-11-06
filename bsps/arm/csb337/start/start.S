/*
 * Cogent CSB337 startup code
 *
 * Copyright (c) 2004 by Jay Monkman <jtm@lopingdog.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
*/

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

.text
.globl  _start
_start:
        /*
         * Since I don't plan to return to the bootloader,
         * I don't have to save the registers.
         */

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

        /* zero the bss */
        ldr     r1, =bsp_section_bss_end
        ldr     r0, =bsp_section_bss_begin

_bss_init:
        mov     r2, #0
        cmp     r0, r1
        strlot  r2, [r0], #4
        blo     _bss_init        /* loop while r0 < r1 */

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
        ldr    pc, handler_addr_reset
        ldr    pc, handler_addr_undef
        ldr    pc, handler_addr_swi
        ldr    pc, handler_addr_prefetch
        ldr    pc, handler_addr_abort
        nop
        ldr    pc, handler_addr_irq
        ldr    pc, handler_addr_fiq

handler_addr_reset:
        .word  bsp_reset

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
