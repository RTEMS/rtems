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

#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/seterr.h>

/*
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_settime(
  clockid_t              clock_id,
  const struct timespec *tp
)
{
  if ( !tp )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( clock_id == CLOCK_REALTIME ) {
    if ( tp->tv_sec < TOD_SECONDS_1970_THROUGH_1988 )
      rtems_set_errno_and_return_minus_one( EINVAL );

    _Thread_Disable_dispatch();
      _TOD_Set( tp );
    _Thread_Enable_dispatch();
  }
#ifdef _POSIX_CPUTIME
  else if ( clock_id == CLOCK_PROCESS_CPUTIME_ID )
    rtems_set_errno_and_return_minus_one( ENOSYS );
#endif
#ifdef _POSIX_THREAD_CPUTIME
  else if ( clock_id == CLOCK_THREAD_CPUTIME_ID )
    rtems_set_errno_and_return_minus_one( ENOSYS );
#endif
  else
    rtems_set_errno_and_return_minus_one( EINVAL );

  return 0;
}
