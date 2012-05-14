/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <stdlib.h>
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
  Objects_Information   *information;
  const char            *s;
  char                  *d;
  uint32_t               i;
  char                   lname[5];
  Objects_Control       *the_object;
  Objects_Locations      location;
  Objects_Id             tmpId;

  if ( length == 0 )
    return NULL;

  if ( name == NULL )
    return NULL;

  tmpId = (id == OBJECTS_ID_OF_SELF) ? _Thread_Executing->Object.id : id;

  information = _Objects_Get_information_id( tmpId );
  if ( !information )
    return NULL;

  the_object = _Objects_Get( information, tmpId, &location );
  switch ( location ) {

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
      /* not supported */
#endif
    case OBJECTS_ERROR:
      return NULL;

    case OBJECTS_LOCAL:

      #if defined(RTEMS_SCORE_OBJECT_ENABLE_STRING_NAMES)
        if ( information->is_string ) {
          s = the_object->name.name_p;
        } else
      #endif
      {
        uint32_t  u32_name = (uint32_t) the_object->name.name_u32;

        lname[ 0 ] = (u32_name >> 24) & 0xff;
        lname[ 1 ] = (u32_name >> 16) & 0xff;
        lname[ 2 ] = (u32_name >>  8) & 0xff;
        lname[ 3 ] = (u32_name >>  0) & 0xff;
        lname[ 4 ] = '\0';
        s = lname;
      }

      d = name;
      if ( s ) {
        for ( i=0 ; i<(length-1) && *s ; i++, s++, d++ ) {
          *d = (isprint((unsigned char)*s)) ? *s : '*';
        }
      }
      *d = '\0';

      _Thread_Enable_dispatch();
      return name;
  }
  return NULL;                  /* unreachable path */
}
