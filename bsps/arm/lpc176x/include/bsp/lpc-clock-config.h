/**
 * @file
 *
 * @ingroup RTEMSBSPsARMLPC176X
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
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_LPC176X_LPC_CLOCK_CONFIG_H
#define LIBBSP_ARM_LPC176X_LPC_CLOCK_CONFIG_H

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/lpc176x.h>
#include <bsp/io.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LPC_CLOCK_INTERRUPT LPC176X_IRQ_TIMER_0
#define LPC_CLOCK_TIMER_BASE TMR0_BASE_ADDR
#define LPC_CLOCK_TIMECOUNTER_BASE TMR1_BASE_ADDR
#define LPC_CLOCK_REFERENCE LPC176X_PCLK
#define LPC_CLOCK_MODULE_ENABLE() \
  lpc176x_module_enable( LPC176X_MODULE_TIMER_0, LPC176X_MODULE_PCLK_DEFAULT )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_LPC176X_LPC_CLOCK_CONFIG_H */
