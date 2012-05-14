/*
 *  3.3.5 Examine and Change Blocked Signals, P1003.1b-1993, p. 73
 *
 *  NOTE: P1003.1c/D10, p. 37 adds pthread_sigmask().
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
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignal.h>
#include <rtems/seterr.h>

int pthread_sigmask(
  int               how,
  const sigset_t   *set,
  sigset_t         *oset
)
{
  POSIX_API_Control  *api;

  if ( !set && !oset )
    rtems_set_errno_and_return_minus_one( EINVAL );

  api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  if ( oset )
    *oset = api->signals_blocked;

  if ( !set )
    return 0;

  switch ( how ) {
    case SIG_BLOCK:
      api->signals_blocked |= *set;
      break;
    case SIG_UNBLOCK:
      api->signals_blocked &= ~*set;
      break;
    case SIG_SETMASK:
      api->signals_blocked = *set;
      break;
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );
  }

  /* XXX are there critical section problems here? */

  /* XXX evaluate the new set */

  if ( ~api->signals_blocked &
       (api->signals_pending | _POSIX_signals_Pending) ) {
    _Thread_Dispatch();
  }

  return 0;
}
