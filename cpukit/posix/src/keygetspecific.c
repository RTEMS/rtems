/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/key.h>

/*PAGE
 *
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

void *pthread_getspecific(
  pthread_key_t  key
)
{
  register POSIX_Keys_Control *the_key;
  unsigned32                   index;
  unsigned32                   class;
  Objects_Locations            location;
  void                        *key_data;
 
  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:   /* should never happen */
      return NULL;
    case OBJECTS_LOCAL:
      index = _Objects_Get_index( _Thread_Executing->Object.id );
      class = _Objects_Get_class( _Thread_Executing->Object.id );
      key_data = (void *) the_key->Values[ class ][ index ];
      _Thread_Enable_dispatch();
      return key_data;
  }
  return (void *) POSIX_BOTTOM_REACHED();
}
