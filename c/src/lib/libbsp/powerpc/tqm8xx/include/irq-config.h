/*===============================================================*\
| Project: RTEMS TQM8xx BSP                                       |
+-----------------------------------------------------------------+
| This file has been adapted to MPC8xx by                         |
|    Thomas Doerfler <Thomas.Doerfler@embedded-brains.de>         |
|                    Copyright (c) 2008                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
|                                                                 |
| See the other copyright notice below for the original parts.    |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the console driver                           |
\*===============================================================*/
/* derived from: generic MPC83xx BSP */
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

#ifndef LIBBSP_POWERPC_TQM8XX_IRQ_CONFIG_H
#define LIBBSP_POWERPC_TQM8XX_IRQ_CONFIG_H

#include <stdint.h>

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

/**
 * @brief Enables the index table.
 *
 * If you enable the index table, you have to define a size for the handler
 * table (@ref BSP_INTERRUPT_HANDLER_TABLE_SIZE) and must provide an integer
 * type capable to index the complete handler table (@ref
 * bsp_interrupt_handler_index_type).
 */
#undef BSP_INTERRUPT_USE_INDEX_TABLE

/**
 * @brief Disables usage of the heap.
 *
 * If you define this, you have to define @ref BSP_INTERRUPT_USE_INDEX_TABLE as
 * well.
 */
#undef BSP_INTERRUPT_NO_HEAP_USAGE

#ifdef BSP_INTERRUPT_USE_INDEX_TABLE

/**
 * @brief Size of the handler table.
 */
#define BSP_INTERRUPT_HANDLER_TABLE_SIZE 63

/**
 * @brief Integer type capable to index the complete handler table.
 */
typedef uint8_t bsp_interrupt_handler_index_type;

#endif /* BSP_INTERRUPT_USE_INDEX_TABLE */

/** @} */

#endif /* LIBBSP_POWERPC_TQM8XX_IRQ_CONFIG_H */
