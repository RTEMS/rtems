/*
 *  RTEMS API Initialization Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

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

/*
 *  _POSIX_API_Initialize
 *
 *  XXX
 */

Objects_Information *_POSIX_Objects[ OBJECTS_POSIX_CLASSES_LAST + 1 ];

void _POSIX_API_Initialize(void)
{
  /*
   * If there are any type size assumptions in the POSIX API, this is
   * the appropriate place to place them.
   *
   * Currently, there are no none type size assumptions.
   */

  /*
   * Install our API Object Management Table and initialize the
   * various managers.
   */
  _Objects_Information_table[OBJECTS_POSIX_API] = _POSIX_Objects;

  _POSIX_signals_Manager_Initialization();
  _POSIX_Threads_Manager_initialization();
  _POSIX_Condition_variables_Manager_initialization();
  _POSIX_Key_Manager_initialization();
  _POSIX_Mutex_Manager_initialization();
  _POSIX_Message_queue_Manager_initialization();
  _POSIX_Semaphore_Manager_initialization();
  _POSIX_Timer_Manager_initialization();
  _POSIX_Barrier_Manager_initialization();
  _POSIX_RWLock_Manager_initialization();
  _POSIX_Spinlock_Manager_initialization();
}

#endif
/* end of file */
