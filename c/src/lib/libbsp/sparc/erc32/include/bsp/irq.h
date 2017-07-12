/**
 * @file
 * @ingroup sparc_erc32
 * @brief ERC32 generic shared IRQ setup
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

#ifndef LIBBSP_ERC32_IRQ_CONFIG_H
#define LIBBSP_ERC32_IRQ_CONFIG_H

#include <rtems/score/processormask.h>

#define BSP_INTERRUPT_VECTOR_MAX_STD 15 /* Standard IRQ controller */
#define BSP_INTERRUPT_VECTOR_MIN 0
#define BSP_INTERRUPT_VECTOR_MAX BSP_INTERRUPT_VECTOR_MAX_STD

/* No extra check is needed */
#undef BSP_INTERRUPT_CUSTOM_VALID_VECTOR

RTEMS_INLINE_ROUTINE void bsp_interrupt_set_affinity(
  rtems_vector_number   vector,
  const Processor_mask *affinity
)
{
  (void) vector;
  (void) affinity;
}

RTEMS_INLINE_ROUTINE void bsp_interrupt_get_affinity(
  rtems_vector_number  vector,
  Processor_mask      *affinity
)
{
  (void) vector;
  _Processor_mask_From_index( affinity, 0 );
}

#endif /* LIBBSP_ERC32_IRQ_CONFIG_H */
