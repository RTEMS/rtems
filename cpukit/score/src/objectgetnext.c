/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Get_next().
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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

Objects_Control *_Objects_Get_next(
  Objects_Id                 id,
  const Objects_Information *information,
  Objects_Id                *next_id_p
)
{
    Objects_Control *the_object;
    Objects_Id       next_id;
    Objects_Maximum  maximum;

    if ( !information )
      return NULL;

    if ( !next_id_p )
      return NULL;

    if (_Objects_Get_index(id) == OBJECTS_ID_INITIAL_INDEX)
        next_id = _Objects_Get_minimum_id( information->maximum_id );
    else
        next_id = id;

    _Objects_Allocator_lock();
    maximum = _Objects_Get_maximum_index( information );

    do {
      /* walked off end of list? */
      if (_Objects_Get_index( next_id ) > maximum) {
        _Objects_Allocator_unlock();
        *next_id_p = OBJECTS_ID_FINAL;
        return NULL;
      }

      /* try to grab one */
      the_object = _Objects_Get_no_protection( next_id, information );

      next_id++;
    } while ( the_object == NULL );

    *next_id_p = next_id;
    return the_object;
}
