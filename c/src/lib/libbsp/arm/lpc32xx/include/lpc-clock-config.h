/**
 * @file
 *
 * @ingroup lpc_clock
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2009
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

#ifndef LIBBSP_ARM_LPC32XX_LPC_CLOCK_CONFIG_H
#define LIBBSP_ARM_LPC32XX_LPC_CLOCK_CONFIG_H

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/lpc32xx.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup lpc_clock Clock Support
 *
 * @ingroup lpc
 *
 * @brief Clock support.
 *
 * @{
 */

#define LPC_CLOCK_INTERRUPT LPC32XX_IRQ_TIMER_0

#define LPC_CLOCK_TIMER_BASE LPC32XX_BASE_TIMER_0

#define LPC_CLOCK_REFERENCE LPC32XX_PERIPH_CLK

#define LPC_CLOCK_MODULE_ENABLE()

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC32XX_LPC_CLOCK_CONFIG_H */
