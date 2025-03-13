/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup OR1K_IRQ
 *
 * @brief Interrupt definitions.
 */

/**
 * COPYRIGHT (c) 2014-2015 Hesham ALMatary <heshamelmatary@gmail.com>
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

#ifndef LIBBSP_GENERIC_OR1K_IRQ_H
#define LIBBSP_GENERIC_OR1K_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#include <bsp/irq-default.h>

/* Interrupt Identification Register */
#define OR1K_BSP_UART_REG_INT_ID_MSI    (0x00)
#define OR1K_BSP_UART_REG_INT_ID_NO_INT (0x01)
#define OR1K_BSP_UART_REG_INT_ID_THRI   (0x02)
#define OR1K_BSP_UART_REG_INT_ID_RDI    (0x04)
#define OR1K_BSP_UART_REG_INT_ID_ID     (0x06)
#define OR1K_BSP_UART_REG_INT_ID_RLSI   (0x06)
#define OR1K_BSP_UART_REG_INT_ID_TOI    (0x0c)

/* Interrupt Enable Register */
#define OR1K_BSP_UART_REG_INT_ENABLE_RDI  (0x01)
#define OR1K_BSP_UART_REG_INT_ENABLE_THRI (0x02)
#define OR1K_BSP_UART_REG_INT_ENABLE_RLSI (0x04)
#define OR1K_BSP_UART_REG_INT_ENABLE_MSI  (0x08)

#endif /* ASM */
#endif /* LIBBSP_GENERIC_OR1K_IRQ_H */
