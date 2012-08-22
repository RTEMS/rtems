/**
 * @file
 *
 * @ingroup beagle_clock
 *
 * @brief Clock driver configuration.
 */

/*
 * Copyright (c) 2012 Claas Ziemke. All rights reserved.
 *
 *  Claas Ziemke
 *  Kernerstrasse 11
 *  70182 Stuttgart
 *  Germany
 *  <claas.ziemke@gmx.net>
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
