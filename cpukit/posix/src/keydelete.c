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
 *  17.1.3 Thread-Specific Data Key Deletion, P1003.1c/Draft 10, p. 167
 */

int pthread_key_delete(
  pthread_key_t  key
)
{
  register POSIX_Keys_Control *the_key;
  Objects_Locations            location;
  unsigned32                   the_class;
 
  the_key = _POSIX_Keys_Get( key, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:   /* should never happen */
      return EINVAL;
    case OBJECTS_LOCAL:
      _Objects_Close( &_POSIX_Keys_Information, &the_key->Object );

      the_key->is_active = FALSE;

      for ( the_class = OBJECTS_CLASSES_FIRST_THREAD_CLASS;
            the_class <= OBJECTS_CLASSES_LAST_THREAD_CLASS;
            the_class++ )
        if ( the_key->Values[ the_class ] )
          _Workspace_Free( the_key->Values[ the_class ] );

      /*
       *  NOTE:  The destructor is not called and it is the responsibility
       *         of the application to free the memory.
       */

      _POSIX_Keys_Free( the_key );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
