/**
 * @file
 *
 * @brief Initialize a Mutex
 * @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/muteximpl.h>
#include <rtems/posix/posixapi.h>
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>

#include <limits.h>

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Mutex_Control, flags )
    == offsetof( pthread_mutex_t, _flags ),
  POSIX_MUTEX_CONTROL_FLAGS
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Mutex_Control, Recursive )
    == offsetof( pthread_mutex_t, _Recursive ),
  POSIX_MUTEX_CONTROL_RECURSIVE
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Mutex_Control, Priority_ceiling )
    == offsetof( pthread_mutex_t, _Priority_ceiling ),
  POSIX_MUTEX_CONTROL_PRIORITY_CEILING
);

RTEMS_STATIC_ASSERT(
  offsetof( POSIX_Mutex_Control, scheduler )
    == offsetof( pthread_mutex_t, _scheduler ),
  POSIX_MUTEX_CONTROL_SCHEDULER
);

RTEMS_STATIC_ASSERT(
  sizeof( POSIX_Mutex_Control ) == sizeof( pthread_mutex_t ),
  POSIX_MUTEX_CONTROL_SIZE
);

const pthread_mutexattr_t _POSIX_Mutex_Default_attributes = {
#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
  .type           = PTHREAD_MUTEX_DEFAULT,
#endif
  .is_initialized = true,
  .process_shared = PTHREAD_PROCESS_PRIVATE,
  .prio_ceiling   = INT_MAX,
  .protocol       = PTHREAD_PRIO_NONE,
  .recursive      = false
};

/**
 * 11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 *
 * NOTE:  XXX Could be optimized so all the attribute error checking
 *            is not performed when attr is NULL.
 */

int pthread_mutex_init(
  pthread_mutex_t           *mutex,
  const pthread_mutexattr_t *attr
)
{
  POSIX_Mutex_Control       *the_mutex;
  const pthread_mutexattr_t *the_attr;
  POSIX_Mutex_Protocol       protocol;
  unsigned long              flags;
  Priority_Control           priority;
  const Scheduler_Control   *scheduler;

  if ( attr ) the_attr = attr;
  else        the_attr = &_POSIX_Mutex_Default_attributes;

  /* Check for NULL mutex */
  if ( !mutex )
    return EINVAL;

  /*
   *  The POSIX specification says:
   *
   *  "Attempting to initialize an already initialized mutex results
   *  in undefined behavior."
   *
   *  Trying to keep the caller from doing the create when *mutex
   *  is actually a valid ID causes grief.  All it takes is the wrong
   *  value in an uninitialized variable to make this fail.
   *
   *  Thus, we do not look at *mutex.
   */

  if ( !the_attr->is_initialized )
    return EINVAL;

  if ( !_POSIX_Is_valid_pshared( the_attr->process_shared ) ) {
    return EINVAL;
  }

  /*
   *  Determine the discipline of the mutex
   */
  switch ( the_attr->protocol ) {
    case PTHREAD_PRIO_NONE:
      protocol = POSIX_MUTEX_NO_PROTOCOL;
      break;
    case PTHREAD_PRIO_INHERIT:
      protocol = POSIX_MUTEX_PRIORITY_INHERIT;
      break;
    case PTHREAD_PRIO_PROTECT:
      protocol = POSIX_MUTEX_PRIORITY_CEILING;
      break;
    default:
      return EINVAL;
  }

#if defined(_UNIX98_THREAD_MUTEX_ATTRIBUTES)
  /*
   *  Validate the mutex type and set appropriate SuperCore mutex
   *  attributes.
   */
  switch ( the_attr->type ) {
    case PTHREAD_MUTEX_NORMAL:
    case PTHREAD_MUTEX_RECURSIVE:
    case PTHREAD_MUTEX_ERRORCHECK:
    case PTHREAD_MUTEX_DEFAULT:
      break;

    default:
      return EINVAL;
  }
#endif

  the_mutex = _POSIX_Mutex_Get( mutex );

  flags = (uintptr_t) the_mutex ^ POSIX_MUTEX_MAGIC;
  flags &= ~POSIX_MUTEX_FLAGS_MASK;
  flags |= protocol;

  if ( the_attr->type == PTHREAD_MUTEX_RECURSIVE ) {
    flags |= POSIX_MUTEX_RECURSIVE;
  }

  the_mutex->flags = flags;

  if ( protocol == POSIX_MUTEX_PRIORITY_CEILING ) {
    int  prio_ceiling;
    bool valid;

    scheduler = _Thread_Scheduler_get_home( _Thread_Get_executing() );
    prio_ceiling = the_attr->prio_ceiling;

    if ( prio_ceiling == INT_MAX ) {
      prio_ceiling = _POSIX_Priority_Get_maximum( scheduler );
    }

    priority = _POSIX_Priority_To_core( scheduler, prio_ceiling, &valid );
    if ( !valid ) {
      return EINVAL;
    }
  } else {
    priority = 0;
    scheduler = NULL;
  }

  _Thread_queue_Queue_initialize(
    &the_mutex->Recursive.Mutex.Queue.Queue,
    NULL
  );
  the_mutex->Recursive.nest_level = 0;
  _Priority_Node_initialize( &the_mutex->Priority_ceiling, priority );
  the_mutex->scheduler = scheduler;
  return 0;
}
