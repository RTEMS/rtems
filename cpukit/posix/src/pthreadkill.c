/*
 *  3.3.10 Send a Signal to a Thread, P1003.1c/D10, p. 43
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignal.h>
#include <rtems/score/isr.h>
#include <rtems/seterr.h>

int pthread_kill(
  pthread_t   thread,
  int         sig
)
{
  POSIX_API_Control  *api;
  Thread_Control     *the_thread;
  Objects_Locations  location;

  if ( sig && !is_valid_signo(sig) )
    rtems_set_errno_and_return_minus_one( EINVAL );

/* commented out when posix timers added 
  if ( _POSIX_signals_Vectors[ sig ].sa_flags == SA_SIGINFO )
    rtems_set_errno_and_return_minus_one( ENOSYS );
*/

  the_thread = _POSIX_Threads_Get( thread, &location );
  switch ( location ) {
    case OBJECTS_ERROR:
    case OBJECTS_REMOTE:
      rtems_set_errno_and_return_minus_one( ESRCH );
    case OBJECTS_LOCAL:
      /*
       *  If sig == 0 then just validate arguments
       */

      api = the_thread->API_Extensions[ THREAD_API_POSIX ];

      if ( sig ) {

        if ( _POSIX_signals_Vectors[ sig ].sa_handler == SIG_IGN ) {
          _Thread_Enable_dispatch();
          return 0;
        }

        /* XXX critical section */

        api->signals_pending |= signo_to_mask( sig );

        (void) _POSIX_signals_Unblock_thread( the_thread, sig, NULL );

        the_thread->do_post_task_switch_extension = TRUE;

        if ( _ISR_Is_in_progress() && _Thread_Is_executing( the_thread ) )
          _ISR_Signals_to_thread_executing = TRUE;
      }
      _Thread_Enable_dispatch();
      return 0;
  }

  return POSIX_BOTTOM_REACHED();
}
