/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Get_by_name().
 */

/*
 *  COPYRIGHT (c) 1989-2010.
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

#include <string.h>

Objects_Control *_Objects_Get_by_name(
  const Objects_Information *information,
  const char                *name,
  size_t                    *name_length_p,
  Objects_Get_by_name_error *error
)
{
  size_t          name_length;
  size_t          max_name_length;
  Objects_Maximum maximum;
  Objects_Maximum index;

  _Assert( _Objects_Has_string_name( information ) );
  _Assert( _Objects_Allocator_is_owner() );

  if ( name == NULL ) {
    *error = OBJECTS_GET_BY_NAME_INVALID_NAME;
    return NULL;
  }

  name_length = strnlen( name, information->name_length + 1 );
  max_name_length = information->name_length;
  if ( name_length > max_name_length ) {
    *error = OBJECTS_GET_BY_NAME_NAME_TOO_LONG;
    return NULL;
  }

  if ( name_length_p != NULL ) {
    *name_length_p = name_length;
  }

  maximum = _Objects_Get_maximum_index( information );

  for ( index = 0; index < maximum; ++index ) {
    Objects_Control *the_object;

    the_object = information->local_table[ index ];

    if ( the_object == NULL )
      continue;

    if ( the_object->name.name_p == NULL )
      continue;

    if ( strncmp( name, the_object->name.name_p, max_name_length ) == 0 ) {
      return the_object;
    }
  }

  *error = OBJECTS_GET_BY_NAME_NO_OBJECT;
  return NULL;
}
