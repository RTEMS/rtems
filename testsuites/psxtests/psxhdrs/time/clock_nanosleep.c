/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 */

/*
 *  Copyright (c) 2016 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <time.h>

#ifndef _POSIX_TIMERS
#error "rtems is supposed to have clock_nanosleep"
#endif

int test( void );

int test( void )
{
  struct timespec rqtp;
  struct timespec rmtp;
  int             result;

  rqtp.tv_sec = 0;
  rqtp.tv_nsec = 0;
  result = clock_nanosleep( CLOCK_REALTIME, 0, &rqtp, &rmtp );

  return result;
}
