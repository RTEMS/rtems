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
 
int pthread_cond_init(
  pthread_cond_t           *cond,
  const pthread_condattr_t *attr
)
{
  POSIX_Condition_variables_Control   *the_cond;
  const pthread_condattr_t            *the_attr;
 
  if ( attr ) the_attr = attr;
  else        the_attr = &_POSIX_Condition_variables_Default_attributes;
 
  /*
   *  XXX: Be careful about attributes when global!!!
   */
 
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    return POSIX_MP_NOT_IMPLEMENTED();
 
  if ( !the_attr->is_initialized )
    return EINVAL;
 
  _Thread_Disable_dispatch();
 
  the_cond = _POSIX_Condition_variables_Allocate();
 
  if ( !the_cond ) {
    _Thread_Enable_dispatch();
    return ENOMEM;
  }
 
#if defined(RTEMS_MULTIPROCESSING)
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED &&
     !( _Objects_MP_Allocate_and_open( &_POSIX_Condition_variables_Information,
                0, the_cond->Object.id, FALSE ) ) ) {
    _POSIX_Condition_variables_Free( the_cond );
    _Thread_Enable_dispatch();
    return EAGAIN;
  }
#endif
 
  the_cond->process_shared  = the_attr->process_shared;

  the_cond->Mutex = POSIX_CONDITION_VARIABLES_NO_MUTEX;

/* XXX some more initialization might need to go here */
  _Thread_queue_Initialize(
    &the_cond->Wait_queue,
    OBJECTS_POSIX_CONDITION_VARIABLES,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_CONDITION_VARIABLE,
#if defined(RTEMS_MULTIPROCESSING)
    _POSIX_Condition_variables_MP_Send_extract_proxy,
#else
    NULL,
#endif
    ETIMEDOUT
  );

  _Objects_Open(
    &_POSIX_Condition_variables_Information,
    &the_cond->Object,
    0
  );
 
  *cond = the_cond->Object.id;
 
#if defined(RTEMS_MULTIPROCESSING)
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    _POSIX_Condition_variables_MP_Send_process_packet(
      POSIX_CONDITION_VARIABLES_MP_ANNOUNCE_CREATE,
      the_cond->Object.id,
      0,                         /* Name not used */
      0                          /* Not used */
    );
#endif
 
  _Thread_Enable_dispatch();

  return 0;
}
