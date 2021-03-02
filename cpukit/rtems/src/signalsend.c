/**
 * @file
 *
 * @ingroup RTEMSImplClassicSignal
 *
 * @brief This source file contains the implementation of
 *   rtems_signal_send().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/signalimpl.h>
#include <rtems/rtems/modesimpl.h>
#include <rtems/rtems/tasksdata.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threaddispatch.h>
#include <rtems/score/threadimpl.h>

#include <rtems/sysinit.h>

static void _Signal_Action_handler(
  Thread_Control   *executing,
  Thread_Action    *action,
  ISR_lock_Context *lock_context
)
{
  RTEMS_API_Control           *api;
  ASR_Information             *asr;
  rtems_signal_set             signal_set;
  bool                         normal_is_preemptible;
  uint32_t                     normal_cpu_time_budget;
  Thread_CPU_budget_algorithms normal_budget_algorithm;
  uint32_t                     normal_isr_level;
  uint32_t                     before_call_isr_level;
  bool                         after_call_is_preemptible;
  bool                         after_call_asr_is_enabled;

  (void) action;

  /*
   * For the variable names the following notation is used.  The prefix
   * "normal" specifies the mode associated with the normal task execution.
   * The prefix "before_call" specifies the mode set up right before the ASR
   * handler is called.  The prefix "after_call" specifies the mode after the
   * ASR handler call returned.  This mode may differ from the "before_call"
   * mode since an ASR handler is free to change the task mode.
   */

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  /* Get and clear the pending signals */
  signal_set = asr->signals_pending;
  _Assert( signal_set != 0 );
  asr->signals_pending = 0;

  /* Save normal mode */

  _Assert( asr->is_enabled );
  normal_is_preemptible = executing->is_preemptible;
  normal_cpu_time_budget = executing->cpu_time_budget;
  normal_budget_algorithm = executing->budget_algorithm;

  /* Set mode for ASR processing */

  executing->is_preemptible = _Modes_Is_preempt( asr->mode_set );
  asr->is_enabled = !_Modes_Is_asr_disabled( asr->mode_set );
  _Modes_Apply_timeslice_to_thread( asr->mode_set, executing );
  before_call_isr_level = _Modes_Get_interrupt_level( asr->mode_set );

  if ( executing->is_preemptible && !normal_is_preemptible ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( lock_context );
    _Scheduler_Schedule( executing );
    _Thread_State_release( executing, lock_context );
    _Thread_Dispatch_direct( cpu_self );
  } else {
    _Thread_State_release( executing, lock_context );
  }

  normal_isr_level = _ISR_Get_level();
  _ISR_Set_level( before_call_isr_level );

  /* Call the ASR handler in the ASR processing mode */
  ( *asr->handler )( signal_set );

  /* Restore normal mode */

  _ISR_Set_level( normal_isr_level );

  _Thread_State_acquire( executing, lock_context );

  executing->cpu_time_budget = normal_cpu_time_budget ;
  executing->budget_algorithm = normal_budget_algorithm ;
  after_call_is_preemptible = executing->is_preemptible;
  executing->is_preemptible = normal_is_preemptible;

  /*
   * We do the best to avoid recursion in the ASR processing.  A well behaved
   * application will disable ASR processing during ASR processing.  In this
   * case, ASR processing is currently disabled.  We do now the thread dispatch
   * necessary due to a re-enabled preemption mode.  This helps to avoid doing
   * the next round of ASR processing recursively in _Thread_Dispatch_direct().
   */
  if ( normal_is_preemptible && !after_call_is_preemptible ) {
    Per_CPU_Control *cpu_self;

    cpu_self = _Thread_Dispatch_disable_critical( lock_context );
    _Scheduler_Schedule( executing );
    _Thread_State_release( executing, lock_context );
    _Thread_Dispatch_direct( cpu_self );
    _Thread_State_acquire( executing, lock_context );
  }

  /*
   * Restore the normal ASR processing mode.  If we enable ASR processing and
   * there are pending signals, then add us as a post-switch action.  The loop
   * in _Thread_Run_post_switch_actions() will continue after our return and
   * call us again.  This avoids a recursion.
   */

  after_call_asr_is_enabled = asr->is_enabled;
  asr->is_enabled = true;

  if ( !after_call_asr_is_enabled && asr->signals_pending != 0 ) {
    _Thread_Append_post_switch_action( executing, action );
  }
}

rtems_status_code rtems_signal_send(
  rtems_id          id,
  rtems_signal_set  signal_set
)
{
  Thread_Control    *the_thread;
  ISR_lock_Context   lock_context;
  RTEMS_API_Control *api;
  ASR_Information   *asr;

  if ( signal_set == 0 ) {
    return RTEMS_INVALID_NUMBER;
  }

  the_thread = _Thread_Get( id, &lock_context );

  if ( the_thread == NULL ) {
#if defined(RTEMS_MULTIPROCESSING)
    return _Signal_MP_Send( id, signal_set );
#else
    return RTEMS_INVALID_ID;
#endif
  }

  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  _Thread_State_acquire_critical( the_thread, &lock_context );

  if ( asr->handler == NULL ) {
    _Thread_State_release( the_thread, &lock_context );
    return RTEMS_NOT_DEFINED;
  }

  /* Make the signals of the set pending */
  asr->signals_pending |= signal_set;

  if ( asr->is_enabled ) {
    Per_CPU_Control *cpu_self;

    _Thread_Add_post_switch_action(
      the_thread,
      &api->Signal_action,
      _Signal_Action_handler
    );
    cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
    _Thread_State_release( the_thread, &lock_context );
    _Thread_Dispatch_enable( cpu_self );
  } else {
    _Thread_State_release( the_thread, &lock_context );
  }

  return RTEMS_SUCCESSFUL;
}

#if defined(RTEMS_MULTIPROCESSING)
static void _Signal_MP_Initialize( void )
{
  _MPCI_Register_packet_processor(
    MP_PACKET_SIGNAL,
    _Signal_MP_Process_packet
  );
}

RTEMS_SYSINIT_ITEM(
  _Signal_MP_Initialize,
  RTEMS_SYSINIT_CLASSIC_SIGNAL_MP,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
#endif
