/*
 *  RTEMS Task Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

/*PAGE
 *
 *  rtems_task_mode
 *
 *  This directive enables and disables several modes of
 *  execution for the requesting thread.
 *
 *  Input parameters:
 *    mode_set          - new mode
 *    mask              - mask
 *    previous_mode_set - address of previous mode set
 *
 *  Output:
 *    *previous_mode_set - previous mode set
 *     always return RTEMS_SUCCESSFUL;
 */

rtems_status_code rtems_task_mode(
  rtems_mode  mode_set,
  rtems_mode  mask,
  rtems_mode *previous_mode_set
)
{
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;
  boolean             is_asr_enabled = FALSE;
  boolean             needs_asr_dispatching = FALSE;
  rtems_mode          old_mode;

  executing     = _Thread_Executing;
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

  if ( mask & RTEMS_PREEMPT_MASK )
    executing->is_preemptible = _Modes_Is_preempt(mode_set) ? TRUE : FALSE;

  if ( mask & RTEMS_TIMESLICE_MASK ) {
    if ( _Modes_Is_timeslice(mode_set) ) 
      executing->budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE;
    else
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

  is_asr_enabled = FALSE;
  needs_asr_dispatching = FALSE;

  if ( mask & RTEMS_ASR_MASK ) {
    is_asr_enabled = _Modes_Is_asr_disabled( mode_set ) ? FALSE : TRUE;
    if ( is_asr_enabled != asr->is_enabled ) {
      asr->is_enabled = is_asr_enabled;
      _ASR_Swap_signals( asr );
      if ( _ASR_Are_signals_pending( asr ) ) {
        needs_asr_dispatching = TRUE;
        executing->do_post_task_switch_extension = TRUE;
      }
    }
  }

  if ( _System_state_Is_up(_System_state_Current) )
    if ( _Thread_Evaluate_mode() || needs_asr_dispatching )
      _Thread_Dispatch();

  return RTEMS_SUCCESSFUL;
}
