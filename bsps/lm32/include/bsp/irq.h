/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief BSP interrupt support for LM32.
 */

/*
 * Based on concepts of Pavel Pisa, Till Straumann and Eric Valette.
 *
 * Copyright (c) 2008, 2009, 2010 embedded brains GmbH. All rights reserved.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_LM32_IRQ_CONFIG_H
#define LIBBSP_LM32_IRQ_CONFIG_H

#include <stdint.h>

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

/**
 * @brief Maximum vector number.
 */
#define BSP_INTERRUPT_VECTOR_COUNT 32

/** @} */

#endif /* LIBBSP_LM32_IRQ_CONFIG_H */
