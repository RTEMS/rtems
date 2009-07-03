/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief i386 interrupt support configuration.
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
 *
 * $Id$
 */

#ifndef LIBBSP_I386_SHARED_IRQ_CONFIG_H
#define LIBBSP_I386_SHARED_IRQ_CONFIG_H

#include <bsp/irq.h>

/**
 * @brief Minimum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MIN BSP_LOWEST_OFFSET

/**
 * @brief Maximum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MAX BSP_MAX_OFFSET

#endif /* LIBBSP_I386_SHARED_IRQ_CONFIG_H */
