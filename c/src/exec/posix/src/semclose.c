/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>


/*PAGE
 *
 *  sem_close
 *  
 *  Routine to close a semaphore that has been opened or initialized.
 *  
 *  11.2.4 Close a Named Semaphore, P1003.1b-1993, p.224
 */

int sem_close(
  sem_t *sem
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;
 
  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      the_semaphore->open_count -= 1;
      _POSIX_Semaphore_Delete( the_semaphore );
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
