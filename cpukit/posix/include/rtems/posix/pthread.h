/**
 * @file rtems/posix/pthread.h
 *
 * This include file contains all the private support information for
 * POSIX threads.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_PTHREAD_H
#define _RTEMS_POSIX_PTHREAD_H


#include <rtems/posix/config.h>
#include <rtems/posix/threadsup.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 *  The following sets the minimum stack size for POSIX threads.
 */
#define PTHREAD_MINIMUM_STACK_SIZE (_Stack_Minimum() * 2)

/**
 *  The following defines the information control block used to manage
 *  this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Threads_Information;

/**
 *  This variable contains the default POSIX Thread attributes.
 */
extern const pthread_attr_t _POSIX_Threads_Default_attributes;

/**
 *  When the user configures a set of POSIX API initialization threads,
 *  This variable will point to the method used to initialize them.
 *
 *  @note It is instantiated and initialized by confdefs.h based upon
 *        application requirements.
 */
extern void (*_POSIX_Threads_Initialize_user_threads_p)(void);

/**
 *  @brief _POSIX_Threads_Manager_initialization
 *
 *  This routine performs the initialization necessary for this manager.
 */
void _POSIX_Threads_Manager_initialization(void);

/**
 *  @brief _POSIX_Threads_Allocate
 *
 *  This function allocates a pthread control block from
 *  the inactive chain of free pthread control blocks.
 *
 *  @return This method returns a newly allocated thread.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Allocate( void );

/**
 *  @brief _POSIX_Threads_Free
 *
 *  This routine frees a pthread control block to the
 *  inactive chain of free pthread control blocks.
 *
 *  @param[in] the_pthread is the thread to free
 */
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Free(
  Thread_Control *the_pthread
);

/**
 *  @brief _POSIX_Threads_Get
 *
 *  This function maps pthread IDs to pthread control blocks.
 *  If ID corresponds to a local pthread, then it returns
 *  the_pthread control pointer which maps to ID and location
 *  is set to OBJECTS_LOCAL.  if the pthread ID is global and
 *  resides on a remote node, then location is set to OBJECTS_REMOTE,
 *  and the_pthread is undefined.  Otherwise, location is set
 *  to OBJECTS_ERROR and the_pthread is undefined.
 *
 *  @param[in] id is the id to lookup
 *  @param[in] location points to the returned location value
 *
 *  @return This methods returns a pointer to the corresponding Thread_Control.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Get(
  pthread_t          id,
  Objects_Locations *location
);

/**
 *  @brief _POSIX_Threads_Is_null
 *
 *  This function returns TRUE if the_pthread is NULL and FALSE otherwise.
 *
 *  @param[in] the_pthread is the thread pointer to check.
 *
 *  @return This method returns true if the thread pointer is null.
 */
RTEMS_INLINE_ROUTINE bool _POSIX_Threads_Is_null(
  Thread_Control *the_pthread
);

/**
 *  @brief _POSIX_Threads_Sporadic_budget_callout
 *
 *  This routine handles the sporadic scheduling algorithm.
 *
 *  @param[in] the_thread is the thread whose budget has been exceeded.
 */
void _POSIX_Threads_Sporadic_budget_callout(
  Thread_Control *the_thread
);

/**
 *  _POSIX_Threads_Sporadic_budget_TSR
 *
 *  This routine supports the sporadic scheduling algorithm.  It
 *  is scheduled to be executed at the end of each replenishment
 *  period.  In sporadic scheduling a thread will execute at a
 *  high priority for a user specified amount of CPU time.  When
 *  it exceeds that amount of CPU time, its priority is automatically
 *  lowered. This TSR is executed when it is time to replenish
 *  the thread's processor budget and raise its priority.
 *
 *  @param[in] id is ignored
 *  @param[in] argument is a pointer to the Thread_Control structure
 *             for the thread being replenished.
 */
void _POSIX_Threads_Sporadic_budget_TSR(
  Objects_Id      id,
  void           *argument
);

/**
 *  @brief Translate sched_param into SuperCore Terms
 *
 *  This method translates the POSIX API sched_param into the corresponding
 *  SuperCore settings.
 *
 *  @param[in] policy is the POSIX scheduling policy
 *  @param[in] param points to the scheduling parameter structure
 *  @param[in] budget_algorithm points to the output CPU Budget algorithm
 *  @param[in] budget_callout points to the output CPU Callout
 *
 *  @return This method returns 0 on success or a POSIX error code.
 */
int _POSIX_Thread_Translate_sched_param(
  int                                  policy,
  struct sched_param                  *param,
  Thread_CPU_budget_algorithms        *budget_algorithm,
  Thread_CPU_budget_algorithm_callout *budget_callout
);

/**
 *  @brief  _POSIX_Threads_Initialize_user_threads_body
 *
 *  This routine creates and starts all configured user
 *  initialization threads.
 */
extern void _POSIX_Threads_Initialize_user_threads_body(void);

#include <rtems/posix/pthread.inl>

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
