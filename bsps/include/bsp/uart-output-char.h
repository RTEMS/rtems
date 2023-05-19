/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup bsp_kit
 *
 * @brief Output character definitions for standard UARTs.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH & Co. KG
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

#ifndef LIBBSP_SHARED_UART_OUTPUT_CHAR_H
#define LIBBSP_SHARED_UART_OUTPUT_CHAR_H

#include <stdint.h>

#include <bsp.h>

#define CONSOLE_RBR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x00))
#define CONSOLE_THR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x00))
#define CONSOLE_DLL (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x00))
#define CONSOLE_DLM (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x04))
#define CONSOLE_IER (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x04))
#define CONSOLE_IIR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x08))
#define CONSOLE_FCR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x08))
#define CONSOLE_LCR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x0C))
#define CONSOLE_MCR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x10))
#define CONSOLE_LSR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x14))
#define CONSOLE_SCR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x1C))
#define CONSOLE_ACR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x20))
#define CONSOLE_ICR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x24))
#define CONSOLE_FDR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x28))
#define CONSOLE_TER (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x30))

#define CONSOLE_LSR_RDR 0x1
#define CONSOLE_LSR_THRE 0x20
#define CONSOLE_LSR_TEMT 0x40

#define BSP_CONSOLE_UART_INIT(dll) \
  do { \
    CONSOLE_LCR = 0x00; \
    CONSOLE_IER = 0x00; \
    CONSOLE_LCR = 0x80; \
    CONSOLE_DLL = (dll & 0xFF); \
    CONSOLE_DLM = (dll >> 8); \
    CONSOLE_LCR = 0x03; \
    CONSOLE_FCR = 0x07; \
  } while (0)

#endif /* LIBBSP_SHARED_UART_OUTPUT_CHAR_H */
