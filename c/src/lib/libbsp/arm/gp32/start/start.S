/*
 * GP32 startup code
 */

/*
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
	b		_start2

@---------------------------------------------------------------------------------
@ AXF addresses
@---------------------------------------------------------------------------------
	.word   bsp_section_text_begin
	.word   bsp_section_rodata_end
	.word   bsp_section_data_begin
	.word   bsp_section_bss_end
	.word   bsp_section_bss_begin
	.word   bsp_section_bss_end

@---------------------------------------------------------------------------------
@ GamePark magic sequence
@---------------------------------------------------------------------------------
	.word   0x44450011
	.word   0x44450011
	.word   0x01234567
	.word   0x12345678
	.word   0x23456789
	.word   0x34567890
	.word   0x45678901
	.word   0x56789012
	.word   0x23456789
	.word   0x34567890
	.word   0x45678901
	.word   0x56789012
	.word   0x23456789
	.word   0x34567890
	.word   0x45678901
	.word   0x56789012

@---------------------------------------------------------------------------------
_start2:
@---------------------------------------------------------------------------------

        /*
         * Since I don't plan to return to the bootloader,
         * I don't have to save the registers.
         *
         * I'll just set the CPSR for SVC mode, interrupts
         * off, and ARM instructions.
         */
        mov     r0, #(PSR_MODE_SVC | PSR_I | PSR_F)
        msr     cpsr, r0

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


	/* disable mmu, I and D caches*/
	nop
	nop
	mrc p15, 0, r0, c1, c0, 0
	bic r0, r0, #0x01
	bic r0, r0, #0x04
	bic r0, r0, #0x01000
	mcr p15, 0, r0, c1, c0, 0
	nop
	nop

	/* clean data cache */
	mov   r1,#0x00
Loop1:
	mov   r2,#0x00
Loop2:
	mov r3, r2, lsl#26
	orr r3, r3, r1, lsl#5
	mcr p15, 0, r3, c7, c14, 2
	add r2, r2, #0x01
	cmp r2, #64
	bne Loop2
	add r1, r1, #0x01
	cmp r1, #8
	bne Loop1


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
