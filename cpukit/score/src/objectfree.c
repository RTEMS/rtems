/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Free_unlimited().
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
#include <rtems/score/chainimpl.h>

void _Objects_Free_unlimited(
  Objects_Information *information,
  Objects_Control     *the_object
)
{
  _Chain_Append_unprotected( &information->Inactive, &the_object->Node );

  if ( _Objects_Is_auto_extend( information ) ) {
    Objects_Maximum objects_per_block;
    Objects_Maximum index;

    objects_per_block = information->objects_per_block;
    index = _Objects_Get_index( the_object->id ) - OBJECTS_INDEX_MINIMUM;

    if ( _Objects_Is_in_allocated_block( index, objects_per_block ) ) {
      Objects_Maximum block;
      Objects_Maximum inactive;

      block = index / objects_per_block;

      ++information->inactive_per_block[ block ];

      inactive = information->inactive;
      ++inactive;
      information->inactive = inactive;

      /*
       *  Check if the threshold level has been met of
       *  1.5 x objects_per_block are free.
       */

      if ( inactive > ( objects_per_block + ( objects_per_block >> 1 ) ) ) {
        _Objects_Shrink_information( information );
      }
    }
  }
}
