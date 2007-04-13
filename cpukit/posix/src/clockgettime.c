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

int clock_gettime(
  clockid_t        clock_id,
  struct timespec *tp
)
{
  if ( !tp )
    rtems_set_errno_and_return_minus_one( EINVAL );

  switch ( clock_id ) {

    case CLOCK_REALTIME:
      _TOD_Get(tp);
      break;

#if 0
    case CLOCK_MONOTONIC:
      _TOD_Get_uptime(tp);
      break;
#endif

#ifdef _POSIX_CPUTIME
    case CLOCK_PROCESS_CPUTIME:
      _TOD_Get_uptime(tp);
      break;
#endif

#ifdef _POSIX_THREAD_CPUTIME
    case CLOCK_THREAD_CPUTIME:
      return POSIX_NOT_IMPLEMENTED();
      break;
#endif
    default:
      rtems_set_errno_and_return_minus_one( EINVAL );

  }
  return 0;
}
