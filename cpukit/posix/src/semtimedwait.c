/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
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

#include <stdarg.h>

#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/posix/semaphore.h>
#include <rtems/posix/time.h>
#include <rtems/seterr.h>

/*PAGE
 *
 *  11.2.6 Lock a Semaphore, P1003.1b-1993, p.226
 *
 *  NOTE: P1003.4b/D8 adds sem_timedwait(), p. 27
 */

int sem_timedwait(
  sem_t                 *sem,
  const struct timespec *abstime
)
{
  /*
   *  The abstime is a walltime.  We turn it into an interval.
   */
  Watchdog_Interval              ticks = 0;
  struct timespec                current_time;
  struct timespec                difference;
  Core_semaphore_Blocking_option blocking = CORE_SEMAPHORE_BLOCK_WITH_TIMEOUT;

  /*
   *  Error check the absolute time to timeout
   */
  if ( !_Timespec_Is_valid( abstime ) ) {
    blocking = CORE_SEMAPHORE_BAD_TIMEOUT_VALUE;
  } else { 
    _TOD_Get( &current_time );
    /*
     *  Make sure the abstime is in the future
     */
    if ( _Timespec_Less_than( abstime, &current_time ) ) {
      blocking = CORE_SEMAPHORE_BAD_TIMEOUT;
    } else {
      _Timespec_Subtract( &current_time, abstime, &difference );
      ticks = _Timespec_To_ticks( &difference );
      blocking = CORE_SEMAPHORE_BLOCK_WITH_TIMEOUT;
    }
   }

   return _POSIX_Semaphore_Wait_support( sem, blocking, ticks );
}
