/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/seterr.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  4.5.1 Get System Time, P1003.1b-1993, p. 91
 */

/* Using the implementation in newlib */
#if 0
time_t time(
  time_t   *tloc
)
{
  time_t  seconds_since_epoch;

  /*
   *  No error is the time of day is not set.   For RTEMS the system time
   *  starts out at the rtems epoch.
   */

  /*
   *  Internally the RTEMS epoch is 1988.  This must be taken into account.
   */

  seconds_since_epoch = _TOD_Seconds_since_epoch;
     
  seconds_since_epoch += POSIX_TIME_SECONDS_1970_THROUGH_1988;

  if ( tloc )
    *tloc = seconds_since_epoch;

  return seconds_since_epoch;
}
#endif
