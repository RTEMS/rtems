/**
 * @file
 *
 * @ingroup lpc24xx
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

#ifndef LIBBSP_ARM_LPC24XX_LPC_CLOCK_CONFIG_H
#define LIBBSP_ARM_LPC24XX_LPC_CLOCK_CONFIG_H

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/lpc24xx.h>
#include <bsp/io.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LPC_CLOCK_INTERRUPT LPC24XX_IRQ_TIMER_0

#define LPC_CLOCK_TIMER_BASE TMR0_BASE_ADDR

#define LPC_CLOCK_REFERENCE LPC24XX_PCLK

#define LPC_CLOCK_MODULE_ENABLE() \
  lpc24xx_module_enable(LPC24XX_MODULE_TIMER_0, LPC24XX_MODULE_PCLK_DEFAULT)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC24XX_LPC_CLOCK_CONFIG_H */
