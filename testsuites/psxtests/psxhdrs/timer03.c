/*
 *  This test file is used to verify that the header files associated with
 *  the callout are correct.
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994, 1995, 1996.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <time.h>
 
#ifndef _POSIX_TIMERS
#error "rtems is supposed to have timer_settime"
#endif

void test( void )
{
  timer_t           timerid = 0;
  int               flags;
  struct itimerspec value;
  struct itimerspec ovalue;
  int               result;

  value.it_interval.tv_sec = 0;
  value.it_interval.tv_nsec = 0;
  value.it_value.tv_sec = 0;
  value.it_value.tv_nsec = 0;

  flags = TIMER_ABSTIME;
  flags = CLOCK_REALTIME;

  result = timer_settime( timerid, flags, &value, &ovalue );
}
