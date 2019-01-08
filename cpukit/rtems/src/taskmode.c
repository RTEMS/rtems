/**
 *  @file
 *
 *  @brief RTEMS Task Mode
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksdata.h>
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/modesimpl.h>
#include <rtems/rtems/signalimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/config.h>

rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
)
{
  ISR_lock_Context    lock_context;
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;
  bool                preempt_enabled;
  bool                needs_asr_dispatching;
  rtems_mode          old_mode;

  if ( !previous_mode_set )
    return RTEMS_INVALID_ADDRESS;

#if defined(RTEMS_SMP)
  if (
    ( mask & RTEMS_PREEMPT_MASK ) != 0
      && !_Modes_Is_preempt( mode_set )
      && rtems_configuration_is_smp_enabled()
  ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

#if defined(RTEMS_SMP) || CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  if (
    ( mask & RTEMS_INTERRUPT_MASK ) != 0
      && _Modes_Get_interrupt_level( mode_set ) != 0
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == FALSE
      && rtems_configuration_is_smp_enabled()
#endif
  ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  /*
   * Complete all error checking before doing any operations which
   * impact the executing thread. There should be no errors returned
   * past this point.
   */
 
  executing     = _Thread_Get_executing();
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

  /*
   *  These are generic thread scheduling characteristics.
   */
  preempt_enabled = false;
  if ( mask & RTEMS_PREEMPT_MASK ) {
    bool is_preempt_enabled = _Modes_Is_preempt( mode_set );

    preempt_enabled = !executing->is_preemptible && is_preempt_enabled;
    executing->is_preemptible = is_preempt_enabled;
  }

  if ( mask & RTEMS_TIMESLICE_MASK ) {
    if ( _Modes_Is_timeslice(mode_set) ) {
      executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;
      executing->cpu_time_budget =
        rtems_configuration_get_ticks_per_timeslice();
    } else
      executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
  }

  /*
   *  Set the new interrupt level
   */
  if ( mask & RTEMS_INTERRUPT_MASK ) {
    _Modes_Set_interrupt_level( mode_set );
  }

  /*
   *  This is specific to the RTEMS API
   */
  needs_asr_dispatching = false;
  if ( mask & RTEMS_ASR_MASK ) {
    bool is_asr_enabled = !_Modes_Is_asr_disabled( mode_set );

    _Thread_State_acquire( executing, &lock_context );

    if ( is_asr_enabled != asr->is_enabled ) {
      asr->is_enabled = is_asr_enabled;

      if ( _ASR_Swap_signals( asr ) != 0 ) {
        needs_asr_dispatching = true;
        _Thread_Add_post_switch_action(
          executing,
          &api->Signal_action,
          _Signal_Action_handler
        );
      }
    }

    _Thread_State_release( executing, &lock_context );
  }

  if ( preempt_enabled || needs_asr_dispatching ) {
    Per_CPU_Control  *cpu_self;

    cpu_self = _Thread_Dispatch_disable();
    _Thread_State_acquire( executing, &lock_context );
    _Scheduler_Schedule( executing );
    _Thread_State_release( executing, &lock_context );
    _Thread_Dispatch_direct( cpu_self );
  }

  return RTEMS_SUCCESSFUL;
}
