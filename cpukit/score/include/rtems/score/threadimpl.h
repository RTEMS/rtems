/**
 * @file
 *
 * @brief Inlined Routines from the Thread Handler
 *
 * This file contains the macro implementation of the inlined
 * routines from the Thread handler.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (c) 2014 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADIMPL_H
#define _RTEMS_SCORE_THREADIMPL_H

#include <rtems/score/thread.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/resourceimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/todimpl.h>
#include <rtems/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreThread
 */
/**@{**/

/**
 *  The following structure contains the information necessary to manage
 *  a thread which it is  waiting for a resource.
 */
#define THREAD_STATUS_PROXY_BLOCKING 0x1111111

/**
 *  Self for the GNU Ada Run-Time
 */
SCORE_EXTERN void *rtems_ada_self;

/**
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
SCORE_EXTERN Objects_Information _Thread_Internal_information;

/**
 *  The following points to the thread whose floating point
 *  context is currently loaded.
 */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
SCORE_EXTERN Thread_Control *_Thread_Allocated_fp;
#endif

#if !defined(__DYNAMIC_REENT__)
/**
 * The C library re-enter-rant global pointer. Some C library implementations
 * such as newlib have a single global pointer that changed during a context
 * switch. The pointer points to that global pointer. The Thread control block
 * holds a pointer to the task specific data.
 */
SCORE_EXTERN struct _reent **_Thread_libc_reent;
#endif

#define THREAD_RBTREE_NODE_TO_THREAD( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, RBNode )

#if defined(RTEMS_SMP)
#define THREAD_RESOURCE_NODE_TO_THREAD( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, Resource_node )
#endif

/**
 *  @brief Initialize thread handler.
 *
 *  This routine performs the initialization necessary for this handler.
 */
void _Thread_Handler_initialization(void);

/**
 *  @brief Create idle thread.
 *
 *  This routine creates the idle thread.
 *
 *  @warning No thread should be created before this one.
 */
void _Thread_Create_idle(void);

/**
 *  @brief Start thread multitasking.
 *
 *  This routine initiates multitasking.  It is invoked only as
 *  part of initialization and its invocation is the last act of
 *  the non-multitasking part of the system initialization.
 */
void _Thread_Start_multitasking( void ) RTEMS_COMPILER_NO_RETURN_ATTRIBUTE;

/**
 *  @brief Allocate the requested stack space for the thread.
 *
 *  Allocate the requested stack space for the thread.
 *  Set the Start.stack field to the address of the stack.
 *
 *  @param[in] the_thread is the thread where the stack space is requested
 *
 *  @retval actual size allocated after any adjustment
 *  @retval zero if the allocation failed
 */
size_t _Thread_Stack_Allocate(
  Thread_Control *the_thread,
  size_t          stack_size
);

/**
 *  @brief Deallocate thread stack.
 *
 *  Deallocate the Thread's stack.
 */
void _Thread_Stack_Free(
  Thread_Control *the_thread
);

/**
 *  @brief Initialize thread.
 *
 *  This routine initializes the specified the thread.  It allocates
 *  all memory associated with this thread.  It completes by adding
 *  the thread to the local object table so operations on this
 *  thread id are allowed.
 *
 *  @note If stack_area is NULL, it is allocated from the workspace.
 *
 *  @note If the stack is allocated from the workspace, then it is
 *        guaranteed to be of at least minimum size.
 */
bool _Thread_Initialize(
  Objects_Information                  *information,
  Thread_Control                       *the_thread,
  const struct Scheduler_Control       *scheduler,
  void                                 *stack_area,
  size_t                                stack_size,
  bool                                  is_fp,
  Priority_Control                      priority,
  bool                                  is_preemptible,
  Thread_CPU_budget_algorithms          budget_algorithm,
  Thread_CPU_budget_algorithm_callout   budget_callout,
  uint32_t                              isr_level,
  Objects_Name                          name
);

/**
 *  @brief Initializes thread and executes it.
 *
 *  This routine initializes the executable information for a thread
 *  and makes it ready to execute.  After this routine executes, the
 *  thread competes with all other threads for CPU time.
 *
 *  @param the_thread is the thread to be initialized
 *  @param the_prototype
 *  @param entry_point
 *  @param pointer_argument
 *  @param numeric_argument
 *  @param[in,out] cpu The processor if used to start an idle thread
 *  during system initialization.  Must be set to @c NULL to start a normal
 *  thread.
 */
bool _Thread_Start(
  Thread_Control            *the_thread,
  Thread_Start_types         the_prototype,
  void                      *entry_point,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument,
  Per_CPU_Control           *cpu
);

bool _Thread_Restart(
  Thread_Control            *the_thread,
  Thread_Control            *executing,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument
);

void _Thread_Yield( Thread_Control *executing );

bool _Thread_Set_life_protection( bool protect );

void _Thread_Life_action_handler(
  Thread_Control  *executing,
  Thread_Action   *action,
  Per_CPU_Control *cpu,
  ISR_Level        level
);

/**
 * @brief Kills all zombie threads in the system.
 *
 * Threads change into the zombie state as the last step in the thread
 * termination sequence right before a context switch to the heir thread is
 * initiated.  Since the thread stack is still in use during this phase we have
 * to postpone the thread stack reclamation until this point.  On SMP
 * configurations we may have to busy wait for context switch completion here.
 */
void _Thread_Kill_zombies( void );

/**
 * @brief Closes the thread.
 *
 * Closes the thread object and starts the thread termination sequence.  In
 * case the executing thread is not terminated, then this function waits until
 * the terminating thread reached the zombie state.
 */
void _Thread_Close( Thread_Control *the_thread, Thread_Control *executing );

/**
 *  @brief Removes any set states for @a the_thread.
 *
 *  This routine removes any set states for @a the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  - INTERRUPT LATENCY:
 *    + ready chain
 *    + select heir
 */
void _Thread_Ready(
  Thread_Control *the_thread
);

/**
 *  @brief Clears the indicated STATES for @a the_thread.
 *
 *  This routine clears the indicated STATES for @a the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  - INTERRUPT LATENCY:
 *    + priority map
 *    + select heir
 */
void _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 *  @brief Sets the indicated @a state for @a the_thread.
 *
 *  This routine sets the indicated @a state for @a the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  @param[in] the_thread is the thread to set the state for.
 *  @param[in] state is the state to set the_thread to.
 *
 *  - INTERRUPT LATENCY:
 *   + ready chain
 *   + select map
 */
void _Thread_Set_state(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 *  @brief Initializes enviroment for a thread.
 *
 *  This routine initializes the context of @a the_thread to its
 *  appropriate starting state.
 *
 *  @param[in] the_thread is the pointer to the thread control block.
 */
void _Thread_Load_environment(
  Thread_Control *the_thread
);

/**
 *  @brief Wrapper function for all threads.
 *
 *  This routine is the wrapper function for all threads.  It is
 *  the starting point for all threads.  The user provided thread
 *  entry point is invoked by this routine.  Operations
 *  which must be performed immediately before and after the user's
 *  thread executes are found here.
 *
 *  @note On entry, it is assumed all interrupts are blocked and that this
 *  routine needs to set the initial isr level.  This may or may not
 *  actually be needed by the context switch routine and as a result
 *  interrupts may already be at there proper level.  Either way,
 *  setting the initial isr level properly here is safe.
 */
void _Thread_Handler( void );

/**
 * @brief Executes the global constructors and then restarts itself as the
 * first initialization thread.
 *
 * The first initialization thread is the first RTEMS initialization task or
 * the first POSIX initialization thread in case no RTEMS initialization tasks
 * are present.
 */
void *_Thread_Global_construction( void );

/**
 *  @brief Ended the delay of a thread.
 *
 *  This routine is invoked when a thread must be unblocked at the
 *  end of a time based delay (i.e. wake after or wake when).
 *  It is called by the watchdog handler.
 *
 *  @param[in] id is the thread id
 */
void _Thread_Delay_ended(
  Objects_Id  id,
  void       *ignored
);

/**
 *  @brief Change the priority of a thread.
 *
 *  This routine changes the current priority of @a the_thread to
 *  @a new_priority.  It performs any necessary scheduling operations
 *  including the selection of a new heir thread.
 *
 *  @param[in] the_thread is the thread to change
 *  @param[in] new_priority is the priority to set @a the_thread to
 *  @param[in] prepend_it is a switch to prepend the thread
 */
void _Thread_Change_priority (
  Thread_Control   *the_thread,
  Priority_Control  new_priority,
  bool              prepend_it
);

/**
 *  @brief Set thread priority.
 *
 *  This routine updates the priority related fields in the_thread
 *  control block to indicate the current priority is now new_priority.
 */
void _Thread_Set_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
);

/**
 *  This routine updates the related suspend fields in the_thread
 *  control block to indicate the current nested level.
 */
#define _Thread_Suspend( _the_thread ) \
        _Thread_Set_state( _the_thread, STATES_SUSPENDED )

/**
 *  This routine updates the related suspend fields in the_thread
 *  control block to indicate the current nested level.  A force
 *  parameter of true will force a resume and clear the suspend count.
 */
#define _Thread_Resume( _the_thread ) \
        _Thread_Clear_state( _the_thread, STATES_SUSPENDED )

/**
 *  @brief Maps thread Id to a TCB pointer.
 *
 *  This function maps thread IDs to thread control
 *  blocks.  If ID corresponds to a local thread, then it
 *  returns the_thread control pointer which maps to ID
 *  and @a location is set to OBJECTS_LOCAL.  If the thread ID is
 *  global and resides on a remote node, then location is set
 *  to OBJECTS_REMOTE, and the_thread is undefined.
 *  Otherwise, location is set to OBJECTS_ERROR and
 *  the_thread is undefined.
 *
 *  @param[in] id is the id of the thread.
 *  @param[in] location is the location of the block.
 *
 *  @note  The performance of many RTEMS services depends upon
 *         the quick execution of the "good object" path in this
 *         routine.  If there is a possibility of saving a few
 *         cycles off the execution time, this routine is worth
 *         further optimization attention.
 */
Thread_Control *_Thread_Get (
  Objects_Id         id,
  Objects_Locations *location
);

/**
 *  @brief Cancel a blocking operation due to ISR.
 *
 *  This method is used to cancel a blocking operation that was
 *  satisfied from an ISR while the thread executing was in the
 *  process of blocking.
 *
 *  This method will restore the previous ISR disable level during the cancel
 *  operation.  Thus it is an implicit _ISR_Enable().
 *
 *  @param[in] sync_state is the synchronization state
 *  @param[in] the_thread is the thread whose blocking is canceled
 *  @param[in] level is the previous ISR disable level
 *
 *  @note This is a rare routine in RTEMS.  It is called with
 *        interrupts disabled and only when an ISR completed
 *        a blocking condition in process.
 */
void _Thread_blocking_operation_Cancel(
  Thread_blocking_operation_States  sync_state,
  Thread_Control                   *the_thread,
  ISR_Level                         level
);

/**
 *  @brief Finalize a blocking operation.
 *
 *  This method is used to finalize a blocking operation that was
 *  satisfied. It may be used with thread queues or any other synchronization
 *  object that uses the blocking states and watchdog times for timeout.
 *
 *  This method will restore the previous ISR disable level during the cancel
 *  operation.  Thus it is an implicit _ISR_Enable().
 *
 *  @param[in] the_thread is the thread whose blocking is canceled
 *  @param[in] level is the previous ISR disable level
 */
void _Thread_blocking_operation_Finalize(
  Thread_Control                   *the_thread,
  ISR_Level                         level
);

RTEMS_INLINE_ROUTINE Per_CPU_Control *_Thread_Get_CPU(
  const Thread_Control *thread
)
{
#if defined(RTEMS_SMP)
  return thread->Scheduler.cpu;
#else
  (void) thread;

  return _Per_CPU_Get();
#endif
}

RTEMS_INLINE_ROUTINE void _Thread_Set_CPU(
  Thread_Control *thread,
  Per_CPU_Control *cpu
)
{
#if defined(RTEMS_SMP)
  thread->Scheduler.cpu = cpu;
#else
  (void) thread;
  (void) cpu;
#endif
}

/**
 * This function returns true if the_thread is the currently executing
 * thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_executing (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Executing );
}

#if defined(RTEMS_SMP)
/**
 * @brief Returns @true in case the thread executes currently on some processor
 * in the system, otherwise @a false.
 *
 * Do not confuse this with _Thread_Is_executing() which checks only the
 * current processor.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_executing_on_a_processor(
  const Thread_Control *the_thread
)
{
  return _CPU_Context_Get_is_executing( &the_thread->Registers );
}
#endif

/**
 * @brief Returns @true and sets time_of_context_switch to the the
 * time of the last context switch when the thread is currently executing
 * in the system, otherwise @a false.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Get_time_of_last_context_switch(
  Thread_Control    *the_thread,
  Timestamp_Control *time_of_context_switch
)
{
  bool retval = false;

  _Thread_Disable_dispatch();
  #ifndef RTEMS_SMP
    if ( _Thread_Executing->Object.id == the_thread->Object.id ) {
      *time_of_context_switch = _Thread_Time_of_last_context_switch;
      retval = true;
    }
  #else
    if ( _Thread_Is_executing_on_a_processor( the_thread ) ) {
      *time_of_context_switch =
        _Thread_Get_CPU( the_thread )->time_of_last_context_switch;
      retval = true;
    }
  #endif
  _Thread_Enable_dispatch();
  return retval;
}


/**
 * This function returns true if the_thread is the heir
 * thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_heir (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Heir );
}

/**
 * This routine clears any blocking state for the_thread.  It performs
 * any necessary scheduling operations including the selection of
 * a new heir thread.
 */

RTEMS_INLINE_ROUTINE void _Thread_Unblock (
  Thread_Control *the_thread
)
{
  _Thread_Clear_state( the_thread, STATES_BLOCKED );
}

/**
 * This routine resets the current context of the calling thread
 * to that of its initial state.
 */

RTEMS_INLINE_ROUTINE void _Thread_Restart_self( Thread_Control *executing )
{
#if defined(RTEMS_SMP)
  ISR_Level level;

  _Giant_Release();

  _ISR_Disable_without_giant( level );
  ( void ) level;
#endif

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( executing->fp_context != NULL )
    _Context_Restore_fp( &executing->fp_context );
#endif

  _CPU_Context_Restart_self( &executing->Registers );
}

/**
 * This function returns true if the floating point context of
 * the_thread is currently loaded in the floating point unit, and
 * false otherwise.
 */

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
RTEMS_INLINE_ROUTINE bool _Thread_Is_allocated_fp (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Allocated_fp );
}
#endif

/**
 * This routine is invoked when the currently loaded floating
 * point context is now longer associated with an active thread.
 */

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
RTEMS_INLINE_ROUTINE void _Thread_Deallocate_fp( void )
{
  _Thread_Allocated_fp = NULL;
}
#endif

/**
 * This function returns true if dispatching is disabled, and false
 * otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_context_switch_necessary( void )
{
  return ( _Thread_Dispatch_necessary );
}

/**
 * This function returns true if the_thread is NULL and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_null (
  const Thread_Control *the_thread
)
{
  return ( the_thread == NULL );
}

/**
 * @brief Is proxy blocking.
 *
 * status which indicates that a proxy is blocking, and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_proxy_blocking (
  uint32_t   code
)
{
  return (code == THREAD_STATUS_PROXY_BLOCKING);
}

RTEMS_INLINE_ROUTINE uint32_t _Thread_Get_maximum_internal_threads(void)
{
  /* Idle threads */
  uint32_t maximum_internal_threads =
    rtems_configuration_get_maximum_processors();

  /* MPCI thread */
#if defined(RTEMS_MULTIPROCESSING)
  if ( _System_state_Is_multiprocessing ) {
    ++maximum_internal_threads;
  }
#endif

  return maximum_internal_threads;
}

RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Internal_allocate( void )
{
  return (Thread_Control *)
    _Objects_Allocate_unprotected( &_Thread_Internal_information );
}

RTEMS_INLINE_ROUTINE void _Thread_Request_dispatch_if_executing(
  Thread_Control *thread
)
{
#if defined(RTEMS_SMP)
  if ( _Thread_Is_executing_on_a_processor( thread ) ) {
    const Per_CPU_Control *cpu_of_executing = _Per_CPU_Get();
    Per_CPU_Control *cpu_of_thread = _Thread_Get_CPU( thread );

    cpu_of_thread->dispatch_necessary = true;

    if ( cpu_of_executing != cpu_of_thread ) {
      _Per_CPU_Send_interrupt( cpu_of_thread );
    }
  }
#else
  (void) thread;
#endif
}

RTEMS_INLINE_ROUTINE void _Thread_Signal_notification( Thread_Control *thread )
{
  if ( _ISR_Is_in_progress() && _Thread_Is_executing( thread ) ) {
    _Thread_Dispatch_necessary = true;
  } else {
#if defined(RTEMS_SMP)
    if ( _Thread_Is_executing_on_a_processor( thread ) ) {
      const Per_CPU_Control *cpu_of_executing = _Per_CPU_Get();
      Per_CPU_Control *cpu_of_thread = _Thread_Get_CPU( thread );

      if ( cpu_of_executing != cpu_of_thread ) {
        cpu_of_thread->dispatch_necessary = true;
        _Per_CPU_Send_interrupt( cpu_of_thread );
      }
    }
#endif
  }
}

/**
 * @brief Gets the heir of the processor and makes it executing.
 *
 * The thread dispatch necessary indicator is cleared as a side-effect.
 *
 * @return The heir thread.
 *
 * @see _Thread_Dispatch(), _Thread_Start_multitasking() and
 * _Thread_Dispatch_update_heir().
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Get_heir_and_make_it_executing(
  Per_CPU_Control *cpu_self
)
{
  Thread_Control *heir;

  cpu_self->dispatch_necessary = false;

#if defined( RTEMS_SMP )
  /*
   * It is critical that we first update the dispatch necessary and then the
   * read the heir so that we don't miss an update by
   * _Thread_Dispatch_update_heir().
   */
  _Atomic_Fence( ATOMIC_ORDER_SEQ_CST );
#endif

  heir = cpu_self->heir;
  cpu_self->executing = heir;

  return heir;
}

#if defined( RTEMS_SMP )
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_update_heir(
  Per_CPU_Control *cpu_self,
  Per_CPU_Control *cpu_for_heir,
  Thread_Control  *heir
)
{
  cpu_for_heir->heir = heir;

  /*
   * It is critical that we first update the heir and then the dispatch
   * necessary so that _Thread_Get_heir_and_make_it_executing() cannot miss an
   * update.
   */
  _Atomic_Fence( ATOMIC_ORDER_SEQ_CST );

  /*
   * Only update the dispatch necessary indicator if not already set to
   * avoid superfluous inter-processor interrupts.
   */
  if ( !cpu_for_heir->dispatch_necessary ) {
    cpu_for_heir->dispatch_necessary = true;

    if ( cpu_for_heir != cpu_self ) {
      _Per_CPU_Send_interrupt( cpu_for_heir );
    }
  }
}
#endif

RTEMS_INLINE_ROUTINE void _Thread_Update_cpu_time_used(
  Thread_Control *executing,
  Timestamp_Control *time_of_last_context_switch
)
{
  Timestamp_Control uptime;
  Timestamp_Control ran;

  _TOD_Get_uptime( &uptime );
  _Timestamp_Subtract(
    time_of_last_context_switch,
    &uptime,
    &ran
  );
  *time_of_last_context_switch = uptime;
  _Timestamp_Add_to( &executing->cpu_time_used, &ran );
}

RTEMS_INLINE_ROUTINE void _Thread_Action_control_initialize(
  Thread_Action_control *action_control
)
{
  _Chain_Initialize_empty( &action_control->Chain );
}

RTEMS_INLINE_ROUTINE void _Thread_Action_initialize(
  Thread_Action         *action,
  Thread_Action_handler  handler
)
{
  action->handler = handler;
  _Chain_Set_off_chain( &action->Node );
}

RTEMS_INLINE_ROUTINE Per_CPU_Control *
  _Thread_Action_ISR_disable_and_acquire_for_executing( ISR_Level *level )
{
  Per_CPU_Control *cpu;

  _ISR_Disable_without_giant( *level );
  cpu = _Per_CPU_Get();
  _Per_CPU_Acquire( cpu );

  return cpu;
}

RTEMS_INLINE_ROUTINE Per_CPU_Control *_Thread_Action_ISR_disable_and_acquire(
  Thread_Control *thread,
  ISR_Level      *level
)
{
  Per_CPU_Control *cpu;

  _ISR_Disable_without_giant( *level );
  cpu = _Thread_Get_CPU( thread );
  _Per_CPU_Acquire( cpu );

  return cpu;
}

RTEMS_INLINE_ROUTINE void _Thread_Action_release_and_ISR_enable(
  Per_CPU_Control *cpu,
  ISR_Level level
)
{
  _Per_CPU_Release_and_ISR_enable( cpu, level );
}

RTEMS_INLINE_ROUTINE void _Thread_Add_post_switch_action(
  Thread_Control *thread,
  Thread_Action  *action
)
{
  Per_CPU_Control *cpu;
  ISR_Level        level;

  cpu = _Thread_Action_ISR_disable_and_acquire( thread, &level );
  _Chain_Append_if_is_off_chain_unprotected(
    &thread->Post_switch_actions.Chain,
    &action->Node
  );
  _Thread_Action_release_and_ISR_enable( cpu, level );
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_restarting(
  Thread_Life_state life_state
)
{
  return ( life_state & THREAD_LIFE_RESTARTING ) != 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_terminating(
  Thread_Life_state life_state
)
{
  return ( life_state & THREAD_LIFE_TERMINATING ) != 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_protected(
  Thread_Life_state life_state
)
{
  return ( life_state & THREAD_LIFE_PROTECTED ) != 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_changing(
  Thread_Life_state life_state
)
{
  return ( life_state & THREAD_LIFE_RESTARTING_TERMINATING ) != 0;
}

/**
 * @brief Returns true if the thread owns resources, and false otherwise.
 *
 * Resources are accounted with the Thread_Control::resource_count resource
 * counter.  This counter is used by semaphore objects for example.
 *
 * In addition to the resource counter there is a resource dependency tree
 * available on SMP configurations.  In case this tree is non-empty, then the
 * thread owns resources.
 *
 * @param[in] the_thread The thread.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Owns_resources(
  const Thread_Control *the_thread
)
{
  bool owns_resources = the_thread->resource_count != 0;

#if defined(RTEMS_SMP)
  owns_resources = owns_resources
    || _Resource_Node_owns_resources( &the_thread->Resource_node );
#endif

  return owns_resources;
}

RTEMS_INLINE_ROUTINE void _Thread_Debug_set_real_processor(
  Thread_Control  *the_thread,
  Per_CPU_Control *cpu
)
{
#if defined(RTEMS_SMP) && defined(RTEMS_DEBUG)
  the_thread->Scheduler.debug_real_cpu = cpu;
#else
  (void) the_thread;
  (void) cpu;
#endif
}

#if !defined(__DYNAMIC_REENT__)
/**
 * This routine returns the C library re-enterant pointer.
 */

RTEMS_INLINE_ROUTINE struct _reent **_Thread_Get_libc_reent( void )
{
  return _Thread_libc_reent;
}

/**
 * This routine set the C library re-enterant pointer.
 */

RTEMS_INLINE_ROUTINE void _Thread_Set_libc_reent (
  struct _reent **libc_reent
)
{
  _Thread_libc_reent = libc_reent;
}
#endif

/** @}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/threadmp.h>
#endif

#endif
/* end of include file */
