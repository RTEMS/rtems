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
  uint32_t                     api;
  uint32_t                     index;
  Objects_Locations            location;
  void                        *key_data;

  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {
#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:   /* should never happen */
#endif
    case OBJECTS_ERROR:
      return NULL;
    case OBJECTS_LOCAL:
      api      = _Objects_Get_API( _Thread_Executing->Object.id );
      index    = _Objects_Get_index( _Thread_Executing->Object.id );
      key_data = (void *) the_key->Values[ api ][ index ];
      _Thread_Enable_dispatch();
      return key_data;
  }
  (void) POSIX_BOTTOM_REACHED();
  return (void *)NULL;
}
