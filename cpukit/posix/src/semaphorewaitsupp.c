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
 *  _POSIX_Semaphore_Wait_support
 */

int _POSIX_Semaphore_Wait_support(
  sem_t              *sem,
  boolean             blocking,
  Watchdog_Interval   timeout
)
{
  register POSIX_Semaphore_Control *the_semaphore;
  Objects_Locations                 location;
  int                               code;
 
  the_semaphore = _POSIX_Semaphore_Get( sem, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_REMOTE:
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      set_errno_and_return_minus_one( EINVAL );
    case OBJECTS_LOCAL:
      _CORE_semaphore_Seize(
        &the_semaphore->Semaphore,
        the_semaphore->Object.id,
        blocking,
        timeout
      );
      _Thread_Enable_dispatch();
      code = _Thread_Executing->Wait.return_code;
      switch (_Thread_Executing->Wait.return_code) {
        case 1:
	  errno = EAGAIN;
	  code = -1;
	  break;
	case 3:
	  errno = ETIMEDOUT;
	  code = -1;
	  break;
      }

      /*return _Thread_Executing->Wait.return_code;*/
      return code;
  }
  return POSIX_BOTTOM_REACHED();
}
