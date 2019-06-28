/**
 * @file
 *
 * @ingroup RTEMSScoreThread
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
 *  Copyright (c) 2014, 2017 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADIMPL_H
#define _RTEMS_SCORE_THREADIMPL_H

#include <rtems/score/thread.h>
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/schedulernodeimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/status.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/timestampimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/config.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreThread
 *
 * @{
 */

/**
 *  The following structure contains the information necessary to manage
 *  a thread which it is  waiting for a resource.
 */
#define THREAD_STATUS_PROXY_BLOCKING 0x1111111

/**
 *  Self for the GNU Ada Run-Time
 */
extern void *rtems_ada_self;

/**
 * @brief Object identifier of the global constructor thread.
 *
 * This variable is set by _RTEMS_tasks_Initialize_user_tasks_body() or
 * _POSIX_Threads_Initialize_user_threads_body().
 *
 * It is consumed by _Thread_Handler().
 */
extern Objects_Id _Thread_Global_constructor;

/**
 *  The following points to the thread whose floating point
 *  context is currently loaded.
 */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
extern Thread_Control *_Thread_Allocated_fp;
#endif

#if defined(RTEMS_SMP)
#define THREAD_OF_SCHEDULER_HELP_NODE( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, Scheduler.Help_node )
#endif

typedef bool ( *Thread_Visitor )( Thread_Control *the_thread, void *arg );

/**
 * @brief Calls the visitor with all threads and the given argument until
 *      it is done.
 *
 * @param visitor Function that gets a thread and @a arg as parameters and
 *      returns if it is done.
 * @param arg Parameter for @a visitor
 */
void _Thread_Iterate(
  Thread_Visitor  visitor,
  void           *arg
);

/**
 * @brief Initializes the thread information
 *
 * @param[out] information Information to initialize.
 */
void _Thread_Initialize_information( Thread_Information *information );

/**
 * @brief Initializes thread handler.
 *
 * This routine performs the initialization necessary for this handler.
 */
void _Thread_Handler_initialization(void);

/**
 * @brief Creates idle thread.
 *
 * This routine creates the idle thread.
 *
 * @warning No thread should be created before this one.
 */
void _Thread_Create_idle(void);

/**
 * @brief Starts thread multitasking.
 *
 * This routine initiates multitasking.  It is invoked only as
 * part of initialization and its invocation is the last act of
 * the non-multitasking part of the system initialization.
 */
void _Thread_Start_multitasking( void ) RTEMS_NO_RETURN;

/**
 * @brief Allocates the requested stack space for the thread.
 *
 * Allocate the requested stack space for the thread.
 * Set the Start.stack field to the address of the stack.
 *
 * @param[out] the_thread The thread where the stack space is requested.
 * @param stack_size The stack space that is requested.
 *
 * @retval actual Size allocated after any adjustment.
 * @retval zero The allocation failed.
 */
size_t _Thread_Stack_Allocate(
  Thread_Control *the_thread,
  size_t          stack_size
);

/**
 * @brief Deallocates thread stack.
 *
 * Deallocate the Thread's stack.
 *
 * @param[out] the_thread The thread to deallocate the stack of.
 */
void _Thread_Stack_Free(
  Thread_Control *the_thread
);

/**
 * @brief Initializes thread.
 *
 * This routine initializes the specified the thread.  It allocates
 * all memory associated with this thread.  It completes by adding
 * the thread to the local object table so operations on this
 * thread id are allowed.
 *
 * @note If stack_area is NULL, it is allocated from the workspace.
 *
 * @note If the stack is allocated from the workspace, then it is
 *       guaranteed to be of at least minimum size.
 *
 * @param information The thread information.
 * @param[out] the_thread The thread to initialize.
 * @param scheduler The scheduler control instance for the thread.
 * @param stack_area The starting address of the thread area.
 * @param stack_size The size of the thread area in bytes.
 * @param is_fp Indicates whether the thread needs a floating point area.
 * @param priority The new thread's priority.
 * @param is_preemptible Indicates whether the new thread is preemptible.
 * @param budget_algorithm The thread's budget algorithm.
 * @param budget_callout The thread's initial budget callout.
 * @param isr_level The thread's initial isr level.
 * @param name Name of the object for the thread.
 *
 * @retval true The thread initialization was successful.
 * @retval false The thread initialization failed.
 */
bool _Thread_Initialize(
  Thread_Information                   *information,
  Thread_Control                       *the_thread,
  const struct _Scheduler_Control      *scheduler,
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
 * @brief Initializes thread and executes it.
 *
 * This routine initializes the executable information for a thread
 * and makes it ready to execute.  After this routine executes, the
 * thread competes with all other threads for CPU time.
 *
 * @param the_thread The thread to be started.
 * @param entry The thread entry information.
 */
bool _Thread_Start(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
);

/**
 * @brief Restarts the currently executing thread.
 *
 * @param[in, out] executing The currently executing thread.
 * @param entry The start entry information for @a executing.
 * @param lock_context The lock context.
 */
void _Thread_Restart_self(
  Thread_Control                 *executing,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
) RTEMS_NO_RETURN;

/**
 * @brief Restarts the thread.
 *
 * @param[in, out] the_thread The thread to restart.
 * @param entry The start entry information for @a the_thread.
 * @param lock_context The lock context.
 *
 * @retval true The operation was successful.
 * @retval false The operation failed.
 */
bool _Thread_Restart_other(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
);

/**
 * @brief Yields the currently executing thread.
 *
 * @param[in, out] executing The thread that performs a yield.
 */
void _Thread_Yield( Thread_Control *executing );

/**
 * @brief Changes the currently executing thread to a new state with the sets.
 *
 * @param clear States to clear.
 * @param set States to set.
 * @param ignore States to ignore.
 *
 * @return The previous state the thread was in.
 */
Thread_Life_state _Thread_Change_life(
  Thread_Life_state clear,
  Thread_Life_state set,
  Thread_Life_state ignore
);

/**
 * @brief Set the thread to life protected.
 *
 * Calls _Thread_Change_life with the given state AND THREAD_LIFE_PROTECTED to
 * set and THREAD_LIFE_PROTECTED to clear.
 *
 * @param state The states to set.
 *
 * @return The previous state the thread was in.
 */
Thread_Life_state _Thread_Set_life_protection( Thread_Life_state state );

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
 * @brief Exits the currently executing thread.
 *
 * @param[in, out] executing The currently executing thread.
 * @param set The states to set.
 * @param[out] exit_value Contains the exit value of the thread.
 */
void _Thread_Exit(
  Thread_Control    *executing,
  Thread_Life_state  set,
  void              *exit_value
);

/**
 * @brief Joins the currently executing thread with the given thread to wait
 *      for.
 *
 * @param[in, out] the_thread The thread to wait for.
 * @param waiting_for_join The states control for the join.
 * @param[in, out] executing The currently executing thread.
 * @param queue_context The thread queue context.
 */
void _Thread_Join(
  Thread_Control       *the_thread,
  States_Control        waiting_for_join,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
);

/**
 * @brief Cancels the thread.
 *
 * @param[in, out] the_thread The thread to cancel.
 * @param executing The currently executing thread.
 * @param exit_value The exit value for the thread.
 */
void _Thread_Cancel(
  Thread_Control *the_thread,
  Thread_Control *executing,
  void           *exit_value
);

typedef struct {
  Thread_queue_Context  Base;
  Thread_Control       *cancel;
} Thread_Close_context;

/**
 * @brief Closes the thread.
 *
 * Closes the thread object and starts the thread termination sequence.  In
 * case the executing thread is not terminated, then this function waits until
 * the terminating thread reached the zombie state.
 *
 * @param the_thread The thread to close.
 * @param executing The currently executing thread.
 * @param[in, out] context The thread close context.
 */
void _Thread_Close(
  Thread_Control       *the_thread,
  Thread_Control       *executing,
  Thread_Close_context *context
);

/**
 * @brief Checks if the thread is ready.
 *
 * @param the_thread The thread to check if it is ready.
 *
 * @retval true The thread is currently in the ready state.
 * @retval false The thread is currently not ready.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_ready( const Thread_Control *the_thread )
{
  return _States_Is_ready( the_thread->current_state );
}

/**
 * @brief Clears the specified thread state without locking the lock context.
 *
 * In the case the previous state is a non-ready state and the next state is
 * the ready state, then the thread is unblocked by the scheduler.
 *
 * @param[in, out] the_thread The thread.
 * @param state The state to clear.  It must not be zero.
 *
 * @return The thread's previous state.
 */
States_Control _Thread_Clear_state_locked(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 * @brief Clears the specified thread state.
 *
 * In the case the previous state is a non-ready state and the next state is
 * the ready state, then the thread is unblocked by the scheduler.
 *
 * @param[in, out] the_thread The thread.
 * @param state The state to clear.  It must not be zero.
 *
 * @return The previous state.
 */
States_Control _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 * @brief Sets the specified thread state without locking the lock context.
 *
 * In the case the previous state is the ready state, then the thread is blocked
 * by the scheduler.
 *
 * @param[in, out] the_thread The thread.
 * @param state The state to set.  It must not be zero.
 *
 * @return The previous state.
 */
States_Control _Thread_Set_state_locked(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 * @brief Sets the specified thread state.
 *
 * In the case the previous state is the ready state, then the thread is blocked
 * by the scheduler.
 *
 * @param[in, out] the_thread The thread.
 * @param state The state to set.  It must not be zero.
 *
 * @return The previous state.
 */
States_Control _Thread_Set_state(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 * @brief Initializes enviroment for a thread.
 *
 * This routine initializes the context of @a the_thread to its
 * appropriate starting state.
 *
 * @param[in, out] the_thread The pointer to the thread control block.
 */
void _Thread_Load_environment(
  Thread_Control *the_thread
);

/**
 * @brief Calls the start kinds idle entry of the thread.
 *
 * @param executing The currently executing thread.
 */
void _Thread_Entry_adaptor_idle( Thread_Control *executing );

/**
 * @brief Calls the start kinds numeric entry of the thread.
 *
 * @param executing The currently executing thread.
 */
void _Thread_Entry_adaptor_numeric( Thread_Control *executing );

/**
 * @brief Calls the start kinds pointer entry of the thread.
 *
 * Stores the return value in the Wait.return_argument of the thread.
 *
 * @param executing The currently executing thread.
 */
void _Thread_Entry_adaptor_pointer( Thread_Control *executing );

/**
 * @brief Wrapper function for all threads.
 *
 * This routine is the wrapper function for all threads.  It is
 * the starting point for all threads.  The user provided thread
 * entry point is invoked by this routine.  Operations
 * which must be performed immediately before and after the user's
 * thread executes are found here.
 *
 * @note On entry, it is assumed all interrupts are blocked and that this
 * routine needs to set the initial isr level.  This may or may not
 * actually be needed by the context switch routine and as a result
 * interrupts may already be at there proper level.  Either way,
 * setting the initial isr level properly here is safe.
 */
void _Thread_Handler( void );

/**
 * @brief Acquires the lock context in a critical section.
 *
 * @param the_thread The thread to acquire the lock context.
 * @param lock_context The lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_State_acquire_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Do_acquire_critical( &the_thread->Join_queue, lock_context );
}

/**
 * @brief Disables interrupts and acquires the lock_context.
 *
 * @param the_thread The thread to acquire the lock context.
 * @param lock_context The lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_State_acquire(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable( lock_context );
  _Thread_State_acquire_critical( the_thread, lock_context );
}

/**
 * @brief Disables interrupts and acquires the lock context for the currently
 *      executing thread.
 *
 * @param lock_context The lock context.
 *
 * @return The currently executing thread.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_State_acquire_for_executing(
  ISR_lock_Context *lock_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( lock_context );
  executing = _Thread_Executing;
  _Thread_State_acquire_critical( executing, lock_context );

  return executing;
}

/**
 * @brief Release the lock context in a critical section.
 *
 * @param the_thread The thread to release the lock context.
 * @param lock_context The lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_State_release_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Do_release_critical( &the_thread->Join_queue, lock_context );
}

/**
 * @brief Releases the lock context and enables interrupts.
 *
 * @param[in, out] the_thread The thread to release the lock context.
 * @param[out] lock_context The lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_State_release(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_State_release_critical( the_thread, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

/**
 * @brief Checks if the thread is owner of the lock of the join queue.
 *
 * @param the_thread The thread for the verification.
 *
 * @retval true The thread is owner of the lock of the join queue.
 * @retval false The thread is not owner of the lock of the join queue.
 */
#if defined(RTEMS_DEBUG)
RTEMS_INLINE_ROUTINE bool _Thread_State_is_owner(
  const Thread_Control *the_thread
)
{
  return _Thread_queue_Is_lock_owner( &the_thread->Join_queue );
}
#endif

/**
 * @brief Performs the priority actions specified by the thread queue context
 * along the thread queue path.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param start_of_path The start thread of the thread queue path.
 * @param queue_context The thread queue context specifying the thread queue
 *   path and initial thread priority actions.
 *
 * @see _Thread_queue_Path_acquire_critical().
 */
void _Thread_Priority_perform_actions(
  Thread_Control       *start_of_path,
  Thread_queue_Context *queue_context
);

/**
 * @brief Adds the specified thread priority node to the corresponding thread
 * priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param priority_node The thread priority node to add.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
void _Thread_Priority_add(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
);

/**
 * @brief Removes the specified thread priority node from the corresponding
 * thread priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param priority_node The thread priority node to remove.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
void _Thread_Priority_remove(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Thread_queue_Context *queue_context
);

/**
 * @brief Propagates a thread priority value change in the specified thread
 * priority node to the corresponding thread priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param[out] priority_node The thread priority node to change.
 * @param prepend_it In case this is true, then the thread is prepended to
 *   its priority group in its home scheduler instance, otherwise it is
 *   appended.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
void _Thread_Priority_changed(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  bool                  prepend_it,
  Thread_queue_Context *queue_context
);

/**
 * @brief Changes the thread priority value of the specified thread priority
 * node in the corresponding thread priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param[out] priority_node The thread priority node to change.
 * @param new_priority The new thread priority value of the thread priority
 *   node to change.
 * @param prepend_it In case this is true, then the thread is prepended to
 *   its priority group in its home scheduler instance, otherwise it is
 *   appended.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Priority_change(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Priority_Control      new_priority,
  bool                  prepend_it,
  Thread_queue_Context *queue_context
)
{
  _Priority_Node_set_priority( priority_node, new_priority );
  _Thread_Priority_changed(
    the_thread,
    priority_node,
    prepend_it,
    queue_context
  );
}

/**
 * @brief Replaces the victim priority node with the replacement priority node
 * in the corresponding thread priority aggregation.
 *
 * The caller must be the owner of the thread wait lock.
 *
 * @param the_thread The thread.
 * @param victim_node The victim thread priority node.
 * @param replacement_node The replacement thread priority node.
 *
 * @see _Thread_Wait_acquire().
 */
void _Thread_Priority_replace(
  Thread_Control *the_thread,
  Priority_Node  *victim_node,
  Priority_Node  *replacement_node
);

/**
 * @brief Updates the priority of all threads in the set
 *
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Priority_add(), _Thread_Priority_change(),
 *   _Thread_Priority_changed() and _Thread_Priority_remove().
 */
void _Thread_Priority_update( Thread_queue_Context *queue_context );

/**
 * @brief Updates the priority of the thread and changes it sticky level.
 *
 * @param the_thread The thread.
 * @param sticky_level_change The new value for the sticky level.
 */
#if defined(RTEMS_SMP)
void _Thread_Priority_and_sticky_update(
  Thread_Control *the_thread,
  int             sticky_level_change
);
#endif

/**
 * @brief Checks if the left thread priority is less than the right thread
 *      priority in the intuitive sense of priority.
 *
 * @param left The left thread priority.
 * @param right The right thread priority.
 *
 * @retval true The left priority is less in the intuitive sense.
 * @retval false The left priority is greater or equal in the intuitive sense.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Priority_less_than(
  Priority_Control left,
  Priority_Control right
)
{
  return left > right;
}

/**
 * @brief Returns the highest priority of the left and right thread priorities
 * in the intuitive sense of priority.
 *
 * @param left The left thread priority.
 * @param right The right thread priority.
 *
 * @return The highest priority in the intuitive sense of priority.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Thread_Priority_highest(
  Priority_Control left,
  Priority_Control right
)
{
  return _Thread_Priority_less_than( left, right ) ? right : left;
}

/**
 * @brief Gets object information for the object id.
 *
 * @param id The id of the object information.
 *
 * @retval pointer The object information for this id.
 * @retval NULL The object id is not valid.
 */
RTEMS_INLINE_ROUTINE Objects_Information *_Thread_Get_objects_information(
  Objects_Id id
)
{
  uint32_t the_api;

  the_api = _Objects_Get_API( id );

  if ( !_Objects_Is_api_valid( the_api ) ) {
    return NULL;
  }

  /*
   * Threads are always first class :)
   *
   * There is no need to validate the object class of the object identifier,
   * since this will be done by the object get methods.
   */
  return _Objects_Information_table[ the_api ][ 1 ];
}

/**
 * @brief Gets a thread by its identifier.
 *
 * @see _Objects_Get().
 *
 * @param id The id of the thread.
 * @param lock_context The lock context.
 */
Thread_Control *_Thread_Get(
  Objects_Id         id,
  ISR_lock_Context  *lock_context
);

/**
 * @brief Gets the cpu of the thread's scheduler.
 *
 * @param thread The thread.
 *
 * @return The cpu of the thread's scheduler.
 */
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

/**
 * @brief Sets the cpu of the thread's scheduler.
 *
 * @param[out] thread The thread.
 * @param cpu The cpu to set.
 */
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
 * @brief Checks if the thread is the currently executing thread.
 *
 * This function returns true if the_thread is the currently executing
 * thread, and false otherwise.
 *
 * @param the_thread The thread to verify if it is the currently executing thread.
 *
 * @retval true @a the_thread is the currently executing one.
 * @retval false @a the_thread is not the currently executing one.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_executing (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Executing );
}

#if defined(RTEMS_SMP)
/**
 * @brief Checks if the thread executes currently on some processor in the
 * system.
 *
 * Do not confuse this with _Thread_Is_executing() which checks only the
 * current processor.
 *
 * @param the_thread The thread for the verification.
 *
 * @retval true @a the_thread is the currently executing one.
 * @retval false @a the_thread is not the currently executing one.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_executing_on_a_processor(
  const Thread_Control *the_thread
)
{
  return _CPU_Context_Get_is_executing( &the_thread->Registers );
}
#endif

/**
 * @brief Checks if the thread is the heir.
 *
 * This function returns true if the_thread is the heir
 * thread, and false otherwise.
 *
 * @param the_thread The thread for the verification.
 *
 * @retval true @a the_thread is the heir.
 * @retval false @a the_thread is not the heir.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_heir (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Heir );
}

/**
 * @brief Unblocks the thread.
 *
 * This routine clears any blocking state for the_thread.  It performs
 * any necessary scheduling operations including the selection of
 * a new heir thread.
 *
 * @param[in, out] the_thread The thread to unblock.
 */
RTEMS_INLINE_ROUTINE void _Thread_Unblock (
  Thread_Control *the_thread
)
{
  _Thread_Clear_state( the_thread, STATES_BLOCKED );
}

/**
 * @brief Checks if the floating point context of the thread is currently
 *      loaded in the floating point unit.
 *
 * This function returns true if the floating point context of
 * the_thread is currently loaded in the floating point unit, and
 * false otherwise.
 *
 * @param the_thread The thread for the verification.
 *
 * @retval true The floating point context of @a the_thread is currently
 *      loaded in the floating point unit.
 * @retval false The floating point context of @a the_thread is currently not
 *      loaded in the floating point unit.
 */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
RTEMS_INLINE_ROUTINE bool _Thread_Is_allocated_fp (
  const Thread_Control *the_thread
)
{
  return ( the_thread == _Thread_Allocated_fp );
}
#endif

/*
 * If the CPU has hardware floating point, then we must address saving
 * and restoring it as part of the context switch.
 *
 * The second conditional compilation section selects the algorithm used
 * to context switch between floating point tasks.  The deferred algorithm
 * can be significantly better in a system with few floating point tasks
 * because it reduces the total number of save and restore FP context
 * operations.  However, this algorithm can not be used on all CPUs due
 * to unpredictable use of FP registers by some compilers for integer
 * operations.
 */

/**
 * @brief Saves the executing thread's floating point area.
 *
 * @param executing The currently executing thread.
 */
RTEMS_INLINE_ROUTINE void _Thread_Save_fp( Thread_Control *executing )
{
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH != TRUE )
  if ( executing->fp_context != NULL )
    _Context_Save_fp( &executing->fp_context );
#endif
#endif
}

/**
 * @brief Restores the executing thread's floating point area.
 *
 * @param executing The currently executing thread.
 */
RTEMS_INLINE_ROUTINE void _Thread_Restore_fp( Thread_Control *executing )
{
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
  if ( (executing->fp_context != NULL) &&
       !_Thread_Is_allocated_fp( executing ) ) {
    if ( _Thread_Allocated_fp != NULL )
      _Context_Save_fp( &_Thread_Allocated_fp->fp_context );
    _Context_Restore_fp( &executing->fp_context );
    _Thread_Allocated_fp = executing;
  }
#else
  if ( executing->fp_context != NULL )
    _Context_Restore_fp( &executing->fp_context );
#endif
#endif
}

/**
 * @brief Deallocates the currently loaded floating point context.
 *
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
 * @brief Checks if dispatching is disabled.
 *
 * This function returns true if dispatching is disabled, and false
 * otherwise.
 *
 * @retval true Dispatching is disabled.
 * @retval false Dispatching is enabled.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_context_switch_necessary( void )
{
  return ( _Thread_Dispatch_necessary );
}

/**
 * @brief Checks if the thread is NULL.
 *
 * @param the_thread The thread for the verification.
 *
 * @retval true The thread is @c NULL.
 * @retval false The thread is not @c NULL.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_null (
  const Thread_Control *the_thread
)
{
  return ( the_thread == NULL );
}

/**
 * @brief Checks if proxy is blocking.
 *
 * status which indicates that a proxy is blocking, and false otherwise.
 *
 * @param code The code for the verification.
 *
 * @retval true Status indicates that a proxy is blocking.
 * @retval false Status indicates that a proxy is not blocking.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_proxy_blocking (
  uint32_t   code
)
{
  return (code == THREAD_STATUS_PROXY_BLOCKING);
}

/**
 * @brief Gets the maximum number of internal threads.
 *
 * @return The maximum number of internal threads.
 */
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

/**
 * @brief Allocates an internal thread and returns it.
 *
 * @retval pointer Pointer to the allocated Thread_Control.
 * @retval NULL The operation failed.
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Internal_allocate( void )
{
  return (Thread_Control *)
    _Objects_Allocate_unprotected( &_Thread_Information.Objects );
}

/**
 * @brief Gets the heir of the processor and makes it executing.
 *
 * Must be called with interrupts disabled.  The thread dispatch necessary
 * indicator is cleared as a side-effect.
 *
 * @param[in, out] cpu_self The processor to get the heir of.
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

  heir = cpu_self->heir;
  cpu_self->dispatch_necessary = false;
  cpu_self->executing = heir;

  return heir;
}

/**
 * @brief Updates the cpu time used of the thread.
 *
 * @param[in, out] the_thread The thread to add additional cpu time that is
 *      used.
 * @param cpu The cpu.
 */
RTEMS_INLINE_ROUTINE void _Thread_Update_CPU_time_used(
  Thread_Control  *the_thread,
  Per_CPU_Control *cpu
)
{
  Timestamp_Control last;
  Timestamp_Control ran;

  last = cpu->cpu_usage_timestamp;
  _TOD_Get_uptime( &cpu->cpu_usage_timestamp );
  _Timestamp_Subtract( &last, &cpu->cpu_usage_timestamp, &ran );
  _Timestamp_Add_to( &the_thread->cpu_time_used, &ran );
}

/**
 * @brief Updates the used cpu time for the heir and dispatches a new heir.
 *
 * @param[in, out] cpu_self The current processor.
 * @param[in, out] cpu_for_heir The processor to do a dispatch on.
 * @param heir The new heir for @a cpu_for_heir.
 */
#if defined( RTEMS_SMP )
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_update_heir(
  Per_CPU_Control *cpu_self,
  Per_CPU_Control *cpu_for_heir,
  Thread_Control  *heir
)
{
  _Thread_Update_CPU_time_used( cpu_for_heir->heir, cpu_for_heir );

  cpu_for_heir->heir = heir;

  _Thread_Dispatch_request( cpu_self, cpu_for_heir );
}
#endif

/**
 * @brief Gets the used cpu time of the thread and stores it in the given
 *      Timestamp_Control.
 *
 * @param the_thread The thread to get the used cpu time of.
 * @param[out] cpu_time_used Stores the used cpu time of @a the_thread.
 */
void _Thread_Get_CPU_time_used(
  Thread_Control    *the_thread,
  Timestamp_Control *cpu_time_used
);

/**
 * @brief Initializes the control chain of the action control.
 *
 * @param[out] action_control The action control to initialize.
 */
RTEMS_INLINE_ROUTINE void _Thread_Action_control_initialize(
  Thread_Action_control *action_control
)
{
  _Chain_Initialize_empty( &action_control->Chain );
}

/**
 * @brief Initializes the Thread action.
 *
 * @param[out] action The Thread_Action to initialize.
 */
RTEMS_INLINE_ROUTINE void _Thread_Action_initialize(
  Thread_Action *action
)
{
  _Chain_Set_off_chain( &action->Node );
}

/**
 * @brief Adds a post switch action to the thread with the given handler.
 *
 * @param[in, out] the_thread The thread.
 * @param[in,  out] action The action to add.
 * @param handler The handler for the action.
 */
RTEMS_INLINE_ROUTINE void _Thread_Add_post_switch_action(
  Thread_Control        *the_thread,
  Thread_Action         *action,
  Thread_Action_handler  handler
)
{
  Per_CPU_Control *cpu_of_thread;

  _Assert( _Thread_State_is_owner( the_thread ) );

  cpu_of_thread = _Thread_Get_CPU( the_thread );

  action->handler = handler;

  _Thread_Dispatch_request( _Per_CPU_Get(), cpu_of_thread );

  _Chain_Append_if_is_off_chain_unprotected(
    &the_thread->Post_switch_actions.Chain,
    &action->Node
  );
}

/**
 * @brief Checks if the thread life state is restarting.
 *
 * @param life_state The thread life state for the verification.
 *
 * @retval true @a life_state is restarting.
 * @retval false @a life_state is not restarting.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_life_restarting(
  Thread_Life_state life_state
)
{
  return ( life_state & THREAD_LIFE_RESTARTING ) != 0;
}

/**
 * @brief Checks if the thread life state is terminating.
 *
 * @param life_state The thread life state for the verification.
 *
 * @retval true @a life_state is terminating.
 * @retval false @a life_state is not terminating.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_life_terminating(
  Thread_Life_state life_state
)
{
  return ( life_state & THREAD_LIFE_TERMINATING ) != 0;
}

/**
 * @brief Checks if the thread life state allos life change.
 *
 * @param life_state The thread life state for the verification.
 *
 * @retval true @a life_state allows life change.
 * @retval false @a life_state does not allow life change.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_life_change_allowed(
  Thread_Life_state life_state
)
{
  return ( life_state
    & ( THREAD_LIFE_PROTECTED | THREAD_LIFE_CHANGE_DEFERRED ) ) == 0;
}

/**
 * @brief Checks if the thread life state is life changing.
 *
 * @param life_state The thread life state for the verification.
 *
 * @retval true @a life_state is life changing.
 * @retval false @a life_state is not life changing.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_life_changing(
  Thread_Life_state life_state
)
{
  return ( life_state
    & ( THREAD_LIFE_RESTARTING | THREAD_LIFE_TERMINATING ) ) != 0;
}

/**
 * @brief Checks if the thread is joinable.
 *
 * @param the_thread The thread for the verification.
 *
 * @retval true @a life_state is joinable.
 * @retval false @a life_state is not joinable.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Is_joinable(
  const Thread_Control *the_thread
)
{
  _Assert( _Thread_State_is_owner( the_thread ) );
  return ( the_thread->Life.state & THREAD_LIFE_DETACHED ) == 0;
}

/**
 * @brief Increments the thread's resource count.
 *
 * @param[in, out] the_thread The thread to increase the resource count of.
 */
RTEMS_INLINE_ROUTINE void _Thread_Resource_count_increment(
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  ++the_thread->resource_count;
#else
  (void) the_thread;
#endif
}

/**
 * @brief Decrements the thread's resource count.
 *
 * @param[in, out] the_thread The thread to decrement the resource count of.
 */
RTEMS_INLINE_ROUTINE void _Thread_Resource_count_decrement(
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
  --the_thread->resource_count;
#else
  (void) the_thread;
#endif
}

#if defined(RTEMS_SCORE_THREAD_ENABLE_RESOURCE_COUNT)
/**
 * @brief Checks if the thread owns resources.
 *
 * Resources are accounted with the Thread_Control::resource_count resource
 * counter.  This counter is used by mutex objects for example.
 *
 * @param the_thread The thread.
 *
 * @retval true The thread owns resources.
 * @retval false The thread does not own resources.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Owns_resources(
  const Thread_Control *the_thread
)
{
  return the_thread->resource_count != 0;
}
#endif

#if defined(RTEMS_SMP)
/**
 * @brief Cancels the thread's need for help.
 *
 * @param the_thread The thread to cancel the help request of.
 * @param cpu The cpu to get the lock context of in order to
 *      cancel the help request.
 */
RTEMS_INLINE_ROUTINE void _Thread_Scheduler_cancel_need_for_help(
  Thread_Control  *the_thread,
  Per_CPU_Control *cpu
)
{
  ISR_lock_Context lock_context;

  _Per_CPU_Acquire( cpu, &lock_context );

  if ( !_Chain_Is_node_off_chain( &the_thread->Scheduler.Help_node ) ) {
    _Chain_Extract_unprotected( &the_thread->Scheduler.Help_node );
    _Chain_Set_off_chain( &the_thread->Scheduler.Help_node );
  }

  _Per_CPU_Release( cpu, &lock_context );
}
#endif

/**
 * @brief Gets the home scheduler of the thread.
 *
 * @param the_thread The thread to get the home scheduler of.
 *
 * @return The thread's home scheduler.
 */
RTEMS_INLINE_ROUTINE const Scheduler_Control *_Thread_Scheduler_get_home(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return the_thread->Scheduler.home_scheduler;
#else
  (void) the_thread;
  return &_Scheduler_Table[ 0 ];
#endif
}

/**
 * @brief Gets the scheduler's home node.
 *
 * @param the_thread The thread to get the home node of.
 *
 * @return The thread's home node.
 */
RTEMS_INLINE_ROUTINE Scheduler_Node *_Thread_Scheduler_get_home_node(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  _Assert( !_Chain_Is_empty( &the_thread->Scheduler.Wait_nodes ) );
  return SCHEDULER_NODE_OF_THREAD_WAIT_NODE(
    _Chain_First( &the_thread->Scheduler.Wait_nodes )
  );
#else
  return the_thread->Scheduler.nodes;
#endif
}

/**
 * @brief Gets the thread's scheduler node by index.
 *
 * @param the_thread The thread of which to get a scheduler node.
 * @param scheduler_index The index of the desired scheduler node.
 *
 * @return The scheduler node with the specified index.
 */
RTEMS_INLINE_ROUTINE Scheduler_Node *_Thread_Scheduler_get_node_by_index(
  const Thread_Control *the_thread,
  size_t                scheduler_index
)
{
#if defined(RTEMS_SMP)
  return (Scheduler_Node *)
    ( (uintptr_t) the_thread->Scheduler.nodes
      + scheduler_index * _Scheduler_Node_size );
#else
  _Assert( scheduler_index == 0 );
  (void) scheduler_index;
  return the_thread->Scheduler.nodes;
#endif
}

#if defined(RTEMS_SMP)
/**
 * @brief Acquires the lock context in a critical section.
 *
 * @param the_thread The thread to acquire the lock context.
 * @param lock_context The lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_Scheduler_acquire_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Acquire( &the_thread->Scheduler.Lock, lock_context );
}

/**
 * @brief Releases the lock context in a critical section.
 *
 * @param the_thread The thread to release the lock context.
 * @param lock_context The lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_Scheduler_release_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release( &the_thread->Scheduler.Lock, lock_context );
}

/**
 * @brief Process the thread's scheduler requests.
 *
 * @param[in, out] the_thread The thread for the operation.
 */
void _Thread_Scheduler_process_requests( Thread_Control *the_thread );

/**
 * @brief Add a scheduler request to the thread.
 *
 * @param[in, out] the_thread The thread to add a scheduler request to.
 * @param[in, out] scheduler_node The scheduler node for the request.
 * @param request The request to add.
 */
RTEMS_INLINE_ROUTINE void _Thread_Scheduler_add_request(
  Thread_Control         *the_thread,
  Scheduler_Node         *scheduler_node,
  Scheduler_Node_request  request
)
{
  ISR_lock_Context       lock_context;
  Scheduler_Node_request current_request;

  _Thread_Scheduler_acquire_critical( the_thread, &lock_context );

  current_request = scheduler_node->Thread.request;

  if ( current_request == SCHEDULER_NODE_REQUEST_NOT_PENDING ) {
    _Assert(
      request == SCHEDULER_NODE_REQUEST_ADD
        || request == SCHEDULER_NODE_REQUEST_REMOVE
    );
    _Assert( scheduler_node->Thread.next_request == NULL );
    scheduler_node->Thread.next_request = the_thread->Scheduler.requests;
    the_thread->Scheduler.requests = scheduler_node;
  } else if ( current_request != SCHEDULER_NODE_REQUEST_NOTHING ) {
    _Assert(
      ( current_request == SCHEDULER_NODE_REQUEST_ADD
        && request == SCHEDULER_NODE_REQUEST_REMOVE )
      || ( current_request == SCHEDULER_NODE_REQUEST_REMOVE
        && request == SCHEDULER_NODE_REQUEST_ADD )
    );
    request = SCHEDULER_NODE_REQUEST_NOTHING;
  }

  scheduler_node->Thread.request = request;

  _Thread_Scheduler_release_critical( the_thread, &lock_context );
}

/**
 * @brief Adds a wait node to the thread and adds a corresponding
 *      request to the thread.
 *
 * @param[in, out] the_thread The thread to add the wait node to.
 * @param scheduler_node The scheduler node which provides the wait node.
 */
RTEMS_INLINE_ROUTINE void _Thread_Scheduler_add_wait_node(
  Thread_Control *the_thread,
  Scheduler_Node *scheduler_node
)
{
  _Chain_Append_unprotected(
    &the_thread->Scheduler.Wait_nodes,
    &scheduler_node->Thread.Wait_node
  );
  _Thread_Scheduler_add_request(
    the_thread,
    scheduler_node,
    SCHEDULER_NODE_REQUEST_ADD
  );
}

/**
 * @brief Remove a wait node from the thread and add a corresponding request to
 *      it.
 *
 * @param the_thread The thread to add the request to remove a wait node.
 * @param scheduler_node The scheduler node to remove a wait node from.
 */
RTEMS_INLINE_ROUTINE void _Thread_Scheduler_remove_wait_node(
  Thread_Control *the_thread,
  Scheduler_Node *scheduler_node
)
{
  _Chain_Extract_unprotected( &scheduler_node->Thread.Wait_node );
  _Thread_Scheduler_add_request(
    the_thread,
    scheduler_node,
    SCHEDULER_NODE_REQUEST_REMOVE
  );
}
#endif

/**
 * @brief Returns the priority of the thread.
 *
 * Returns the user API and thread wait information relevant thread priority.
 * This includes temporary thread priority adjustments due to locking
 * protocols, a job release or the POSIX sporadic server for example.
 *
 * @param the_thread The thread of which to get the priority.
 *
 * @return The priority of the thread.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Thread_Get_priority(
  const Thread_Control *the_thread
)
{
  Scheduler_Node *scheduler_node;

  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  return _Priority_Get_priority( &scheduler_node->Wait.Priority );
}

/**
 * @brief Returns the unmapped priority of the thread.
 *
 * @param the_thread The thread of which to get the unmapped priority.
 *
 * @return The unmapped priority of the thread.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Thread_Get_unmapped_priority(
  const Thread_Control *the_thread
)
{
  return SCHEDULER_PRIORITY_UNMAP( _Thread_Get_priority( the_thread ) );
}

/**
 * @brief Returns the unmapped real priority of the thread.
 *
 * @param the_thread The thread of which to get the unmapped real priority.
 *
 * @return The unmapped real priority of the thread.
 */
RTEMS_INLINE_ROUTINE Priority_Control _Thread_Get_unmapped_real_priority(
  const Thread_Control *the_thread
)
{
  return SCHEDULER_PRIORITY_UNMAP( the_thread->Real_priority.priority );
}

/**
 * @brief Acquires the thread wait default lock inside a critical section
 * (interrupts disabled).
 *
 * @param[in, out] the_thread The thread.
 * @param lock_context The lock context used for the corresponding lock
 *   release.
 *
 * @see _Thread_Wait_release_default_critical().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire_default_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Acquire( &the_thread->Wait.Lock.Default, lock_context );
}

/**
 * @brief Acquires the thread wait default lock and returns the executing
 * thread.
 *
 * @param lock_context The lock context used for the corresponding lock
 *   release.
 *
 * @return The executing thread.
 *
 * @see _Thread_Wait_release_default().
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Wait_acquire_default_for_executing(
  ISR_lock_Context *lock_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( lock_context );
  executing = _Thread_Executing;
  _Thread_Wait_acquire_default_critical( executing, lock_context );

  return executing;
}

/**
 * @brief Acquires the thread wait default lock and disables interrupts.
 *
 * @param[in, out] the_thread The thread.
 * @param[out] lock_context The lock context used for the corresponding lock
 *   release.
 *
 * @see _Thread_Wait_release_default().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire_default(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable( lock_context );
  _Thread_Wait_acquire_default_critical( the_thread, lock_context );
}

/**
 * @brief Releases the thread wait default lock inside a critical section
 * (interrupts disabled).
 *
 * The previous interrupt status is not restored.
 *
 * @param[in, out] the_thread The thread.
 * @param lock_context The lock context used for the corresponding lock
 *   acquire.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release_default_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release( &the_thread->Wait.Lock.Default, lock_context );
}

/**
 * @brief Releases the thread wait default lock and restores the previous
 * interrupt status.
 *
 * @param[in, out] the_thread The thread.
 * @param[out] lock_context The lock context used for the corresponding lock
 *   acquire.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release_default(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_Wait_release_default_critical( the_thread, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

#if defined(RTEMS_SMP)
#define THREAD_QUEUE_CONTEXT_OF_REQUEST( node ) \
  RTEMS_CONTAINER_OF( node, Thread_queue_Context, Lock_context.Wait.Gate.Node )

/**
 * @brief Removes the first pending wait lock request.
 *
 * @param the_thread The thread to remove the request from.
 * @param queue_lock_context The queue lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_remove_request_locked(
  Thread_Control            *the_thread,
  Thread_queue_Lock_context *queue_lock_context
)
{
  Chain_Node *first;

  _Chain_Extract_unprotected( &queue_lock_context->Wait.Gate.Node );
  first = _Chain_First( &the_thread->Wait.Lock.Pending_requests );

  if ( first != _Chain_Tail( &the_thread->Wait.Lock.Pending_requests ) ) {
    _Thread_queue_Gate_open( (Thread_queue_Gate *) first );
  }
}

/**
 * @brief Acquires the wait queue inside a critical section.
 *
 * @param queue The queue that acquires.
 * @param queue_lock_context The queue lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire_queue_critical(
  Thread_queue_Queue        *queue,
  Thread_queue_Lock_context *queue_lock_context
)
{
  _Thread_queue_Queue_acquire_critical(
    queue,
    &_Thread_Executing->Potpourri_stats,
    &queue_lock_context->Lock_context
  );
}

/**
 * @brief Releases the wait queue inside a critical section.
 *
 * @param queue The queue that releases.
 * @param queue_lock_context The queue lock context.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release_queue_critical(
  Thread_queue_Queue        *queue,
  Thread_queue_Lock_context *queue_lock_context
)
{
  _Thread_queue_Queue_release_critical(
    queue,
    &queue_lock_context->Lock_context
  );
}
#endif

/**
 * @brief Acquires the thread wait lock inside a critical section (interrupts
 * disabled).
 *
 * @param[in, out] the_thread The thread.
 * @param[in, out] queue_context The thread queue context for the corresponding
 *   _Thread_Wait_release_critical().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire_critical(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
#if defined(RTEMS_SMP)
  Thread_queue_Queue *queue;

  _Thread_Wait_acquire_default_critical(
    the_thread,
    &queue_context->Lock_context.Lock_context
  );

  queue = the_thread->Wait.queue;
  queue_context->Lock_context.Wait.queue = queue;

  if ( queue != NULL ) {
    _Thread_queue_Gate_add(
      &the_thread->Wait.Lock.Pending_requests,
      &queue_context->Lock_context.Wait.Gate
    );
    _Thread_Wait_release_default_critical(
      the_thread,
      &queue_context->Lock_context.Lock_context
    );
    _Thread_Wait_acquire_queue_critical( queue, &queue_context->Lock_context );

    if ( queue_context->Lock_context.Wait.queue == NULL ) {
      _Thread_Wait_release_queue_critical(
        queue,
        &queue_context->Lock_context
      );
      _Thread_Wait_acquire_default_critical(
        the_thread,
        &queue_context->Lock_context.Lock_context
      );
      _Thread_Wait_remove_request_locked(
        the_thread,
        &queue_context->Lock_context
      );
      _Assert( the_thread->Wait.queue == NULL );
    }
  }
#else
  (void) the_thread;
  (void) queue_context;
#endif
}

/**
 * @brief Acquires the thread wait default lock and disables interrupts.
 *
 * @param[in, out] the_thread The thread.
 * @param[in, out] queue_context The thread queue context for the corresponding
 *   _Thread_Wait_release().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_acquire(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _ISR_lock_ISR_disable( &queue_context->Lock_context.Lock_context );
  _Thread_Wait_acquire_critical( the_thread, queue_context );
}

/**
 * @brief Releases the thread wait lock inside a critical section (interrupts
 * disabled).
 *
 * The previous interrupt status is not restored.
 *
 * @param[in, out] the_thread The thread.
 * @param[in, out] queue_context The thread queue context used for corresponding
 *   _Thread_Wait_acquire_critical().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release_critical(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
#if defined(RTEMS_SMP)
  Thread_queue_Queue *queue;

  queue = queue_context->Lock_context.Wait.queue;

  if ( queue != NULL ) {
    _Thread_Wait_release_queue_critical(
      queue, &queue_context->Lock_context
    );
    _Thread_Wait_acquire_default_critical(
      the_thread,
      &queue_context->Lock_context.Lock_context
    );
    _Thread_Wait_remove_request_locked(
      the_thread,
      &queue_context->Lock_context
    );
  }

  _Thread_Wait_release_default_critical(
    the_thread,
    &queue_context->Lock_context.Lock_context
  );
#else
  (void) the_thread;
  (void) queue_context;
#endif
}

/**
 * @brief Releases the thread wait lock and restores the previous interrupt
 * status.
 *
 * @param[in, out] the_thread The thread.
 * @param[in, out] queue_context The thread queue context used for corresponding
 *   _Thread_Wait_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_release(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  _Thread_Wait_release_critical( the_thread, queue_context );
  _ISR_lock_ISR_enable( &queue_context->Lock_context.Lock_context );
}

/**
 * @brief Claims the thread wait queue.
 *
 * The caller must not be the owner of the default thread wait lock.  The
 * caller must be the owner of the corresponding thread queue lock.  The
 * registration of the corresponding thread queue operations is deferred and
 * done after the deadlock detection.  This is crucial to support timeouts on
 * SMP configurations.
 *
 * @param[in, out] the_thread The thread.
 * @param[in, out] queue The new thread queue.
 *
 * @see _Thread_Wait_claim_finalize() and _Thread_Wait_restore_default().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_claim(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue
)
{
  ISR_lock_Context lock_context;

  _Thread_Wait_acquire_default_critical( the_thread, &lock_context );

  _Assert( the_thread->Wait.queue == NULL );

#if defined(RTEMS_SMP)
  _Chain_Initialize_empty( &the_thread->Wait.Lock.Pending_requests );
  _Chain_Initialize_node( &the_thread->Wait.Lock.Tranquilizer.Node );
  _Thread_queue_Gate_close( &the_thread->Wait.Lock.Tranquilizer );
#endif

  the_thread->Wait.queue = queue;

  _Thread_Wait_release_default_critical( the_thread, &lock_context );
}

/**
 * @brief Finalizes the thread wait queue claim via registration of the
 * corresponding thread queue operations.
 *
 * @param[in, out] the_thread The thread.
 * @param operations The corresponding thread queue operations.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_claim_finalize(
  Thread_Control                *the_thread,
  const Thread_queue_Operations *operations
)
{
  the_thread->Wait.operations = operations;
}

/**
 * @brief Removes a thread wait lock request.
 *
 * On SMP configurations, removes a thread wait lock request.
 *
 * On other configurations, this function does nothing.
 *
 * @param[in, out] the_thread The thread.
 * @param[in, out] queue_lock_context The thread queue lock context used for
 *   corresponding _Thread_Wait_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_remove_request(
  Thread_Control            *the_thread,
  Thread_queue_Lock_context *queue_lock_context
)
{
#if defined(RTEMS_SMP)
  ISR_lock_Context lock_context;

  _Thread_Wait_acquire_default( the_thread, &lock_context );
  _Thread_Wait_remove_request_locked( the_thread, queue_lock_context );
  _Thread_Wait_release_default( the_thread, &lock_context );
#else
  (void) the_thread;
  (void) queue_lock_context;
#endif
}

/**
 * @brief Restores the default thread wait queue and operations.
 *
 * The caller must be the owner of the current thread wait queue lock.
 *
 * On SMP configurations, the pending requests are updated to use the stale
 * thread queue operations.
 *
 * @param[in, out] the_thread The thread.
 *
 * @see _Thread_Wait_claim().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_restore_default(
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  ISR_lock_Context  lock_context;
  Chain_Node       *node;
  const Chain_Node *tail;

  _Thread_Wait_acquire_default_critical( the_thread, &lock_context );

  node = _Chain_First( &the_thread->Wait.Lock.Pending_requests );
  tail = _Chain_Immutable_tail( &the_thread->Wait.Lock.Pending_requests );

  if ( node != tail ) {
    do {
      Thread_queue_Context *queue_context;

      queue_context = THREAD_QUEUE_CONTEXT_OF_REQUEST( node );
      queue_context->Lock_context.Wait.queue = NULL;

      node = _Chain_Next( node );
    } while ( node != tail );

    _Thread_queue_Gate_add(
      &the_thread->Wait.Lock.Pending_requests,
      &the_thread->Wait.Lock.Tranquilizer
    );
  } else {
    _Thread_queue_Gate_open( &the_thread->Wait.Lock.Tranquilizer );
  }
#endif

  the_thread->Wait.queue = NULL;
  the_thread->Wait.operations = &_Thread_queue_Operations_default;

#if defined(RTEMS_SMP)
  _Thread_Wait_release_default_critical( the_thread, &lock_context );
#endif
}

/**
 * @brief Tranquilizes the thread after a wait on a thread queue.
 *
 * After the violent blocking procedure this function makes the thread calm and
 * peaceful again so that it can carry out its normal work.
 *
 * On SMP configurations, ensures that all pending thread wait lock requests
 * completed before the thread is able to begin a new thread wait procedure.
 *
 * On other configurations, this function does nothing.
 *
 * It must be called after a _Thread_Wait_claim() exactly once
 *  - after the corresponding thread queue lock was released, and
 *  - the default wait state is restored or some other processor is about to do
 *    this.
 *
 * @param the_thread The thread.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_tranquilize(
  Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  _Thread_queue_Gate_wait( &the_thread->Wait.Lock.Tranquilizer );
#else
  (void) the_thread;
#endif
}

/**
 * @brief Cancels a thread wait on a thread queue.
 *
 * @param[in, out] the_thread The thread.
 * @param queue_context The thread queue context used for corresponding
 *   _Thread_Wait_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_cancel(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  Thread_queue_Queue *queue;

  queue = the_thread->Wait.queue;

#if defined(RTEMS_SMP)
  if ( queue != NULL ) {
    _Assert( queue_context->Lock_context.Wait.queue == queue );
#endif

    ( *the_thread->Wait.operations->extract )(
      queue,
      the_thread,
      queue_context
    );
    _Thread_Wait_restore_default( the_thread );

#if defined(RTEMS_SMP)
    _Assert( queue_context->Lock_context.Wait.queue == NULL );
    queue_context->Lock_context.Wait.queue = queue;
  }
#endif
}

/**
 * @brief The initial thread wait flags value set by _Thread_Initialize().
 */
#define THREAD_WAIT_FLAGS_INITIAL 0x0U

/**
 * @brief Mask to get the thread wait state flags.
 */
#define THREAD_WAIT_STATE_MASK 0xffU

/**
 * @brief Indicates that the thread begins with the blocking operation.
 *
 * A blocking operation consists of an optional watchdog initialization and the
 * setting of the appropriate thread blocking state with the corresponding
 * scheduler block operation.
 */
#define THREAD_WAIT_STATE_INTEND_TO_BLOCK 0x1U

/**
 * @brief Indicates that the thread completed the blocking operation.
 */
#define THREAD_WAIT_STATE_BLOCKED 0x2U

/**
 * @brief Indicates that a condition to end the thread wait occurred.
 *
 * This could be a timeout, a signal, an event or a resource availability.
 */
#define THREAD_WAIT_STATE_READY_AGAIN 0x4U

/**
 * @brief Mask to get the thread wait class flags.
 */
#define THREAD_WAIT_CLASS_MASK 0xff00U

/**
 * @brief Indicates that the thread waits for an event.
 */
#define THREAD_WAIT_CLASS_EVENT 0x100U

/**
 * @brief Indicates that the thread waits for a system event.
 */
#define THREAD_WAIT_CLASS_SYSTEM_EVENT 0x200U

/**
 * @brief Indicates that the thread waits for an object.
 */
#define THREAD_WAIT_CLASS_OBJECT 0x400U

/**
 * @brief Indicates that the thread waits for a period.
 */
#define THREAD_WAIT_CLASS_PERIOD 0x800U

/**
 * @brief Sets the thread's wait flags.
 *
 * @param[in, out] the_thread The thread to set the wait flags of.
 * @param flags The flags to set.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_flags_set(
  Thread_Control    *the_thread,
  Thread_Wait_flags  flags
)
{
#if defined(RTEMS_SMP)
  _Atomic_Store_uint( &the_thread->Wait.flags, flags, ATOMIC_ORDER_RELAXED );
#else
  the_thread->Wait.flags = flags;
#endif
}

/**
 * @brief Gets the thread's wait flags according to the ATOMIC_ORDER_RELAXED.
 *
 * @param the_thread The thread to get the wait flags of.
 *
 * @return The thread's wait flags.
 */
RTEMS_INLINE_ROUTINE Thread_Wait_flags _Thread_Wait_flags_get(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return _Atomic_Load_uint( &the_thread->Wait.flags, ATOMIC_ORDER_RELAXED );
#else
  return the_thread->Wait.flags;
#endif
}

/**
 * @brief Gets the thread's wait flags according to the ATOMIC_ORDER_ACQUIRE.
 *
 * @param the_thread The thread to get the wait flags of.
 *
 * @return The thread's wait flags.
 */
RTEMS_INLINE_ROUTINE Thread_Wait_flags _Thread_Wait_flags_get_acquire(
  const Thread_Control *the_thread
)
{
#if defined(RTEMS_SMP)
  return _Atomic_Load_uint( &the_thread->Wait.flags, ATOMIC_ORDER_ACQUIRE );
#else
  return the_thread->Wait.flags;
#endif
}

/**
 * @brief Tries to change the thread wait flags with release semantics in case
 * of success.
 *
 * Must be called inside a critical section (interrupts disabled).
 *
 * In case the wait flags are equal to the expected wait flags, then the wait
 * flags are set to the desired wait flags.
 *
 * @param the_thread The thread.
 * @param expected_flags The expected wait flags.
 * @param desired_flags The desired wait flags.
 *
 * @retval true The wait flags were equal to the expected wait flags.
 * @retval false The wait flags were not equal to the expected wait flags.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Wait_flags_try_change_release(
  Thread_Control    *the_thread,
  Thread_Wait_flags  expected_flags,
  Thread_Wait_flags  desired_flags
)
{
  _Assert( _ISR_Get_level() != 0 );

#if defined(RTEMS_SMP)
  return _Atomic_Compare_exchange_uint(
    &the_thread->Wait.flags,
    &expected_flags,
    desired_flags,
    ATOMIC_ORDER_RELEASE,
    ATOMIC_ORDER_RELAXED
  );
#else
  bool success = ( the_thread->Wait.flags == expected_flags );

  if ( success ) {
    the_thread->Wait.flags = desired_flags;
  }

  return success;
#endif
}

/**
 * @brief Tries to change the thread wait flags with acquire semantics.
 *
 * In case the wait flags are equal to the expected wait flags, then the wait
 * flags are set to the desired wait flags.
 *
 * @param the_thread The thread.
 * @param expected_flags The expected wait flags.
 * @param desired_flags The desired wait flags.
 *
 * @retval true The wait flags were equal to the expected wait flags.
 * @retval false The wait flags were not equal to the expected wait flags.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Wait_flags_try_change_acquire(
  Thread_Control    *the_thread,
  Thread_Wait_flags  expected_flags,
  Thread_Wait_flags  desired_flags
)
{
#if defined(RTEMS_SMP)
  return _Atomic_Compare_exchange_uint(
    &the_thread->Wait.flags,
    &expected_flags,
    desired_flags,
    ATOMIC_ORDER_ACQUIRE,
    ATOMIC_ORDER_ACQUIRE
  );
#else
  bool      success;
  ISR_Level level;

  _ISR_Local_disable( level );

  success = _Thread_Wait_flags_try_change_release(
    the_thread,
    expected_flags,
    desired_flags
  );

  _ISR_Local_enable( level );
  return success;
#endif
}

/**
 * @brief Returns the object identifier of the object containing the current
 * thread wait queue.
 *
 * This function may be used for debug and system information purposes.  The
 * caller must be the owner of the thread lock.
 *
 * @param the_thread The thread.
 *
 * @retval 0 The thread waits on no thread queue currently, the thread wait
 *   queue is not contained in an object, or the current thread state provides
 *   insufficient information, e.g. the thread is in the middle of a blocking
 *   operation.
 * @retval other The object identifier of the object containing the thread wait
 *   queue.
 */
Objects_Id _Thread_Wait_get_id( const Thread_Control *the_thread );

/**
 * @brief Get the status of the wait return code of the thread.
 *
 * @param the_thread The thread to get the status of the wait return code of.
 */
RTEMS_INLINE_ROUTINE Status_Control _Thread_Wait_get_status(
  const Thread_Control *the_thread
)
{
  return (Status_Control) the_thread->Wait.return_code;
}

/**
 * @brief Cancels a blocking operation so that the thread can continue its
 * execution.
 *
 * In case this function actually cancelled the blocking operation, then the
 * thread wait return code is set to the specified status.
 *
 * A specialization of this function is _Thread_Timeout().
 *
 * @param[in, out] the_thread The thread.
 * @param status The thread wait status.
 */
void _Thread_Continue( Thread_Control *the_thread, Status_Control status );

/**
 * @brief General purpose thread wait timeout.
 *
 * @param the_watchdog The thread timer watchdog.
 */
void _Thread_Timeout( Watchdog_Control *the_watchdog );

/**
 * @brief Initializes the thread timer.
 *
 * @param [in, out] timer The timer to initialize.
 * @param cpu The cpu for the operation.
 */
RTEMS_INLINE_ROUTINE void _Thread_Timer_initialize(
  Thread_Timer_information *timer,
  Per_CPU_Control          *cpu
)
{
  _ISR_lock_Initialize( &timer->Lock, "Thread Timer" );
  timer->header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];
  _Watchdog_Preinitialize( &timer->Watchdog, cpu );
}

/**
 * @brief Adds timeout ticks to the thread.
 *
 * @param[in, out] the_thread The thread to add the timeout ticks to.
 * @param cpu The cpu for the operation.
 * @param ticks The ticks to add to the timeout ticks.
 */
RTEMS_INLINE_ROUTINE void _Thread_Add_timeout_ticks(
  Thread_Control    *the_thread,
  Per_CPU_Control   *cpu,
  Watchdog_Interval  ticks
)
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &the_thread->Timer.Lock, &lock_context );

  the_thread->Timer.header =
    &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_TICKS ];
  the_thread->Timer.Watchdog.routine = _Thread_Timeout;
  _Watchdog_Per_CPU_insert_ticks( &the_thread->Timer.Watchdog, cpu, ticks );

  _ISR_lock_Release_and_ISR_enable( &the_thread->Timer.Lock, &lock_context );
}

/**
 * @brief Inserts the cpu's watchdog realtime into the thread's timer.
 *
 * @param[in, out] the_thread for the operation.
 * @param cpu The cpu to get the watchdog header from.
 * @param routine The watchdog routine for the thread.
 * @param expire Expiration for the watchdog.
 */
RTEMS_INLINE_ROUTINE void _Thread_Timer_insert_realtime(
  Thread_Control                 *the_thread,
  Per_CPU_Control                *cpu,
  Watchdog_Service_routine_entry  routine,
  uint64_t                        expire
)
{
  ISR_lock_Context  lock_context;
  Watchdog_Header  *header;

  _ISR_lock_ISR_disable_and_acquire( &the_thread->Timer.Lock, &lock_context );

  header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_REALTIME ];
  the_thread->Timer.header = header;
  the_thread->Timer.Watchdog.routine = routine;
  _Watchdog_Per_CPU_insert( &the_thread->Timer.Watchdog, cpu, header, expire );

  _ISR_lock_Release_and_ISR_enable( &the_thread->Timer.Lock, &lock_context );
}

/**
 * @brief Remove the watchdog timer from the thread.
 *
 * @param[in, out] the_thread The thread to remove the watchdog from.
 */
RTEMS_INLINE_ROUTINE void _Thread_Timer_remove( Thread_Control *the_thread )
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &the_thread->Timer.Lock, &lock_context );

  _Watchdog_Per_CPU_remove(
    &the_thread->Timer.Watchdog,
#if defined(RTEMS_SMP)
    the_thread->Timer.Watchdog.cpu,
#else
    _Per_CPU_Get(),
#endif
    the_thread->Timer.header
  );

  _ISR_lock_Release_and_ISR_enable( &the_thread->Timer.Lock, &lock_context );
}

/**
 * @brief Remove the watchdog timer from the thread and unblock if necessary.
 *
 * @param[in, out] the_thread The thread to remove the watchdog from and unblock
 *      if necessary.
 * @param queue The thread queue.
 */
RTEMS_INLINE_ROUTINE void _Thread_Remove_timer_and_unblock(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue
)
{
  _Thread_Wait_tranquilize( the_thread );
  _Thread_Timer_remove( the_thread );

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Objects_Is_local_id( the_thread->Object.id ) ) {
    _Thread_Unblock( the_thread );
  } else {
    _Thread_queue_Unblock_proxy( queue, the_thread );
  }
#else
  (void) queue;
  _Thread_Unblock( the_thread );
#endif
}

/**
 * @brief Sets the name of the thread.
 *
 * @param[out] the_thread  The thread to change the name of.
 * @param name The new name for the thread.
 *
 * @retval STATUS_SUCCESSFUL The operation succeeded.
 * @retval STATUS_RESULT_TOO_LARGE The name was too long.
 */
Status_Control _Thread_Set_name(
  Thread_Control *the_thread,
  const char     *name
);

/**
 * @brief Gets the name of the thread.
 *
 * @param the_thread The thread to get the name of.
 * @param[out] buffer Contains the thread's name.
 * @param buffer_size The size of @a buffer.
 *
 * @return The number of bytes copied to @a buffer.
 */
size_t _Thread_Get_name(
  const Thread_Control *the_thread,
  char                 *buffer,
  size_t                buffer_size
);

#if defined(RTEMS_SMP)
#define THREAD_PIN_STEP 2

#define THREAD_PIN_PREEMPTION 1

/**
 * @brief Unpins the thread.
 *
 * @param executing The currently executing thread.
 * @param cpu_self The cpu for the operation.
 */
void _Thread_Do_unpin(
  Thread_Control  *executing,
  Per_CPU_Control *cpu_self
);
#endif

/**
 * @brief Pin the executing thread.
 *
 * @param executing The currently executing thread.
 */
RTEMS_INLINE_ROUTINE void _Thread_Pin( Thread_Control *executing )
{
#if defined(RTEMS_SMP)
  _Assert( executing == _Thread_Executing );

  executing->Scheduler.pin_level += THREAD_PIN_STEP;
#else
  (void) executing;
#endif
}

/**
 * @brief Unpins the thread.
 *
 * @param executing The currently executing thread.
 * @param cpu_self The cpu for the operation.
 */
RTEMS_INLINE_ROUTINE void _Thread_Unpin(
  Thread_Control  *executing,
  Per_CPU_Control *cpu_self
)
{
#if defined(RTEMS_SMP)
  unsigned int pin_level;

  _Assert( executing == _Thread_Executing );

  pin_level = executing->Scheduler.pin_level;
  _Assert( pin_level > 0 );

  if (
    RTEMS_PREDICT_TRUE(
      pin_level != ( THREAD_PIN_STEP | THREAD_PIN_PREEMPTION )
    )
  ) {
    executing->Scheduler.pin_level = pin_level - THREAD_PIN_STEP;
  } else {
    _Thread_Do_unpin( executing, cpu_self );
  }
#else
  (void) executing;
  (void) cpu_self;
#endif
}

/** @}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/threadmp.h>
#endif

#endif
/* end of include file */
