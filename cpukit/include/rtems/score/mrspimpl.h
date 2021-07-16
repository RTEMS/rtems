/**
 * @file
 *
 * @ingroup RTEMSScoreMRSP
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreMRSP which are only used by the implementation.
 */

/*
 * Copyright (c) 2014, 2019 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_MRSPIMPL_H
#define _RTEMS_SCORE_MRSPIMPL_H

#include <rtems/score/mrsp.h>

#if defined(RTEMS_SMP)

#include <rtems/score/assert.h>
#include <rtems/score/status.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdogimpl.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @addtogroup RTEMSScoreMRSP
 *
 * @{
 */

#define MRSP_TQ_OPERATIONS &_Thread_queue_Operations_priority_inherit

/**
 * @brief Acquires critical accordingt to MrsP.
 *
 * @param mrsp The MrsP control for the operation.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _MRSP_Acquire_critical(
  MRSP_Control         *mrsp,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Acquire_critical( &mrsp->Wait_queue, queue_context );
}

/**
 * @brief Releases according to MrsP.
 *
 * @param mrsp The MrsP control for the operation.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _MRSP_Release(
  MRSP_Control         *mrsp,
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Release( &mrsp->Wait_queue, queue_context );
}

/**
 * @brief Gets owner of the MrsP control.
 *
 * @param mrsp The MrsP control to get the owner from.
 *
 * @return The owner of the Mrsp control.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_MRSP_Get_owner(
  const MRSP_Control *mrsp
)
{
  return mrsp->Wait_queue.Queue.owner;
}

/**
 * @brief Sets owner of the MrsP control.
 *
 * @param[out] mrsp The MrsP control to set the owner of.
 * @param owner The desired new owner for @a mrsp.
 */
RTEMS_INLINE_ROUTINE void _MRSP_Set_owner(
  MRSP_Control   *mrsp,
  Thread_Control *owner
)
{
  mrsp->Wait_queue.Queue.owner = owner;
}

/**
 * @brief Gets priority of the MrsP control.
 *
 * @param mrsp The mrsp to get the priority from.
 * @param scheduler The corresponding scheduler.
 *
 * @return The priority of the MrsP control.
 */
RTEMS_INLINE_ROUTINE Priority_Control _MRSP_Get_priority(
  const MRSP_Control      *mrsp,
  const Scheduler_Control *scheduler
)
{
  uint32_t scheduler_index;

  scheduler_index = _Scheduler_Get_index( scheduler );
  return mrsp->ceiling_priorities[ scheduler_index ];
}

/**
 * @brief Sets priority of the MrsP control
 *
 * @param[out] mrsp The MrsP control to set the priority of.
 * @param schedulger The corresponding scheduler.
 * @param new_priority The new priority for the MrsP control
 */
RTEMS_INLINE_ROUTINE void _MRSP_Set_priority(
  MRSP_Control            *mrsp,
  const Scheduler_Control *scheduler,
  Priority_Control         new_priority
)
{
  uint32_t scheduler_index;

  scheduler_index = _Scheduler_Get_index( scheduler );
  mrsp->ceiling_priorities[ scheduler_index ] = new_priority;
}

/**
 * @brief Adds the priority to the given thread.
 *
 * @param mrsp The MrsP control for the operation.
 * @param[in, out] thread The thread to add the priority node to.
 * @param[out] priority_node The priority node to initialize and add to
 *      the thread.
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 * @retval STATUS_MUTEX_CEILING_VIOLATED The wait priority of the thread
 *      exceeds the ceiling priority.
 */
RTEMS_INLINE_ROUTINE Status_Control _MRSP_Raise_priority(
  MRSP_Control         *mrsp,
  Thread_Control       *thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
)
{
  Status_Control           status;
  ISR_lock_Context         lock_context;
  const Scheduler_Control *scheduler;
  Priority_Control         ceiling_priority;
  Scheduler_Node          *scheduler_node;

  _Thread_queue_Context_clear_priority_updates( queue_context );
  _Thread_Wait_acquire_default_critical( thread, &lock_context );

  scheduler = _Thread_Scheduler_get_home( thread );
  scheduler_node = _Thread_Scheduler_get_home_node( thread );
  ceiling_priority = _MRSP_Get_priority( mrsp, scheduler );

  if (
    ceiling_priority
      <= _Priority_Get_priority( &scheduler_node->Wait.Priority )
  ) {
    _Priority_Node_initialize( priority_node, ceiling_priority );
    _Thread_Priority_add( thread, priority_node, queue_context );
    status = STATUS_SUCCESSFUL;
  } else {
    status = STATUS_MUTEX_CEILING_VIOLATED;
  }

  _Thread_Wait_release_default_critical( thread, &lock_context );
  return status;
}

/**
 * @brief Removes the priority from the given thread.
 *
 * @param[in, out] The thread to remove the priority from.
 * @param priority_node The priority node to remove from the thread
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _MRSP_Remove_priority(
  Thread_Control       *thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
)
{
  ISR_lock_Context lock_context;

  _Thread_queue_Context_clear_priority_updates( queue_context );
  _Thread_Wait_acquire_default_critical( thread, &lock_context );
  _Thread_Priority_remove( thread, priority_node, queue_context );
  _Thread_Wait_release_default_critical( thread, &lock_context );
}

/**
 * @brief Replaces the given priority node with the ceiling priority of
 *      the MrsP control.
 *
 * @param mrsp The mrsp control for the operation.
 * @param[out] thread The thread to replace the priorities.
 * @param ceiling_priority The node to be replaced.
 */
RTEMS_INLINE_ROUTINE void _MRSP_Replace_priority(
  MRSP_Control   *mrsp,
  Thread_Control *thread,
  Priority_Node  *ceiling_priority
)
{
  ISR_lock_Context lock_context;

  _Thread_Wait_acquire_default( thread, &lock_context );
  _Thread_Priority_replace( 
    thread,
    ceiling_priority,
    &mrsp->Ceiling_priority
  );
  _Thread_Wait_release_default( thread, &lock_context );
}

/**
 * @brief Claims ownership of the MrsP control.
 *
 * @param mrsp The MrsP control to claim the ownership of.
 * @param[in, out] executing The currently executing thread.
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 * @retval STATUS_MUTEX_CEILING_VIOLATED The wait priority of the executing
 *      thread exceeds the ceiling priority.
 */
RTEMS_INLINE_ROUTINE Status_Control _MRSP_Claim_ownership(
  MRSP_Control         *mrsp,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  Status_Control   status;
  Per_CPU_Control *cpu_self;

  status = _MRSP_Raise_priority(
    mrsp,
    executing,
    &mrsp->Ceiling_priority,
    queue_context
  );

  if ( status != STATUS_SUCCESSFUL ) {
    _MRSP_Release( mrsp, queue_context );
    return status;
  }

  _MRSP_Set_owner( mrsp, executing );
  cpu_self = _Thread_queue_Dispatch_disable( queue_context );
  _MRSP_Release( mrsp, queue_context );
  _Thread_Priority_and_sticky_update( executing, 1 );
  _Thread_Dispatch_enable( cpu_self );
  return STATUS_SUCCESSFUL;
}

/**
 * @brief Initializes a MrsP control.
 *
 * @param[out] mrsp The MrsP control that is initialized.
 * @param scheduler The scheduler for the operation.
 * @param ceiling_priority
 * @param executing The currently executing thread.  Ignored in this method.
 * @param initially_locked Indicates whether the MrsP control shall be initally
 *      locked. If it is initially locked, this method returns STATUS_INVALID_NUMBER.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 * @retval STATUS_INVALID_NUMBER The MrsP control is initially locked.
 * @retval STATUS_NO_MEMORY There is not enough memory to allocate.
 */
RTEMS_INLINE_ROUTINE Status_Control _MRSP_Initialize(
  MRSP_Control            *mrsp,
  const Scheduler_Control *scheduler,
  Priority_Control         ceiling_priority,
  Thread_Control          *executing,
  bool                     initially_locked
)
{
  Thread_queue_Context queue_context;
  ISR_Level            level;
  size_t               scheduler_count;
  size_t               i;
  Status_Control       status;

  scheduler_count = _Scheduler_Count;

  for ( i = 0 ; i < scheduler_count ; ++i ) {
    const Scheduler_Control *scheduler_of_index;

    scheduler_of_index = &_Scheduler_Table[ i ];

    if ( scheduler != scheduler_of_index ) {
      mrsp->ceiling_priorities[ i ] =
        _Scheduler_Map_priority( scheduler_of_index, 0 );
    } else {
      mrsp->ceiling_priorities[ i ] = ceiling_priority;
    }
  }

  _Thread_queue_Object_initialize( &mrsp->Wait_queue );

  if ( !initially_locked ) {
    return STATUS_SUCCESSFUL;
  }

  _Thread_queue_Context_initialize( &queue_context );
  _Thread_queue_Context_ISR_disable( &queue_context, level );
  _Thread_queue_Context_set_ISR_level( &queue_context, level );
  _MRSP_Acquire_critical( mrsp, &queue_context );
  status = _MRSP_Claim_ownership( mrsp, executing, &queue_context );

  if ( status != STATUS_SUCCESSFUL ) {
    _Thread_queue_Destroy( &mrsp->Wait_queue );
  }

  return status;
}

/**
 * @brief Waits for the ownership of the MrsP control.
 *
 * @param[in, out] mrsp The MrsP control to get the ownership of.
 * @param[in, out] executing The currently executing thread.
 * @param queue_context the thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 * @retval STATUS_MUTEX_CEILING_VIOLATED The wait priority of the
 *      currently executing thread exceeds the ceiling priority.
 * @retval STATUS_DEADLOCK A deadlock occurred.
 * @retval STATUS_TIMEOUT A timeout occurred.
 */
RTEMS_INLINE_ROUTINE Status_Control _MRSP_Wait_for_ownership(
  MRSP_Control         *mrsp,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  Status_Control status;
  Priority_Node  ceiling_priority;

  status = _MRSP_Raise_priority(
    mrsp,
    executing,
    &ceiling_priority,
    queue_context
  );

  if ( status != STATUS_SUCCESSFUL ) {
    _MRSP_Release( mrsp, queue_context );
    return status;
  }

  _Thread_queue_Context_set_deadlock_callout(
    queue_context,
    _Thread_queue_Deadlock_status
  );
  status = _Thread_queue_Enqueue_sticky(
    &mrsp->Wait_queue.Queue,
    MRSP_TQ_OPERATIONS,
    executing,
    queue_context
  );

  if ( status == STATUS_SUCCESSFUL ) {
    _MRSP_Replace_priority( mrsp, executing, &ceiling_priority );
  } else {
    Per_CPU_Control *cpu_self;
    int              sticky_level_change;

    if ( status != STATUS_DEADLOCK ) {
      sticky_level_change = -1;
    } else {
      sticky_level_change = 0;
    }

    _ISR_lock_ISR_disable( &queue_context->Lock_context.Lock_context );
    _MRSP_Remove_priority( executing, &ceiling_priority, queue_context );
    cpu_self = _Thread_Dispatch_disable_critical(
      &queue_context->Lock_context.Lock_context
    );
    _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
    _Thread_Priority_and_sticky_update( executing, sticky_level_change );
    _Thread_Dispatch_enable( cpu_self );
  }

  return status;
}

/**
 * @brief Seizes the MrsP mutex.
 *
 * @param[in, out] mrsp is the MrsP mutex to seize.
 *
 * @param[in, out] executing is the currently executing thread.
 *
 * @param wait shall be true, if the executing thread is willing to wait,
 *   otherwise it shall be false.
 *
 * @param[in, out] queue_context is the thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The requested operation was successful.
 *
 * @retval STATUS_UNAVAILABLE Seizing the mutex was not immmediately possible.
 *
 * @retval STATUS_DEADLOCK The executing thread was already the owner of
 *   the mutex.
 *
 * @retval STATUS_MUTEX_CEILING_VIOLATED The current priority of the executing
 *   thread exceeds the ceiling priority of the mutex.
 */
RTEMS_INLINE_ROUTINE Status_Control _MRSP_Seize(
  MRSP_Control         *mrsp,
  Thread_Control       *executing,
  bool                  wait,
  Thread_queue_Context *queue_context
)
{
  Status_Control  status;
  Thread_Control *owner;

  _MRSP_Acquire_critical( mrsp, queue_context );

  owner = _MRSP_Get_owner( mrsp );

  if ( owner == NULL ) {
    status = _MRSP_Claim_ownership( mrsp, executing, queue_context );
  } else if ( owner == executing ) {
    _MRSP_Release( mrsp, queue_context );
    status = STATUS_DEADLOCK;
  } else if ( wait ) {
    status = _MRSP_Wait_for_ownership( mrsp, executing, queue_context );
  } else {
    _MRSP_Release( mrsp, queue_context );
    status = STATUS_UNAVAILABLE;
  }

  return status;
}

/**
 * @brief Surrenders the MrsP control.
 *
 * @param[in, out] mrsp The MrsP control to surrender the control of.
 * @param[in, out] executing The currently executing thread.
 * @param queue_context The thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 * @retval STATUS_NOT_OWNER The executing thread does not own the MrsP control.
 */
RTEMS_INLINE_ROUTINE Status_Control _MRSP_Surrender(
  MRSP_Control         *mrsp,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
)
{
  Thread_queue_Heads *heads;

  if ( _MRSP_Get_owner( mrsp ) != executing ) {
    _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
    return STATUS_NOT_OWNER;
  }

  _MRSP_Acquire_critical( mrsp, queue_context );

  _MRSP_Set_owner( mrsp, NULL );
  _MRSP_Remove_priority( executing, &mrsp->Ceiling_priority, queue_context );

  heads = mrsp->Wait_queue.Queue.heads;

  if ( heads == NULL ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical(
      &queue_context->Lock_context.Lock_context
    );
    _MRSP_Release( mrsp, queue_context );
    _Thread_Priority_and_sticky_update( executing, -1 );
    _Thread_Dispatch_enable( cpu_self );
    return STATUS_SUCCESSFUL;
  }

  _Thread_queue_Surrender_sticky(
    &mrsp->Wait_queue.Queue,
    heads,
    executing,
    queue_context,
    MRSP_TQ_OPERATIONS
  );
  return STATUS_SUCCESSFUL;
}

/**
 * @brief Checks if the MrsP control can be destroyed.
 *
 * @param mrsp The MrsP control for the operation.
 *
 * @retval STATUS_SUCCESSFUL The MrsP is currently not used
 *      and can be destroyed.
 * @retval STATUS_RESOURCE_IN_USE The MrsP control is in use,
 *      it cannot be destroyed.
 */
RTEMS_INLINE_ROUTINE Status_Control _MRSP_Can_destroy( MRSP_Control *mrsp )
{
  if ( _MRSP_Get_owner( mrsp ) != NULL ) {
    return STATUS_RESOURCE_IN_USE;
  }

  return STATUS_SUCCESSFUL;
}

/**
 * @brief Destroys the MrsP control
 *
 * @param[in, out] The mrsp that is about to be destroyed.
 * @param queue_context The thread queue context.
 */
RTEMS_INLINE_ROUTINE void _MRSP_Destroy(
  MRSP_Control         *mrsp,
  Thread_queue_Context *queue_context
)
{
  _MRSP_Release( mrsp, queue_context );
  _Thread_queue_Destroy( &mrsp->Wait_queue );
}

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* RTEMS_SMP */

#endif /* _RTEMS_SCORE_MRSPIMPL_H */
