/**
 * @file
 *
 * @ingroup beagle_clock
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

#ifndef LIBBSP_ARM_BEAGLE_BEAGLE_CLOCK_CONFIG_H
#define LIBBSP_ARM_BEAGLE_BEAGLE_CLOCK_CONFIG_H

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/beagle.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @defgroup beagle_clock Clock Support
 *
 * @ingroup beagle
 *
 * @brief Clock support.
 *
 * @{
 */

#define BEAGLE_CLOCK_INTERRUPT BEAGLE_IRQ_TIMER_0

#define BEAGLE_CLOCK_TIMER_BASE BEAGLE_BASE_TIMER_0

#define BEAGLE_CLOCK_REFERENCE BEAGLE_PERIPH_CLK

#define BEAGLE_CLOCK_MODULE_ENABLE()

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_BEAGLE_BEAGLE_CLOCK_CONFIG_H */
