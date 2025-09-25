/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreThread which are only used by the implementation.
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  Copyright (C) 2014, 2017 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
 * @brief The thread zombie registry is used to register threads in the
 *   #STATES_ZOMBIE state.
 */
typedef struct {
#if defined(RTEMS_SMP)
  /**
   * @brief This lock protects the zombie chain.
   */
  ISR_lock_Control Lock;
#endif

  /**
   * @brief This chain contains the registered zombie threads.
   */
  Chain_Control Chain;
} Thread_Zombie_registry;

/**
 * @brief This object is a registry for threads in the #STATES_ZOMBIE state.
 *
 * The registry contains zombie threads waiting to get killed by
 * _Thread_Kill_zombies().  Use _Thread_Add_to_zombie_registry() to add zombie
 * threads to the registry.
 */
extern Thread_Zombie_registry _Thread_Zombies;

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
RTEMS_NO_RETURN void _Thread_Start_multitasking( void );

/**
 * @brief The configuration of a new thread to initialize.
 */
typedef struct {
  /**
   * @brief The scheduler control instance for the thread.
   */
  const struct _Scheduler_Control *scheduler;

  /**
   * @brief The starting address of the stack area.
   */
  void *stack_area;

  /**
   * @brief The size of the stack area in bytes.
   */
  size_t stack_size;

  /**
   * @brief This member contains the handler to free the stack.
   *
   * It shall not be NULL.  Use _Objects_Free_nothing() if nothing is to free.
   */
  void ( *stack_free )( void * );

  /**
   * @brief The new thread's priority.
   */
  Priority_Control priority;

  /**
   * @brief The thread's initial CPU budget operations.
   */
  const Thread_CPU_budget_operations *cpu_budget_operations;

  /**
   * @brief 32-bit unsigned integer name of the object for the thread.
   */
  uint32_t name;

  /**
   * @brief The thread's initial ISR level.
   */
  uint32_t isr_level;

  /**
   * @brief Indicates whether the thread needs a floating-point area.
   */
  bool is_fp;

  /**
   * @brief Indicates whether the new thread is preemptible.
   */
  bool is_preemptible;
} Thread_Configuration;

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
 * @param the_thread The thread to initialize.
 * @param config The configuration of the thread to initialize.
 *
 * @retval STATUS_SUCCESSFUL The thread initialization was successful.
 *
 * @retval STATUS_UNSATISFIED The thread initialization failed.
 */
Status_Control _Thread_Initialize(
  Thread_Information         *information,
  Thread_Control             *the_thread,
  const Thread_Configuration *config
);

/**
 * @brief Frees the thread.
 *
 * This routine invokes the thread delete extensions and frees all resources
 * associated with the thread.  Afterwards the thread object is closed.
 *
 * @param[in, out] information is the thread information.
 *
 * @param[in, out] the_thread is the thread to free.
 */
void _Thread_Free(
  Thread_Information *information,
  Thread_Control     *the_thread
);

/**
 * @brief Starts the specified thread.
 *
 * If the thread is not in the dormant state, the routine returns with a value
 * of false and performs no actions except enabling interrupts as indicated by
 * the ISR lock context.
 *
 * Otherwise, this routine initializes the executable information for the
 * thread and makes it ready to execute.  After the call of this routine, the
 * thread competes with all other ready threads for CPU time.
 *
 * Then the routine enables the local interrupts as indicated by the ISR lock
 * context.
 *
 * Then the thread start user extensions are called with thread dispatching
 * disabled and interrupts enabled after making the thread ready.  Please note
 * that in SMP configurations, the thread switch and begin user extensions may
 * be called in parallel on another processor.
 *
 * Then thread dispatching is enabled and other threads may execute before the
 * routine returns.
 *
 * @param[in, out] the_thread is the thread to start.
 *
 * @param entry is the thread entry information.
 *
 * @param[in, out] is the ISR lock context which shall be used to disable the
 *   local interrupts before the call of this routine.
 *
 * @retval STATUS_SUCCESSFUL The thread start was successful.
 *
 * @retval STATUS_INCORRECT_STATE The thread was already started.
 */
Status_Control _Thread_Start(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
);

/**
 * @brief Restarts the thread.
 *
 * @param[in, out] the_thread is the thread to restart.
 *
 * @param entry is the new start entry information for the thread to restart.
 *
 * @param[in, out] lock_context is the lock context with interrupts disabled.
 *
 * @retval STATUS_SUCCESSFUL The operation was successful.
 *
 * @retval STATUS_INCORRECT_STATE The thread was dormant.
 */
Status_Control _Thread_Restart(
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
 * @brief Changes the life of currently executing thread.
 *
 * @param life_states_to_clear are the thread life states to clear.
 *
 * @param life_states_to_set are the thread life states to set.
 *
 * @param ignored_life_states are the ignored thread life states.
 *
 * @return Returns the thread life state before the changes.
 */
Thread_Life_state _Thread_Change_life(
  Thread_Life_state life_states_to_clear,
  Thread_Life_state life_states_to_set,
  Thread_Life_state ignored_life_states
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
 * @param exit_value is the exit value of the thread.
 *
 * @param life_states_to_set are the thread life states to set.
 */
RTEMS_NO_RETURN void _Thread_Exit(
  void              *exit_value,
  Thread_Life_state  life_states_to_set
);

/**
 * @brief Joins the currently executing thread with the thread.
 *
 * @param[in, out] the_thread is the thread to join.
 *
 * @param waiting_for_join is the thread state for the currently executing
 *   thread.
 *
 * @param[in, out] executing is the currently executing thread.
 *
 * @param queue_context[in, out] is the thread queue context.  The caller shall
 *   have acquired the thread state lock using the thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The operation was successful.
 *
 * @retval STATUS_DEADLOCK A deadlock occurred.
 */
Status_Control _Thread_Join(
  Thread_Control       *the_thread,
  States_Control        waiting_for_join,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
);

/**
 * @brief Indicates the resulting state of _Thread_Cancel().
 */
typedef enum {
  /**
   * @brief Indicates that the thread cancel operation is done.
   */
  THREAD_CANCEL_DONE,

  /**
   * @brief Indicates that the thread cancel operation is in progress.
   *
   * The cancelled thread is terminating.  It may be joined.
   */
  THREAD_CANCEL_IN_PROGRESS
} Thread_Cancel_state;

/**
 * @brief Cancels the thread.
 *
 * @param[in, out] the_thread is the thread to cancel.

 * @param[in, out] executing is the currently executing thread.

 * @param[in, out] life_states_to_clear is the set of thread life states to
 *   clear for the thread to cancel.
 */
Thread_Cancel_state _Thread_Cancel(
  Thread_Control   *the_thread,
  Thread_Control   *executing,
  Thread_Life_state life_states_to_clear
);

/**
 * @brief Closes the thread.
 *
 * Closes the thread object and starts the thread termination sequence.  In
 * case the executing thread is not terminated, then this function waits until
 * the terminating thread reached the zombie state.
 *
 * @param the_thread is the thread to close.
 *
 * @param[in, out] executing is the currently executing thread.
 *
 * @param[in, out] queue_context is the thread queue context.  The caller shall
 *   have disabled interrupts using the thread queue context.
 *
 * @retval STATUS_SUCCESSFUL The operation was successful.
 *
 * @retval STATUS_DEADLOCK A deadlock occurred.
 */
Status_Control _Thread_Close(
  Thread_Control       *the_thread,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
);

/**
 * @brief Checks if the thread is ready.
 *
 * @param the_thread The thread to check if it is ready.
 *
 * @retval true The thread is currently in the ready state.
 * @retval false The thread is currently not ready.
 */
static inline bool _Thread_Is_ready( const Thread_Control *the_thread )
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
static inline void _Thread_State_acquire_critical(
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
static inline void _Thread_State_acquire(
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
static inline Thread_Control *_Thread_State_acquire_for_executing(
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
static inline void _Thread_State_release_critical(
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
static inline void _Thread_State_release(
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
static inline bool _Thread_State_is_owner(
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
 * @param priority_group_order The priority group order determines if the
 *   thread is inserted as the first or last node into the ready or scheduled
 *   queues of its home scheduler, see #PRIORITY_GROUP_FIRST and
 *   #PRIORITY_GROUP_LAST.
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
  Priority_Group_order  priority_group_order,
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
 * @param priority_group_order The priority group order determines if the
 *   thread is inserted as the first or last node into the ready or scheduled
 *   queues of its home scheduler, see #PRIORITY_GROUP_FIRST and
 *   #PRIORITY_GROUP_LAST.
 * @param queue_context The thread queue context to return an updated set of
 *   threads for _Thread_Priority_update().  The thread queue context must be
 *   initialized via _Thread_queue_Context_clear_priority_updates() before a
 *   call of this function.
 *
 * @see _Thread_Wait_acquire().
 */
static inline void _Thread_Priority_change(
  Thread_Control       *the_thread,
  Priority_Node        *priority_node,
  Priority_Control      new_priority,
  Priority_Group_order  priority_group_order,
  Thread_queue_Context *queue_context
)
{
  _Priority_Node_set_priority( priority_node, new_priority );

#if defined(RTEMS_SCORE_THREAD_REAL_PRIORITY_MAY_BE_INACTIVE)
  if ( !_Priority_Node_is_active( priority_node ) ) {
    /* The priority change is picked up once the node is added */
    return;
  }
#endif

  _Thread_Priority_changed(
    the_thread,
    priority_node,
    priority_group_order,
    queue_context
  );
}

#if defined(RTEMS_SMP)
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
#endif

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

#if defined(RTEMS_SMP)
/**
 * @brief Updates the priority of the thread and makes its home scheduler node
 *   sticky.
 *
 * @param the_thread is the thread to work on.
 */
void _Thread_Priority_update_and_make_sticky( Thread_Control *the_thread );

/**
 * @brief Updates the priority of the thread and cleans the sticky property of
 *   its home scheduler node.
 *
 * @param the_thread is the thread to work on.
 */
void _Thread_Priority_update_and_clean_sticky( Thread_Control *the_thread );

/**
 * @brief Updates the priority of the thread.
 *
 * @param the_thread is the thread to update the priority.
 */
void _Thread_Priority_update_ignore_sticky( Thread_Control *the_thread );
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
static inline bool _Thread_Priority_less_than(
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
static inline Priority_Control _Thread_Priority_highest(
  Priority_Control left,
  Priority_Control right
)
{
  return _Thread_Priority_less_than( left, right ) ? right : left;
}

/**
 * @brief Gets the thread object information for the API of the object
 *   identifier.
 *
 * @param id is an object identifier which defines the API to get the
 *   associated thread objects information.
 *
 * @retval NULL The object identifier had an invalid API.
 *
 * @return Returns the thread object information associated with the API of the
 *   object identifier.
 */
static inline Objects_Information *_Thread_Get_objects_information_by_id(
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
 * @brief Gets the thread object information of the thread.
 *
 * @param the_thread is the thread to get the thread object information.
 *
 * @return Returns the thread object information of the thread.
 */
static inline Thread_Information *_Thread_Get_objects_information(
  Thread_Control *the_thread
)
{
  size_t              the_api;
  Thread_Information *information;

  the_api = (size_t) _Objects_Get_API( the_thread->Object.id );
  _Assert( _Objects_Is_api_valid( the_api ) );

  information = (Thread_Information *)
    _Objects_Information_table[ the_api ][ 1 ];
  _Assert( information != NULL );

  return information;
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
 * @brief Gets the identifier of the calling thread.
 *
 * @return Returns the identifier of the calling thread.
 */
Objects_Id _Thread_Self_id( void );

/**
 * @brief Gets the cpu of the thread's scheduler.
 *
 * @param thread The thread.
 *
 * @return The cpu of the thread's scheduler.
 */
static inline Per_CPU_Control *_Thread_Get_CPU(
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
static inline void _Thread_Set_CPU(
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
static inline bool _Thread_Is_executing (
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
static inline bool _Thread_Is_executing_on_a_processor(
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
static inline bool _Thread_Is_heir (
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
static inline void _Thread_Unblock (
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
static inline bool _Thread_Is_allocated_fp (
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
static inline void _Thread_Save_fp( Thread_Control *executing )
{
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH != TRUE )
  if ( executing->fp_context != NULL )
    _Context_Save_fp( &executing->fp_context );
#else
  (void) executing;
#endif
#else
  (void) executing;
#endif
}

/**
 * @brief Restores the executing thread's floating point area.
 *
 * @param executing The currently executing thread.
 */
static inline void _Thread_Restore_fp( Thread_Control *executing )
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
#else
  (void) executing;
#endif
}

/**
 * @brief Deallocates the currently loaded floating point context.
 *
 * This routine is invoked when the currently loaded floating
 * point context is now longer associated with an active thread.
 */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
static inline void _Thread_Deallocate_fp( void )
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
static inline bool _Thread_Is_context_switch_necessary( void )
{
  return ( _Thread_Dispatch_necessary );
}

/**
 * @brief Gets the maximum number of internal threads.
 *
 * @return The maximum number of internal threads.
 */
static inline uint32_t _Thread_Get_maximum_internal_threads(void)
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
static inline Thread_Control *_Thread_Internal_allocate( void )
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
static inline Thread_Control *_Thread_Get_heir_and_make_it_executing(
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
static inline void _Thread_Update_CPU_time_used(
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
static inline void _Thread_Dispatch_update_heir(
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
 * @brief Gets the used processor time of the thread throughout its entire
 *   lifetime.
 *
 * @param[in, out] the_thread is the thread.
 *
 * @return Returns the used processor time of the thread throughout its entire
 *   lifetime.
 */
Timestamp_Control _Thread_Get_CPU_time_used( Thread_Control *the_thread );

/**
 * @brief Gets the used processor time of the thread throughout its entire
 *   lifetime if the caller already acquired the thread state and home
 *   scheduler locks.
 *
 * @param[in, out] the_thread is the thread.
 *
 * @return Returns the used processor time of the thread throughout its entire
 *   lifetime.
 */
Timestamp_Control _Thread_Get_CPU_time_used_locked(
  Thread_Control *the_thread
);

/**
 * @brief Gets the used processor time of the thread after the last CPU usage
 *   reset.
 *
 * @param[in, out] the_thread is the thread.
 *
 * @return Returns the used processor time of the thread after the last CPU usage
 *   reset.
 */
Timestamp_Control _Thread_Get_CPU_time_used_after_last_reset(
  Thread_Control *the_thread
);

/**
 * @brief Initializes the control chain of the action control.
 *
 * @param[out] action_control The action control to initialize.
 */
static inline void _Thread_Action_control_initialize(
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
static inline void _Thread_Action_initialize(
  Thread_Action *action
)
{
  _Chain_Set_off_chain( &action->Node );
}

/**
 * @brief Adds the post switch action to the thread.
 *
 * The caller shall own the thread state lock.  A thread dispatch is
 * requested.
 *
 * @param[in, out] the_thread is the thread of the action.
 *
 * @param[in, out] action is the action to add.
 *
 * @param handler is the handler for the action.
 */
static inline void _Thread_Add_post_switch_action(
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
 * @brief Appends the post switch action to the thread.
 *
 * The caller shall own the thread state lock.  The action shall be inactive.
 * The handler of the action shall be already set.  A thread dispatch is not
 * requested.
 *
 * @param[in, out] the_thread is the thread of the action.
 *
 * @param[in, out] action is the action to add.
 */
static inline void _Thread_Append_post_switch_action(
  Thread_Control *the_thread,
  Thread_Action  *action
)
{
  _Assert( _Thread_State_is_owner( the_thread ) );
  _Assert( action->handler != NULL );

  _Chain_Append_unprotected(
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
static inline bool _Thread_Is_life_restarting(
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
static inline bool _Thread_Is_life_terminating(
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
static inline bool _Thread_Is_life_change_allowed(
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
static inline bool _Thread_Is_life_changing(
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
static inline bool _Thread_Is_joinable(
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
static inline void _Thread_Resource_count_increment(
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
static inline void _Thread_Resource_count_decrement(
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
static inline bool _Thread_Owns_resources(
  const Thread_Control *the_thread
)
{
  return the_thread->resource_count != 0;
}
#endif

/**
 * @brief Gets the home scheduler of the thread.
 *
 * @param the_thread The thread to get the home scheduler of.
 *
 * @return The thread's home scheduler.
 */
static inline const Scheduler_Control *_Thread_Scheduler_get_home(
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
static inline Scheduler_Node *_Thread_Scheduler_get_home_node(
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
static inline Scheduler_Node *_Thread_Scheduler_get_node_by_index(
  const Thread_Control *the_thread,
  size_t                scheduler_index
)
{
  _Assert( scheduler_index < _Scheduler_Count );
#if defined(RTEMS_SMP)
  return (Scheduler_Node *)
    ( (uintptr_t) the_thread->Scheduler.nodes
      + scheduler_index * _Scheduler_Node_size );
#else
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
static inline void _Thread_Scheduler_acquire_critical(
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
static inline void _Thread_Scheduler_release_critical(
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
static inline void _Thread_Scheduler_add_request(
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
static inline void _Thread_Scheduler_add_wait_node(
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
static inline void _Thread_Scheduler_remove_wait_node(
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
static inline Priority_Control _Thread_Get_priority(
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
static inline Priority_Control _Thread_Get_unmapped_priority(
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
static inline Priority_Control _Thread_Get_unmapped_real_priority(
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
static inline void _Thread_Wait_acquire_default_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Acquire( &the_thread->Wait.Lock.Default, lock_context );
#ifndef RTEMS_SMP
  (void) the_thread;
#endif
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
static inline Thread_Control *_Thread_Wait_acquire_default_for_executing(
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
static inline void _Thread_Wait_acquire_default(
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
static inline void _Thread_Wait_release_default_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release( &the_thread->Wait.Lock.Default, lock_context );
#ifndef RTEMS_SMP
   (void) the_thread;
#endif
}

/**
 * @brief Releases the thread wait default lock and restores the previous
 * interrupt status.
 *
 * @param[in, out] the_thread The thread.
 * @param[out] lock_context The lock context used for the corresponding lock
 *   acquire.
 */
static inline void _Thread_Wait_release_default(
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
static inline void _Thread_Wait_remove_request_locked(
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
static inline void _Thread_Wait_acquire_queue_critical(
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
static inline void _Thread_Wait_release_queue_critical(
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
static inline void _Thread_Wait_acquire_critical(
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
static inline void _Thread_Wait_acquire(
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
static inline void _Thread_Wait_release_critical(
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
static inline void _Thread_Wait_release(
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
static inline void _Thread_Wait_claim(
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
static inline void _Thread_Wait_claim_finalize(
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
static inline void _Thread_Wait_remove_request(
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
static inline void _Thread_Wait_restore_default(
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
static inline void _Thread_Wait_tranquilize(
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
static inline void _Thread_Wait_cancel(
  Thread_Control       *the_thread,
  Thread_queue_Context *queue_context
)
{
  Thread_queue_Queue *queue;

  queue = the_thread->Wait.queue;

  if ( queue != NULL ) {
#if defined(RTEMS_SMP)
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
#endif
  }
}

/**
 * @brief Mask to get the thread wait state flags.
 */
#define THREAD_WAIT_STATE_MASK 0xffU

/**
 * @brief Indicates that the thread does not wait on something.
 *
 * In this wait state, the wait class is zero.  This wait state is set
 * initially by _Thread_Initialize() and after each wait operation once the
 * thread is ready again.
 */
#define THREAD_WAIT_STATE_READY 0x0U

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
static inline void _Thread_Wait_flags_set(
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
static inline Thread_Wait_flags _Thread_Wait_flags_get(
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
static inline Thread_Wait_flags _Thread_Wait_flags_get_acquire(
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
static inline bool _Thread_Wait_flags_try_change_release(
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
static inline bool _Thread_Wait_flags_try_change_acquire(
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
static inline Status_Control _Thread_Wait_get_status(
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
static inline void _Thread_Timer_initialize(
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
static inline void _Thread_Add_timeout_ticks(
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
static inline void _Thread_Timer_insert_realtime(
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
static inline void _Thread_Timer_remove( Thread_Control *the_thread )
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
static inline void _Thread_Remove_timer_and_unblock(
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
static inline void _Thread_Pin( Thread_Control *executing )
{
#if defined(RTEMS_SMP)
  _Assert( executing == _Thread_Get_executing() );

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
static inline void _Thread_Unpin(
  Thread_Control  *executing,
  Per_CPU_Control *cpu_self
)
{
#if defined(RTEMS_SMP)
  unsigned int pin_level;

  _Assert( executing == _Per_CPU_Get_executing( cpu_self ) );

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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @ingroup RTEMSScoreThread
 *
 * @brief Removes the watchdog timer from the thread and lets the thread
 *   continue its execution.
 *
 * @param[in, out] the_thread is the thread.
 *
 * @param status is the thread wait status.
 */
static inline void _Thread_Timer_remove_and_continue(
  Thread_Control *the_thread,
  Status_Control  status
)
{
  _Thread_Timer_remove( the_thread );
#if defined(RTEMS_MULTIPROCESSING)
  _Thread_MP_Extract_proxy( the_thread );
#endif
  _Thread_Continue( the_thread, status );
}

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
