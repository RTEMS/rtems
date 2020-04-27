/**
 * @file
 *
 * @ingroup POSIX_SIGNALS
 *
 * @brief POSIX Signals Thread Unlock
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <rtems/score/isr.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/seterr.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/posix/pthreadimpl.h>
#include <stdio.h>

static void _POSIX_signals_Check_signal(
  POSIX_API_Control  *api,
  int                 signo,
  bool                is_global
)
{
  siginfo_t siginfo_struct;
  sigset_t  saved_signals_unblocked;

  if ( ! _POSIX_signals_Clear_signals( api, signo, &siginfo_struct,
                                       is_global, true, true ) )
    return;

  /*
   *  Since we made a union of these, only one test is necessary but this is
   *  safer.
   */
  #if defined(RTEMS_DEBUG)
    assert( _POSIX_signals_Vectors[ signo ].sa_handler ||
            _POSIX_signals_Vectors[ signo ].sa_sigaction );
  #endif

  /*
   *  Just to prevent sending a signal which is currently being ignored.
   */
  if ( _POSIX_signals_Vectors[ signo ].sa_handler == SIG_IGN )
    return;

  /*
   *  Block the signals requested in sa_mask
   */
  saved_signals_unblocked = api->signals_unblocked;
  api->signals_unblocked &= ~_POSIX_signals_Vectors[ signo ].sa_mask;

  /*
   *  Here, the signal handler function executes
   */
  switch ( _POSIX_signals_Vectors[ signo ].sa_flags ) {
    case SA_SIGINFO:
      (*_POSIX_signals_Vectors[ signo ].sa_sigaction)(
        signo,
        &siginfo_struct,
        NULL        /* context is undefined per 1003.1b-1993, p. 66 */
      );
      break;
    default:
      (*_POSIX_signals_Vectors[ signo ].sa_handler)( signo );
      break;
  }

  /*
   *  Restore the previous set of unblocked signals
   */
  api->signals_unblocked = saved_signals_unblocked;
}

static void _POSIX_signals_Action_handler(
  Thread_Control   *executing,
  Thread_Action    *action,
  ISR_lock_Context *lock_context
)
{
  POSIX_API_Control *api;
  int                signo;
  uint32_t           hold_errno;

  (void) action;
  _Thread_State_release( executing, lock_context );

  api = executing->API_Extensions[ THREAD_API_POSIX ];

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
   *  In case the executing thread is blocked or about to block on something
   *  that uses the thread wait information, then this is a kernel bug.
   */
  _Assert(
    ( _Thread_Wait_flags_get( executing )
      & ( THREAD_WAIT_STATE_BLOCKED | THREAD_WAIT_STATE_INTEND_TO_BLOCK ) ) == 0
  );

  /*
   *  If we invoke any user code, there is the possibility that
   *  a new signal has been posted that we should process so we
   *  restart the loop if a signal handler was invoked.
   *
   *  The first thing done is to check there are any signals to be
   *  processed at all.  No point in doing this loop otherwise.
   */
  while (1) {
    Thread_queue_Context queue_context;

    _Thread_queue_Context_initialize( &queue_context );
    _POSIX_signals_Acquire( &queue_context );
      if ( !(api->signals_unblocked &
            (api->signals_pending | _POSIX_signals_Pending)) ) {
       _POSIX_signals_Release( &queue_context );
       break;
     }
    _POSIX_signals_Release( &queue_context );

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

static bool _POSIX_signals_Unblock_thread_done(
  Thread_Control    *the_thread,
  POSIX_API_Control *api,
  bool               status
)
{
  ISR_lock_Context lock_context;

  _Thread_State_acquire( the_thread, &lock_context );
  _Thread_Add_post_switch_action(
    the_thread,
    &api->Signal_action,
    _POSIX_signals_Action_handler
  );
  _Thread_State_release( the_thread, &lock_context );

  return status;
}

bool _POSIX_signals_Unblock_thread(
  Thread_Control  *the_thread,
  int              signo,
  siginfo_t       *info
)
{
  POSIX_API_Control  *api;
  sigset_t            mask;
  siginfo_t          *the_info = NULL;

  api = the_thread->API_Extensions[ THREAD_API_POSIX ];

  mask = signo_to_mask( signo );

  /*
   *  Is the thread is specifically waiting for a signal?
   */

  if ( _States_Is_interruptible_signal( the_thread->current_state ) ) {

    if ( (the_thread->Wait.option & mask) || (api->signals_unblocked & mask) ) {
      the_thread->Wait.return_code = STATUS_INTERRUPTED;

      the_info = (siginfo_t *) the_thread->Wait.return_argument;

      if ( !info ) {
        the_info->si_signo = signo;
        the_info->si_code = SI_USER;
        the_info->si_value.sival_int = 0;
      } else {
        *the_info = *info;
      }

      _Thread_queue_Extract_with_proxy( the_thread );
      return _POSIX_signals_Unblock_thread_done( the_thread, api, true );
    }

    /*
     *  This should only be reached via pthread_kill().
     */

    return _POSIX_signals_Unblock_thread_done( the_thread, api, false );
  }

  /*
   *  Thread is not waiting due to a sigwait.
   */
  if ( api->signals_unblocked & mask ) {

    /*
     *  The thread is interested in this signal.  We are going
     *  to post it.  We have a few broad cases:
     *    + If it is blocked on an interruptible signal, THEN
     *        we unblock the thread.
     *    + If it is in the ready state AND
     *      we are sending from an ISR AND
     *      it is the interrupted thread AND
     *      it is not blocked, THEN
     *        we need to dispatch at the end of this ISR.
     *    + Any other combination, do nothing.
     */

    if ( _States_Is_interruptible_by_signal( the_thread->current_state ) ) {
      the_thread->Wait.return_code = STATUS_INTERRUPTED;
      _Thread_queue_Extract_with_proxy( the_thread );
    }
  }
  return _POSIX_signals_Unblock_thread_done( the_thread, api, false );
}
