/*
 *  Marginal implementations of some POSIX API routines
 *  to be used when POSIX is disabled.
 *
 *    + kill
 *    + _kill_r
 *    + __kill
 *    + sleep
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>

#include <unistd.h>

/*
 *  3.4.3 Delay Process Execution, P1003.1b-1993, p. 81
 *
 *  $Id$
 */

#include <time.h>
#include <unistd.h>

#include <rtems.h>

#if !defined(RTEMS_POSIX_API)
unsigned int sleep(
  unsigned int seconds
)
{
  rtems_status_code status;
  rtems_interval    ticks_per_second;

  ticks_per_second = rtems_clock_get_ticks_per_second() * seconds;

  status = rtems_task_wake_after( ticks_per_second );

  /*
   *  Returns the "unslept" amount of time.  In RTEMS signals are not
   *  interruptable, so tasks really sleep all of the requested time.
   */

  return 0;
}
#endif
