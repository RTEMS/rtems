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
 *  11.4.2 Initializing and Destroying a Condition Variable, 
 *         P1003.1c/Draft 10, p. 87
 */
 
int pthread_cond_destroy(
  pthread_cond_t           *cond
)
{
  register POSIX_Condition_variables_Control *the_cond;
  Objects_Locations                           location;
 
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
 
      if ( _Thread_queue_First( &the_cond->Wait_queue ) ) {
        _Thread_Enable_dispatch();
        return EBUSY;
      }
 
      _Objects_Close(
        &_POSIX_Condition_variables_Information,
        &the_cond->Object
      );
 
      _POSIX_Condition_variables_Free( the_cond );
 
#if defined(RTEMS_MULTIPROCESSING)
      if ( the_cond->process_shared == PTHREAD_PROCESS_SHARED ) {
 
        _Objects_MP_Close(
          &_POSIX_Condition_variables_Information,
          the_cond->Object.id
        );
 
        _POSIX_Condition_variables_MP_Send_process_packet(
          POSIX_CONDITION_VARIABLES_MP_ANNOUNCE_DELETE,
          the_cond->Object.id,
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
