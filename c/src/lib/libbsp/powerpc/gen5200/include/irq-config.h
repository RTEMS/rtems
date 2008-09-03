/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief BSP interrupt support configuration.
 */

/*
 * Copyright (c) 2008
 * Embedded Brains GmbH
 * Obere Lagerstr. 30
 * D-82178 Puchheim
 * Germany
 * rtems@embedded-brains.de
 *
 * The license and distribution terms for this file may be found in the file
 * LICENSE in this distribution or at http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_GEN5200_IRQ_CONFIG_H
#define LIBBSP_POWERPC_GEN5200_IRQ_CONFIG_H

#include <bsp/irq.h>

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

/**
 * @brief Minimum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MIN BSP_LOWEST_OFFSET

/**
 * @brief Maximum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MAX BSP_MAX_OFFSET

/** @} */

#endif /* LIBBSP_POWERPC_GEN5200_IRQ_CONFIG_H */
