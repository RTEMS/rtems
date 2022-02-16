/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Set_name().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
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
#include <rtems/score/wkspace.h>

#include <string.h>

Status_Control _Objects_Set_name(
  const Objects_Information *information,
  Objects_Control           *the_object,
  const char                *name
)
{
  if ( _Objects_Has_string_name( information ) ) {
    size_t  length;
    char   *dup;

    length = strnlen( name, information->name_length );
    dup = _Workspace_String_duplicate( name, length );
    if ( dup == NULL ) {
      return STATUS_NO_MEMORY;
    }

    _Workspace_Free( RTEMS_DECONST( char *, the_object->name.name_p ) );
    the_object->name.name_p = dup;
  } else {
    char c[ 4 ];
    size_t i;

    memset( c, ' ', sizeof( c ) );

    for ( i = 0; i < 4; ++i ) {
      if ( name[ i ] == '\0') {
        break;
      }

      c[ i ] = name[ i ];
    }

    the_object->name.name_u32 =
      _Objects_Build_name( c[ 0 ], c[ 1 ], c[ 2 ], c[ 3 ] );
  }

  return STATUS_SUCCESSFUL;
}
