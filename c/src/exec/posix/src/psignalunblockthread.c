/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
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


/*PAGE
 *
 *  _POSIX_signals_Unblock_thread
 */

/* XXX this routine could probably be cleaned up */
boolean _POSIX_signals_Unblock_thread( 
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
      return TRUE;
    }

    /*
     *  This should only be reached via pthread_kill().
     */

    return FALSE;
  }

  if ( ~api->signals_blocked & mask ) {
    the_thread->do_post_task_switch_extension = TRUE;

    if ( the_thread->current_state & STATES_INTERRUPTIBLE_BY_SIGNAL ) {
      the_thread->Wait.return_code = EINTR;
      if ( _States_Is_waiting_on_thread_queue(the_thread->current_state) )
        _Thread_queue_Extract_with_proxy( the_thread );
      else if ( _States_Is_delaying(the_thread->current_state)){
        if ( _Watchdog_Is_active( &the_thread->Timer ) )
          (void) _Watchdog_Remove( &the_thread->Timer );
        _Thread_Unblock( the_thread );
      }
    }
  }
  return FALSE;

}
