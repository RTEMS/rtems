/**
 * @file
 *
 * @brief Support Call to function Enables Locking of Mutex Object 
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>

#include <rtems/system.h>
#include <rtems/score/coremuteximpl.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/muteximpl.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/posix/time.h>

/*
 *  _POSIX_Mutex_Lock_support
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of mutex lock.
 */

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t           *mutex,
  bool                       blocking,
  Watchdog_Interval          timeout
)
{
  register POSIX_Mutex_Control *the_mutex;
  Objects_Locations             location;
  ISR_Level                     level;
  Thread_Control               *executing;

  the_mutex = _POSIX_Mutex_Get_interrupt_disable( mutex, &location, &level );
  switch ( location ) {

    case OBJECTS_LOCAL:
      executing = _Thread_Executing;
      _CORE_mutex_Seize(
        &the_mutex->Mutex,
        executing,
        the_mutex->Object.id,
        blocking,
        timeout,
        level
      );
      _Objects_Put_for_get_isr_disable( &the_mutex->Object );
      return _POSIX_Mutex_Translate_core_mutex_return_code(
        (CORE_mutex_Status) executing->Wait.return_code
      );

#if defined(RTEMS_MULTIPROCESSING)
    case OBJECTS_REMOTE:
#endif
    case OBJECTS_ERROR:
      break;
  }

  return EINVAL;
}
