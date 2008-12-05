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

#ifndef HAVE_SLEEP
#include <time.h>
#include <unistd.h>

#include <rtems/system.h>


unsigned int sleep(
  unsigned int seconds
)
{
  struct timespec tp;
  struct timespec tm;

  tp.tv_sec = seconds;
  tp.tv_nsec = 0;

  nanosleep( &tp, &tm );

  return tm.tv_sec;       /* seconds remaining */
}

#endif
