/**
 * @file
 *
 * @ingroup RTEMSScoreMutex
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreMutex which are only used by the implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_COREMUTEXIMPL_H
#define _RTEMS_SCORE_COREMUTEXIMPL_H

#include <rtems/score/coremutex.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreMutex
 *
 * @{
 */

#define CORE_MUTEX_TQ_OPERATIONS &_Thread_queue_Operations_priority

#define CORE_MUTEX_TQ_PRIORITY_INHERIT_OPERATIONS \
  &_Thread_queue_Operations_priority_inherit

/**
 * @brief Initializes the mutex.
 *
 * @param[out] the_mutex The mutex to initialize.
 */
RTEMS_INLINE_ROUTINE void _CORE_mutex_Initialize(
  CORE_mutex_Control *the_mutex
)
{
  _Thread_queue_Object_initialize( &the_mutex->Wait_queue );
}

/**
 * @brief Destroys the mutex.
 *
 * @param[out] the_mutex the mutex to destroy.
 */
RTEMS_INLINE_ROUTINE void _CORE_mutex_Destroy( CORE_mutex_Control *the_mutex )
{
  _Thread_queue_Destroy( &the_mutex->Wait_queue );
}

/**
 * @brief Acquires the mutex critical.
 *
 * @param[in, out] the_mutex The mutex to acquire critical.
 * @param queue_context The queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_mutex_Acquire_critical(
  CORE_mutex_Control   *the_mutex,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Acquire_critical( &the_mutex->Wait_queue, queue_context );
}

/**
 * @brief Releases the mutex.
 *
 * @param[in, out] the_mutex The mutex to release.
 * @param queue_context The queue context.
 */
RTEMS_INLINE_ROUTINE void _CORE_mutex_Release(
  CORE_mutex_Control   *the_mutex,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Release( &the_mutex->Wait_queue, queue_context );
}

/**
 * @brief Gets the owner of the mutex.
 *
 * @param the_mutex The mutex to get the owner from.
 *
 * @return The owner of the mutex.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_CORE_mutex_Get_owner(
  const CORE_mutex_Control *the_mutex
)
{
  return the_mutex->Wait_queue.Queue.owner;
}

/**
 * @brief Checks if the mutex is locked.
 *
 * This routine returns true if the specified mutex is locked and false
 * otherwise.
 *
 * @param the_mutex The mutex to check if it is locked.
 *
 * @retval true The mutex is locked.
 * @retval false The mutex is not locked.
 */
RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_locked(
  const CORE_mutex_Control *the_mutex
)
{
  return _CORE_mutex_Get_owner( the_mutex ) != NULL;
}

/**
 * @brief Seize the mutex slowly.
 *
 * @param[in, out] the_mutex The mutex to seize.
 * @param operations The thread queue operations.
 * @param executing The calling thread.
 * @param wait Indicates whether the calling thread is willing to wait.
 * @param queue_context The thread queue context.
 *
 * @retval _Thread_Wait_get_status The status of the executing thread.
 * @retval STATUS_UNAVAILABLE The calling thread is not willing to wait.
 */
Status_Control _CORE_mutex_Seize_slow(
  CORE_mutex_Control            *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  bool                           wait,
  Thread_queue_Context          *queue_context
);

/**
 * @brief Sets the owner of the mutex.
 *
 * @param[out] the_mutex The mutex to set the owner from.
 * @param owner The new owner of the mutex.
 */
RTEMS_INLINE_ROUTINE void _CORE_mutex_Set_owner(
  CORE_mutex_Control *the_mutex,
  Thread_Control     *owner
)
{
  the_mutex->Wait_queue.Queue.owner = owner;
}

/**
 * @brief Checks if the the thread is the owner of the mutex.
 *
 * @param the_mutex The mutex to check the owner of.
 * @param the_thread The thread to check if it is the owner of @a the_mutex.
 *
 * @retval true @a the_thread is the owner of @a the_mutex.
 * @retval false @a the_thread is not the owner of @a the_mutex.
 */
RTEMS_INLINE_ROUTINE bool _CORE_mutex_Is_owner(
  const CORE_mutex_Control *the_mutex,
  const Thread_Control     *the_thread
)
{
  return _CORE_mutex_Get_owner( the_mutex ) == the_thread;
}

/**
 * @brief Initializes a recursive mutex.
 *
 * @param[out] the_mutex The recursive mutex to initialize.
 */
RTEMS_INLINE_ROUTINE void _CORE_recursive_mutex_Initialize(
  CORE_recursive_mutex_Control *the_mutex
)
{
  _CORE_mutex_Initialize( &the_mutex->Mutex );
  the_mutex->nest_level = 0;
}

/**
 * @brief Seizes the recursive mutex nested.
 *
 * @param[out] the_mutex The recursive mutex to seize nested.
 *
 * @return STATUS_SUCCESSFUL, this method is always successful.
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_recursive_mutex_Seize_nested(
  CORE_recursive_mutex_Control *the_mutex
)
{
  ++the_mutex->nest_level;
  return STATUS_SUCCESSFUL;
}

/**
 * @brief Seizes the recursive mutex.
 *
 * @param[in, out] the_mutex The recursive mutex to seize.
 * @param operations The thread queue operations.
 * @param[out] executing The executing thread.
 * @param wait Indicates whether the calling thread is willing to wait.
 * @param nested Returns the status of a recursive mutex.
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The owner of the mutex was NULL, successful
 *      seizing of the mutex.
 * @retval _Thread_Wait_get_status The status of the executing thread.
 * @retval STATUS_UNAVAILABLE The calling thread is not willing to wait.
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_recursive_mutex_Seize(
  CORE_recursive_mutex_Control  *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  bool                           wait,
  Status_Control              ( *nested )( CORE_recursive_mutex_Control * ),
  Thread_queue_Context          *queue_context
)
{
  Thread_Control *owner;

  _CORE_mutex_Acquire_critical( &the_mutex->Mutex, queue_context );

  owner = _CORE_mutex_Get_owner( &the_mutex->Mutex );

  if ( owner == NULL ) {
    _CORE_mutex_Set_owner( &the_mutex->Mutex, executing );
    _Thread_Resource_count_increment( executing );
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  if ( owner == executing ) {
    Status_Control status;

    status = ( *nested )( the_mutex );
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return status;
  }

  return _CORE_mutex_Seize_slow(
    &the_mutex->Mutex,
    operations,
    executing,
    wait,
    queue_context
  );
}

/**
 * @brief Surrenders the recursive mutex.
 *
 * @param[in, out] the_mutex The recursive mutex to surrender.
 * @param operations The thread queue operations.
 * @param executing The executing thread.
 * @param queue_context the thread queue context.
 *
 * @retval STATUS_SUCCESSFUL @a the_mutex is successfully surrendered.
 * @retval STATUS_NOT_OWNER The executing thread does not own @a the_mutex.
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_recursive_mutex_Surrender(
  CORE_recursive_mutex_Control  *the_mutex,
  const Thread_queue_Operations *operations,
  Thread_Control                *executing,
  Thread_queue_Context          *queue_context
)
{
  unsigned int        nest_level;
  Thread_queue_Heads *heads;

  _CORE_mutex_Acquire_critical( &the_mutex->Mutex, queue_context );

  if ( !_CORE_mutex_Is_owner( &the_mutex->Mutex, executing ) ) {
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return STATUS_NOT_OWNER;
  }

  nest_level = the_mutex->nest_level;

  if ( nest_level > 0 ) {
    the_mutex->nest_level = nest_level - 1;
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  _Thread_Resource_count_decrement( executing );
  _CORE_mutex_Set_owner( &the_mutex->Mutex, NULL );

  heads = the_mutex->Mutex.Wait_queue.Queue.heads;

  if ( heads == NULL ) {
    _CORE_mutex_Release( &the_mutex->Mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  _Thread_queue_Surrender(
    &the_mutex->Mutex.Wait_queue.Queue,
    heads,
    executing,
    queue_context,
    operations
  );
  return STATUS_SUCCESSFUL;
}

/**
 * @brief initializes a ceiling mutex.
 *
 * @param[out] the_mutex The ceiling mutex to initialize.
 * @param scheduler The scheduler for the new ceiling mutex.
 *      Only needed if RTEMS_SMP is defined
 * @param priority_ceiling The priority ceiling for the initialized mutex.
 */
RTEMS_INLINE_ROUTINE void _CORE_ceiling_mutex_Initialize(
  CORE_ceiling_mutex_Control *the_mutex,
  const Scheduler_Control    *scheduler,
  Priority_Control            priority_ceiling
)
{
  _CORE_recursive_mutex_Initialize( &the_mutex->Recursive );
  _Priority_Node_initialize( &the_mutex->Priority_ceiling, priority_ceiling );
#if defined(RTEMS_SMP)
  the_mutex->scheduler = scheduler;
#endif
}

/**
 * @brief Gets the scheduler of the ceiling mutex.
 *
 * @param the_mutex The ceiling mutex to get the scheduler from.
 *
 * @return The scheduler of the mutex. If RTEMS_SMP is not defined, the first entry of the _Scheduler_Table is returned.
 */
RTEMS_INLINE_ROUTINE const Scheduler_Control *
_CORE_ceiling_mutex_Get_scheduler(
  const CORE_ceiling_mutex_Control *the_mutex
)
{
#if defined(RTEMS_SMP)
  return the_mutex->scheduler;
#else
  return &_Scheduler_Table[ 0 ];
#endif
}

/**
 * @brief Sets the priority of the ceiling mutex.
 *
 * @param[out] the_mutex The ceiling mutex to set the priority of.
 * @param priority_ceiling The new priority ceiling of the mutex.
 */
RTEMS_INLINE_ROUTINE void _CORE_ceiling_mutex_Set_priority(
  CORE_ceiling_mutex_Control *the_mutex,
  Priority_Control            priority_ceiling
)
{
  Thread_Control *owner;

  owner = _CORE_mutex_Get_owner( &the_mutex->Recursive.Mutex );

  if ( owner != NULL ) {
    Thread_queue_Context queue_context;

    _Thread_queue_Context_initialize( &queue_context );
    _Thread_queue_Context_clear_priority_updates( &queue_context );
    _Thread_Wait_acquire_critical( owner, &queue_context );
    _Thread_Priority_change(
      owner,
      &the_mutex->Priority_ceiling,
      priority_ceiling,
      PRIORITY_GROUP_LAST,
      &queue_context
    );
    _Thread_Wait_release_critical( owner, &queue_context );
  } else {
    the_mutex->Priority_ceiling.priority = priority_ceiling;
  }
}

/**
 * @brief Gets the priority of the ceiling mutex.
 *
 * @param the_mutex The mutex to get the priority from.
 *
 * @return The priority ceiling of @a the_mutex.
 */
RTEMS_INLINE_ROUTINE Priority_Control _CORE_ceiling_mutex_Get_priority(
  const CORE_ceiling_mutex_Control *the_mutex
)
{
  return the_mutex->Priority_ceiling.priority;
}

/**
 * @brief Sets the owner of the ceiling mutex.
 *
 * @param[in, out] the_mutex The mutex to set the owner of.
 * @param owner The new owner of @a the_mutex.
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The owner of the mutex was changed successfully.
 * @retval STATUS_MUTEX_CEILING_VIOLATED The owners wait priority
 *          is smaller than the priority of the ceiling mutex.
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_ceiling_mutex_Set_owner(
  CORE_ceiling_mutex_Control *the_mutex,
  Thread_Control             *owner,
  Thread_queue_Context       *queue_context
)
{
  ISR_lock_Context  lock_context;
  Scheduler_Node   *scheduler_node;
  Per_CPU_Control  *cpu_self;

  _Thread_Wait_acquire_default_critical( owner, &lock_context );

  scheduler_node = _Thread_Scheduler_get_home_node( owner );

  if (
    _Priority_Get_priority( &scheduler_node->Wait.Priority )
      < the_mutex->Priority_ceiling.priority
  ) {
    _Thread_Wait_release_default_critical( owner, &lock_context );
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return STATUS_MUTEX_CEILING_VIOLATED;
  }

  _CORE_mutex_Set_owner( &the_mutex->Recursive.Mutex, owner );
  _Thread_Resource_count_increment( owner );
  _Thread_Priority_add(
    owner,
    &the_mutex->Priority_ceiling,
    queue_context
  );
  _Thread_Wait_release_default_critical( owner, &lock_context );

  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
  _Thread_Priority_update( queue_context );
  _Thread_Dispatch_enable( cpu_self );
  return STATUS_SUCCESSFUL;
}

/**
 * @brief Seizes the ceiling mutex.
 *
 * @param[in, out] the_mutex The mutex to seize.
 * @param executing The executing thread.
 * @param wait Indicates whether the calling thread is willing to wait.
 * @param nested Function that returns the status of the recursive mutex
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The owner of the mutex was changed successfully.
 * @retval STATUS_NOT_DEFINED If the scheduler of the executing thread is not equal to the owner of @a the_mutex .
 * @retval STATUS_MUTEX_CEILING_VIOLATED The owners wait priority
 *          is smaller than the priority of the ceiling mutex.
 * @retval other Return value of @a nested.
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_ceiling_mutex_Seize(
  CORE_ceiling_mutex_Control    *the_mutex,
  Thread_Control                *executing,
  bool                           wait,
  Status_Control              ( *nested )( CORE_recursive_mutex_Control * ),
  Thread_queue_Context          *queue_context
)
{
  Thread_Control *owner;

  _CORE_mutex_Acquire_critical( &the_mutex->Recursive.Mutex, queue_context );

#if defined(RTEMS_SMP)
  if (
    _Thread_Scheduler_get_home( executing )
      != _CORE_ceiling_mutex_Get_scheduler( the_mutex )
  ) {
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return STATUS_NOT_DEFINED;
  }
#endif

  owner = _CORE_mutex_Get_owner( &the_mutex->Recursive.Mutex );

  if ( owner == NULL ) {
    _Thread_queue_Context_clear_priority_updates( queue_context );
    return _CORE_ceiling_mutex_Set_owner(
      the_mutex,
      executing,
      queue_context
    );
  }

  if ( owner == executing ) {
    Status_Control status;

    status = ( *nested )( &the_mutex->Recursive );
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return status;
  }

  return _CORE_mutex_Seize_slow(
    &the_mutex->Recursive.Mutex,
    CORE_MUTEX_TQ_OPERATIONS,
    executing,
    wait,
    queue_context
  );
}

/**
 * @brief Surrenders the ceiling mutex.
 *
 * @param[in, out] the_mutex The ceiling mutex to surrender.
 * @param executing The executing thread.
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The ceiling mutex was successfullysurrendered.
 * @retval STATUS_NOT_OWNER The executing thread is not the owner of @a the_mutex.
 */
RTEMS_INLINE_ROUTINE Status_Control _CORE_ceiling_mutex_Surrender(
  CORE_ceiling_mutex_Control *the_mutex,
  Thread_Control             *executing,
  Thread_queue_Context       *queue_context
)
{
  unsigned int nest_level;

  _CORE_mutex_Acquire_critical( &the_mutex->Recursive.Mutex, queue_context );

  if ( !_CORE_mutex_Is_owner( &the_mutex->Recursive.Mutex, executing ) ) {
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return STATUS_NOT_OWNER;
  }

  nest_level = the_mutex->Recursive.nest_level;

  if ( nest_level > 0 ) {
    the_mutex->Recursive.nest_level = nest_level - 1;
    _CORE_mutex_Release( &the_mutex->Recursive.Mutex, queue_context );
    return STATUS_SUCCESSFUL;
  }

  return _Thread_queue_Surrender_priority_ceiling(
    &the_mutex->Recursive.Mutex.Wait_queue.Queue,
    executing,
    &the_mutex->Priority_ceiling,
    queue_context,
    CORE_MUTEX_TQ_OPERATIONS
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
