/**
 * @file
 *
 * @ingroup RTEMSImplClassicSignal
 *
 * @brief This source file contains the implementation of
 *   rtems_signal_catch() and the Signal Manager multiprocessing (MP) support
 *   system initialization.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
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
#include <rtems/score/threadimpl.h>

RTEMS_STATIC_ASSERT( RTEMS_DEFAULT_MODES == 0, _ASR_Create_mode_set );

rtems_status_code rtems_signal_catch(
  rtems_asr_entry   asr_handler,
  rtems_mode        mode_set
)
{
  Thread_Control     *executing;
  RTEMS_API_Control  *api;
  ASR_Information    *asr;
  ISR_lock_Context    lock_context;

#if defined(RTEMS_SMP) || CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  if ( !_Modes_Is_interrupt_level_supported( mode_set ) ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  executing = _Thread_State_acquire_for_executing( &lock_context );

#if defined(RTEMS_SMP)
  if ( !_Modes_Is_preempt_mode_supported( mode_set, executing ) ) {
    _Thread_State_release( executing, &lock_context );
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;
  asr->handler = asr_handler;
  asr->mode_set = mode_set;

  if ( asr_handler == NULL ) {
    Chain_Node *node;

    asr->signals_pending = 0;
    node = &api->Signal_action.Node;

    if ( !_Chain_Is_node_off_chain( node ) ) {
      _Chain_Extract_unprotected( node );
      _Chain_Set_off_chain( node );
    }
  }

  _Thread_State_release( executing, &lock_context );
  return RTEMS_SUCCESSFUL;
}
