/**
 * @file
 *
 * @ingroup bsp_kit
 *
 * @brief Output character definitions for standard UARTs.
 */

/*
 * Copyright (c) 2010
 * embedded brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
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
#define CONSOLE_LSR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x14))
#define CONSOLE_SCR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x1C))
#define CONSOLE_ACR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x20))
#define CONSOLE_ICR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x24))
#define CONSOLE_FDR (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x28))
#define CONSOLE_TER (*(volatile uint32_t *) (BSP_CONSOLE_UART_BASE + 0x30))

#define CONSOLE_LSR_RDR 0x1
#define CONSOLE_LSR_THRE 0x20

#define BSP_CONSOLE_UART_INIT(dll) \
  do { \
    CONSOLE_LCR = 0x00; \
    CONSOLE_IER = 0x00; \
    CONSOLE_LCR = 0x80; \
    CONSOLE_DLL = (dll); \
    CONSOLE_DLM = 0x00; \
    CONSOLE_LCR = 0x03; \
    CONSOLE_FCR = 0x07; \
  } while (0)

#endif /* LIBBSP_SHARED_UART_OUTPUT_CHAR_H */
