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
#include <rtems/score/assert.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/resourceimpl.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threadqimpl.h>
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

#define THREAD_CHAIN_NODE_TO_THREAD( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, Wait.Node.Chain )

#define THREAD_RBTREE_NODE_TO_THREAD( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, Wait.Node.RBTree )

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
 *  @param[in] stack_size is the stack space is requested
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
 * @brief Clears the specified thread state.
 *
 * In case the previous state is a non-ready state and the next state is the
 * ready state, then the thread is unblocked by the scheduler.
 *
 * @param[in] the_thread The thread.
 * @param[in] state The state to clear.  It must not be zero.
 *
 * @return The previous state.
 */
States_Control _Thread_Clear_state(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 * @brief Sets the specified thread state.
 *
 * In case the previous state is the ready state, then the thread is blocked by
 * the scheduler.
 *
 * @param[in] the_thread The thread.
 * @param[in] state The state to set.  It must not be zero.
 *
 * @return The previous state.
 */
States_Control _Thread_Set_state(
  Thread_Control *the_thread,
  States_Control  state
);

/**
 * @brief Clears all thread states.
 *
 * In case the previous state is a non-ready state, then the thread is
 * unblocked by the scheduler.
 *
 * @param[in] the_thread The thread.
 */
RTEMS_INLINE_ROUTINE void _Thread_Ready(
  Thread_Control *the_thread
)
{
  _Thread_Clear_state( the_thread, STATES_ALL_SET );
}

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
 *  @param[in] ignored is not used
 */
void _Thread_Delay_ended(
  Objects_Id  id,
  void       *ignored
);

/**
 * @brief Returns true if the left thread priority is less than the right
 * thread priority in the intuitive sense of priority and false otherwise.
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
 */
RTEMS_INLINE_ROUTINE Priority_Control _Thread_Priority_highest(
  Priority_Control left,
  Priority_Control right
)
{
  return _Thread_Priority_less_than( left, right ) ? right : left;
}

/**
 * @brief Filters a thread priority change.
 *
 * Called by _Thread_Change_priority() under the protection of the thread lock.
 *
 * @param[in] the_thread The thread.
 * @param[in, out] new_priority The new priority of the thread.  The filter may
 * alter this value.
 * @param[in] arg The argument passed to _Thread_Change_priority().
 *
 * @retval true Change the current priority.
 * @retval false Otherwise.
 */
typedef bool ( *Thread_Change_priority_filter )(
  Thread_Control   *the_thread,
  Priority_Control *new_priority,
  void             *arg
);

/**
 * @brief Changes the priority of a thread if allowed by the filter function.
 *
 * It changes current priority of the thread to the new priority in case the
 * filter function returns true.  In this case the scheduler is notified of the
 * priority change as well.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_priority The new priority of the thread.
 * @param[in] arg The argument for the filter function.
 * @param[in] filter The filter function to determine if a priority change is
 * allowed and optionally perform other actions under the protection of the
 * thread lock simultaneously with the update of the current priority.
 * @param[in] prepend_it In case this is true, then the thread is prepended to
 * its priority group in its scheduler instance, otherwise it is appended.
 */
void _Thread_Change_priority(
  Thread_Control                *the_thread,
  Priority_Control               new_priority,
  void                          *arg,
  Thread_Change_priority_filter  filter,
  bool                           prepend_it
);

/**
 * @brief Raises the priority of a thread.
 *
 * It changes the current priority of the thread to the new priority if the new
 * priority is higher than the current priority.  In this case the thread is
 * appended to its new priority group in its scheduler instance.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_priority The new priority of the thread.
 *
 * @see _Thread_Change_priority().
 */
void _Thread_Raise_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority
);

/**
 * @brief Sets the current to the real priority of a thread.
 *
 * Sets the priority restore hint to false.
 */
void _Thread_Restore_priority( Thread_Control *the_thread );

/**
 * @brief Sets the priority of a thread.
 *
 * It sets the real priority of the thread.  In addition it changes the current
 * priority of the thread if the new priority is higher than the current
 * priority or the thread owns no resources.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_priority The new priority of the thread.
 * @param[out] old_priority The old real priority of the thread.  This pointer
 * must not be @c NULL.
 * @param[in] prepend_it In case this is true, then the thread is prepended to
 * its priority group in its scheduler instance, otherwise it is appended.
 *
 * @see _Thread_Change_priority().
 */
void _Thread_Set_priority(
  Thread_Control   *the_thread,
  Priority_Control  new_priority,
  Priority_Control *old_priority,
  bool              prepend_it
);

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
 * @brief Gets a thread by its identifier.
 *
 * @see _Objects_Get_isr_disable().
 */
Thread_Control *_Thread_Get_interrupt_disable(
  Objects_Id         id,
  Objects_Locations *location,
  ISR_lock_Context  *lock_context
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
 * @brief Returns @a true in case the thread executes currently on some
 * processor in the system, otherwise @a false.
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
 * @brief Returns @a true and sets time_of_context_switch to the
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

  _Giant_Release( _Per_CPU_Get() );

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

/*
 *  If the CPU has hardware floating point, then we must address saving
 *  and restoring it as part of the context switch.
 *
 *  The second conditional compilation section selects the algorithm used
 *  to context switch between floating point tasks.  The deferred algorithm
 *  can be significantly better in a system with few floating point tasks
 *  because it reduces the total number of save and restore FP context
 *  operations.  However, this algorithm can not be used on all CPUs due
 *  to unpredictable use of FP registers by some compilers for integer
 *  operations.
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
  Per_CPU_Control *cpu_of_thread;
  ISR_Level        level;

  cpu_of_thread = _Thread_Action_ISR_disable_and_acquire( thread, &level );
  cpu_of_thread->dispatch_necessary = true;

#if defined(RTEMS_SMP)
  if ( _Per_CPU_Get() != cpu_of_thread ) {
    _Per_CPU_Send_interrupt( cpu_of_thread );
  }
#endif

  _Chain_Append_if_is_off_chain_unprotected(
    &thread->Post_switch_actions.Chain,
    &action->Node
  );

  _Thread_Action_release_and_ISR_enable( cpu_of_thread, level );
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

/**
 * @brief Acquires the default thread lock and returns the executing thread.
 *
 * @param[in] lock_context The lock context used for the corresponding lock
 * release.
 *
 * @return The executing thread.
 *
 * @see _Thread_Lock_release_default().
 */
RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Lock_acquire_default_for_executing(
  ISR_lock_Context *lock_context
)
{
  Thread_Control *executing;

  _ISR_lock_ISR_disable( lock_context );
  executing = _Thread_Executing;
  _ISR_lock_Acquire( &executing->Lock.Default, lock_context );

  return executing;
}

/**
 * @brief Acquires the default thread lock inside a critical section
 * (interrupts disabled).
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for the corresponding lock
 * release.
 *
 * @see _Thread_Lock_release_default().
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_acquire_default_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Assert( _ISR_Get_level() != 0 );
  _ISR_lock_Acquire( &the_thread->Lock.Default, lock_context );
}

/**
 * @brief Acquires the default thread lock.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for the corresponding lock
 * release.
 *
 * @see _Thread_Lock_release_default().
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_acquire_default(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable_and_acquire( &the_thread->Lock.Default, lock_context );
}

/**
 * @brief Releases the thread lock inside a critical section (interrupts
 * disabled).
 *
 * The previous interrupt status is not restored.
 *
 * @param[in] lock The lock.
 * @param[in] lock_context The lock context used for the corresponding lock
 * acquire.
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_release_critical(
  ISR_lock_Control *lock,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release( lock, lock_context );
}

/**
 * @brief Releases the thread lock.
 *
 * @param[in] lock The lock returned by _Thread_Lock_acquire().
 * @param[in] lock_context The lock context used for _Thread_Lock_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_release(
  ISR_lock_Control *lock,
  ISR_lock_Context *lock_context
)
{
  _Thread_Lock_release_critical( lock, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

/**
 * @brief Releases the default thread lock inside a critical section
 * (interrupts disabled).
 *
 * The previous interrupt status is not restored.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for the corresponding lock
 * acquire.
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_release_default_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_Lock_release_critical(
#if defined(RTEMS_SMP)
    &the_thread->Lock.Default,
#else
    NULL,
#endif
    lock_context
  );
}

/**
 * @brief Releases the default thread lock.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context used for the corresponding lock
 * acquire.
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_release_default(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_Lock_release_default_critical( the_thread, lock_context );
  _ISR_lock_ISR_enable( lock_context );
}

/**
 * @brief Acquires the thread lock.
 *
 * @param[in] the_thread The thread.
 * @param[in] lock_context The lock context for _Thread_Lock_release().
 *
 * @return The lock required by _Thread_Lock_release().
 */
RTEMS_INLINE_ROUTINE ISR_lock_Control *_Thread_Lock_acquire(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
#if defined(RTEMS_SMP)
  ISR_lock_Control *lock;

  while ( true ) {
    uint32_t my_generation;

    _ISR_lock_ISR_disable( lock_context );
    my_generation = the_thread->Lock.generation;

    /*
     * Ensure that we read the initial lock generation before we obtain our
     * current lock.
     */
    _Atomic_Fence( ATOMIC_ORDER_ACQUIRE );

    lock = the_thread->Lock.current;
    _ISR_lock_Acquire( lock, lock_context );

    /*
     * Ensure that we read the second lock generation after we obtained our
     * current lock.
     */
    _Atomic_Fence( ATOMIC_ORDER_ACQUIRE );

    if ( the_thread->Lock.generation == my_generation ) {
      break;
    }

    _Thread_Lock_release( lock, lock_context );
  }

  return lock;
#else
  _ISR_Disable( lock_context->isr_level );

  return NULL;
#endif
}

#if defined(RTEMS_SMP)
/*
 * Internal function, use _Thread_Lock_set() or _Thread_Lock_restore_default()
 * instead.
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_set_unprotected(
  Thread_Control   *the_thread,
  ISR_lock_Control *new_lock
)
{
  the_thread->Lock.current = new_lock;

  /*
   * Ensure that the new lock is visible before we update the generation
   * number.  Otherwise someone would be able to read an up to date generation
   * number and an old lock.
   */
  _Atomic_Fence( ATOMIC_ORDER_RELEASE );

  /*
   * Since we set a new lock right before, this increment is not protected by a
   * lock and thus must be an atomic operation.
   */
  _Atomic_Fetch_add_uint(
    &the_thread->Lock.generation,
    1,
    ATOMIC_ORDER_RELAXED
  );
}
#endif

/**
 * @brief Sets a new thread lock.
 *
 * The caller must not be the owner of the default thread lock.  The caller
 * must be the owner of the new lock.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_lock The new thread lock.
 */
#if defined(RTEMS_SMP)
RTEMS_INLINE_ROUTINE void _Thread_Lock_set(
  Thread_Control   *the_thread,
  ISR_lock_Control *new_lock
)
{
  ISR_lock_Context lock_context;

  _Thread_Lock_acquire_default_critical( the_thread, &lock_context );
  _Assert( the_thread->Lock.current == &the_thread->Lock.Default );
  _Thread_Lock_set_unprotected( the_thread, new_lock );
  _Thread_Lock_release_default_critical( the_thread, &lock_context );
}
#else
#define _Thread_Lock_set( the_thread, new_lock ) \
  do { } while ( 0 )
#endif

/**
 * @brief Restores the default thread lock.
 *
 * The caller must be the owner of the current thread lock.
 *
 * @param[in] the_thread The thread.
 */
#if defined(RTEMS_SMP)
RTEMS_INLINE_ROUTINE void _Thread_Lock_restore_default(
  Thread_Control *the_thread
)
{
  _Atomic_Fence( ATOMIC_ORDER_RELEASE );

  _Thread_Lock_set_unprotected( the_thread, &the_thread->Lock.Default );
}
#else
#define _Thread_Lock_restore_default( the_thread ) \
  do { } while ( 0 )
#endif

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
 * @brief Indicates that the thread waits for a object.
 */
#define THREAD_WAIT_CLASS_OBJECT 0x400U

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
 * @brief Tries to change the thread wait flags inside a critical section
 * (interrupts disabled).
 *
 * In case the wait flags are equal to the expected wait flags, then the wait
 * flags are set to the desired wait flags.
 *
 * @param[in] the_thread The thread.
 * @param[in] expected_flags The expected wait flags.
 * @param[in] desired_flags The desired wait flags.
 *
 * @retval true The wait flags were equal to the expected wait flags.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Wait_flags_try_change_critical(
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
    ATOMIC_ORDER_RELAXED,
    ATOMIC_ORDER_RELAXED
  );
#else
  bool success = the_thread->Wait.flags == expected_flags;

  if ( success ) {
    the_thread->Wait.flags = desired_flags;
  }

  return success;
#endif
}

/**
 * @brief Tries to change the thread wait flags.
 *
 * @see _Thread_Wait_flags_try_change_critical().
 */
RTEMS_INLINE_ROUTINE bool _Thread_Wait_flags_try_change(
  Thread_Control    *the_thread,
  Thread_Wait_flags  expected_flags,
  Thread_Wait_flags  desired_flags
)
{
  bool success;
#if !defined(RTEMS_SMP)
  ISR_Level level;

  _ISR_Disable_without_giant( level );
#endif

  success = _Thread_Wait_flags_try_change_critical(
    the_thread,
    expected_flags,
    desired_flags
  );

#if !defined(RTEMS_SMP)
  _ISR_Enable_without_giant( level );
#endif

  return success;
}

/**
 * @brief Sets the thread queue.
 *
 * The caller must be the owner of the thread lock.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_queue The new queue.
 *
 * @see _Thread_Lock_set().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_set_queue(
  Thread_Control       *the_thread,
  Thread_queue_Control *new_queue
)
{
  the_thread->Wait.queue = new_queue;
}

/**
 * @brief Sets the thread queue operations.
 *
 * The caller must be the owner of the thread lock.
 *
 * @param[in] the_thread The thread.
 * @param[in] new_operations The new queue operations.
 *
 * @see _Thread_Lock_set() and _Thread_Wait_restore_default_operations().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_set_operations(
  Thread_Control                *the_thread,
  const Thread_queue_Operations *new_operations
)
{
  the_thread->Wait.operations = new_operations;
}

/**
 * @brief Restores the default thread queue operations.
 *
 * The caller must be the owner of the thread lock.
 *
 * @param[in] the_thread The thread.
 *
 * @see _Thread_Wait_set_operations().
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_restore_default_operations(
  Thread_Control *the_thread
)
{
  the_thread->Wait.operations = &_Thread_queue_Operations_default;
}

/**
 * @brief Sets the thread wait timeout code.
 *
 * @param[in] the_thread The thread.
 * @param[in] timeout_code The new thread wait timeout code.
 */
RTEMS_INLINE_ROUTINE void _Thread_Wait_set_timeout_code(
  Thread_Control *the_thread,
  uint32_t        timeout_code
)
{
  the_thread->Wait.timeout_code = timeout_code;
}

/**
 * @brief General purpose thread wait timeout.
 *
 * @param[in] id Unused.
 * @param[in] arg The thread.
 */
void _Thread_Timeout( Objects_Id id, void *arg );

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
