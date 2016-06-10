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
#include <rtems/posix/priorityimpl.h>
#include <rtems/score/schedulerimpl.h>

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
  const Scheduler_Control   *scheduler;
  Priority_Control           priority;

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

  /*
   *  We only support process private mutexes.
   */
  if ( the_attr->process_shared == PTHREAD_PROCESS_SHARED )
    return ENOSYS;

  if ( the_attr->process_shared != PTHREAD_PROCESS_PRIVATE )
    return EINVAL;

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

  if ( protocol == POSIX_MUTEX_PRIORITY_CEILING ) {
    int  prio_ceiling;
    bool valid;

    scheduler = _Scheduler_Get_own( _Thread_Get_executing() );
    prio_ceiling = the_attr->prio_ceiling;

    if ( prio_ceiling == INT_MAX ) {
      prio_ceiling = _POSIX_Priority_Get_maximum( scheduler );
    }

    priority = _POSIX_Priority_To_core( scheduler, prio_ceiling, &valid );
    if ( !valid ) {
      return EINVAL;
    }
  }

  the_mutex = _POSIX_Mutex_Allocate();

  if ( !the_mutex ) {
    _Objects_Allocator_unlock();
    return EAGAIN;
  }

  the_mutex->protocol = protocol;
  the_mutex->is_recursive = ( the_attr->type == PTHREAD_MUTEX_RECURSIVE );

  switch ( protocol ) {
    case POSIX_MUTEX_PRIORITY_CEILING:
      _CORE_ceiling_mutex_Initialize( &the_mutex->Mutex, scheduler, priority );
      break;
    default:
      _Assert(
        the_mutex->protocol == POSIX_MUTEX_NO_PROTOCOL
          || the_mutex->protocol == POSIX_MUTEX_PRIORITY_INHERIT
      );
      _CORE_recursive_mutex_Initialize( &the_mutex->Mutex.Recursive );
      break;
  }

  _Objects_Open_u32( &_POSIX_Mutex_Information, &the_mutex->Object, 0 );

  *mutex = the_mutex->Object.id;

  _Objects_Allocator_unlock();
  return 0;
}
