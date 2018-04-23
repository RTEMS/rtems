/**
 * @file
 *
 * @ingroup bsp_interrupt
 *
 * @brief LPC176X interrupt support.
 */

/*
 * Copyright (c) 2008-2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#include <rtems/score/armv4.h>
#include <rtems/score/armv7m.h>

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/lpc176x.h>
#include <bsp/linker-symbols.h>

/**
 * @brief Checks if the current interrupt vector length is valid or not.
 *
 * @param  vector The current interrupt vector length.
 * @return  TRUE if valid.
 *          FALSE otherwise.
 */
static inline bool lpc176x_irq_is_valid( const rtems_vector_number vector )
{
  return vector <= BSP_INTERRUPT_VECTOR_MAX;
}

void lpc176x_irq_set_priority(
  const rtems_vector_number vector,
  unsigned                  priority
)
{
  if ( lpc176x_irq_is_valid( vector ) ) {
    if ( priority > LPC176X_IRQ_PRIORITY_VALUE_MAX ) {
      priority = LPC176X_IRQ_PRIORITY_VALUE_MAX;
    }

    /* else implies that the priority is unlocked. Also,
       there is nothing to do. */

    _ARMV7M_NVIC_Set_priority( (int) vector, (int) ( priority << 3u ) );
  }

  /* else implies that the rtems vector number is invalid. Also,
     there is nothing to do. */
}

unsigned lpc176x_irq_get_priority( const rtems_vector_number vector )
{
  unsigned priority;

  if ( lpc176x_irq_is_valid( vector ) ) {
    priority = (unsigned) ( _ARMV7M_NVIC_Get_priority( (int) vector ) >> 3u );
  } else {
    priority = LPC176X_IRQ_PRIORITY_VALUE_MIN - 1u;
  }

  return priority;
}
