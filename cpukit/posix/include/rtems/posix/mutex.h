/**
 * @file rtems/posix/mutex.h
 *
 * This include file contains all the private support information for
 * POSIX mutex's.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_MUTEX_H
#define _RTEMS_POSIX_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/coremutex.h>
#include <pthread.h>

/*
 *  Data Structure used to manage a POSIX mutex
 */

typedef struct {
   Objects_Control     Object;
   int                 process_shared;
   CORE_mutex_Control  Mutex;
}  POSIX_Mutex_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_Mutex_Information;

/*
 *  The default mutex attributes structure.
 */

POSIX_EXTERN pthread_mutexattr_t _POSIX_Mutex_Default_attributes;

/*
 *  _POSIX_Mutex_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _POSIX_Mutex_Manager_initialization(void);

/*
 *  _POSIX_Mutex_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a mutexes control block from
 *  the inactive chain of free mutexes control blocks.
 */

RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Allocate( void );

/*
 *  _POSIX_Mutex_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a mutexes control block to the
 *  inactive chain of free mutexes control blocks.
 */

RTEMS_INLINE_ROUTINE void _POSIX_Mutex_Free (
  POSIX_Mutex_Control *the_mutex
);

#if 0
/*
 *  _POSIX_Mutex_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps mutexes IDs to mutexes control blocks.
 *  If ID corresponds to a local mutexes, then it returns
 *  the_mutex control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the mutexes ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_mutex is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_mutex is undefined.
 */

RTEMS_INLINE_ROUTINE POSIX_Mutex_Control *_POSIX_Mutex_Get (
  Objects_Id        *id,
  Objects_Locations *location
);

/*
 *  _POSIX_Mutex_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_mutex is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE bool    _POSIX_Mutex_Is_null (
  POSIX_Mutex_Control *the_mutex
);
#endif

/*
 *  _POSIX_Mutex_Lock_support
 *
 *  DESCRIPTION:
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of mutex lock.
 */

int _POSIX_Mutex_Lock_support(
  pthread_mutex_t           *mutex,
  bool                       blocking,
  Watchdog_Interval          timeout
);

/*
 *  _POSIX_Mutex_Translate_core_mutex_return_code
 *
 *  DESCRIPTION:
 *
 *  A support routine which converts core mutex status codes into the
 *  appropriate POSIX status values.
 */

int _POSIX_Mutex_Translate_core_mutex_return_code(
  CORE_mutex_Status  the_mutex_status
);


/*
 *  _POSIX_Mutex_Get
 *
 *  DESCRIPTION:
 *
 *  A support routine which translates the mutex id into a local pointer.
 *  As a side-effect, it may create the mutex.
 *
 *  NOTE:
 *
 *  This version of the method uses a dispatching critical section.
 */

POSIX_Mutex_Control *_POSIX_Mutex_Get (
  pthread_mutex_t   *mutex,
  Objects_Locations *location
);

/*
 *  _POSIX_Mutex_Get
 *
 *  DESCRIPTION:
 *
 *  A support routine which translates the mutex id into a local pointer.
 *  As a side-effect, it may create the mutex.
 *
 *  NOTE:
 *
 *  This version of the method uses an interrupt critical section.
 */

POSIX_Mutex_Control *_POSIX_Mutex_Get_interrupt_disable (
  pthread_mutex_t   *mutex,
  Objects_Locations *location,
  ISR_Level         *level
);

#include <rtems/posix/mutex.inl>

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
