/**
 * @file
 *
 * This include file contains the static inline implementation of the private 
 * inlined routines for POSIX condition variables.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_CONDIMPL_H
#define _RTEMS_POSIX_CONDIMPL_H
 
#include <rtems/posix/cond.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdog.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  Constant to indicate condition variable does not currently have
 *  a mutex assigned to it.
 */
#define POSIX_CONDITION_VARIABLES_NO_MUTEX 0

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Condition_variables_Information;

/**
 *  The default condition variable attributes structure.
 */
extern const pthread_condattr_t _POSIX_Condition_variables_Default_attributes;

/**
 *  @brief POSIX Condition Variable Manager Initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _POSIX_Condition_variables_Manager_initialization(void);

/**
 *  @brief POSIX Condition Variable Allocate
 *
 *  This function allocates a condition variable control block from
 *  the inactive chain of free condition variable control blocks.
 */
RTEMS_INLINE_ROUTINE POSIX_Condition_variables_Control *
  _POSIX_Condition_variables_Allocate( void )
{
  return (POSIX_Condition_variables_Control *)
    _Objects_Allocate( &_POSIX_Condition_variables_Information );
}

/**
 *  @brief POSIX Condition Variable Free
 *
 *  This routine frees a condition variable control block to the
 *  inactive chain of free condition variable control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Condition_variables_Free (
  POSIX_Condition_variables_Control *the_condition_variable
)
{
  _Thread_queue_Destroy( &the_condition_variable->Wait_queue );
  _Objects_Free(
    &_POSIX_Condition_variables_Information,
    &the_condition_variable->Object
  );
}

/**
 *  @brief POSIX Condition Variable Get
 *
 *  This function maps condition variable IDs to condition variable control
 *  blocks.  If ID corresponds to a local condition variable, then it returns
 *  the_condition variable control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the condition variable ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_condition variable is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_condition variable is undefined.
 */
POSIX_Condition_variables_Control *_POSIX_Condition_variables_Get (
  pthread_cond_t    *cond,
  Objects_Locations *location
);

/**
 * @brief Implements wake up version of the "signal" operation.
 * 
 * A support routine which implements guts of the broadcast and single task
 * wake up version of the "signal" operation.
 */
int _POSIX_Condition_variables_Signal_support(
  pthread_cond_t            *cond,
  bool                       is_broadcast
);

/**
 * @brief POSIX condition variables wait support.
 *
 * A support routine which implements guts of the blocking, non-blocking, and
 * timed wait version of condition variable wait routines.
 */
int _POSIX_Condition_variables_Wait_support(
  pthread_cond_t            *cond,
  pthread_mutex_t           *mutex,
  Watchdog_Interval          timeout,
  bool                       already_timedout
);

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
