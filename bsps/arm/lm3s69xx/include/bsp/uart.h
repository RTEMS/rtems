/**
 * @file
 *
 * @ingroup lm3s69xx_uart
 *
 * brief UART support.
 */

/*
 * Copyright (c) 2011 Sebastian Huber.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LM3S69XX_UART_H
#define LIBBSP_ARM_LM3S69XX_UART_H

#include <libchip/serial.h>

/**
 * defgroup lm3s69xx_uart UART Support
 *
 * @ingroup RTEMSBSPsARMLM3S69XX
 *
 * @brief UART support.
 */

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

extern const console_fns lm3s69xx_uart_fns;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LM3S69XX_UART_H */
