/**
 * @file bsp.h
 *
 * @ingroup tms570
 *
 * @brief Global BSP definitions.
 */

/*
 * Copyright (c) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * Based on LPC24xx and LPC1768 BSP
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_BSP_H
#define LIBBSP_ARM_TMS570_BSP_H

#include <bspopts.h>

#define BSP_FEATURE_IRQ_EXTENSION

#ifndef ASM

#include <rtems.h>
#include <bsp/default-initial-extension.h>

#define BSP_OSCILATOR_CLOCK 8000000
#define BSP_PLL_OUT_CLOCK 160000000

#endif /* ASM */

#endif /* LIBBSP_ARM_TMS570_BSP_H */
