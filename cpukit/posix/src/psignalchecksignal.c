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
 *  _POSIX_signals_Check_signal
 */

boolean _POSIX_signals_Check_signal(
  POSIX_API_Control  *api,
  int                 signo,
  boolean             is_global
)
{
  siginfo_t                   siginfo_struct;
  sigset_t                    saved_signals_blocked;

  if ( ! _POSIX_signals_Clear_signals( api, signo, &siginfo_struct,
                                       is_global, TRUE ) )
    return FALSE;

  /*
   *  Since we made a union of these, only one test is necessary but this is
   *  safer.
   */ 

  assert( _POSIX_signals_Vectors[ signo ].sa_handler ||
          _POSIX_signals_Vectors[ signo ].sa_sigaction );
 
  /*
   *  Just to prevent sending a signal which is currently being ignored.
   */

  if ( _POSIX_signals_Vectors[ signo ].sa_handler == SIG_IGN )
    return FALSE;

  /*
   *  Block the signals requested in sa_mask
   */

  saved_signals_blocked = api->signals_blocked;
  api->signals_blocked |= _POSIX_signals_Vectors[ signo ].sa_mask;

  /* Here, the signal handler function executes */

  switch ( _POSIX_signals_Vectors[ signo ].sa_flags ) {
    case SA_SIGINFO:
/*
 *
 *     assert( is_global );
 */
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
   *  Restore the previous set of blocked signals
   */
 
  api->signals_blocked = saved_signals_blocked;

  return TRUE;
}

