/* time.c
 *
 *  $Id$
 */

#include <time.h>
#include <rtems/score/tod.h>

/*
 *  Seconds from January 1, 1970 to January 1, 1988.  Used to account for
 *  differences between POSIX API and RTEMS core.
 */

#define POSIX_TIME_SECONDS_1970_THROUGH_1988 \
  (((1987 - 1970 + 1)  * TOD_SECONDS_PER_NON_LEAP_YEAR) + \
  (4 * TOD_SECONDS_PER_DAY))

/*
 *  4.5.1 Get System Time, P1003.1b-1993, p. 91
 */

time_t time(
  time_t   *tloc
)
{
  time_t  seconds_since_epoch;

  if ( !_TOD_Is_set() ) {
    /* XXX set errno */
    return -1;
  }

  /*
   *  Internally the RTEMS epoch is 1988.  This must be taken into account.
   */

  seconds_since_epoch = _TOD_Seconds_since_epoch;
     
  seconds_since_epoch += POSIX_TIME_SECONDS_1970_THROUGH_1988;

  if ( tloc )
    *tloc = seconds_since_epoch;

  return seconds_since_epoch;
}

#ifdef NOT_IMPLEMENTED_YET
/*
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_settime(
  clockid_t              clock_id,
  const struct timespec *tp
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_gettime(
  clockid_t        clock_id,
  struct timespec *tp
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  14.2.1 Clocks, P1003.1b-1993, p. 263
 */

int clock_getres(
  clockid_t        clock_id,
  struct timespec *res
)
{
  return POSIX_NOT_IMPLEMENTED();
}

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

/*
 *  14.2.5 High Resolution Sleep, P1003.1b-1993, p. 269
 */

int nanosleep(
  const struct timespec  *rqtp,
  struct timespec        *rmtp
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  20.1.3 Accessing a Process CPU-time CLock, P1003.4b/D8, p. 55
 */

int clock_getcpuclockid(
  pid_t      pid,
  clockid_t *clock_id
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  20.1.5 CPU-time Clock Attribute Access, P1003.4b/D8, p. 58
 */

int clock_setenable_attr(
  clockid_t    clock_id,
  int          attr
)
{
  return POSIX_NOT_IMPLEMENTED();
}

/*
 *  20.1.5 CPU-time Clock Attribute Access, P1003.4b/D8, p. 58
 */

int clock_getenable_attr(
  clockid_t    clock_id,
  int         *attr
)
{
  return POSIX_NOT_IMPLEMENTED();
}

#endif
