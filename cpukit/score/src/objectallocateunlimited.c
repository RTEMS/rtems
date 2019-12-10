/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 */

/*
 * SPDX-License-Identifier: BSD-2-Clause
 *
 * Copyright (C) 1989, 2007 On-Line Applications Research Corporation (OAR)
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

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectdata.h>
#include <rtems/score/objectimpl.h>

Objects_Control *_Objects_Allocate_unlimited( Objects_Information *information )
{
  Objects_Control *the_object;

  _Assert( _Objects_Is_auto_extend( information ) );

  /*
   *  OK.  The manager should be initialized and configured to have objects.
   *  With any luck, it is safe to attempt to allocate an object.
   */
  the_object = _Objects_Get_inactive( information );

  /*
   *  If the list is empty then we are out of objects and need to
   *  extend information base.
   */

  if ( the_object == NULL ) {
    _Objects_Extend_information( information );
    the_object = _Objects_Get_inactive( information );
  }

  if ( the_object != NULL ) {
    Objects_Maximum objects_per_block;
    Objects_Maximum block;

    objects_per_block = information->objects_per_block;
    block = _Objects_Get_index( the_object->id ) - OBJECTS_INDEX_MINIMUM;

    if ( block > objects_per_block ) {
      block /= objects_per_block;

      information->inactive_per_block[ block ]--;
      information->inactive--;
    }
  }

  return the_object;
}
