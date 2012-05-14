/**
 *  @file  score/src/timespecaddto.c
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <sys/types.h>
#include <rtems/score/timespec.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/**
 *
 *  This routines adds two timespecs.  The second argument is added
 *  to the first.
 */

uint32_t _Timespec_Add_to(
  struct timespec       *time,
  const struct timespec *add
)
{
  uint32_t seconds = add->tv_sec;

  /* Add the basics */
  time->tv_sec += add->tv_sec;
  time->tv_nsec += add->tv_nsec;

  /* Now adjust it so nanoseconds is in range */
  while ( time->tv_nsec >= TOD_NANOSECONDS_PER_SECOND ) {
    time->tv_nsec -= TOD_NANOSECONDS_PER_SECOND;
    time->tv_sec++;
    seconds++;
  }

  return seconds;
}
