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
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#ifndef LIBBSP_GENERIC_OR1K_IRQ_H
#define LIBBSP_GENERIC_OR1K_IRQ_H

#ifndef ASM

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#define BSP_INTERRUPT_VECTOR_MIN  0x100
#define BSP_INTERRUPT_VECTOR_MAX  0x1F00

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
