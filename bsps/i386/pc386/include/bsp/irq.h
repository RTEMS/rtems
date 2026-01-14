/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @ingroup i386_irq
 * @brief Interrupt handlers
 *
 * This include file describe the data structure and the functions implemented
 * by rtems to write interrupt handlers.
 *
 * This code is heavily inspired by the public specification of STREAM V2
 * that can be found at:
 *
 *  - <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 */

/*
 * Copyright (c) 1998 Eric Valette <eric.valette@free.fr>
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
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @defgroup i386_irq Interrupt handlers
 * @ingroup RTEMSBSPsI386
 * @brief Data structure and the functions to write interrupt handlers
 * @{
 */

#ifndef _IRQ_H_
#define _IRQ_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @brief
 * Include some preprocessor value also used by assember code
 */

#include <bsp/irq_asm.h>
#include <rtems.h>
#ifndef BSP_SHARED_HANDLER_SUPPORT
#define BSP_SHARED_HANDLER_SUPPORT      1
#endif
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

/*-------------------------------------------------------------------------+
| Constants
+--------------------------------------------------------------------------*/

/** @brief Base vector for our IRQ handlers. */
#define BSP_IRQ_VECTOR_BASE		BSP_ASM_IRQ_VECTOR_BASE
#define BSP_IRQ_LINES_NUMBER    	16
#define BSP_IRQ_MAX_ON_i8259A		(BSP_IRQ_LINES_NUMBER - 1)

/*
 * Define the number of valid vectors. This is different to the number of IRQ
 * signals supported. Use this value to allocation vector data or range check.
 */
#define BSP_IRQ_VECTOR_NUMBER        17
#define BSP_IRQ_VECTOR_LOWEST_OFFSET 0
#define BSP_IRQ_VECTOR_MAX_OFFSET    (BSP_IRQ_VECTOR_NUMBER - 1)

/** @brief
 * Interrupt offset in comparison to BSP_ASM_IRQ_VECTOR_BASE
 * NB : 1) Interrupt vector number in IDT = offset + BSP_ASM_IRQ_VECTOR_BASE
 * 	    2) The same name should be defined on all architecture
 *	       so that handler connection can be unchanged.
 */
#define BSP_PERIODIC_TIMER      0 /* fixed on all builds of PC */
#define BSP_KEYBOARD          	1 /* fixed on all builds of PC */
#define BSP_UART_COM2_IRQ	3 /* fixed for ISA bus */
#define BSP_UART_COM1_IRQ	4 /* fixed for ISA bus */
#define BSP_UART_COM3_IRQ	5
#define BSP_UART_COM4_IRQ	6
#define BSP_RT_TIMER1	      	8
#define BSP_RT_TIMER3		10
#define BSP_SMP_IPI   		16 /* not part of the ATPIC */

#define BSP_INTERRUPT_VECTOR_COUNT BSP_IRQ_VECTOR_NUMBER

/** @brief
 * Type definition for RTEMS managed interrupts
 */
typedef unsigned short rtems_i8259_masks;

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _IRQ_H_ */
