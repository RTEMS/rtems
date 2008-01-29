/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>
#include <string.h>


/*
 *  This method sets the name of an object based upon a C string.
 */

boolean _Objects_Set_name(
  Objects_Information *information,
  Objects_Control     *the_object,
  const char          *name
)
{
  size_t                 length;
  const char            *s;

  s      = name;
  length = strnlen( name, information->name_length ) + 1;

  if ( information->is_string ) {
    char *d;

    d = _Workspace_Allocate( length );
    if ( !d )
      return FALSE;

    if ( the_object->name.name_p )
      _Workspace_Free( (void *)the_object->name.name_p );

    the_object->name.name_p = NULL;

    strncpy( d, name, length );
    the_object->name.name_p = d;
  } else {
    uint32_t  name_u32 = 0;
    char      lname[5] = "    ";
    int       i;

    for ( i=0 ; i<4 && i<length ; i++ )
      lname[ i ] = s[ i ];

    name_u32 |= ((uint32_t)lname[ 0 ] << 24);
    name_u32 |= ((uint32_t)lname[ 1 ] << 16);
    name_u32 |= ((uint32_t)lname[ 2 ] << 8);
    name_u32 |=  (uint32_t)lname[ 3 ];
    the_object->name.name_u32 = name_u32;
  }

  return TRUE;
}
