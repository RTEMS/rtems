/**
 * @file
 *
 * @brief Examine Pending Signals
 * @ingroup POSIX
 */

/*
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

#include <signal.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/psignalimpl.h>
#include <rtems/seterr.h>

int sigpending(
  sigset_t  *set
)
{
  POSIX_API_Control  *api;

  if ( !set )
    rtems_set_errno_and_return_minus_one( EINVAL );

  api = _Thread_Executing->API_Extensions[ THREAD_API_POSIX ];

  *set = api->signals_pending | _POSIX_signals_Pending;

  return 0;
}
