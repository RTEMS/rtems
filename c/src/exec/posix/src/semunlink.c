/*
 *  $Id$
 */

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
#include <rtems/posix/seterr.h>

/*PAGE
 *
 *  11.2.5 Remove a Named Semaphore, P1003.1b-1993, p.225
 */

int sem_unlink(
  const char *name
)
{
  int  status;
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Id                        the_semaphore_id;
  Objects_Locations                 location;
 
  status = _POSIX_Semaphore_Name_to_id( name, &the_semaphore_id );
  if ( status != 0 )
    set_errno_and_return_minus_one( status );

  the_semaphore = _POSIX_Semaphore_Get( &the_semaphore_id, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:

#if defined(RTEMS_MULTIPROCESSING)
      if ( the_semaphore->process_shared == PTHREAD_PROCESS_SHARED ) {
        _Objects_MP_Close(
          &_POSIX_Semaphore_Information,
          the_semaphore->Object.id
        );
      }
#endif

      the_semaphore->linked = FALSE;
      _POSIX_Semaphore_Namespace_remove( the_semaphore );
      _POSIX_Semaphore_Delete( the_semaphore );

      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
