/**
 * @file
 *
 * @ingroup generic_or1k_uart
 *
 * @brief UART support.
 */

/*
 * COPYRIGHT (c) 2014-2015 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

/**
 * @defgroup generic_or1k_uart UART Support
 *
 * @ingroup RTEMSBSPsOR1K
 *
 * @brief Universal Asynchronous Receiver/Transmitter (UART) Support
 */

#ifndef LIBBSP_GENERIC_OR1K_UART_H
#define LIBBSP_GENERIC_OR1K_UART_H

#include <libchip/serial.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define OR1K_UART_DEFAULT_BAUD  115200
#define OR1K_BSP_UART_IRQ       2
extern const console_fns generic_or1k_uart_fns;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_GENERIC_OR1K_UART_H */
