/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief POSIX Signals Support
 *
 * This include file defines internal information about POSIX signals.
 */

/*
 *  COPYRIGHT (c) 1989-2013.
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

#ifndef _RTEMS_POSIX_PSIGNALIMPL_H
#define _RTEMS_POSIX_PSIGNALIMPL_H

/**
 * @defgroup POSIX_SIGNALS POSIX Signals Support
 *
 * @ingroup POSIXAPI
 *
 * @brief Internal Information about POSIX Signals
 * 
 */
/**@{**/

#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/sigset.h>
#include <rtems/score/isrlock.h>
#include <rtems/score/percpu.h>
#include <rtems/score/threadqimpl.h>

#define POSIX_SIGNALS_TQ_OPERATIONS &_Thread_queue_Operations_FIFO

#define _States_Is_interruptible_signal( _states ) \
  ( ((_states) & \
    (STATES_WAITING_FOR_SIGNAL|STATES_INTERRUPTIBLE_BY_SIGNAL)) == \
      (STATES_WAITING_FOR_SIGNAL|STATES_INTERRUPTIBLE_BY_SIGNAL))

#define SIGACTION_TERMINATE \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Abnormal_termination_handler} }
#define SIGACTION_IGNORE \
  { 0, SIGNAL_ALL_MASK, {SIG_IGN} }
#define SIGACTION_STOP \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Stop_handler} }
#define SIGACTION_CONTINUE \
  { 0, SIGNAL_ALL_MASK, {_POSIX_signals_Continue_handler} }

#define SIG_ARRAY_MAX  (SIGRTMAX + 1)

/*
 *  Variables
 */

extern sigset_t  _POSIX_signals_Pending;

extern const struct sigaction _POSIX_signals_Default_vectors[ SIG_ARRAY_MAX ];

extern struct sigaction _POSIX_signals_Vectors[ SIG_ARRAY_MAX ];

extern Thread_queue_Control _POSIX_signals_Wait_queue;

extern Chain_Control _POSIX_signals_Inactive_siginfo;

extern Chain_Control _POSIX_signals_Siginfo[ SIG_ARRAY_MAX ];

/*
 *  Internal routines
 */

static inline void _POSIX_signals_Acquire(
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Acquire( &_POSIX_signals_Wait_queue, queue_context );
}

static inline void _POSIX_signals_Release(
  Thread_queue_Context *queue_context
)
{
  _Thread_queue_Release( &_POSIX_signals_Wait_queue, queue_context );
}

/**
 * @brief Unlock POSIX signals thread.
 */
bool _POSIX_signals_Unblock_thread(
  Thread_Control  *the_thread,
  int              signo,
  siginfo_t       *info
);

/**
 * @brief Clear POSIX signals.
 */
bool _POSIX_signals_Clear_signals(
  POSIX_API_Control  *api,
  int                 signo,
  siginfo_t          *info,
  bool                is_global,
  bool                check_blocked,
  bool                do_signals_acquire_release
);

int _POSIX_signals_Send(
  pid_t               pid,
  int                 sig,
  const union sigval *value
);

/**
 *  @brief Set POSIX process signals.
 */
void _POSIX_signals_Set_process_signals(
  sigset_t   mask
);

void _POSIX_signals_Clear_process_signals(
  int        signo
);

/*
 *  Default signal handlers
 */

#define _POSIX_signals_Stop_handler NULL
#define _POSIX_signals_Continue_handler NULL

void _POSIX_signals_Abnormal_termination_handler( int signo );

/** @} */

#endif
/* end of file */
