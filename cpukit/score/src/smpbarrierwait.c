/*
 * Copyright (c) 2013-2014 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/smpbarrier.h>

bool _SMP_barrier_Wait(
  SMP_barrier_Control *control,
  SMP_barrier_State *state,
  unsigned int count
)
{
  unsigned int sense = ~state->sense;
  unsigned int previous_value;
  bool performed_release;

  state->sense = sense;

  previous_value = _Atomic_Fetch_add_uint(
    &control->value,
    1U,
    ATOMIC_ORDER_RELAXED
  );

  if ( previous_value + 1U == count ) {
    _Atomic_Store_uint( &control->value, 0U, ATOMIC_ORDER_RELAXED );
    _Atomic_Store_uint( &control->sense, sense, ATOMIC_ORDER_RELEASE );
    performed_release = true;
  } else {
    while (
      _Atomic_Load_uint( &control->sense, ATOMIC_ORDER_ACQUIRE ) != sense
    ) {
      /* Wait */
    }

    performed_release = false;
  }

  return performed_release;
}
