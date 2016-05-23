/**
 * @file
 *
 * @brief Private Inlined Routines for POSIX Mutex's.
 * 
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX mutex's.
 */

/*  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_MUTEXIMPL_H
#define _RTEMS_POSIX_MUTEXIMPL_H

#include <rtems/posix/mutex.h>
#include <rtems/score/coremuteximpl.h>

#include <errno.h>
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
extern Objects_Information _POSIX_Mutex_Information;

/**
 *  The default mutex attributes structure.
 */
extern pthread_mutexattr_t _POSIX_Mutex_Default_attributes;

/**
 *  @brief POSIX Mutex Allocate
 *
 *  This function allocates a mutexes control block from
 *  the inactive chain of free mutexes control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Allocate( void )
{
  return (POSIX_Mutex_Control *) _Objects_Allocate( &_POSIX_Mutex_Information );
}

/**
 *  @brief POSIX Mutex Free
 *
 *  This routine frees a mutexes control block to the
 *  inactive chain of free mutexes control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Free(
  POSIX_Mutex_Control *the_mutex
)
{
  _Objects_Free( &_POSIX_Mutex_Information, &the_mutex->Object );
}


/**
 *  @brief POSIX Mutex Lock Support Method
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of mutex lock.
 */
int _POSIX_Mutex_Lock_support(
  pthread_mutex_t           *mutex,
  bool                       blocking,
  Watchdog_Interval          timeout
);

/**
 *  @brief POSIX Mutex Get (Interrupt Disable)
 *
 *  A support routine which translates the mutex id into a local pointer.
 *  As a side-effect, it may create the mutex.
 *
 *  @note: This version of the method uses an interrupt critical section.
 */
POSIX_Mutex_Control *_POSIX_Mutex_Get(
  pthread_mutex_t      *mutex,
  Thread_queue_Context *queue_context
);

RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Get_no_protection(
  const pthread_mutex_t *mutex
)
{
  return (POSIX_Mutex_Control *) _Objects_Get_no_protection(
    (Objects_Id) *mutex,
    &_POSIX_Mutex_Information
  );
}

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */

