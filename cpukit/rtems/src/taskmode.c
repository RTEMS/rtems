/**
 *  @file
 *
 *  @brief RTEMS Task Mode
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modesimpl.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>

rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
)
{
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;
  bool                needs_asr_dispatching;
  rtems_mode          old_mode;

  if ( !previous_mode_set )
    return RTEMS_INVALID_ADDRESS;

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
  if ( mask & RTEMS_PREEMPT_MASK ) {
#if defined( RTEMS_SMP )
    if (
      rtems_configuration_is_smp_enabled()
        && !_Modes_Is_preempt( mode_set )
    ) {
      return RTEMS_NOT_IMPLEMENTED;
    }
#endif

    executing->is_preemptible = _Modes_Is_preempt( mode_set );
  }

  if ( mask & RTEMS_TIMESLICE_MASK ) {
    if ( _Modes_Is_timeslice(mode_set) ) {
      executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;
      executing->cpu_time_budget  = _Thread_Ticks_per_timeslice;
    } else
      executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
  }

  /*
   *  Set the new interrupt level
   */
  if ( mask & RTEMS_INTERRUPT_MASK )
    _Modes_Set_interrupt_level( mode_set );

  /*
   *  This is specific to the RTEMS API
   */
  needs_asr_dispatching = false;
  if ( mask & RTEMS_ASR_MASK ) {
    bool is_asr_enabled = !_Modes_Is_asr_disabled( mode_set );

    if ( is_asr_enabled != asr->is_enabled ) {
      asr->is_enabled = is_asr_enabled;
      _ASR_Swap_signals( asr );
      if ( _ASR_Are_signals_pending( asr ) ) {
        needs_asr_dispatching = true;
      }
    }
  }

  _Thread_Dispatch_if_necessary( executing, needs_asr_dispatching );

  return RTEMS_SUCCESSFUL;
}
