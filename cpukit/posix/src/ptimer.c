/*
 *  $Id$
 */

#include <assert.h>
#include <time.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/thread.h>
#include <rtems/score/tod.h>

#include <rtems/posix/time.h>

/*
 *  14.2.2 Create a Per-Process Timer, P1003.1b-1993, p. 264
 */

int timer_create(
  clockid_t        clock_id,
  struct sigevent *evp,
  timer_t         *timerid
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  14.2.3 Delete a Per_process Timer, P1003.1b-1993, p. 266
 */

int timer_delete(
  timer_t timerid
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

int timer_settime(
  timer_t                  timerid,
  int                      flags,
  const struct itimerspec *value,
  struct itimerspec       *ovalue
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

int timer_gettime(
  timer_t            timerid,
  struct itimerspec *value
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  14.2.4 Per-Process Timers, P1003.1b-1993, p. 267
 */

int timer_getoverrun(
  timer_t   timerid
)
{
  return POSIX_NOT_IMPLEMENTED();
}
