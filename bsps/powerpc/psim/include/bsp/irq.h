/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * @brief Interrupt Handler Interfaces
 *
 * This include file describe the data structure and the functions implemented
 * by rtems to write interrupt handlers.
 *
 * This code is heavily inspired by the public specification of STREAM V2
 * that can be found at:
 *
 * - <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 */

/*
 * Copyright (c) 1999 Eric Valette <eric.valette@free.fr>
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

#ifndef LIBBSP_POWERPC_IRQ_H
#define LIBBSP_POWERPC_IRQ_H

#ifndef BSP_SHARED_HANDLER_SUPPORT
#define BSP_SHARED_HANDLER_SUPPORT      1
#endif

#ifndef ASM

#include <rtems/irq.h>


/*
 * Symblolic IRQ names and related definitions.
 */

/*
 * PCI IRQ handlers related definitions
 * CAUTION : BSP_PCI_IRQ_LOWEST_OFFSET should be equal to OPENPIC_VEC_SOURCE
 */
#define BSP_PCI_IRQ_NUMBER		(16)
#define BSP_PCI_IRQ_LOWEST_OFFSET	(0)
#define BSP_PCI_IRQ_MAX_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET + BSP_PCI_IRQ_NUMBER - 1)

#define BSP_PROCESSOR_IRQ_NUMBER	    (1)
#define BSP_PROCESSOR_IRQ_LOWEST_OFFSET	(BSP_PCI_IRQ_MAX_OFFSET)
#define BSP_PROCESSOR_IRQ_MAX_OFFSET	(BSP_PROCESSOR_IRQ_LOWEST_OFFSET+BSP_PROCESSOR_IRQ_NUMBER-1)


  /*
   * Summary
   */
#define BSP_IRQ_NUMBER		 	(BSP_PROCESSOR_IRQ_MAX_OFFSET + 1)
#define BSP_LOWEST_OFFSET		(BSP_PCI_IRQ_LOWEST_OFFSET)
#define BSP_MAX_OFFSET			(BSP_IRQ_NUMBER - 1)

  /*
   * Some Processor execption handled as rtems IRQ symbolic name definition
   */
#define BSP_DECREMENTER			(BSP_PROCESSOR_IRQ_LOWEST_OFFSET)

#include <bsp/irq_supp.h>

#define BSP_INTERRUPT_VECTOR_COUNT BSP_IRQ_NUMBER

#ifdef __cplusplus
extern "C" {
#endif

void BSP_rtems_irq_mng_init(unsigned cpuId);

#ifdef __cplusplus
}
#endif

#endif
#endif
