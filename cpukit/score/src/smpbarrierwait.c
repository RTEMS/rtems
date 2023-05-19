/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreSMPBarrier
 *
 * @brief This source file contains the implementation of
 *   _SMP_barrier_Wait().
 */

/*
 * Copyright (C) 2013, 2014 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
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
