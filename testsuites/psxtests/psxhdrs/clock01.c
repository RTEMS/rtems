/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <time.h>
 
#ifndef _POSIX_TIMERS
#error "rtems is supposed to have clock_gettime"
#endif

void test( void )
{
  clockid_t        clock_id = 0;
  struct timespec  tp;
  int              result;

  result = clock_gettime( clock_id, &tp );
}
