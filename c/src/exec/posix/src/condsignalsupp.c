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
 *  _POSIX_Condition_variables_Signal_support
 *
 *  A support routine which implements guts of the broadcast and single task
 *  wake up version of the "signal" operation.
 */
 
int _POSIX_Condition_variables_Signal_support(
  pthread_cond_t            *cond,
  boolean                    is_broadcast
)
{
  register POSIX_Condition_variables_Control *the_cond;
  Objects_Locations                           location;
  Thread_Control                             *the_thread;
 
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
 
      do { 
        the_thread = _Thread_queue_Dequeue( &the_cond->Wait_queue );
        if ( !the_thread )
          the_cond->Mutex = POSIX_CONDITION_VARIABLES_NO_MUTEX;
      } while ( is_broadcast && the_thread );

      _Thread_Enable_dispatch();

      return 0;
  }
  return POSIX_BOTTOM_REACHED();
}
