/*
 *  XXX 3.4.3 Delay Process Execution, P1003.1b-1993, p. 81
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


unsigned usleep(
  unsigned int useconds
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
