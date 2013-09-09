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
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include <rtems/score/isrlevel.h>
#include <rtems/score/statesimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/time.h>
#include <rtems/config.h>
#include <rtems/seterr.h>

/*
 *  Ensure we have the same number of vectors and default vector entries
 */
RTEMS_STATIC_ASSERT(
  sizeof( _POSIX_signals_Vectors ) == sizeof( _POSIX_signals_Default_vectors ),
  posix_signals_vectors
);

/*** PROCESS WIDE STUFF ****/

ISR_lock_Control _POSIX_signals_Lock = ISR_LOCK_INITIALIZER;

sigset_t  _POSIX_signals_Pending;

void _POSIX_signals_Abnormal_termination_handler(
  int signo __attribute__((unused)) )
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

Thread_queue_Control _POSIX_signals_Wait_queue;

Chain_Control _POSIX_signals_Inactive_siginfo;
Chain_Control _POSIX_signals_Siginfo[ SIG_ARRAY_MAX ];

/*
 *  XXX - move these
 */

#define _States_Is_interruptible_signal( _states ) \
  ( ((_states) & \
    (STATES_WAITING_FOR_SIGNAL|STATES_INTERRUPTIBLE_BY_SIGNAL)) == \
      (STATES_WAITING_FOR_SIGNAL|STATES_INTERRUPTIBLE_BY_SIGNAL))

/*
 *  _POSIX_signals_Post_switch_extension
 */

static void _POSIX_signals_Post_switch_hook(
  Thread_Control  *the_thread
)
{
  POSIX_API_Control  *api;
  int                 signo;
  ISR_Level           level;
  int                 hold_errno;
  Thread_Control     *executing;

  executing = _Thread_Get_executing();
  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  /*
   *  We need to ensure that if the signal handler executes a call
   *  which overwrites the unblocking status, we restore it.
   */
  hold_errno = executing->Wait.return_code;

  /*
   * api may be NULL in case of a thread close in progress
   */
  if ( !api )
    return;

  /*
   *  If we invoke any user code, there is the possibility that
   *  a new signal has been posted that we should process so we
   *  restart the loop if a signal handler was invoked.
   *
   *  The first thing done is to check there are any signals to be
   *  processed at all.  No point in doing this loop otherwise.
   */
  while (1) {
    _POSIX_signals_Acquire( level );
      if ( !(~api->signals_blocked &
            (api->signals_pending | _POSIX_signals_Pending)) ) {
       _POSIX_signals_Release( level );
       break;
     }
    _POSIX_signals_Release( level );

    for ( signo = SIGRTMIN ; signo <= SIGRTMAX ; signo++ ) {
      _POSIX_signals_Check_signal( api, signo, false );
      _POSIX_signals_Check_signal( api, signo, true );
    }
    /* Unfortunately - nothing like __SIGFIRSTNOTRT in newlib signal .h */

    for ( signo = SIGHUP ; signo <= __SIGLASTNOTRT ; signo++ ) {
      _POSIX_signals_Check_signal( api, signo, false );
      _POSIX_signals_Check_signal( api, signo, true );
    }
  }

  executing->Wait.return_code = hold_errno;
}

API_extensions_Post_switch_control _POSIX_signals_Post_switch = {
  .hook = _POSIX_signals_Post_switch_hook
};

void _POSIX_signals_Manager_Initialization(void)
{
  uint32_t   signo;
  uint32_t   maximum_queued_signals;

  maximum_queued_signals = Configuration_POSIX_API.maximum_queued_signals;

  memcpy(
    _POSIX_signals_Vectors,
    _POSIX_signals_Default_vectors,
    sizeof( _POSIX_signals_Vectors )
  );

  /*
   *  Initialize the set of pending signals for the entire process
   */
  sigemptyset( &_POSIX_signals_Pending );

  /*
   *  Initialize the queue we use to block for signals
   */
  _Thread_queue_Initialize(
    &_POSIX_signals_Wait_queue,
    THREAD_QUEUE_DISCIPLINE_FIFO,
    STATES_WAITING_FOR_SIGNAL | STATES_INTERRUPTIBLE_BY_SIGNAL,
    EAGAIN
  );

  /* XXX status codes */

  /*
   *  Allocate the siginfo pools.
   */
  for ( signo=1 ; signo<= SIGRTMAX ; signo++ )
    _Chain_Initialize_empty( &_POSIX_signals_Siginfo[ signo ] );

  if ( maximum_queued_signals ) {
    _Chain_Initialize(
      &_POSIX_signals_Inactive_siginfo,
      _Workspace_Allocate_or_fatal_error(
        maximum_queued_signals * sizeof( POSIX_signals_Siginfo_node )
      ),
      maximum_queued_signals,
      sizeof( POSIX_signals_Siginfo_node )
    );
  } else {
    _Chain_Initialize_empty( &_POSIX_signals_Inactive_siginfo );
  }
}
