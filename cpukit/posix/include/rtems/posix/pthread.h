/*  rtems/posix/pthread.h
 *
 *  This include file contains all the private support information for
 *  POSIX threads.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */
 
#ifndef __RTEMS_POSIX_THREADS_h
#define __RTEMS_POSIX_THREADS_h
 
#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/posix/config.h>
#include <rtems/posix/threadsup.h>

#define PTHREAD_MINIMUM_STACK_SIZE (STACK_MINIMUM_SIZE * 2)

/*
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
 
POSIX_EXTERN Objects_Information  _POSIX_Threads_Information;

/*
 *  These are used to manage the user initialization threads.
 */
 
POSIX_EXTERN posix_initialization_threads_table 
                   *_POSIX_Threads_User_initialization_threads;
POSIX_EXTERN unsigned32   _POSIX_Threads_Number_of_initialization_threads;

extern const pthread_attr_t _POSIX_Threads_Default_attributes;
 
/*
 *  _POSIX_Threads_Manager_initialization
 *
 *  DESCRIPTION:
 *
 *  This routine performs the initialization necessary for this manager.
 */
 
void _POSIX_Threads_Manager_initialization(
  unsigned32                          maximum_pthreads,
  unsigned32                          number_of_initialization_threads,
  posix_initialization_threads_table *user_threads
);
 
/*
 *  _POSIX_Threads_Allocate
 *
 *  DESCRIPTION:
 *
 *  This function allocates a pthread control block from
 *  the inactive chain of free pthread control blocks.
 */
 
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Allocate( void );
 
/*
 *  _POSIX_Threads_Free
 *
 *  DESCRIPTION:
 *
 *  This routine frees a pthread control block to the
 *  inactive chain of free pthread control blocks.
 */
 
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Free(
  Thread_Control *the_pthread
);
 
/*
 *  _POSIX_Threads_Get
 *
 *  DESCRIPTION:
 *
 *  This function maps pthread IDs to pthread control blocks.
 *  If ID corresponds to a local pthread, then it returns
 *  the_pthread control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the pthread ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_pthread is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_pthread is undefined.
 */
 
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Get(
  pthread_t          id,
  Objects_Locations *location
);
 
/*
 *  _POSIX_Threads_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_pthread is NULL and FALSE otherwise.
 */
 
RTEMS_INLINE_ROUTINE boolean _POSIX_Threads_Is_null(
  Thread_Control *the_pthread
);

/*
 *  _POSIX_Threads_Sporadic_budget_callout
 *
 *  DESCRIPTION:
 *
 *  This routine handles the sporadic scheduling algorithm.
 */

void _POSIX_Threads_Sporadic_budget_callout(
  Thread_Control *the_thread
);

/*
 *  _POSIX_Threads_Sporadic_budget_TSR
 *
 *  DESCRIPTION:
 *
 *  This routine supports the sporadic scheduling algorithm.
 */

void _POSIX_Threads_Sporadic_budget_TSR(
  Objects_Id      id,
  void           *argument
);

#include <rtems/posix/pthread.inl>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/posix/pthreadmp.h>
#endif

#ifdef __cplusplus
}
#endif
 
#endif
/*  end of include file */

