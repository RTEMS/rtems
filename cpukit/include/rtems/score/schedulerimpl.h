/**
 * @file
 *
 * @ingroup RTEMSScoreScheduler
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreScheduler which are only used by the implementation.
 */

/*
 *  Copyright (C) 2010 Gedare Bloom.
 *  Copyright (C) 2011 On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2014, 2017 embedded brains GmbH
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_SCHEDULERIMPL_H
#define _RTEMS_SCORE_SCHEDULERIMPL_H

#include <rtems/score/scheduler.h>
#include <rtems/score/assert.h>
#include <rtems/score/priorityimpl.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/threadimpl.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup RTEMSScoreScheduler Scheduler Handler
 *
 * @ingroup RTEMSScore
 *
 * @brief This group contains the Scheduler Handler implementation.
 *
 * This handler encapsulates functionality related to managing sets of threads
 * that are ready for execution.
 *
 * Schedulers are used by the system to manage sets of threads that are ready
 * for execution.  A scheduler consists of
 *
 * * a scheduler algorithm implementation,
 *
 * * a scheduler index and an associated name, and
 *
 * * a set of processors owned by the scheduler (may be empty, but never
 *   overlaps with a set owned by another scheduler).
 *
 * Each thread uses exactly one scheduler as its home scheduler.  Threads may
 * temporarily use another scheduler due to actions of locking protocols.
 *
 * All properties of a scheduler can be configured and controlled by the user.
 * Some properties are fixed at link time (defined by application configuration
 * options), other properties can be changed at runtime through directive
 * calls.
 *
 * The scheduler index, name, and initial processor set are defined for a
 * particular application by the application configuration.  The schedulers are
 * registered in the ::_Scheduler_Table which has ::_Scheduler_Count entries.
 *
 * @{
 */

/**
 * @brief Initializes the scheduler to the policy chosen by the user.
 *
 * This routine initializes the scheduler to the policy chosen by the user
 * through confdefs, or to the priority scheduler with ready chains by
 * default.
 */
void _Scheduler_Handler_initialization( void );

/**
 * @brief Gets the context of the scheduler.
 *
 * @param scheduler The scheduler to get the context of.
 *
 * @return The context of @a scheduler.
 */
RTEMS_INLINE_ROUTINE Scheduler_Context *_Scheduler_Get_context(
  const Scheduler_Control *scheduler
)
{
  return scheduler->context;
}

/**
 * @brief Gets the scheduler for the cpu.
 *
 * @param cpu The cpu control to get the scheduler of.
 *
 * @return The scheduler for the cpu.
 */
RTEMS_INLINE_ROUTINE const Scheduler_Control *_Scheduler_Get_by_CPU(
  const Per_CPU_Control *cpu
)
{
#if defined(RTEMS_SMP)
  return cpu->Scheduler.control;
#else
  (void) cpu;
  return &_Scheduler_Table[ 0 ];
#endif
}

/**
 * @brief Acquires the scheduler instance inside a critical section (interrupts
 * disabled).
 *
 * @param scheduler The scheduler instance.
 * @param lock_context The lock context to use for
 *   _Scheduler_Release_critical().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Acquire_critical(
  const Scheduler_Control *scheduler,
  ISR_lock_Context        *lock_context
)
{
#if defined(RTEMS_SMP)
  Scheduler_Context *context;

  context = _Scheduler_Get_context( scheduler );
  _ISR_lock_Acquire( &context->Lock, lock_context );
#else
  (void) scheduler;
  (void) lock_context;
#endif
}

/**
 * @brief Releases the scheduler instance inside a critical section (interrupts
 * disabled).
 *
 * @param scheduler The scheduler instance.
 * @param lock_context The lock context used for
 *   _Scheduler_Acquire_critical().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Release_critical(
  const Scheduler_Control *scheduler,
  ISR_lock_Context        *lock_context
)
{
#if defined(RTEMS_SMP)
  Scheduler_Context *context;

  context = _Scheduler_Get_context( scheduler );
  _ISR_lock_Release( &context->Lock, lock_context );
#else
  (void) scheduler;
  (void) lock_context;
#endif
}

#if defined(RTEMS_SMP)
/**
 * @brief Indicate if the thread non-preempt mode is supported by the
 * scheduler.
 *
 * @param scheduler The scheduler instance.
 *
 * @return True if the non-preempt mode for threads is supported by the
 *   scheduler, otherwise false.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_Is_non_preempt_mode_supported(
  const Scheduler_Control *scheduler
)
{
  return scheduler->is_non_preempt_mode_supported;
}
#endif

#if defined(RTEMS_SMP)
void _Scheduler_Request_ask_for_help( Thread_Control *the_thread );

/**
 * @brief Registers an ask for help request if necessary.
 *
 * The actual ask for help operation is carried out during
 * _Thread_Do_dispatch() on a processor related to the thread.  This yields a
 * better separation of scheduler instances.  A thread of one scheduler
 * instance should not be forced to carry out too much work for threads on
 * other scheduler instances.
 *
 * @param the_thread The thread in need for help.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Ask_for_help( Thread_Control *the_thread )
{
  _Assert( _Thread_State_is_owner( the_thread ) );

  if ( the_thread->Scheduler.helping_nodes > 0 ) {
    _Scheduler_Request_ask_for_help( the_thread );
  }
}
#endif

/**
 * The preferred method to add a new scheduler is to define the jump table
 * entries and add a case to the _Scheduler_Initialize routine.
 *
 * Generic scheduling implementations that rely on the ready queue only can
 * be found in the _Scheduler_queue_XXX functions.
 */

/*
 * Passing the Scheduler_Control* to these functions allows for multiple
 * scheduler's to exist simultaneously, which could be useful on an SMP
 * system.  Then remote Schedulers may be accessible.  How to protect such
 * accesses remains an open problem.
 */

/**
 * @brief General scheduling decision.
 *
 * This kernel routine implements the scheduling decision logic for
 * the scheduler. It does NOT dispatch.
 *
 * @param the_thread The thread which state changed previously.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Schedule( Thread_Control *the_thread )
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context         lock_context;

  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &lock_context );

  ( *scheduler->Operations.schedule )( scheduler, the_thread );

  _Scheduler_Release_critical( scheduler, &lock_context );
}

/**
 * @brief Scheduler yield with a particular thread.
 *
 * This routine is invoked when a thread wishes to voluntarily transfer control
 * of the processor to another thread.
 *
 * @param the_thread The yielding thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Yield( Thread_Control *the_thread )
{
  const Scheduler_Control *scheduler;
  ISR_lock_Context         lock_context;

  scheduler = _Thread_Scheduler_get_home( the_thread );
  _Scheduler_Acquire_critical( scheduler, &lock_context );
  ( *scheduler->Operations.yield )(
    scheduler,
    the_thread,
    _Thread_Scheduler_get_home_node( the_thread )
  );
  _Scheduler_Release_critical( scheduler, &lock_context );
}

/**
 * @brief Blocks a thread with respect to the scheduler.
 *
 * This routine removes @a the_thread from the scheduling decision for
 * the scheduler. The primary task is to remove the thread from the
 * ready queue.  It performs any necessary scheduling operations
 * including the selection of a new heir thread.
 *
 * @param the_thread The thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Block( Thread_Control *the_thread )
{
#if defined(RTEMS_SMP)
  Chain_Node              *node;
  const Chain_Node        *tail;
  Scheduler_Node          *scheduler_node;
  const Scheduler_Control *scheduler;
  ISR_lock_Context         lock_context;

  node = _Chain_First( &the_thread->Scheduler.Scheduler_nodes );
  tail = _Chain_Immutable_tail( &the_thread->Scheduler.Scheduler_nodes );

  scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
  scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

  _Scheduler_Acquire_critical( scheduler, &lock_context );
  ( *scheduler->Operations.block )(
    scheduler,
    the_thread,
    scheduler_node
  );
  _Scheduler_Release_critical( scheduler, &lock_context );

  node = _Chain_Next( node );

  while ( node != tail ) {
    scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
    scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

    _Scheduler_Acquire_critical( scheduler, &lock_context );
    ( *scheduler->Operations.withdraw_node )(
      scheduler,
      the_thread,
      scheduler_node,
      THREAD_SCHEDULER_BLOCKED
    );
    _Scheduler_Release_critical( scheduler, &lock_context );

    node = _Chain_Next( node );
  }
#else
  const Scheduler_Control *scheduler;

  scheduler = _Thread_Scheduler_get_home( the_thread );
  ( *scheduler->Operations.block )(
    scheduler,
    the_thread,
    _Thread_Scheduler_get_home_node( the_thread )
  );
#endif
}

/**
 * @brief Unblocks a thread with respect to the scheduler.
 *
 * This operation must fetch the latest thread priority value for this
 * scheduler instance and update its internal state if necessary.
 *
 * @param the_thread The thread.
 *
 * @see _Scheduler_Node_get_priority().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Unblock( Thread_Control *the_thread )
{
  Scheduler_Node          *scheduler_node;
  const Scheduler_Control *scheduler;
  ISR_lock_Context         lock_context;

#if defined(RTEMS_SMP)
  scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE(
    _Chain_First( &the_thread->Scheduler.Scheduler_nodes )
  );
  scheduler = _Scheduler_Node_get_scheduler( scheduler_node );
#else
  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  scheduler = _Thread_Scheduler_get_home( the_thread );
#endif

  _Scheduler_Acquire_critical( scheduler, &lock_context );
  ( *scheduler->Operations.unblock )( scheduler, the_thread, scheduler_node );
  _Scheduler_Release_critical( scheduler, &lock_context );
}

/**
 * @brief Propagates a priority change of a thread to the scheduler.
 *
 * On uni-processor configurations, this operation must evaluate the thread
 * state.  In case the thread is not ready, then the priority update should be
 * deferred to the next scheduler unblock operation.
 *
 * The operation must update the heir and thread dispatch necessary variables
 * in case the set of scheduled threads changes.
 *
 * @param the_thread The thread changing its priority.
 *
 * @see _Scheduler_Node_get_priority().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Update_priority( Thread_Control *the_thread )
{
#if defined(RTEMS_SMP)
  Chain_Node       *node;
  const Chain_Node *tail;

  _Thread_Scheduler_process_requests( the_thread );

  node = _Chain_First( &the_thread->Scheduler.Scheduler_nodes );
  tail = _Chain_Immutable_tail( &the_thread->Scheduler.Scheduler_nodes );

  do {
    Scheduler_Node          *scheduler_node;
    const Scheduler_Control *scheduler;
    ISR_lock_Context         lock_context;

    scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
    scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

    _Scheduler_Acquire_critical( scheduler, &lock_context );
    ( *scheduler->Operations.update_priority )(
      scheduler,
      the_thread,
      scheduler_node
    );
    _Scheduler_Release_critical( scheduler, &lock_context );

    node = _Chain_Next( node );
  } while ( node != tail );
#else
  const Scheduler_Control *scheduler;

  scheduler = _Thread_Scheduler_get_home( the_thread );
  ( *scheduler->Operations.update_priority )(
    scheduler,
    the_thread,
    _Thread_Scheduler_get_home_node( the_thread )
  );
#endif
}

#if defined(RTEMS_SMP)
/**
 * @brief Changes the sticky level of the home scheduler node and propagates a
 * priority change of a thread to the scheduler.
 *
 * @param the_thread The thread changing its priority or sticky level.
 *
 * @see _Scheduler_Update_priority().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Priority_and_sticky_update(
  Thread_Control *the_thread,
  int             sticky_level_change
)
{
  Chain_Node              *node;
  const Chain_Node        *tail;
  Scheduler_Node          *scheduler_node;
  const Scheduler_Control *scheduler;
  ISR_lock_Context         lock_context;

  _Thread_Scheduler_process_requests( the_thread );

  node = _Chain_First( &the_thread->Scheduler.Scheduler_nodes );
  scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
  scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

  _Scheduler_Acquire_critical( scheduler, &lock_context );

  scheduler_node->sticky_level += sticky_level_change;
  _Assert( scheduler_node->sticky_level >= 0 );

  ( *scheduler->Operations.update_priority )(
    scheduler,
    the_thread,
    scheduler_node
  );

  _Scheduler_Release_critical( scheduler, &lock_context );

  tail = _Chain_Immutable_tail( &the_thread->Scheduler.Scheduler_nodes );
  node = _Chain_Next( node );

  while ( node != tail ) {
    scheduler_node = SCHEDULER_NODE_OF_THREAD_SCHEDULER_NODE( node );
    scheduler = _Scheduler_Node_get_scheduler( scheduler_node );

    _Scheduler_Acquire_critical( scheduler, &lock_context );
    ( *scheduler->Operations.update_priority )(
      scheduler,
      the_thread,
      scheduler_node
    );
    _Scheduler_Release_critical( scheduler, &lock_context );

    node = _Chain_Next( node );
  }
}
#endif

/**
 * @brief Maps a thread priority from the user domain to the scheduler domain.
 *
 * Let M be the maximum scheduler priority.  The mapping must be bijective in
 * the closed interval [0, M], e.g. _Scheduler_Unmap_priority( scheduler,
 * _Scheduler_Map_priority( scheduler, p ) ) == p for all p in [0, M].  For
 * other values the mapping is undefined.
 *
 * @param scheduler The scheduler instance.
 * @param priority The user domain thread priority.
 *
 * @return The corresponding thread priority of the scheduler domain is returned.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Scheduler_Map_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return ( *scheduler->Operations.map_priority )( scheduler, priority );
}

/**
 * @brief Unmaps a thread priority from the scheduler domain to the user domain.
 *
 * @param scheduler The scheduler instance.
 * @param priority The scheduler domain thread priority.
 *
 * @return The corresponding thread priority of the user domain is returned.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Scheduler_Unmap_priority(
  const Scheduler_Control *scheduler,
  Priority_Control         priority
)
{
  return ( *scheduler->Operations.unmap_priority )( scheduler, priority );
}

/**
 * @brief Initializes a scheduler node.
 *
 * The scheduler node contains arbitrary data on function entry.  The caller
 * must ensure that _Scheduler_Node_destroy() will be called after a
 * _Scheduler_Node_initialize() before the memory of the scheduler node is
 * destroyed.
 *
 * @param scheduler The scheduler instance.
 * @param[out] node The scheduler node to initialize.
 * @param the_thread The thread of the scheduler node to initialize.
 * @param priority The thread priority.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Node_initialize(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  ( *scheduler->Operations.node_initialize )(
    scheduler,
    node,
    the_thread,
    priority
  );
}

/**
 * @brief Destroys a scheduler node.
 *
 * The caller must ensure that _Scheduler_Node_destroy() will be called only
 * after a corresponding _Scheduler_Node_initialize().
 *
 * @param scheduler The scheduler instance.
 * @param[out] node The scheduler node to destroy.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Node_destroy(
  const Scheduler_Control *scheduler,
  Scheduler_Node          *node
)
{
  ( *scheduler->Operations.node_destroy )( scheduler, node );
}

/**
 * @brief Releases a job of a thread with respect to the scheduler.
 *
 * @param the_thread The thread.
 * @param priority_node The priority node of the job.
 * @param deadline The deadline in watchdog ticks since boot.
 * @param queue_context The thread queue context to provide the set of
 *   threads for _Thread_Priority_update().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Release_job(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  uint64_t              deadline,
  Thread_queue_Context *queue_context
)
{
  const Scheduler_Control *scheduler = _Thread_Scheduler_get_home( the_thread );

  _Thread_queue_Context_clear_priority_updates( queue_context );
  ( *scheduler->Operations.release_job )(
    scheduler,
    the_thread,
    priority_node,
    deadline,
    queue_context
  );
}

/**
 * @brief Cancels a job of a thread with respect to the scheduler.
 *
 * @param the_thread The thread.
 * @param priority_node The priority node of the job.
 * @param queue_context The thread queue context to provide the set of
 *   threads for _Thread_Priority_update().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Cancel_job(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
)
{
  const Scheduler_Control *scheduler = _Thread_Scheduler_get_home( the_thread );

  _Thread_queue_Context_clear_priority_updates( queue_context );
  ( *scheduler->Operations.cancel_job )(
    scheduler,
    the_thread,
    priority_node,
    queue_context
  );
}

/**
 * @brief Scheduler method invoked at each clock tick.
 *
 * This method is invoked at each clock tick to allow the scheduler
 * implementation to perform any activities required.  For the
 * scheduler which support standard RTEMS features, this includes
 * time-slicing management.
 *
 * @param cpu The cpu control for the operation.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Tick( const Per_CPU_Control *cpu )
{
  const Scheduler_Control *scheduler;
  Thread_Control          *executing;

  scheduler = _Scheduler_Get_by_CPU( cpu );

#if defined(RTEMS_SMP)
  if ( scheduler == NULL ) {
    /*
     * In SMP configurations, processors may be removed/added at runtime
     * from/to a scheduler.  There may be still clock interrupts on currently
     * unassigned processors.
     */
    return;
  }
#endif

  /*
   * Each online processor has at least an idle thread as the executing thread
   * even in case it has currently no scheduler assigned.  Clock interrupts on
   * processors which are not online would be a severe bug of the Clock Driver.
   */
  executing = _Per_CPU_Get_executing( cpu );
  _Assert( executing != NULL );
  ( *scheduler->Operations.tick )( scheduler, executing );
}

/**
 * @brief Starts the idle thread for a particular processor.
 *
 * @param scheduler The scheduler instance.
 * @param[in,out] the_thread The idle thread for the processor.
 * @param[in,out] cpu The processor for the idle thread.
 *
 * @see _Thread_Create_idle().
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Start_idle(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Per_CPU_Control         *cpu
)
{
  ( *scheduler->Operations.start_idle )( scheduler, the_thread, cpu );
}

/**
 * @brief Checks if the scheduler of the cpu with the given index is equal
 *      to the given scheduler.
 *
 * @param scheduler The scheduler for the comparison.
 * @param cpu_index The index of the cpu for the comparison.
 *
 * @retval true The scheduler of the cpu is the given @a scheduler.
 * @retval false The scheduler of the cpu is not the given @a scheduler.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_Has_processor_ownership(
  const Scheduler_Control *scheduler,
  uint32_t                 cpu_index
)
{
#if defined(RTEMS_SMP)
  const Per_CPU_Control   *cpu;
  const Scheduler_Control *scheduler_of_cpu;

  cpu = _Per_CPU_Get_by_index( cpu_index );
  scheduler_of_cpu = _Scheduler_Get_by_CPU( cpu );

  return scheduler_of_cpu == scheduler;
#else
  (void) scheduler;
  (void) cpu_index;

  return true;
#endif
}

/**
 * @brief Gets the processors of the scheduler
 *
 * @param scheduler The scheduler to get the processors of.
 *
 * @return The processors of the context of the given scheduler.
 */
RTEMS_INLINE_ROUTINE const Processor_mask *_Scheduler_Get_processors(
  const Scheduler_Control *scheduler
)
{
#if defined(RTEMS_SMP)
  return &_Scheduler_Get_context( scheduler )->Processors;
#else
  return &_Processor_mask_The_one_and_only;
#endif
}

/**
 * @brief Copies the thread's scheduler's affinity to the given cpuset.
 *
 * @param the_thread The thread to get the affinity of its scheduler.
 * @param cpusetsize The size of @a cpuset.
 * @param[out] cpuset The cpuset that serves as destination for the copy operation
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 *
 * @retval STATUS_INVALID_SIZE The processor set was too small.
 */
Status_Control _Scheduler_Get_affinity(
  Thread_Control *the_thread,
  size_t          cpusetsize,
  cpu_set_t      *cpuset
);

/**
 * @brief Checks if the affinity is a subset of the online processors.
 *
 * @param scheduler This parameter is unused.
 * @param the_thread This parameter is unused.
 * @param node This parameter is unused.
 * @param affinity The processor mask to check.
 *
 * @retval STATUS_SUCCESSFUL The affinity is a subset of the online processors.
 *
 * @retval STATUS_INVALID_NUMBER The affinity is not a subset of the online
 *   processors.
 */
RTEMS_INLINE_ROUTINE Status_Control _Scheduler_default_Set_affinity_body(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  const Processor_mask    *affinity
)
{
  (void) scheduler;
  (void) the_thread;
  (void) node;

  if ( !_Processor_mask_Is_subset( affinity, _SMP_Get_online_processors() ) ) {
    return STATUS_INVALID_NUMBER;
  }

  return STATUS_SUCCESSFUL;
}

/**
 * @brief Sets the thread's scheduler's affinity.
 *
 * @param[in, out] the_thread The thread to set the affinity of.
 * @param cpusetsize The size of @a cpuset.
 * @param cpuset The cpuset to set the affinity.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 *
 * @retval STATUS_INVALID_NUMBER The processor set was not a valid new
 *   processor affinity set for the thread.
 */
Status_Control _Scheduler_Set_affinity(
  Thread_Control  *the_thread,
  size_t           cpusetsize,
  const cpu_set_t *cpuset
);

/**
 * @brief Blocks the thread.
 *
 * @param scheduler The scheduler instance.
 * @param the_thread The thread to block.
 * @param node The corresponding scheduler node.
 * @param extract Method to extract the thread.
 * @param schedule Method for scheduling threads.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Generic_block(
  const Scheduler_Control *scheduler,
  Thread_Control          *the_thread,
  Scheduler_Node          *node,
  void                  ( *extract )(
                             const Scheduler_Control *,
                             Thread_Control *,
                             Scheduler_Node *
                        ),
  void                  ( *schedule )(
                             const Scheduler_Control *,
                             Thread_Control *,
                             bool
                        )
)
{
  ( *extract )( scheduler, the_thread, node );

  /* TODO: flash critical section? */

  if ( _Thread_Is_executing( the_thread ) || _Thread_Is_heir( the_thread ) ) {
    ( *schedule )( scheduler, the_thread, true );
  }
}

/**
 * @brief Gets the number of processors of the scheduler.
 *
 * @param scheduler The scheduler instance to get the number of processors of.
 *
 * @return The number of processors.
 */
RTEMS_INLINE_ROUTINE uint32_t _Scheduler_Get_processor_count(
  const Scheduler_Control *scheduler
)
{
#if defined(RTEMS_SMP)
  const Scheduler_Context *context = _Scheduler_Get_context( scheduler );

  return _Processor_mask_Count( &context->Processors );
#else
  (void) scheduler;

  return 1;
#endif
}

/**
 * @brief Builds an object build id.
 *
 * @param scheduler_index The index to build the build id out of.
 *
 * @return The build id.
 */
RTEMS_INLINE_ROUTINE Objects_Id _Scheduler_Build_id( uint32_t scheduler_index )
{
  return _Objects_Build_id(
    OBJECTS_FAKE_OBJECTS_API,
    OBJECTS_FAKE_OBJECTS_SCHEDULERS,
    _Objects_Local_node,
    (uint16_t) ( scheduler_index + 1 )
  );
}

/**
 * @brief Gets the scheduler index from the given object build id.
 *
 * @param id The object build id.
 *
 * @return The scheduler index.
 */
RTEMS_INLINE_ROUTINE uint32_t _Scheduler_Get_index_by_id( Objects_Id id )
{
  uint32_t minimum_id = _Scheduler_Build_id( 0 );

  return id - minimum_id;
}

/**
 * @brief Gets the scheduler from the given object build id.
 *
 * @param id The object build id.
 *
 * @return The scheduler to the object id.
 */
RTEMS_INLINE_ROUTINE const Scheduler_Control *_Scheduler_Get_by_id(
  Objects_Id id
)
{
  uint32_t index;

  index = _Scheduler_Get_index_by_id( id );

  if ( index >= _Scheduler_Count ) {
    return NULL;
  }

  return &_Scheduler_Table[ index ];
}

/**
 * @brief Gets the index of the scheduler
 *
 * @param scheduler The scheduler to get the index of.
 *
 * @return The index of the given scheduler.
 */
RTEMS_INLINE_ROUTINE uint32_t _Scheduler_Get_index(
  const Scheduler_Control *scheduler
)
{
  return (uint32_t) (scheduler - &_Scheduler_Table[ 0 ]);
}

#if defined(RTEMS_SMP)
/**
 * @brief Gets an idle thread from the scheduler instance.
 *
 * @param context The scheduler instance context.
 *
 * @return idle An idle thread for use.  This function must always return an
 * idle thread.  If none is available, then this is a fatal error.
 */
typedef Thread_Control *( *Scheduler_Get_idle_thread )(
  Scheduler_Context *context
);

/**
 * @brief Releases an idle thread to the scheduler instance for reuse.
 *
 * @param context The scheduler instance context.
 * @param idle The idle thread to release.
 */
typedef void ( *Scheduler_Release_idle_thread )(
  Scheduler_Context *context,
  Thread_Control    *idle
);

/**
 * @brief Changes the threads state to the given new state.
 *
 * @param[out] the_thread The thread to change the state of.
 * @param new_state The new state for @a the_thread.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Thread_change_state(
  Thread_Control         *the_thread,
  Thread_Scheduler_state  new_state
)
{
  _Assert(
    _ISR_lock_Is_owner( &the_thread->Scheduler.Lock )
      || the_thread->Scheduler.state == THREAD_SCHEDULER_BLOCKED
      || !_System_state_Is_up( _System_state_Get() )
  );

  the_thread->Scheduler.state = new_state;
}

/**
 * @brief Sets the scheduler node's idle thread.
 *
 * @param[in, out] node The node to receive an idle thread.
 * @param idle The idle thread control for the operation.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Set_idle_thread(
  Scheduler_Node *node,
  Thread_Control *idle
)
{
  _Assert( _Scheduler_Node_get_idle( node ) == NULL );
  _Assert(
    _Scheduler_Node_get_owner( node ) == _Scheduler_Node_get_user( node )
  );

  _Scheduler_Node_set_user( node, idle );
  node->idle = idle;
}

/**
 * @brief Uses an idle thread for this scheduler node.
 *
 * A thread whose home scheduler node has a sticky level greater than zero may
 * use an idle thread in the home scheduler instance in the case it executes
 * currently in another scheduler instance or in the case it is in a blocking
 * state.
 *
 * @param context The scheduler instance context.
 * @param[in, out] node The node which wants to use the idle thread.
 * @param cpu The processor for the idle thread.
 * @param get_idle_thread Function to get an idle thread.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Use_idle_thread(
  Scheduler_Context         *context,
  Scheduler_Node            *node,
  Per_CPU_Control           *cpu,
  Scheduler_Get_idle_thread  get_idle_thread
)
{
  Thread_Control *idle = ( *get_idle_thread )( context );

  _Scheduler_Set_idle_thread( node, idle );
  _Thread_Set_CPU( idle, cpu );
  return idle;
}

/**
 * @brief This enumeration defines what a scheduler should do with a node which
 * could be scheduled.
 */
typedef enum {
  SCHEDULER_TRY_TO_SCHEDULE_DO_SCHEDULE,
  SCHEDULER_TRY_TO_SCHEDULE_DO_IDLE_EXCHANGE,
  SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK
} Scheduler_Try_to_schedule_action;

/**
 * @brief Tries to schedule the scheduler node.
 *
 * When a scheduler needs to schedule a node, it shall use this function to
 * determine what it shall do with the node.  The node replaces a victim node if
 * it can be scheduled.
 *
 * This function uses the state of the node and the scheduler state of the owner
 * thread to determine what shall be done.  Each scheduler maintains its nodes
 * independent of other schedulers.  This function ensures that a thread is
 * scheduled by at most one scheduler.  If a node requires an executing thread
 * due to some locking protocol and the owner thread is already scheduled by
 * another scheduler, then an idle thread shall be attached to the node.
 *
 * @param[in, out] context is the scheduler context.
 * @param[in, out] node is the node which could be scheduled.
 * @param idle is an idle thread used by the victim node or NULL.
 * @param get_idle_thread points to a function to get an idle thread.
 *
 * @retval SCHEDULER_TRY_TO_SCHEDULE_DO_SCHEDULE The node shall be scheduled.
 *
 * @retval SCHEDULER_TRY_TO_SCHEDULE_DO_IDLE_EXCHANGE The node shall be
 *   scheduled and the provided idle thread shall be attached to the node.  This
 *   action is returned, if the node cannot use the owner thread and shall use
 *   an idle thread instead.  In this case, the idle thread is provided by the
 *   victim node.
 *
 * @retval SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK The node shall be blocked.  This
 *   action is returned, if the owner thread is already scheduled by another
 *   scheduler.
 */
RTEMS_INLINE_ROUTINE Scheduler_Try_to_schedule_action
_Scheduler_Try_to_schedule_node(
  Scheduler_Context         *context,
  Scheduler_Node            *node,
  const Thread_Control      *idle,
  Scheduler_Get_idle_thread  get_idle_thread
)
{
  ISR_lock_Context                  lock_context;
  Scheduler_Try_to_schedule_action  action;
  Thread_Control                   *owner;

  action = SCHEDULER_TRY_TO_SCHEDULE_DO_SCHEDULE;
  owner = _Scheduler_Node_get_owner( node );
  _Assert( _Scheduler_Node_get_user( node ) == owner );
  _Assert( _Scheduler_Node_get_idle( node ) == NULL );

  _Thread_Scheduler_acquire_critical( owner, &lock_context );

  if ( owner->Scheduler.state == THREAD_SCHEDULER_READY ) {
    _Thread_Scheduler_cancel_need_for_help( owner, _Thread_Get_CPU( owner ) );
    _Scheduler_Thread_change_state( owner, THREAD_SCHEDULER_SCHEDULED );
  } else if (
    owner->Scheduler.state == THREAD_SCHEDULER_SCHEDULED
      && node->sticky_level <= 1
  ) {
    action = SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK;
  } else if ( node->sticky_level == 0 ) {
    action = SCHEDULER_TRY_TO_SCHEDULE_DO_BLOCK;
  } else if ( idle != NULL ) {
    action = SCHEDULER_TRY_TO_SCHEDULE_DO_IDLE_EXCHANGE;
  } else {
    _Scheduler_Use_idle_thread(
      context,
      node,
      _Thread_Get_CPU( owner ),
      get_idle_thread
    );
  }

  _Thread_Scheduler_release_critical( owner, &lock_context );
  return action;
}

/**
 * @brief Releases an idle thread using this scheduler node.
 *
 * @param context The scheduler instance context.
 * @param[in, out] node The node which may have an idle thread as user.
 * @param release_idle_thread Function to release an idle thread.
 *
 * @retval idle The idle thread which used this node.
 * @retval NULL This node had no idle thread as an user.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Scheduler_Release_idle_thread(
  Scheduler_Context             *context,
  Scheduler_Node                *node,
  Scheduler_Release_idle_thread  release_idle_thread
)
{
  Thread_Control *idle = _Scheduler_Node_get_idle( node );

  if ( idle != NULL ) {
    Thread_Control *owner = _Scheduler_Node_get_owner( node );

    node->idle = NULL;
    _Scheduler_Node_set_user( node, owner );
    ( *release_idle_thread )( context, idle );
  }

  return idle;
}

/**
 * @brief Exchanges an idle thread from the scheduler node that uses it
 *      right now to another scheduler node.
 *
 * @param needs_idle The scheduler node that needs an idle thread.
 * @param uses_idle The scheduler node that used the idle thread.
 * @param idle The idle thread that is exchanged.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Exchange_idle_thread(
  Scheduler_Node *needs_idle,
  Scheduler_Node *uses_idle,
  Thread_Control *idle
)
{
  uses_idle->idle = NULL;
  _Scheduler_Node_set_user(
    uses_idle,
    _Scheduler_Node_get_owner( uses_idle )
  );
  _Scheduler_Set_idle_thread( needs_idle, idle );
}

/**
 * @brief Blocks this scheduler node.
 *
 * @param context The scheduler instance context.
 * @param[in, out] thread The thread which wants to get blocked referencing this
 *   node.  This is not necessarily the user of this node in case the node
 *   participates in the scheduler helping protocol.
 * @param[in, out] node The node which wants to get blocked.
 * @param is_scheduled This node is scheduled.
 * @param get_idle_thread Function to get an idle thread.
 *
 * @retval thread_cpu The processor of the thread.  Indicates to continue with
 *   the blocking operation.
 * @retval NULL Otherwise.
 */
RTEMS_INLINE_ROUTINE Per_CPU_Control *_Scheduler_Block_node(
  Scheduler_Context         *context,
  Thread_Control            *thread,
  Scheduler_Node            *node,
  bool                       is_scheduled,
  Scheduler_Get_idle_thread  get_idle_thread
)
{
  int               sticky_level;
  ISR_lock_Context  lock_context;
  Per_CPU_Control  *thread_cpu;

  sticky_level = node->sticky_level;
  --sticky_level;
  node->sticky_level = sticky_level;
  _Assert( sticky_level >= 0 );

  _Thread_Scheduler_acquire_critical( thread, &lock_context );
  thread_cpu = _Thread_Get_CPU( thread );
  _Thread_Scheduler_cancel_need_for_help( thread, thread_cpu );
  _Scheduler_Thread_change_state( thread, THREAD_SCHEDULER_BLOCKED );
  _Thread_Scheduler_release_critical( thread, &lock_context );

  if ( sticky_level > 0 ) {
    if ( is_scheduled && _Scheduler_Node_get_idle( node ) == NULL ) {
      Thread_Control *idle;

      idle = _Scheduler_Use_idle_thread(
        context,
        node,
        thread_cpu,
        get_idle_thread
      );
      _Thread_Dispatch_update_heir( _Per_CPU_Get(), thread_cpu, idle );
    }

    return NULL;
  }

  _Assert( thread == _Scheduler_Node_get_user( node ) );
  return thread_cpu;
}

/**
 * @brief Discard the idle thread from the scheduler node.
 *
 * @param context The scheduler context.
 * @param[in, out] the_thread The thread for the operation.
 * @param[in, out] node The scheduler node to discard the idle thread from.
 * @param release_idle_thread Method to release the idle thread from the context.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Discard_idle_thread(
  Scheduler_Context             *context,
  Thread_Control                *the_thread,
  Scheduler_Node                *node,
  Scheduler_Release_idle_thread  release_idle_thread
)
{
  Thread_Control  *idle;
  Thread_Control  *owner;
  Per_CPU_Control *cpu;

  idle = _Scheduler_Node_get_idle( node );
  owner = _Scheduler_Node_get_owner( node );

  node->idle = NULL;
  _Assert( _Scheduler_Node_get_user( node ) == idle );
  _Scheduler_Node_set_user( node, owner );
  ( *release_idle_thread )( context, idle );

  cpu = _Thread_Get_CPU( idle );
  _Thread_Set_CPU( the_thread, cpu );
  _Thread_Dispatch_update_heir( _Per_CPU_Get(), cpu, the_thread );
}

/**
 * @brief Unblocks this scheduler node.
 *
 * @param context The scheduler instance context.
 * @param[in, out] the_thread The thread which wants to get unblocked.
 * @param[in, out] node The node which wants to get unblocked.
 * @param is_scheduled This node is scheduled.
 * @param release_idle_thread Function to release an idle thread.
 *
 * @retval true Continue with the unblocking operation.
 * @retval false Do not continue with the unblocking operation.
 */
RTEMS_INLINE_ROUTINE bool _Scheduler_Unblock_node(
  Scheduler_Context             *context,
  Thread_Control                *the_thread,
  Scheduler_Node                *node,
  bool                           is_scheduled,
  Scheduler_Release_idle_thread  release_idle_thread
)
{
  bool unblock;

  ++node->sticky_level;
  _Assert( node->sticky_level > 0 );

  if ( is_scheduled ) {
    _Scheduler_Discard_idle_thread(
      context,
      the_thread,
      node,
      release_idle_thread
    );
    _Scheduler_Thread_change_state( the_thread, THREAD_SCHEDULER_SCHEDULED );
    unblock = false;
  } else {
    _Scheduler_Thread_change_state( the_thread, THREAD_SCHEDULER_READY );
    unblock = true;
  }

  return unblock;
}
#endif

/**
 * @brief Updates the heir.
 *
 * @param[in, out] new_heir The new heir.
 * @param force_dispatch Indicates whether the dispatch happens also if the
 *      currently running thread is set as not preemptible.
 */
RTEMS_INLINE_ROUTINE void _Scheduler_Update_heir(
  Thread_Control *new_heir,
  bool            force_dispatch
)
{
  Thread_Control *heir = _Thread_Heir;

  if ( heir != new_heir && ( heir->is_preemptible || force_dispatch ) ) {
#if defined(RTEMS_SMP)
    /*
     * We need this state only for _Thread_Get_CPU_time_used().  Cannot use
     * _Scheduler_Thread_change_state() since THREAD_SCHEDULER_BLOCKED to
     * THREAD_SCHEDULER_BLOCKED state changes are illegal for the real SMP
     * schedulers.
     */
    heir->Scheduler.state = THREAD_SCHEDULER_BLOCKED;
    new_heir->Scheduler.state = THREAD_SCHEDULER_SCHEDULED;
#endif
    _Thread_Update_CPU_time_used( heir, _Thread_Get_CPU( heir ) );
    _Thread_Heir = new_heir;
    _Thread_Dispatch_necessary = true;
  }
}

/**
 * @brief Sets a new scheduler.
 *
 * @param new_scheduler The new scheduler to set.
 * @param[in, out] the_thread The thread for the operations.
 * @param priority The initial priority for the thread with the new scheduler.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 * @retval STATUS_RESOURCE_IN_USE The thread's wait queue is not empty.
 * @retval STATUS_UNSATISFIED The new scheduler has no processors.
 */
RTEMS_INLINE_ROUTINE Status_Control _Scheduler_Set(
  const Scheduler_Control *new_scheduler,
  Thread_Control          *the_thread,
  Priority_Control         priority
)
{
  Scheduler_Node          *new_scheduler_node;
  Scheduler_Node          *old_scheduler_node;
#if defined(RTEMS_SMP)
  ISR_lock_Context         lock_context;
  const Scheduler_Control *old_scheduler;

#endif

  if ( the_thread->Wait.queue != NULL ) {
    return STATUS_RESOURCE_IN_USE;
  }

  old_scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  _Priority_Plain_extract(
    &old_scheduler_node->Wait.Priority,
    &the_thread->Real_priority
  );

  if (
    !_Priority_Is_empty( &old_scheduler_node->Wait.Priority )
#if defined(RTEMS_SMP)
      || !_Chain_Has_only_one_node( &the_thread->Scheduler.Wait_nodes )
      || the_thread->Scheduler.pin_level != 0
#endif
  ) {
    _Priority_Plain_insert(
      &old_scheduler_node->Wait.Priority,
      &the_thread->Real_priority,
      the_thread->Real_priority.priority
    );
    return STATUS_RESOURCE_IN_USE;
  }

#if defined(RTEMS_SMP)
  old_scheduler = _Thread_Scheduler_get_home( the_thread );
  new_scheduler_node = _Thread_Scheduler_get_node_by_index(
    the_thread,
    _Scheduler_Get_index( new_scheduler )
  );

  _Scheduler_Acquire_critical( new_scheduler, &lock_context );

  if (
    _Scheduler_Get_processor_count( new_scheduler ) == 0
      || ( *new_scheduler->Operations.set_affinity )(
        new_scheduler,
        the_thread,
        new_scheduler_node,
        &the_thread->Scheduler.Affinity
      ) != STATUS_SUCCESSFUL
  ) {
    _Scheduler_Release_critical( new_scheduler, &lock_context );
    _Priority_Plain_insert(
      &old_scheduler_node->Wait.Priority,
      &the_thread->Real_priority,
      the_thread->Real_priority.priority
    );
    return STATUS_UNSATISFIED;
  }

  _Assert( the_thread->Scheduler.pinned_scheduler == NULL );
  the_thread->Scheduler.home_scheduler = new_scheduler;

  _Scheduler_Release_critical( new_scheduler, &lock_context );

  _Thread_Scheduler_process_requests( the_thread );
#else
  new_scheduler_node = old_scheduler_node;
#endif

  the_thread->Start.initial_priority = priority;
  _Priority_Node_set_priority( &the_thread->Real_priority, priority );
  _Priority_Initialize_one(
    &new_scheduler_node->Wait.Priority,
    &the_thread->Real_priority
  );

#if defined(RTEMS_SMP)
  if ( old_scheduler != new_scheduler ) {
    States_Control current_state;

    current_state = the_thread->current_state;

    if ( _States_Is_ready( current_state ) ) {
      _Scheduler_Block( the_thread );
    }

    _Assert( old_scheduler_node->sticky_level == 0 );
    _Assert( new_scheduler_node->sticky_level == 0 );

    _Chain_Extract_unprotected( &old_scheduler_node->Thread.Wait_node );
    _Assert( _Chain_Is_empty( &the_thread->Scheduler.Wait_nodes ) );
    _Chain_Initialize_one(
      &the_thread->Scheduler.Wait_nodes,
      &new_scheduler_node->Thread.Wait_node
    );
    _Chain_Extract_unprotected(
      &old_scheduler_node->Thread.Scheduler_node.Chain
    );
    _Assert( _Chain_Is_empty( &the_thread->Scheduler.Scheduler_nodes ) );
    _Chain_Initialize_one(
      &the_thread->Scheduler.Scheduler_nodes,
      &new_scheduler_node->Thread.Scheduler_node.Chain
    );

    _Scheduler_Node_set_priority(
      new_scheduler_node,
      priority,
      PRIORITY_GROUP_LAST
    );

    if ( _States_Is_ready( current_state ) ) {
      _Scheduler_Unblock( the_thread );
    }

    return STATUS_SUCCESSFUL;
  }
#endif

  _Scheduler_Node_set_priority(
    new_scheduler_node,
    priority,
    PRIORITY_GROUP_LAST
  );
  _Scheduler_Update_priority( the_thread );
  return STATUS_SUCCESSFUL;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
