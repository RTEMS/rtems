/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  @file
 *
 *  @brief POSIX Signals Manager Initialization
 *  @ingroup POSIX_SIGNALS
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

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include <rtems/score/isrlevel.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>

/*
 *  Ensure we have the same number of vectors and default vector entries
 */
RTEMS_STATIC_ASSERT(
  sizeof( _POSIX_signals_Vectors ) == sizeof( _POSIX_signals_Default_vectors ),
  posix_signals_vectors
);

/*** PROCESS WIDE STUFF ****/

sigset_t  _POSIX_signals_Pending;

void _POSIX_signals_Abnormal_termination_handler(
  int signo RTEMS_UNUSED )
{
  exit( 1 );
}

#define SIG_ARRAY_MAX  (SIGRTMAX + 1)
const struct sigaction _POSIX_signals_Default_vectors[ SIG_ARRAY_MAX ] = {
  /* NO SIGNAL 0 */  SIGACTION_IGNORE,
  /* SIGHUP    1 */  SIGACTION_TERMINATE,
  /* SIGINT    2 */  SIGACTION_TERMINATE,
  /* SIGQUIT   3 */  SIGACTION_TERMINATE,
  /* SIGILL    4 */  SIGACTION_TERMINATE,
  /* SIGTRAP   5 */  SIGACTION_TERMINATE,
  /* SIGIOT    6 */  SIGACTION_TERMINATE,
  /* SIGABRT   6     SIGACTION_TERMINATE, -- alias for SIGIOT */
  /* SIGEMT    7 */  SIGACTION_TERMINATE,
  /* SIGFPE    8 */  SIGACTION_TERMINATE,
  /* SIGKILL   9 */  SIGACTION_TERMINATE,
  /* SIGBUS   10 */  SIGACTION_TERMINATE,
  /* SIGSEGV  11 */  SIGACTION_TERMINATE,
  /* SIGSYS   12 */  SIGACTION_TERMINATE,
  /* SIGPIPE  13 */  SIGACTION_TERMINATE,
  /* SIGALRM  14 */  SIGACTION_TERMINATE,
  /* SIGTERM  15 */  SIGACTION_TERMINATE,
  /* SIGURG   16 */  SIGACTION_TERMINATE,
  /* SIGSTOP  17 */  SIGACTION_TERMINATE,
  /* SIGTSTP  18 */  SIGACTION_TERMINATE,
  /* SIGCONT  19 */  SIGACTION_TERMINATE,
  /* SIGCHLD  20 */  SIGACTION_TERMINATE,
  /* SIGTTIN  21 */  SIGACTION_TERMINATE,
  /* SIGTTOU  22 */  SIGACTION_TERMINATE,
  /* SIGIO    23 */  SIGACTION_TERMINATE,
  /* SIGWINCH 24 */  SIGACTION_TERMINATE,
  /* SIGUSR1  25 */  SIGACTION_TERMINATE,
  /* SIGUSR2  26 */  SIGACTION_TERMINATE,
  /* SIGRT    27 */  SIGACTION_TERMINATE,
  /* SIGRT    28 */  SIGACTION_TERMINATE,
  /* SIGRT    29 */  SIGACTION_TERMINATE,
  /* SIGRT    30 */  SIGACTION_TERMINATE,
  /* SIGRTMAX 31 */  SIGACTION_TERMINATE
};

struct sigaction _POSIX_signals_Vectors[ SIG_ARRAY_MAX ];

Thread_queue_Control _POSIX_signals_Wait_queue =
  THREAD_QUEUE_INITIALIZER( "POSIX Signals" );

Chain_Control _POSIX_signals_Inactive_siginfo;
Chain_Control _POSIX_signals_Siginfo[ SIG_ARRAY_MAX ];

static void _POSIX_signals_Manager_Initialization(void)
{
  uint32_t signo;

  memcpy(
    _POSIX_signals_Vectors,
    _POSIX_signals_Default_vectors,
    sizeof( _POSIX_signals_Vectors )
  );

  /*
   *  Initialize the set of pending signals for the entire process
   */
  sigemptyset( &_POSIX_signals_Pending );

  /* XXX status codes */

  /*
   *  Allocate the siginfo pools.
   */
  for ( signo=1 ; signo<= SIGRTMAX ; signo++ ) {
    _Chain_Initialize_empty( &_POSIX_signals_Siginfo[ signo ] );
  }

  _Chain_Initialize(
    &_POSIX_signals_Inactive_siginfo,
    &_POSIX_signals_Siginfo_nodes[ 0 ],
    _POSIX_signals_Maximum_queued_signals,
    sizeof( _POSIX_signals_Siginfo_nodes[ 0 ] )
  );
}

RTEMS_SYSINIT_ITEM(
  _POSIX_signals_Manager_Initialization,
  RTEMS_SYSINIT_POSIX_SIGNALS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
