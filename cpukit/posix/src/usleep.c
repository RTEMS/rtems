/*
 *  3.4.3 Delay Process Execution, P1003.1b-1993, p. 81
 *
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

#include <time.h>
#include <unistd.h>

#include <rtems/system.h>
#include <rtems/score/tod.h>


int usleep(
  useconds_t useconds
)
{
  struct timespec tp;
  struct timespec tm;
  unsigned remaining;

  tp.tv_sec = useconds / TOD_MICROSECONDS_PER_SECOND;
  tp.tv_nsec = (useconds % TOD_MICROSECONDS_PER_SECOND) * 1000;

  nanosleep( &tp, &tm );

  remaining  = tm.tv_sec * TOD_MICROSECONDS_PER_SECOND;
  remaining += tm.tv_nsec / 1000;
  return remaining;       /* seconds remaining */
}
