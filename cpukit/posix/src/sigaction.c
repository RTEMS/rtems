/**
 * @file
 *
 * @brief Allows calling process to examine action of a Specific Signal
 * @ingroup POSIXAPI
 */

/*
 *  3.3.4 Examine and Change Signal Action, P1003.1b-1993, p. 70
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthreadimpl.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/seterr.h>
#include <rtems/score/isr.h>

/*
 * PARAMETERS_PASSING_S is defined in ptimer.c
 */

extern void PARAMETERS_PASSING_S (int num_signal, const struct sigaction inf);

int sigaction(
  int                     sig,
  const struct sigaction *act,
  struct sigaction       *oact
)
{
  ISR_Level     level;

  if ( !sig )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( !is_valid_signo(sig) )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  Some signals cannot be ignored (P1003.1b-1993, pp. 70-72 and references.
   *
   *  NOTE: Solaris documentation claims to "silently enforce" this which
   *        contradicts the POSIX specification.
   */

  if ( sig == SIGKILL )
    rtems_set_errno_and_return_minus_one( EINVAL );

  _Thread_Disable_dispatch();

  if ( oact )
    *oact = _POSIX_signals_Vectors[ sig ];

  /*
   *  Evaluate the new action structure and set the global signal vector
   *  appropriately.
   */

  if ( act ) {

    /*
     *  Unless the user is installing the default signal actions, then
     *  we can just copy the provided sigaction structure into the vectors.
     */

    _ISR_Disable( level );
      if ( act->sa_handler == SIG_DFL ) {
        _POSIX_signals_Vectors[ sig ] = _POSIX_signals_Default_vectors[ sig ];
      } else {
         _POSIX_signals_Clear_process_signals( sig );
         _POSIX_signals_Vectors[ sig ] = *act;
      }
    _ISR_Enable( level );
  }

  _Thread_Enable_dispatch();

  return 0;
}
