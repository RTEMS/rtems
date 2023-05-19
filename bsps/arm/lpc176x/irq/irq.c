/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief LPC176X interrupt support.
 */

/*
 * Copyright (C) 2008, 2012 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  return vector < BSP_INTERRUPT_VECTOR_COUNT;
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
