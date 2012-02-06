/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief BSP interrupt support configuration template.
 */

/*
 * Based on concepts of Pavel Pisa, Till Straumann and Eric Valette.
 *
 * Copyright (c) 2008, 2009, 2010
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

#ifndef LIBBSP_SHARED_IRQ_CONFIG_H
#define LIBBSP_SHARED_IRQ_CONFIG_H

#include <stdint.h>

/**
 * @addtogroup bsp_interrupt
 *
 * @{
 */

/**
 * @brief Minimum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MIN 0

/**
 * @brief Maximum vector number.
 */
#define BSP_INTERRUPT_VECTOR_MAX 0

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
  #define BSP_INTERRUPT_HANDLER_TABLE_SIZE 0

  /**
   * @brief Integer type capable to index the complete handler table.
   */
  typedef uint8_t bsp_interrupt_handler_index_type;
#endif

/**
 * @brief Enable custom vector checking
 *
 * If defined the BSP must implement the custom bsp_interrupt_is_valid_vector()
 * vector validator check routine.
 */
#undef BSP_INTERRUPT_CUSTOM_VALID_VECTOR

/** @} */

#endif /* LIBBSP_SHARED_IRQ_CONFIG_H */
