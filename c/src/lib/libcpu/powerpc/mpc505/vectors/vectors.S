/*  vectors.s	1.1 - 95/12/04
 *
 *  This file contains the assembly code for the PowerPC 505
 *  interrupt veneers for RTEMS.
 *
 *  Author:	Sergei Organov <osv@javad.ru>
 *
 *  COPYRIGHT (c) 1998 by JPS.
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *      for any purpose is hereby granted without fee, provided that
 *      the above copyright notice and this notice appears in all
 *      copies, and that the name of i-cubed limited not be used in
 *      advertising or publicity pertaining to distribution of the
 *      software without specific, written prior permission.
 *      i-cubed limited makes no representations about the suitability
 *      of this software for any purpose.
 *
 */

#include <rtems/asm.h>
#include <rtems/score/powerpc.h>
#include <rtems/score/ppc_offs.h>

	/* Vector offsets */
        .set    reset,                  0x0100 # PPC_IRQ_SYSTEM_RESET
        .set    machine_check,          0x0200 # PPC_IRQ_MCHECK
        .set    dsi,                    0x0300 # PPC_IRQ_PROTECT
        .set    isi,                    0x0400 # PPC_IRQ_ISI
	.set    external_interrupt,	0x0500 # PPC_IRQ_EXTERNAL
        .set    alignment,              0x0600 # PPC_IRQ_ALIGNMENT
	.set    program,		0x0700 # PPC_IRQ_PROGRAM
	.set    fp_unavailable,		0x0800 # PPC_IRQ_NOFP
        .set    decrementer,            0x0900 # PPC_IRQ_DECREMENTER
	.set    system_call,		0x0C00 # PPC_IRQ_SCALL
	.set    trace,                  0x0D00 # PPC_IRQ_TRACE
        .set    fp_assist,              0x0E00 # PPC_IRQ_FP_ASST
	.set    software_emulation,	0x1000 # PPC_IRQ_SOFTEMU
	.set	data_bp,        	0x1C00 # PPC_IRQ_DATA_BP
	.set	istruction_bp,  	0x1D00 # PPC_IRQ_INST_BP
	.set	m_extern_bp,    	0x1E00 # PPC_IRQ_MEXT_BP
	.set    nm_extern_bp,		0x1F00 # PPC_IRQ_NMEXT_BP

#define ABI_ADD 0
        .extern led_green
#define ISR_HANDLER(vector, irq) \
        .org	vector; \
	stwu	r1, -(ABI_ADD + IP_END)(r1); \
	stw	r0, IP_0(r1); \
	li      r0, irq; \
	b       PROC (_ISR_Handler);

        /* Go to the right section */
	.section .vect,"ax",@progbits
        .globl  __vect
__vect:
        ISR_HANDLER(reset,              PPC_IRQ_SYSTEM_RESET)
        ISR_HANDLER(machine_check,      PPC_IRQ_MCHECK)
        ISR_HANDLER(dsi,                PPC_IRQ_PROTECT)
        ISR_HANDLER(isi,                PPC_IRQ_ISI)
        ISR_HANDLER(external_interrupt, PPC_IRQ_EXTERNAL)
        ISR_HANDLER(alignment,          PPC_IRQ_ALIGNMENT)
        ISR_HANDLER(program,            PPC_IRQ_PROGRAM)
        ISR_HANDLER(fp_unavailable,     PPC_IRQ_NOFP)
        ISR_HANDLER(decrementer,        PPC_IRQ_DECREMENTER)
        ISR_HANDLER(system_call,        PPC_IRQ_SCALL)
        ISR_HANDLER(trace,              PPC_IRQ_TRACE)
        ISR_HANDLER(fp_assist,          PPC_IRQ_FP_ASST)
        ISR_HANDLER(software_emulation, PPC_IRQ_SOFTEMU)
	ISR_HANDLER(data_bp,		PPC_IRQ_DATA_BP)
	ISR_HANDLER(istruction_bp,  	PPC_IRQ_INST_BP)
	ISR_HANDLER(m_extern_bp,    	PPC_IRQ_MEXT_BP)
	ISR_HANDLER(nm_extern_bp,	PPC_IRQ_NMEXT_BP)
