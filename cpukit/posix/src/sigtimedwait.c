/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief Wait for Queued Signals
 *  @ingroup POSIXAPI
 */

/*
 *  COPYRIGHT (c) 1989-2008.
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

#include <signal.h>

#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/posixapi.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/todimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/isr.h>

static int _POSIX_signals_Get_lowest(
  sigset_t   set
)
{
  int signo;

  for ( signo = SIGRTMIN ; signo <= SIGRTMAX ; signo++ ) {
    if ( set & signo_to_mask( signo ) ) {
      goto found_it;
    }
  }

  /*
   *  We assume SIGHUP == 1 and is the first non-real-time signal.
   */

  #if (SIGHUP != 1)
    #error "Assumption that SIGHUP==1 violated!!"
  #endif
  for ( signo = SIGHUP ; signo <= __SIGLASTNOTRT ; signo++ ) {
    if ( set & signo_to_mask( signo ) ) {
      goto found_it;
    }
  }

  /*
   *  This is structured this way to eliminate the need to have
   *  a return 0.  This routine will NOT be called unless a signal
   *  is pending in the set passed in.
   */
found_it:
  return signo;
}

/**
 *  3.3.8 Synchronously Accept a Signal, P1003.1b-1993, p. 76
 */
int sigtimedwait(
  const sigset_t         *__restrict set,
  siginfo_t              *__restrict info,
  const struct timespec  *__restrict timeout
)
{
  Thread_Control       *executing;
  POSIX_API_Control    *api;
  siginfo_t             signal_information;
  siginfo_t            *the_info;
  int                   signo;
  Thread_queue_Context  queue_context;
  int                   error;

  /*
   *  Error check parameters before disabling interrupts.
   */
  if ( !set )
    rtems_set_errno_and_return_minus_one( EINVAL );

  _Thread_queue_Context_initialize( &queue_context );

  /*  NOTE: This is very specifically a RELATIVE not ABSOLUTE time
   *        in the Open Group specification.
   */

  if ( timeout != NULL ) {
    _Thread_queue_Context_set_enqueue_timeout_monotonic_timespec(
      &queue_context,
      timeout,
      false
    );
  } else {
    _Thread_queue_Context_set_enqueue_do_nothing_extra( &queue_context );
  }

  /*
   *  Initialize local variables.
   */

  the_info = ( info ) ? info : &signal_information;

  executing = _Thread_Get_executing();
  api = executing->API_Extensions[ THREAD_API_POSIX ];

  /*
   *  What if they are already pending?
   */

  /* API signals pending? */

  _POSIX_signals_Acquire( &queue_context );
  if ( *set & api->signals_pending ) {
    /* XXX real info later */
    the_info->si_signo = _POSIX_signals_Get_lowest( api->signals_pending );
    _POSIX_signals_Clear_signals(
      api,
      the_info->si_signo,
      the_info,
      false,
      false,
      false
    );
    _POSIX_signals_Release( &queue_context );

    the_info->si_code = SI_USER;
    the_info->si_value.sival_int = 0;
    return the_info->si_signo;
  }

  /* Process pending signals? */

  if ( *set & _POSIX_signals_Pending ) {
    signo = _POSIX_signals_Get_lowest( _POSIX_signals_Pending );
    _POSIX_signals_Clear_signals( api, signo, the_info, true, false, false );
    _POSIX_signals_Release( &queue_context );

    the_info->si_signo = signo;
    the_info->si_code = SI_USER;
    the_info->si_value.sival_int = 0;
    return signo;
  }

  the_info->si_signo = -1;

  executing->Wait.option          = *set;
  executing->Wait.return_argument = the_info;
  _Thread_queue_Context_set_thread_state(
    &queue_context,
    STATES_WAITING_FOR_SIGNAL | STATES_INTERRUPTIBLE_BY_SIGNAL
  );
  _Thread_queue_Enqueue(
    &_POSIX_signals_Wait_queue.Queue,
    POSIX_SIGNALS_TQ_OPERATIONS,
    executing,
    &queue_context
  );

  /*
   * When the thread is set free by a signal, it is need to eliminate
   * the signal.
   */

  _POSIX_signals_Clear_signals(
    api,
    the_info->si_signo,
    the_info,
    false,
    false,
    true
  );

  /* Set errno only if return code is not EINTR or
   * if EINTR was caused by a signal being caught, which
   * was not in our set.
   */

  error = _POSIX_Get_error_after_wait( executing );

  if (
    error != EINTR
     || ( *set & signo_to_mask( the_info->si_signo ) ) == 0
  ) {
    if ( error == ETIMEDOUT ) {
      error = EAGAIN;
    }

    rtems_set_errno_and_return_minus_one( error );
  }

  return the_info->si_signo;
}
