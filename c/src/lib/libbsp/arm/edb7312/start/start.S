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

#include <bsp/linker-symbols.h>

/* Some standard definitions...*/

.equ Mode_USR,        	     0x10
.equ Mode_FIQ,        	     0x11
.equ Mode_IRQ,        	     0x12
.equ Mode_SVC,        	     0x13
.equ Mode_ABT,        	     0x17
.equ Mode_ABORT,             0x17
.equ Mode_UNDEF,      	     0x1B
.equ Mode_SYS,        	     0x1F /*only available on ARM Arch. v4*/

.equ I_Bit,           	     0x80
.equ F_Bit,           	     0x40

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
	/* store the sp */
	mov	r12, sp
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

/* --- Initialise stack pointer registers */

/* Enter IRQ mode and set up the IRQ stack pointer */
    MOV     r0, #Mode_IRQ | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    ldr	    r1, =bsp_stack_irq_size
    LDR     sp, =bsp_stack_irq_begin
    add	    sp, sp, r1
    sub     sp, sp, #0x64

/* Enter FIQ mode and set up the FIQ stack pointer */
    MOV     r0, #Mode_FIQ | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    ldr	    r1, =bsp_stack_fiq_size
    LDR     sp, =bsp_stack_fiq_begin
    add	    sp, sp, r1
    sub     sp, sp, #0x64

/* Enter ABT mode and set up the ABT stack pointer */
    MOV     r0, #Mode_ABT | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    ldr	    r1, =bsp_stack_abt_size
    LDR     sp, =bsp_stack_abt_begin
    add	    sp, sp, r1
    sub     sp, sp, #0x64

/* Set up the SVC stack pointer last and stay in SVC mode */
    MOV     r0, #Mode_SVC | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    ldr	    r1, =bsp_stack_svc_size
    LDR     sp, =bsp_stack_svc_begin
    add	    sp, sp, r1
    sub     sp, sp, #0x64

	/* save the original registers */
	stmdb	sp!, {r4-r12, lr}

/* --- Now we enter the C code */

	mov     r0, #0
	bl	boot_card

	ldmia	sp!, {r4-r12, lr}
	mov	sp, r12
	mov	pc, lr
