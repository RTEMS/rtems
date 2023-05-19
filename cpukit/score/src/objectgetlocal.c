/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Get().
 */

/*
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <rtems/score/objectimpl.h>

Objects_Control *_Objects_Get(
  Objects_Id                 id,
  ISR_lock_Context          *lock_context,
  const Objects_Information *information
)
{
  Objects_Id delta;
  Objects_Id maximum_id;
  Objects_Id end;

  maximum_id = information->maximum_id;
  delta = maximum_id - id;
  end = _Objects_Get_index( maximum_id );

  if ( RTEMS_PREDICT_TRUE( delta < end ) ) {
    ISR_Level        level;
    Objects_Control *the_object;

    _ISR_Local_disable( level );
    _ISR_lock_Context_set_level( lock_context, level );

    the_object =
      information->local_table[ end - OBJECTS_INDEX_MINIMUM - delta ];
    if ( RTEMS_PREDICT_TRUE( the_object != NULL ) ) {
      /* ISR disabled on behalf of caller */
      return the_object;
    }

    _ISR_Local_enable( level );
  }

  return NULL;
}
