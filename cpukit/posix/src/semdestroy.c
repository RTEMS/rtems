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
 *  11.2.2 Destroy an Unnamed Semaphore, P1003.1b-1993, p.220
 */

int sem_destroy(
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
      /*
       *  Undefined operation on a named semaphore.
       */

      if ( the_semaphore->named == TRUE ) {
        set_errno_and_return_minus_one( EINVAL );
      }
 
      _Objects_Close( &_POSIX_Semaphore_Information, &the_semaphore->Object );
 
      _CORE_semaphore_Flush(
        &the_semaphore->Semaphore,
#if defined(RTEMS_MULTIPROCESSING)
        _POSIX_Semaphore_MP_Send_object_was_deleted,
#else
        NULL,
#endif
        -1  /* XXX should also seterrno -> EINVAL */
      );
 
      _POSIX_Semaphore_Free( the_semaphore );
 
#if defined(RTEMS_MULTIPROCESSING)
      if ( the_semaphore->process_shared == PTHREAD_PROCESS_SHARED ) {
 
        _Objects_MP_Close(
          &_POSIX_Semaphore_Information,
          the_semaphore->Object.id
        );
 
        _POSIX_Semaphore_MP_Send_process_packet(
          POSIX_SEMAPHORE_MP_ANNOUNCE_DELETE,
          the_semaphore->Object.id,
          0,                         /* Not used */
          0                          /* Not used */
        );
      }
#endif
      _Thread_Enable_dispatch();
      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
