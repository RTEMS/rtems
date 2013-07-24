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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_THREADIMPL_H
#define _RTEMS_SCORE_THREADIMPL_H

#include <rtems/score/thread.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/statesimpl.h>

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
 *  The following context area contains the context of the "thread"
 *  which invoked the start multitasking routine.  This context is
 *  restored as the last action of the stop multitasking routine.  Thus
 *  control of the processor can be returned to the environment
 *  which initiated the system.
 */
SCORE_EXTERN Context_Control _Thread_BSP_context;

/**
 *  The following holds how many user extensions are in the system.  This
 *  is used to determine how many user extension data areas to allocate
 *  per thread.
 */
SCORE_EXTERN uint32_t   _Thread_Maximum_extensions;

/**
 *  The following is used to manage the length of a timeslice quantum.
 */
SCORE_EXTERN uint32_t   _Thread_Ticks_per_timeslice;

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
 *
 *
 *  - INTERRUPT LATENCY:
 *    + ready chain
 *    + select heir
 */
void _Thread_Start_multitasking( void );

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
 *  @param[in,out] processor The processor if used to start an idle thread
 *  during system initialization.  Must be set to @c NULL to start a normal
 *  thread.
 */
bool _Thread_Start(
  Thread_Control            *the_thread,
  Thread_Start_types         the_prototype,
  void                      *entry_point,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument,
  Per_CPU_Control           *processor
);

/**
 *  @brief Restarts the specified thread.
 *
 *  This support routine restarts the specified task in a way that the
 *  next time this thread executes, it will begin execution at its
 *  original starting point.
 *
 *  TODO:  multiple task arg profiles
 */
bool _Thread_Restart(
  Thread_Control            *the_thread,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument
);

/**
 *  @brief Resets a thread to its initial state.
 *
 *  This routine resets a thread to its initial state but does
 *  not restart it. Some APIs do this in separate
 *  operations and this division helps support this.
 *
 *  @param[in] the_thread is the thread to resets
 *  @param[in] pointer_argument
 *  @param[in] numeric_argument
 */
void _Thread_Reset(
  Thread_Control            *the_thread,
  void                      *pointer_argument,
  Thread_Entry_numeric_type  numeric_argument
);

/**
 *  @brief Frees all memory associated with the specified thread.
 *
 *  This routine frees all memory associated with the specified
 *  thread and removes it from the local object table so no further
 *  operations on this thread are allowed.
 */
void _Thread_Close(
  Objects_Information  *information,
  Thread_Control       *the_thread
);

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
 *  @brief Sets the transient state for a thread.
 *
 *  This routine sets the Transient state for @a the_thread.  It performs
 *  any necessary scheduling operations including the selection of
 *  a new heir thread.
 *
 *  @param[in] the_thread is the thread to preform the action upon.
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
void _Thread_Set_transient(
  Thread_Control *the_thread
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
 * This routine halts multitasking and returns control to
 * the "thread" (i.e. the BSP) which initially invoked the
 * routine which initialized the system.
 */

RTEMS_INLINE_ROUTINE void _Thread_Stop_multitasking( void )
{
#if defined(_CPU_Stop_multitasking)
  _CPU_Stop_multitasking( &_Thread_BSP_context );
#else
  /*
   *  This may look a bit of an odd but _Context_Restart_self is just
   *  a very careful restore of a specific context which ensures that
   *  if we were running within the same context, it would work.
   *
   *  And we will not return to this thread, so there is no point of
   *  saving the context.
   */
  _Context_Restart_self( &_Thread_BSP_context );
#endif

  /***************************************************************
   ***************************************************************
   *   SYSTEM SHUTS DOWN!!!  WE DO NOT RETURN TO THIS POINT!!!   *
   ***************************************************************
   ***************************************************************
   */
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
 * This function returns true if the currently executing thread
 * is also the heir thread, and false otherwise.
 */

RTEMS_INLINE_ROUTINE bool _Thread_Is_executing_also_the_heir( void )
{
  return ( _Thread_Executing == _Thread_Heir );
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

RTEMS_INLINE_ROUTINE void _Thread_Restart_self( void )
{
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( _Thread_Executing->fp_context != NULL )
    _Context_Restore_fp( &_Thread_Executing->fp_context );
#endif

  _CPU_Context_Restart_self( &_Thread_Executing->Registers );
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

/**
 * This routine allocates an internal thread.
 */

RTEMS_INLINE_ROUTINE Thread_Control *_Thread_Internal_allocate( void )
{
  return (Thread_Control *) _Objects_Allocate( &_Thread_Internal_information );
}

/**
 * This routine frees an internal thread.
 */

RTEMS_INLINE_ROUTINE void _Thread_Internal_free (
  Thread_Control *the_task
)
{
  _Objects_Free( &_Thread_Internal_information, &the_task->Object );
}

RTEMS_INLINE_ROUTINE void _Thread_Set_global_exit_status(
  uint32_t exit_status
)
{
  Thread_Control *idle = (Thread_Control *)
    _Thread_Internal_information.local_table[ 1 ];

  idle->Wait.return_code = exit_status;
}

RTEMS_INLINE_ROUTINE uint32_t _Thread_Get_global_exit_status( void )
{
  const Thread_Control *idle = (const Thread_Control *)
    _Thread_Internal_information.local_table[ 1 ];

  return idle->Wait.return_code;
}

/**
 * @brief Issues a thread dispatch if necessary.
 *
 * @param[in] executing The executing thread.
 * @param[in] needs_asr_dispatching Indicates whether or not the API
 *            level signals are pending and a dispatch is necessary.
 */
RTEMS_INLINE_ROUTINE void _Thread_Dispatch_if_necessary(
  Thread_Control *executing,
  bool            needs_asr_dispatching
)
{
  if ( _Thread_Dispatch_is_enabled() ) {
    bool dispatch_necessary = needs_asr_dispatching;

    if ( !_Thread_Is_heir( executing ) && executing->is_preemptible ) {
      dispatch_necessary = true;
      _Thread_Dispatch_necessary = dispatch_necessary;
    }

    if ( dispatch_necessary ) {
      _Thread_Dispatch();
    }
  }
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
