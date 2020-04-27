/**
 * @file
 *
 * @ingroup RTEMSScoreObject
 *
 * @brief Set Objects Name
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/objectimpl.h>
#include <rtems/score/wkspace.h>

#include <string.h>

bool _Objects_Set_name(
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
      return false;
    }

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

  return true;
}
