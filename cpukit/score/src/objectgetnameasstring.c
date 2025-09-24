/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief This source file contains the implementation of
 *   _Objects_Name_to_string() and _Objects_Get_name_as_string().
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <rtems/score/threadimpl.h>

/*
 * Do not use isprint() from <ctypes.h> since this depends on the heavy weight
 * C locale support of Newlib.
 */
static bool _Objects_Name_char_is_printable( char c )
{
  unsigned char uc;

  uc = (unsigned char) c;
  return uc >= ' ' && uc <= '~';
}

size_t _Objects_Name_to_string(
  Objects_Name name,
  bool         is_string,
  char        *buffer,
  size_t       buffer_size
)
{
  char        lname[ 5 ];
  const char *s;
  char       *d;
  size_t      i;

  if ( is_string ) {
    s = name.name_p;
  } else {
    lname[ 0 ] = ( name.name_u32 >> 24 ) & 0xff;
    lname[ 1 ] = ( name.name_u32 >> 16 ) & 0xff;
    lname[ 2 ] = ( name.name_u32 >> 8 ) & 0xff;
    lname[ 3 ] = ( name.name_u32 >> 0 ) & 0xff;
    lname[ 4 ] = '\0';
    s = lname;
  }

  d = buffer;
  i = 1;

  if ( s != NULL ) {
    while ( *s != '\0' ) {
      if ( i < buffer_size ) {
        *d = _Objects_Name_char_is_printable( *s ) ? *s : '*';
        ++d;
      }

      ++s;
      ++i;
    }
  }

  if ( buffer_size > 0 ) {
    *d = '\0';
  }

  return i - 1;
}

/*
 *  This method objects the name of an object and returns its name
 *  in the form of a C string.  It attempts to be careful about
 *  overflowing the user's string and about returning unprintable characters.
 */

char *_Objects_Get_name_as_string( Objects_Id id, size_t length, char *name )
{
  const Objects_Information *information;
  const Objects_Control     *the_object;
  ISR_lock_Context           lock_context;
  Objects_Id                 tmpId;

  if ( length == 0 ) {
    return NULL;
  }

  if ( name == NULL ) {
    return NULL;
  }

  tmpId = ( id == OBJECTS_ID_OF_SELF ) ? _Thread_Get_executing()->Object.id
                                       : id;

  information = _Objects_Get_information_id( tmpId );
  if ( !information ) {
    return NULL;
  }

  the_object = _Objects_Get( tmpId, &lock_context, information );
  if ( the_object == NULL ) {
    return NULL;
  }

  _Objects_Name_to_string(
    the_object->name,
    _Objects_Has_string_name( information ),
    name,
    length
  );

  _ISR_lock_ISR_enable( &lock_context );
  return name;
}
