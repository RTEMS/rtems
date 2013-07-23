/**
 *  @file
 *
 *  @brief RTEMS Catch Signal
 *  @ingroup ClassicSignal
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/asr.h>
#include <rtems/score/isr.h>
#include <rtems/rtems/modes.h>
#include <rtems/rtems/signal.h>
#include <rtems/score/thread.h>
#include <rtems/score/apiext.h>
#include <rtems/rtems/tasks.h>

static void _RTEMS_signal_Post_switch_hook( Thread_Control *executing )
{
  ISR_Level          level;
  RTEMS_API_Control *api;
  ASR_Information   *asr;
  rtems_signal_set   signal_set;
  Modes_Control      prev_mode;

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  if ( !api )
    return;

  /*
   *  Signal Processing
   */

  asr = &api->Signal;

  _ISR_Disable( level );
    signal_set = asr->signals_posted;
    asr->signals_posted = 0;
  _ISR_Enable( level );


  if ( !signal_set ) /* similar to _ASR_Are_signals_pending( asr ) */
    return;

  asr->nest_level += 1;
  rtems_task_mode( asr->mode_set, RTEMS_ALL_MODE_MASKS, &prev_mode );

  (*asr->handler)( signal_set );

  asr->nest_level -= 1;
  rtems_task_mode( prev_mode, RTEMS_ALL_MODE_MASKS, &prev_mode );

}

static API_extensions_Post_switch_control _RTEMS_signal_Post_switch = {
  .hook = _RTEMS_signal_Post_switch_hook
};

rtems_status_code rtems_signal_catch(
  rtems_asr_entry   asr_handler,
  rtems_mode        mode_set
)
{
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;

/* XXX normalize mode */
  executing = _Thread_Get_executing();
  api = (RTEMS_API_Control*)executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  _Thread_Disable_dispatch(); /* cannot reschedule while */
                              /*   the thread is inconsistent */

  _API_extensions_Add_post_switch( &_RTEMS_signal_Post_switch );

  if ( !_ASR_Is_null_handler( asr_handler ) ) {
    asr->mode_set = mode_set;
    asr->handler = asr_handler;
  }
  else
    _ASR_Initialize( asr );
  _Thread_Enable_dispatch();
  return RTEMS_SUCCESSFUL;
}
