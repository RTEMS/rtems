/*
 *  $Id$
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

/*PAGE
 *
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_settime(
  clockid_t              clock_id,
  const struct timespec *tp
)
{
  if ( !tp )
    rtems_set_errno_and_return_minus_one( EINVAL );

  switch ( clock_id ) {

    case CLOCK_REALTIME:
      if ( tp->tv_sec < TOD_SECONDS_1970_THROUGH_1988 )
        rtems_set_errno_and_return_minus_one( EINVAL );

      _Thread_Disable_dispatch();
        _TOD_Set( tp );
      _Thread_Enable_dispatch();
      break;

#ifdef _POSIX_CPUTIME
    case CLOCK_PROCESS_CPUTIME:
      rtems_set_errno_and_return_minus_one( ENOSYS );
      break;
#endif

#ifdef _POSIX_THREAD_CPUTIME
    case CLOCK_THREAD_CPUTIME:
      rtems_set_errno_and_return_minus_one( ENOSYS );
      break;
#endif
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );

  }
  return 0;
}
