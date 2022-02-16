/* SPDX-License-Identifier: BSD-2-Clause */

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
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
  if (
    asr_handler != NULL &&
    !_Modes_Is_interrupt_level_supported( mode_set )
  ) {
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  executing = _Thread_State_acquire_for_executing( &lock_context );

#if defined(RTEMS_SMP)
  if (
    asr_handler != NULL &&
    !_Modes_Is_preempt_mode_supported( mode_set, executing )
  ) {
    _Thread_State_release( executing, &lock_context );
    return RTEMS_NOT_IMPLEMENTED;
  }
#endif

  api = executing->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;
  asr->handler = asr_handler;
  asr->mode_set = mode_set;

#if defined(RTEMS_SMP)
  if ( asr_handler == NULL ) {
    Chain_Node *node;

    /*
     * In SMP configurations, signals may be sent on other processors
     * (interrupts or threads) in parallel.  This will cause an inter-processor
     * interrupt which may be blocked by the above interrupt disable.
     */

    node = &api->Signal_action.Node;
    _Assert( asr->signals_pending == 0 || !_Chain_Is_node_off_chain( node ) );
    _Assert( asr->signals_pending != 0 || _Chain_Is_node_off_chain( node ) );

    if ( !_Chain_Is_node_off_chain( node ) ) {
      asr->signals_pending = 0;
      _Chain_Extract_unprotected( node );
      _Chain_Set_off_chain( node );
    }
  }
#else
  /*
   * In uniprocessor configurations, as soon as interrupts are disabled above
   * nobody can send signals to the executing thread.  So, pending signals at
   * this point cannot appear.
   */
  _Assert( asr->signals_pending == 0 );
  _Assert( _Chain_Is_node_off_chain( &api->Signal_action.Node ) );
#endif

  _Thread_State_release( executing, &lock_context );
  return RTEMS_SUCCESSFUL;
}
