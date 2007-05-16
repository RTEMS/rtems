/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <inttypes.h>


/*
 *  This method objects the name of an object and returns its name
 *  in the form of a C string.  It attempts to be careful about
 *  overflowing the user's string and about returning unprintable characters.
 */

char *_Objects_Get_name_as_string(
  Objects_Id        id,
  size_t            length,
  char             *name
)
{
  Objects_Information *information;
  char                  *s;
  char                  *d;
  uint32_t               i;
  char                   lname[5];
  Objects_Control       *the_object;
  Objects_Locations      location;

  if ( length == 0 )
    return NULL;

  if ( name == NULL )
    return NULL;

  information = _Objects_Get_information( id );
  if ( !information )
    return NULL;

  the_object = _Objects_Get( information, id, &location );
  switch ( location ) {

    case OBJECTS_REMOTE:
      /* not supported */
    case OBJECTS_ERROR:
      return NULL;

    case OBJECTS_LOCAL:

      if ( information->is_string ) {
        s = the_object->name;
      } else {
        uint32_t  u32_name = (uint32_t) the_object->name;

        lname[ 0 ] = (u32_name >> 24) & 0xff;
        lname[ 1 ] = (u32_name >> 16) & 0xff;
        lname[ 2 ] = (u32_name >>  8) & 0xff;
        lname[ 3 ] = (u32_name >>  0) & 0xff;
        lname[ 4 ] = '\0';
        s = lname;
      }

      for ( i=0, d=name ; i<(length-1) && *s ; i++, s++, d++ ) {
        *d = (!isprint(*s)) ?  '*' : *s;
      }
      *d = '\0';

      _Thread_Enable_dispatch();
      return name;
  }
  return NULL;                  /* unreachable path */
}
