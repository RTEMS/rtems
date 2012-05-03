/*
 *  COPYRIGHT (c) 1989-2007.
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
#include <pthread.h>
#include <signal.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tqdata.h>
#include <rtems/score/wkspace.h>
#include <rtems/seterr.h>
#include <rtems/posix/threadsup.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/time.h>
#include <stdio.h>


/*
 *  _POSIX_signals_Unblock_thread
 */

/* XXX this routine could probably be cleaned up */
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
      return true;
    }

    /*
     *  This should only be reached via pthread_kill().
     */

    return false;
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
      /*
       *  In pthread_cond_wait, a thread will be blocking on a thread
       *  queue, but is also interruptible by a POSIX signal.
       */
       if ( _States_Is_waiting_on_thread_queue(the_thread->current_state) )
         _Thread_queue_Extract_with_proxy( the_thread );
       else if ( _States_Is_delaying(the_thread->current_state) ) {
          (void) _Watchdog_Remove( &the_thread->Timer );
          _Thread_Unblock( the_thread );
       }

    } else if ( the_thread->current_state == STATES_READY ) {
      if ( _ISR_Is_in_progress() && _Thread_Is_executing( the_thread ) )
        _Thread_Dispatch_necessary = true;
    }
  }
  return false;
}
