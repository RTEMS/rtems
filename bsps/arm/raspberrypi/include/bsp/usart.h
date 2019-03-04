/**
 * @file
 *
 * @ingroup raspberrypi_usart
 *
 * @brief USART support.
 */


/**
 * @defgroup raspberrypi_usart USART Support
 *
 * @ingroup RTEMSBSPsARMRaspberryPi
 *
 * @brief Universal Synchronous/Asynchronous Receiver/Transmitter (USART) Support
 */

/*
 * Copyright (c) 2013 Alan Cudmore.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE
 */

#ifndef LIBBSP_ARM_RASPBERRYPI_USART_H
#define LIBBSP_ARM_RASPBERRYPI_USART_H

#include <libchip/serial.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define USART0_DEFAULT_BAUD 115000

extern const console_fns bcm2835_usart_fns;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_RASPBERRYPI_USART_H */
