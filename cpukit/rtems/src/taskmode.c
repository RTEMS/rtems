/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_mode().
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

#include <rtems/rtems/tasksdata.h>
#include <rtems/rtems/modesimpl.h>
#include <rtems/rtems/signalimpl.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/smpimpl.h>
#include <rtems/score/threadimpl.h>

rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
)
{
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;
  rtems_mode          old_mode;

  executing = _Thread_Get_executing();

  if ( !previous_mode_set )
    return RTEMS_INVALID_ADDRESS;

#if defined(RTEMS_SMP)
  if (
    ( mask & RTEMS_PREEMPT_MASK ) != 0 &&
    !_Modes_Is_preempt_mode_supported( mode_set, executing )
  ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

#if defined(RTEMS_SMP) || CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  if (
    ( mask & RTEMS_INTERRUPT_MASK ) != 0 &&
    !_Modes_Is_interrupt_level_supported( mode_set )
  ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  /*
   * Complete all error checking before doing any operations which
   * impact the executing thread. There should be no errors returned
   * past this point.
   */

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  old_mode  = (executing->is_preemptible) ? RTEMS_PREEMPT : RTEMS_NO_PREEMPT;

  if ( executing->budget_algorithm == THREAD_CPU_BUDGET_ALGORITHM_NONE )
    old_mode |= RTEMS_NO_TIMESLICE;
  else
    old_mode |= RTEMS_TIMESLICE;

  old_mode |= (asr->is_enabled) ? RTEMS_ASR : RTEMS_NO_ASR;
  old_mode |= _ISR_Get_level();

  *previous_mode_set = old_mode;

  if ( ( mask & RTEMS_TIMESLICE_MASK ) != 0 ) {
    _Modes_Apply_timeslice_to_thread( mode_set, executing );
  }

  if ( ( mask & RTEMS_INTERRUPT_MASK ) != 0 ) {
    _ISR_Set_level( _Modes_Get_interrupt_level( mode_set ) );
  }

  if ( ( mask & ( RTEMS_ASR_MASK | RTEMS_PREEMPT_MASK ) ) != 0 ) {
    bool             need_thread_dispatch;
    ISR_lock_Context lock_context;
    bool             previous_asr_is_enabled;
    bool             previous_is_preemptible;

    need_thread_dispatch = false;

    _Thread_State_acquire( executing, &lock_context );

    previous_asr_is_enabled = asr->is_enabled;
    asr->is_enabled = !_Modes_Is_asr_disabled( mode_set );

    if (
      !previous_asr_is_enabled &&
        asr->is_enabled &&
        asr->signals_pending != 0
    ) {
      need_thread_dispatch = true;
      _Thread_Append_post_switch_action( executing, &api->Signal_action );
    }

    previous_is_preemptible = executing->is_preemptible;
    executing->is_preemptible = _Modes_Is_preempt( mode_set );

    if ( executing->is_preemptible && !previous_is_preemptible ) {
      need_thread_dispatch = true;
      _Scheduler_Schedule( executing );
    }

    if ( need_thread_dispatch ) {
      Per_CPU_Control *cpu_self;

      cpu_self = _Thread_Dispatch_disable_critical( &lock_context );
      _Thread_State_release( executing, &lock_context );
      _Thread_Dispatch_direct( cpu_self );
    } else {
      _Thread_State_release( executing, &lock_context );
    }
  }

  return RTEMS_SUCCESSFUL;
}
