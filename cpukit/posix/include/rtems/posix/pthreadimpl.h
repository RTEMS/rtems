/**
 * @file
 *
 * @brief POSIX Threads Private Support
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
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_POSIX_PTHREADIMPL_H
#define _RTEMS_POSIX_PTHREADIMPL_H

#include <rtems/posix/pthread.h>
#include <rtems/posix/config.h>
#include <rtems/posix/threadsup.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup POSIX_PTHREAD
 */
/**@{**/

/**
 * The following sets the minimum stack size for POSIX threads.
 */
#define PTHREAD_MINIMUM_STACK_SIZE (_Stack_Minimum() * 2)

/**
 * The following defines the information control block used to manage
 * this class of objects.
 */
POSIX_EXTERN Objects_Information  _POSIX_Threads_Information;

/**
 * This variable contains the default POSIX Thread attributes.
 */
extern pthread_attr_t _POSIX_Threads_Default_attributes;

/**
 * When the user configures a set of POSIX API initialization threads,
 * This variable will point to the method used to initialize them.
 *
 * NOTE: It is instantiated and initialized by confdefs.h based upon
 *       application requirements.
 */
extern void (*_POSIX_Threads_Initialize_user_threads_p)(void);

/**
 * @brief POSIX threads manager initialization.
 *
 * This routine performs the initialization necessary for this manager.
 */
void _POSIX_Threads_Manager_initialization(void);

/**
 * @brief Copy POSIX Thread attribute structure.
 *
 * This routine copies the attr2 thread attribute structure
 * to the attr1 Thread Attribute structure.
 *
 * @param[in] dst_attr is a pointer to the thread attribute
 * structure to copy into.
 *
 * @param[out] src_attr is a pointer to the thread attribute
 * structure to copy from.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Copy_attributes(
  pthread_attr_t        *dst_attr,
  const pthread_attr_t  *src_attr
);

/**
 * @brief Free POSIX control block.
 *
 * This routine frees a pthread control block to the
 * inactive chain of free pthread control blocks.
 *
 * @param[in] the_pthread is a pointer to the thread to free.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Free(
  Thread_Control *the_pthread
);

/**
 * @brief POSIX threads initialize user threads body.
 *
 * This routine initializes the thread attributes structure.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Threads_Initialize_attributes(
  pthread_attr_t  *attr
);

/**
 * @brief POSIX threads sporadic budget callout.
 *
 * This routine handles the sporadic scheduling algorithm.
 *
 * @param[in] the_thread is a pointer to the thread whose budget
 * has been exceeded.
 */
void _POSIX_Threads_Sporadic_budget_callout(
  Thread_Control *the_thread
);

/**
 * This routine supports the sporadic scheduling algorithm.  It
 * is scheduled to be executed at the end of each replenishment
 * period.  In sporadic scheduling a thread will execute at a
 * high priority for a user specified amount of CPU time.  When
 * it exceeds that amount of CPU time, its priority is automatically
 * lowered. This TSR is executed when it is time to replenish
 * the thread's processor budget and raise its priority.
 *
 * @param[in] id is ignored
 * @param[in] argument is a pointer to the Thread_Control structure
 *            for the thread being replenished.
 */
void _POSIX_Threads_Sporadic_budget_TSR(
  Objects_Id      id,
  void           *argument
);

/**
 * @brief Translate sched_param into SuperCore terms.
 *
 * This method translates the POSIX API sched_param into the corresponding
 * SuperCore settings.
 *
 * @param[in] policy is the POSIX scheduling policy
 * @param[in] param points to the scheduling parameter structure
 * @param[in] budget_algorithm points to the output CPU Budget algorithm
 * @param[in] budget_callout points to the output CPU Callout
 *
 * @retval 0 Indicates success.
 * @retval error_code POSIX error code indicating failure.
 */
int _POSIX_Thread_Translate_sched_param(
  int                                  policy,
  struct sched_param                  *param,
  Thread_CPU_budget_algorithms        *budget_algorithm,
  Thread_CPU_budget_algorithm_callout *budget_callout
);

/*
 * rtems_pthread_attribute_compare
 */
int rtems_pthread_attribute_compare(
  const pthread_attr_t *attr1,
  const pthread_attr_t *attr2
);

RTEMS_INLINE_ROUTINE Thread_Control *_POSIX_Threads_Allocate(void)
{
  _Objects_Allocator_lock();

  _Thread_Kill_zombies();

  return (Thread_Control *)
    _Objects_Allocate_unprotected( &_POSIX_Threads_Information );
}

/*
 * _POSIX_Threads_Copy_attributes
 */

RTEMS_INLINE_ROUTINE void _POSIX_Threads_Copy_attributes(
  pthread_attr_t        *dst_attr,
  const pthread_attr_t  *src_attr
)
{
  *dst_attr = *src_attr;
#if defined(RTEMS_SMP) && defined(__RTEMS_HAVE_SYS_CPUSET_H__)
  _Assert(
    dst_attr->affinitysetsize == sizeof(dst_attr->affinitysetpreallocated)
  );
  dst_attr->affinityset = &dst_attr->affinitysetpreallocated;
#endif
}

/*
 *  _POSIX_Threads_Free
 */

RTEMS_INLINE_ROUTINE void _POSIX_Threads_Free (
  Thread_Control *the_pthread
)
{
  _Objects_Free( &_POSIX_Threads_Information, &the_pthread->Object );
}

/*
 * _POSIX_Threads_Initialize_attributes
 */

RTEMS_INLINE_ROUTINE void _POSIX_Threads_Initialize_attributes(
  pthread_attr_t  *attr
)
{
  _POSIX_Threads_Copy_attributes(
    attr,
    &_POSIX_Threads_Default_attributes
  );
}

/*
 *  _POSIX_Threads_Is_null
 */

RTEMS_INLINE_ROUTINE bool _POSIX_Threads_Is_null (
  Thread_Control *the_pthread
)
{
  return !the_pthread;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
