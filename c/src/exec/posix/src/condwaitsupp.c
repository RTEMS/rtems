/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/time.h>
#include <rtems/posix/mutex.h>

/*PAGE
 *
 *  _POSIX_Condition_variables_Wait_support
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of condition variable wait routines.
 */
 
int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  Watchdog_Interval          timeout,
  boolean                    already_timedout
)
{
  register POSIX_Condition_variables_Control *the_cond;
  Objects_Locations                           location;
  int                                         status;
  int                                         mutex_status;
 
  if ( !_POSIX_Mutex_Get( mutex, &location ) ) {
     return EINVAL;
  }

  _Thread_Unnest_dispatch();

  the_cond = _POSIX_Condition_variables_Get( cond, &location );
  switch ( location ) {
    case OBJECTS_REMOTE:
#if defined(RTEMS_MULTIPROCESSING)
      _Thread_Dispatch();
      return POSIX_MP_NOT_IMPLEMENTED();
      return EINVAL;
#endif
    case OBJECTS_ERROR:
      return EINVAL;
    case OBJECTS_LOCAL:
 
      if ( the_cond->Mutex && ( the_cond->Mutex != *mutex ) ) {
        _Thread_Enable_dispatch();
        return EINVAL;
      }
 
      (void) pthread_mutex_unlock( mutex );
/* XXX ignore this for now  since behavior is undefined
      if ( mutex_status ) {
        _Thread_Enable_dispatch();
        return EINVAL;
      }
*/

      if ( !already_timedout ) {
        the_cond->Mutex = *mutex;
 
        _Thread_queue_Enter_critical_section( &the_cond->Wait_queue );
        _Thread_Executing->Wait.return_code = 0;
        _Thread_Executing->Wait.queue       = &the_cond->Wait_queue;
        _Thread_Executing->Wait.id          = *cond;

        _Thread_queue_Enqueue( &the_cond->Wait_queue, timeout );

        _Thread_Enable_dispatch();

        /*
         *  Switch ourself out because we blocked as a result of the 
         *  _Thread_queue_Enqueue.
         */

        status = _Thread_Executing->Wait.return_code;
        if ( status && status != ETIMEDOUT )
          return status;

      } else {
        _Thread_Enable_dispatch();
        status = ETIMEDOUT;
      }

      /*
       *  When we get here the dispatch disable level is 0.
       */

      mutex_status = pthread_mutex_lock( mutex );
      if ( mutex_status )
        return EINVAL;
    
      return status;
  }
  return POSIX_BOTTOM_REACHED();
}
