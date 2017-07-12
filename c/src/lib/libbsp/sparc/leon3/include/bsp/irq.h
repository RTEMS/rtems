/**
 * @file
 * @ingroup sparc_leon3
 * @brief LEON3 generic shared IRQ setup
 *
 * Based on libbsp/shared/include/irq.h.
 */

/*
 * Copyright (c) 2012.
 * Aeroflex Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_LEON3_IRQ_CONFIG_H
#define LIBBSP_LEON3_IRQ_CONFIG_H

#include <leon.h>
#include <rtems/score/processormask.h>

#define BSP_INTERRUPT_VECTOR_MAX_STD 15 /* Standard IRQ controller */
#define BSP_INTERRUPT_VECTOR_MAX_EXT 31 /* Extended IRQ controller */

#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX BSP_INTERRUPT_VECTOR_MAX_EXT

/* The check is different depending on IRQ controller, runtime detected */
#define BSP_INTERRUPT_CUSTOM_VALID_VECTOR

/**
 * @brief Returns true if the interrupt vector with number @a vector is valid.
 */
static inline bool bsp_interrupt_is_valid_vector(rtems_vector_number vector)
{
  return (rtems_vector_number) BSP_INTERRUPT_VECTOR_MIN <= vector
    && ((vector <= (rtems_vector_number) BSP_INTERRUPT_VECTOR_MAX_STD &&
        LEON3_IrqCtrl_EIrq == 0) ||
       (vector <= (rtems_vector_number) BSP_INTERRUPT_VECTOR_MAX_EXT &&
        LEON3_IrqCtrl_EIrq != 0));
}

void bsp_interrupt_set_affinity(
  rtems_vector_number vector,
  const Processor_mask *affinity
);

void bsp_interrupt_get_affinity(
  rtems_vector_number vector,
  Processor_mask *affinity
);

#endif /* LIBBSP_LEON3_IRQ_CONFIG_H */
