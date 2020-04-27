/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Allows calling process to examine action of a Specific Signal
 */

/*
 *  3.3.4 Examine and Change Signal Action, P1003.1b-1993, p. 70
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <signal.h>
#include <errno.h>

#include <rtems/posix/psignalimpl.h>
#include <rtems/seterr.h>

int sigaction(
  int                     sig,
  const struct sigaction *__restrict act,
  struct sigaction       *__restrict oact
)
{
  Thread_queue_Context queue_context;

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

  _Thread_queue_Context_initialize( &queue_context );
  _POSIX_signals_Acquire( &queue_context );

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

    if ( act->sa_handler == SIG_DFL ) {
      _POSIX_signals_Vectors[ sig ] = _POSIX_signals_Default_vectors[ sig ];
    } else {
       _POSIX_signals_Clear_process_signals( sig );
       _POSIX_signals_Vectors[ sig ] = *act;
    }
  }

  _POSIX_signals_Release( &queue_context );

  return 0;
}
