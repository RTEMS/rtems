/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/score/coremutex.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/time.h>

/*
 *  11.3.2 Initializing and Destroying a Mutex, P1003.1c/Draft 10, p. 87
 *
 *  NOTE:  XXX Could be optimized so all the attribute error checking
 *             is not performed when attr is NULL.
 */

int pthread_mutex_init(
  pthread_mutex_t           *mutex,
  const pthread_mutexattr_t *attr
)
{
  POSIX_Mutex_Control          *the_mutex;
  CORE_mutex_Attributes        *the_mutex_attr;
  const pthread_mutexattr_t    *the_attr;
  CORE_mutex_Disciplines        the_discipline;

  if ( attr ) the_attr = attr;
  else        the_attr = &_POSIX_Mutex_Default_attributes;

  /* Check for NULL mutex */
  if ( !mutex )
    return EINVAL;

  /*
   *  This code should eventually be removed.
   *
   *  Although the POSIX specification says:
   *
   *  "Attempting to initialize an already initialized mutex results
   *  in undefined behavior."
   *
   *  Trying to keep the caller from doing the create when *mutex
   *  is actually a valid ID causes grief.  All it takes is the wrong
   *  value in an uninitialized variable to make this fail.  As best
   *  I can tell, RTEMS was the only pthread implementation to choose
   *  this option for "undefined behavior" and doing so has created
   *  portability problems.  In particular, Rosimildo DaSilva
   *  <rdasilva@connecttel.com> saw seemingly random failures in the
   *  RTEMS port of omniORB2 when this code was enabled.
   *
   *  Joel Sherrill <joel@OARcorp.com>     14 May 1999
   *  NOTE: Be careful to avoid infinite recursion on call to this
   *        routine in _POSIX_Mutex_Get.
   */
  #if 0
  {
    POSIX_Mutex_Control *mutex_in_use;
    Objects_Locations    location;

    if ( *mutex != PTHREAD_MUTEX_INITIALIZER ) {

      /* EBUSY if *mutex is a valid id */

      mutex_in_use = _POSIX_Mutex_Get( mutex, &location );
      switch ( location ) {
        case OBJECTS_LOCAL:
          _Thread_Enable_dispatch();
          return EBUSY;
        #if defined(RTEMS_MULTIPROCESSING)
          case OBJECTS_REMOTE:
        #endif
        case OBJECTS_ERROR:
          break;
      }
    }
  }
  #endif

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
      the_discipline = CORE_MUTEX_DISCIPLINES_FIFO;
      break;
    case PTHREAD_PRIO_INHERIT:
      the_discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_INHERIT;
      break;
    case PTHREAD_PRIO_PROTECT:
      the_discipline = CORE_MUTEX_DISCIPLINES_PRIORITY_CEILING;
      break;
    default:
      return EINVAL;
  }

  /*
   *  Validate the priority ceiling field -- should always be valid.
   */
  if ( !_POSIX_Priority_Is_valid( the_attr->prio_ceiling ) )
    return EINVAL;

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

  /*
   *  Enter a dispatching critical section and begin to do the real work.
   */
  _Thread_Disable_dispatch();

  the_mutex = _POSIX_Mutex_Allocate();

  if ( !the_mutex ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  the_mutex->process_shared = the_attr->process_shared;

  the_mutex_attr = &the_mutex->Mutex.Attributes;

  if ( the_attr->recursive )
    the_mutex_attr->lock_nesting_behavior = CORE_MUTEX_NESTING_ACQUIRES;
  else
    the_mutex_attr->lock_nesting_behavior = CORE_MUTEX_NESTING_IS_ERROR;
  the_mutex_attr->only_owner_release = true;
  the_mutex_attr->priority_ceiling =
    _POSIX_Priority_To_core( the_attr->prio_ceiling );
  the_mutex_attr->discipline = the_discipline;

  /*
   *  Must be initialized to unlocked.
   */
  _CORE_mutex_Initialize(
    &the_mutex->Mutex,
    the_mutex_attr,
    CORE_MUTEX_UNLOCKED
  );

  _Objects_Open_u32( &_POSIX_Mutex_Information, &the_mutex->Object, 0 );

  *mutex = the_mutex->Object.id;

  _Thread_Enable_dispatch();
  return 0;
}
