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

#ifndef ASM

#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#define BSP_POWERPC_IRQ_GENERIC_SUPPORT 1

/* The external interrupt sources of the OpenPIC */
#define BSP_OPENPIC_SOURCE_NUMBER 16
#define BSP_OPENPIC_SOURCE_LOWEST_OFFSET 0

/* The decrementer exception */
#define BSP_DECREMENTER 16

/* The interprocessor interrupts of the OpenPIC */
#define BSP_OPENPIC_IPI_NUMBER 4
#define BSP_OPENPIC_IPI_LOWEST_OFFSET 17

#define BSP_INTERRUPT_VECTOR_COUNT \
  (BSP_OPENPIC_IPI_LOWEST_OFFSET + BSP_OPENPIC_IPI_NUMBER)

#ifdef __cplusplus
extern "C" {
#endif

void bsp_interrupt_dispatch(uintptr_t exception_number);

#ifdef __cplusplus
}
#endif

#endif
#endif
