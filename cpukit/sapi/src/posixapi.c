/*
 *  RTEMS API Initialization Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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
#include <rtems/posix/barrier.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/config.h>
#include <rtems/posix/key.h>
#include <rtems/posix/mqueue.h>
#include <rtems/posix/mutex.h>
#include <rtems/posix/posixapi.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/rwlock.h>
#include <rtems/posix/timer.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/spinlock.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_API_Initialize
 *
 *  XXX
 */

Objects_Information *_POSIX_Objects[ OBJECTS_POSIX_CLASSES_LAST + 1 ];

void _POSIX_API_Initialize(void)
{
  const posix_api_configuration_table *api;

  /* XXX need to assert here based on size assumptions */

  assert( sizeof(pthread_t) == sizeof(Objects_Id) );

  /*
   * Install our API Object Management Table and initialize the
   * various managers.
   */
  api = &Configuration_POSIX_API;

  _Objects_Information_table[OBJECTS_POSIX_API] = _POSIX_Objects;

  _POSIX_signals_Manager_Initialization( api->maximum_queued_signals );

  _POSIX_Threads_Manager_initialization(
    api->maximum_threads,
    api->number_of_initialization_threads,
    api->User_initialization_threads_table
  );

  _POSIX_Condition_variables_Manager_initialization(
    api->maximum_condition_variables
  );

  _POSIX_Key_Manager_initialization( api->maximum_keys );

  _POSIX_Mutex_Manager_initialization( api->maximum_mutexes );

  _POSIX_Message_queue_Manager_initialization( api->maximum_message_queues );

  _POSIX_Semaphore_Manager_initialization( api->maximum_semaphores );

  _POSIX_Timer_Manager_initialization( api->maximum_timers );

  _POSIX_Barrier_Manager_initialization( api->maximum_barriers );

  _POSIX_RWLock_Manager_initialization( api->maximum_rwlocks );

  _POSIX_Spinlock_Manager_initialization(api->maximum_spinlocks);
}

#endif
/* end of file */
