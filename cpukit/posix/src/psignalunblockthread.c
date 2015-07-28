/**
 * @file
 *
 * @brief POSIX Signals Thread Unlock
 * @ingroup POSIX_SIGNALS
 */

/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <pthread.h>
#include <signal.h>

#include <rtems/system.h>
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

static bool _POSIX_signals_Unblock_thread_done(
  Thread_Control    *the_thread,
  POSIX_API_Control *api,
  bool               status
)
{
  _Thread_Add_post_switch_action( the_thread, &api->Signal_action );

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

    if ( (the_thread->Wait.option & mask) || (~api->signals_blocked & mask) ) {
      the_thread->Wait.return_code = EINTR;

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
  if ( ~api->signals_blocked & mask ) {

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
      the_thread->Wait.return_code = EINTR;
      _Thread_queue_Extract_with_proxy( the_thread );
    }
  }
  return _POSIX_signals_Unblock_thread_done( the_thread, api, false );
}
