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
 *  Copyright (c) 2014-2015 embedded brains GmbH.
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
#include <rtems/score/status.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/freechain.h>
#include <rtems/score/watchdogimpl.h>
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
extern void *rtems_ada_self;

typedef struct {
  Objects_Information Objects;

  Freechain_Control Free_thread_queue_heads;
} Thread_Information;

/**
 *  The following defines the information control block used to
 *  manage this class of objects.
 */
extern Thread_Information _Thread_Internal_information;

/**
 *  The following points to the thread whose floating point
 *  context is currently loaded.
 */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
extern Thread_Control *_Thread_Allocated_fp;
#endif

#define THREAD_CHAIN_NODE_TO_THREAD( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, Wait.Node.Chain )

#define THREAD_RBTREE_NODE_TO_THREAD( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, Wait.Node.RBTree )

#if defined(RTEMS_SMP)
#define THREAD_RESOURCE_NODE_TO_THREAD( node ) \
  RTEMS_CONTAINER_OF( node, Thread_Control, Resource_node )
#endif

void _Thread_Initialize_information(
  Thread_Information  *information,
  Objects_APIs         the_api,
  uint16_t             the_class,
  uint32_t             maximum,
  bool                 is_string,
  uint32_t             maximum_name_length
);

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
void _Thread_Start_multitasking( void ) RTEMS_NO_RETURN;

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
  Thread_Information                   *information,
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
 *  @param the_thread The thread to be started.
 *  @param entry The thread entry information.
 */
bool _Thread_Start(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
);

void _Thread_Restart_self(
  Thread_Control                 *executing,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
) RTEMS_NO_RETURN;

bool _Thread_Restart_other(
  Thread_Control                 *the_thread,
  const Thread_Entry_information *entry,
  ISR_lock_Context               *lock_context
);

void _Thread_Yield( Thread_Control *executing );

Thread_Life_state _Thread_Change_life(
  Thread_Life_state clear,
  Thread_Life_state set,
  Thread_Life_state ignore
);

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

void _Thread_Exit(
  Thread_Control    *executing,
  Thread_Life_state  set,
  void              *exit_value
);

void _Thread_Join(
  Thread_Control       *the_thread,
  States_Control        waiting_for_join,
  Thread_Control       *executing,
  Thread_queue_Context *queue_context
);

void _Thread_Cancel(
  Thread_Control *the_thread,
  Thread_Control *executing,
  void           *exit_value
);

/**
 * @brief Closes the thread.
 *
 * Closes the thread object and starts the thread termination sequence.  In
 * case the executing thread is not terminated, then this function waits until
 * the terminating thread reached the zombie state.
 */
void _Thread_Close( Thread_Control *the_thread, Thread_Control *executing );

RTEMS_INLINE_ROUTINE bool _Thread_Is_ready( const Thread_Control *the_thread )
{
  return _States_Is_ready( the_thread->current_state );
}

States_Control _Thread_Clear_state_locked(
  Thread_Control *the_thread,
  States_Control  state
);

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

States_Control _Thread_Set_state_locked(
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

void _Thread_Entry_adaptor_idle( Thread_Control *executing );

void _Thread_Entry_adaptor_numeric( Thread_Control *executing );

void _Thread_Entry_adaptor_pointer( Thread_Control *executing );

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
void _Thread_Global_construction(
  Thread_Control                 *executing,
  const Thread_Entry_information *entry
) RTEMS_NO_RETURN;

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

RTEMS_INLINE_ROUTINE void _Thread_State_acquire_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Acquire_critical( &the_thread->Join_queue, lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_State_acquire(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Acquire( &the_thread->Join_queue, lock_context );
}

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

RTEMS_INLINE_ROUTINE void _Thread_State_release_critical(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Release_critical( &the_thread->Join_queue, lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_State_release(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _Thread_queue_Release( &the_thread->Join_queue, lock_context );
}

#if defined(RTEMS_DEBUG)
RTEMS_INLINE_ROUTINE bool _Thread_State_is_owner(
  const Thread_Control *the_thread
)
{
  return _Thread_queue_Is_lock_owner( &the_thread->Join_queue );
}
#endif

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
 * @brief Inherit the priority of a thread.
 *
 * It changes the current priority of the inheritor thread to the current priority
 * of the ancestor thread if it is higher than the current priority of the inheritor
 * thread.  In this case the inheritor thread is appended to its new priority group
 * in its scheduler instance.
 *
 * On SMP configurations, the priority is changed to PRIORITY_PSEUDO_ISR in
 * case the own schedulers of the inheritor and ancestor thread differ (priority
 * boosting).
 *
 * @param[in] inheritor The thread to inherit the priority.
 * @param[in] ancestor The thread to bequeath its priority to the inheritor
 *   thread.
 */
#if defined(RTEMS_SMP)
void _Thread_Inherit_priority(
  Thread_Control *inheritor,
  Thread_Control *ancestor
);
#else
RTEMS_INLINE_ROUTINE void _Thread_Inherit_priority(
  Thread_Control *inheritor,
  Thread_Control *ancestor
)
{
  _Thread_Raise_priority( inheritor, ancestor->current_priority );
}
#endif

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
 */
Thread_Control *_Thread_Get(
  Objects_Id         id,
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
    _Objects_Allocate_unprotected( &_Thread_Internal_information.Objects );
}

/**
 * @brief Gets the heir of the processor and makes it executing.
 *
 * Must be called with interrupts disabled.  The thread dispatch necessary
 * indicator is cleared as a side-effect.
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

void _Thread_Get_CPU_time_used(
  Thread_Control    *the_thread,
  Timestamp_Control *cpu_time_used
);

RTEMS_INLINE_ROUTINE void _Thread_Action_control_initialize(
  Thread_Action_control *action_control
)
{
  _Chain_Initialize_empty( &action_control->Chain );
}

RTEMS_INLINE_ROUTINE void _Thread_Action_initialize(
  Thread_Action *action
)
{
  _Chain_Set_off_chain( &action->Node );
}

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

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_change_allowed(
  Thread_Life_state life_state
)
{
  return ( life_state
    & ( THREAD_LIFE_PROTECTED | THREAD_LIFE_CHANGE_DEFERRED ) ) == 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_life_changing(
  Thread_Life_state life_state
)
{
  return ( life_state
    & ( THREAD_LIFE_RESTARTING | THREAD_LIFE_TERMINATING ) ) != 0;
}

RTEMS_INLINE_ROUTINE bool _Thread_Is_joinable(
  const Thread_Control *the_thread
)
{
  _Assert( _Thread_State_is_owner( the_thread ) );
  return ( the_thread->Life.state & THREAD_LIFE_DETACHED ) == 0;
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
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Acquire(
    &the_thread->Lock.Default,
    &_Thread_Executing->Lock.Stats,
    &lock_context->Lock_context.Stats_context
  );
#else
  (void) the_thread;
  (void) lock_context;
#endif
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
  _Thread_Lock_acquire_default_critical( executing, lock_context );

  return executing;
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
  _ISR_lock_ISR_disable( lock_context );
  _Thread_Lock_acquire_default_critical( the_thread, lock_context );
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
  void             *lock,
  ISR_lock_Context *lock_context
)
{
#if defined(RTEMS_SMP)
  _SMP_ticket_lock_Release(
    (SMP_ticket_lock_Control *) lock,
    &lock_context->Lock_context.Stats_context
  );
#else
  (void) lock;
  (void) lock_context;
#endif
}

/**
 * @brief Releases the thread lock.
 *
 * @param[in] lock The lock returned by _Thread_Lock_acquire().
 * @param[in] lock_context The lock context used for _Thread_Lock_acquire().
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_release(
  void             *lock,
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
RTEMS_INLINE_ROUTINE void *_Thread_Lock_acquire(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
#if defined(RTEMS_SMP)
  SMP_ticket_lock_Control *lock_0;

  while ( true ) {
    SMP_ticket_lock_Control *lock_1;

    _ISR_lock_ISR_disable( lock_context );

    /*
     * We assume that a normal load of pointer is identical to a relaxed atomic
     * load.  Here, we may read an out-of-date lock.  However, only the owner
     * of this out-of-date lock is allowed to set a new one.  Thus, we read at
     * least this new lock ...
     */
    lock_0 = (SMP_ticket_lock_Control *) _Atomic_Load_uintptr(
      &the_thread->Lock.current.atomic,
      ATOMIC_ORDER_RELAXED
    );

    _SMP_ticket_lock_Acquire(
      lock_0,
      &_Thread_Executing->Lock.Stats,
      &lock_context->Lock_context.Stats_context
    );

    /*
     * We must use a load acquire here paired with the store release in
     * _Thread_Lock_set_unprotected() to observe corresponding thread wait
     * queue and thread wait operations.  It is important to do this after the
     * lock acquire, since we may have the following scenario.
     *
     * - We read the default lock and try to acquire it.
     * - The thread lock changes to a thread queue lock.
     * - The thread lock is restored to the default lock.
     * - We acquire the default lock and read it here again.
     * - Now, we must read the restored default thread wait queue and thread
     *   wait operations and this is not synchronized via the default thread
     *   lock.
     */
    lock_1 = (SMP_ticket_lock_Control *) _Atomic_Load_uintptr(
      &the_thread->Lock.current.atomic,
      ATOMIC_ORDER_ACQUIRE
    );

    /*
     * ... here, and so on.
     */
    if ( lock_0 == lock_1 ) {
      return lock_0;
    }

    _Thread_Lock_release( lock_0, lock_context );
  }
#else
  _ISR_Local_disable( lock_context->isr_level );

  return NULL;
#endif
}

#if defined(RTEMS_SMP)
/*
 * Internal function, use _Thread_Lock_set() or _Thread_Lock_restore_default()
 * instead.
 */
RTEMS_INLINE_ROUTINE void _Thread_Lock_set_unprotected(
  Thread_Control          *the_thread,
  SMP_ticket_lock_Control *new_lock
)
{
  _Atomic_Store_uintptr(
    &the_thread->Lock.current.atomic,
    (uintptr_t) new_lock,
    ATOMIC_ORDER_RELEASE
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
  Thread_Control          *the_thread,
  SMP_ticket_lock_Control *new_lock
)
{
  ISR_lock_Context lock_context;

  _Thread_Lock_acquire_default_critical( the_thread, &lock_context );
  _Assert( the_thread->Lock.current.normal == &the_thread->Lock.Default );
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
 * @brief Indicates that the thread waits for an object.
 */
#define THREAD_WAIT_CLASS_OBJECT 0x400U

/**
 * @brief Indicates that the thread waits for a period.
 */
#define THREAD_WAIT_CLASS_PERIOD 0x800U

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
 * @brief Tries to change the thread wait flags with release semantics in case
 * of success.
 *
 * Must be called inside a critical section (interrupts disabled).
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
 * @param[in] the_thread The thread.
 * @param[in] expected_flags The expected wait flags.
 * @param[in] desired_flags The desired wait flags.
 *
 * @retval true The wait flags were equal to the expected wait flags.
 * @retval false Otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Thread_Wait_flags_try_change_acquire(
  Thread_Control    *the_thread,
  Thread_Wait_flags  expected_flags,
  Thread_Wait_flags  desired_flags
)
{
  bool success;
#if defined(RTEMS_SMP)
  return _Atomic_Compare_exchange_uint(
    &the_thread->Wait.flags,
    &expected_flags,
    desired_flags,
    ATOMIC_ORDER_ACQUIRE,
    ATOMIC_ORDER_ACQUIRE
  );
#else
  ISR_Level level;

  _ISR_Local_disable( level );

  success = _Thread_Wait_flags_try_change_release(
    the_thread,
    expected_flags,
    desired_flags
  );

  _ISR_Local_enable( level );
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
  Thread_Control     *the_thread,
  Thread_queue_Queue *new_queue
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
 * @brief Returns the object identifier of the object containing the current
 * thread wait queue.
 *
 * This function may be used for debug and system information purposes.  The
 * caller must be the owner of the thread lock.
 *
 * @retval 0 The thread waits on no thread queue currently, the thread wait
 *   queue is not contained in an object, or the current thread state provides
 *   insufficient information, e.g. the thread is in the middle of a blocking
 *   operation.
 * @retval other The object identifier of the object containing the thread wait
 *   queue.
 */
Objects_Id _Thread_Wait_get_id( const Thread_Control *the_thread );

RTEMS_INLINE_ROUTINE Status_Control _Thread_Wait_get_status(
  const Thread_Control *the_thread
)
{
  return (Status_Control) the_thread->Wait.return_code;
}

/**
 * @brief General purpose thread wait timeout.
 *
 * @param[in] watchdog The thread timer watchdog.
 */
void _Thread_Timeout( Watchdog_Control *watchdog );

RTEMS_INLINE_ROUTINE void _Thread_Timer_initialize(
  Thread_Timer_information *timer,
  Per_CPU_Control          *cpu
)
{
  _ISR_lock_Initialize( &timer->Lock, "Thread Timer" );
  timer->header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_RELATIVE ];
  _Watchdog_Preinitialize( &timer->Watchdog, cpu );
}

RTEMS_INLINE_ROUTINE void _Thread_Timer_insert_relative(
  Thread_Control                 *the_thread,
  Per_CPU_Control                *cpu,
  Watchdog_Service_routine_entry  routine,
  Watchdog_Interval               ticks
)
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &the_thread->Timer.Lock, &lock_context );

  the_thread->Timer.header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_RELATIVE ];
  the_thread->Timer.Watchdog.routine = routine;
  _Watchdog_Per_CPU_insert_relative( &the_thread->Timer.Watchdog, cpu, ticks );

  _ISR_lock_Release_and_ISR_enable( &the_thread->Timer.Lock, &lock_context );
}

RTEMS_INLINE_ROUTINE void _Thread_Timer_insert_absolute(
  Thread_Control                 *the_thread,
  Per_CPU_Control                *cpu,
  Watchdog_Service_routine_entry  routine,
  uint64_t                        expire
)
{
  ISR_lock_Context lock_context;

  _ISR_lock_ISR_disable_and_acquire( &the_thread->Timer.Lock, &lock_context );

  the_thread->Timer.header = &cpu->Watchdog.Header[ PER_CPU_WATCHDOG_ABSOLUTE ];
  the_thread->Timer.Watchdog.routine = routine;
  _Watchdog_Per_CPU_insert_absolute( &the_thread->Timer.Watchdog, cpu, expire );

  _ISR_lock_Release_and_ISR_enable( &the_thread->Timer.Lock, &lock_context );
}

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

RTEMS_INLINE_ROUTINE void _Thread_Remove_timer_and_unblock(
  Thread_Control     *the_thread,
  Thread_queue_Queue *queue
)
{
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

/** @}*/

#ifdef __cplusplus
}
#endif

#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/threadmp.h>
#endif

#endif
/* end of include file */
