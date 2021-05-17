/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Suspends Execution of calling thread until Time elapses
 */

/*
 *  COPYRIGHT (c) 1989-2015.
 *  On-Line Applications Research Corporation (OAR).
 * 
 *  Copyright (c) 2016. Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>

#include <rtems/seterr.h>

/*
 *  14.2.5 High Resolution Sleep, P1003.1b-1993, p. 269
 */
int nanosleep(
  const struct timespec  *rqtp,
  struct timespec        *rmtp
)
{
  int eno;

  eno = clock_nanosleep( CLOCK_REALTIME, 0, rqtp, rmtp );

  if ( eno != 0 ) {
    rtems_set_errno_and_return_minus_one( eno );
  }

  return eno;
}
