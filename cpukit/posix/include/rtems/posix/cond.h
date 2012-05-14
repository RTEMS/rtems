/**
 * @file rtems/posix/cond.h
 *
 * This include file contains all the private support information for
 * POSIX condition variables.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_COND_H
#define _RTEMS_POSIX_COND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/object.h>
#include <rtems/score/threadq.h>

/*
 *  Constant to indicate condition variable does not currently have
 *  a mutex assigned to it.
 */

#define POSIX_CONDITION_VARIABLES_NO_MUTEX 0

/*
 *  Data Structure used to manage a POSIX condition variable
 */

typedef struct {
   Objects_Control       Object;
   int                   process_shared;
   pthread_mutex_t       Mutex;
   Thread_queue_Control  Wait_queue;
}  POSIX_Condition_variables_Control;

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */

POSIX_EXTERN Objects_Information  _POSIX_Condition_variables_Information;

/*
 *  The default condition variable attributes structure.
 */

extern const pthread_condattr_t _POSIX_Condition_variables_Default_attributes;

/*
 *  _POSIX_Condition_variables_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */

void _POSIX_Condition_variables_Manager_initialization(void);

/*
 *  _POSIX_Condition_variables_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a condition variable control block from
 *  the inactive chain of free condition variable control blocks.
 */

RTEMS_INLINE_ROUTINE POSIX_Condition_variables_Control *
  _POSIX_Condition_variables_Allocate( void );

/*
 *  _POSIX_Condition_variables_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a condition variable control block to the
 *  inactive chain of free condition variable control blocks.
 */

RTEMS_INLINE_ROUTINE void _POSIX_Condition_variables_Free (
  POSIX_Condition_variables_Control *the_condition_variable
);

/*
 *  _POSIX_Condition_variables_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps condition variable IDs to condition variable control
 *  blocks.  If ID corresponds to a local condition variable, then it returns
 *  the_condition variable control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the condition variable ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_condition variable is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_condition variable is undefined.
 */

#if 0
RTEMS_INLINE_ROUTINE POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get (
  Objects_Id        *id,
  Objects_Locations *location
);
#endif

/*
 *  _POSIX_Condition_variables_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_condition variable is NULL
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE bool _POSIX_Condition_variables_Is_null (
  POSIX_Condition_variables_Control *the_condition_variable
);

/*
 *  _POSIX_Condition_variables_Signal_support
 *
 *  DESCRIPTION:
 *
 *  A support routine which implements guts of the broadcast and single task
 *  wake up version of the "signal" operation.
 */

int _POSIX_Condition_variables_Signal_support(
  pthread_cond_t            *cond,
  bool                       is_broadcast
);

/*
 *  _POSIX_Condition_variables_Wait_support
 *
 *  DESCRIPTION:
 *
 *  A support routine which implements guts of the blocking, non-blocking, and
 *  timed wait version of condition variable wait routines.
 */

int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  Watchdog_Interval          timeout,
  bool                       already_timedout
);

/*
 *  _POSIX_Condition_variables_Get
 *
 *  DESCRIPTION:
 *
 *  A support routine which translates the condition variable id into
 *  a local pointer.  As a side-effect, it may create the condition
 *  variable.
 */

POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get (
  pthread_cond_t    *cond,
  Objects_Locations *location
);

#include <rtems/posix/cond.inl>

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
