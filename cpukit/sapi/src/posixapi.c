/*
 *  RTEMS API Initialization Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <assert.h>

/*
 *  POSIX_API_INIT is defined so all of the POSIX API
 *  data will be included in this object file.
 */

#define POSIX_API_INIT

#include <rtems/system.h>    /* include this before checking RTEMS_POSIX_API */
#ifdef RTEMS_POSIX_API

#include <sys/types.h>
#include <mqueue.h>
#include <rtems/config.h>
#include <rtems/score/object.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/config.h>
#include <rtems/posix/key.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/ptimer.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_API_Initialize
 *
 *  XXX
 */

posix_api_configuration_table _POSIX_Default_configuration = {
  0,                             /* maximum_threads */
  0,                             /* maximum_mutexes */
  0,                             /* maximum_condition_variables */
  0,                             /* maximum_keys */
  0,                             /* maximum_timers */
  0,                             /* maximum_queued_signals */
  0,                             /* number_of_initialization_threads */
  0,                             /* maximum_message_queues */
  0,                             /* maximum_semaphores */
  NULL                           /* User_initialization_threads_table */
};

Objects_Information *_POSIX_Objects[ OBJECTS_POSIX_CLASSES_LAST + 1 ];


void _POSIX_API_Initialize(
  rtems_configuration_table *configuration_table
)
{
  posix_api_configuration_table *api_configuration;

  /* XXX need to assert here based on size assumptions */

  assert( sizeof(pthread_t) == sizeof(Objects_Id) );

  api_configuration = configuration_table->POSIX_api_configuration;
  if ( !api_configuration ) 
    api_configuration = &_POSIX_Default_configuration;

  _Objects_Information_table[OBJECTS_POSIX_API] = _POSIX_Objects;

  _POSIX_signals_Manager_Initialization(
    api_configuration->maximum_queued_signals
  );

  _POSIX_Threads_Manager_initialization(
    api_configuration->maximum_threads,
    api_configuration->number_of_initialization_threads,
    api_configuration->User_initialization_threads_table
  );
 
  _POSIX_Condition_variables_Manager_initialization(
    api_configuration->maximum_condition_variables
  );

  _POSIX_Key_Manager_initialization( api_configuration->maximum_keys );

  _POSIX_Mutex_Manager_initialization( 
    api_configuration->maximum_mutexes
  );

  _POSIX_Message_queue_Manager_initialization(
    api_configuration->maximum_message_queues
  );

  _POSIX_Semaphore_Manager_initialization(
    api_configuration->maximum_semaphores
  );

  _POSIX_Timer_Manager_initialization ( api_configuration->maximum_timers );
}

#endif
/* end of file */
