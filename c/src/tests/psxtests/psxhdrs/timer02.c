/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#include <time.h>
 
#ifndef _POSIX_TIMERS
#error "rtems is supposed to have timer_delete"
#endif

void test( void )
{
  timer_t timerid = 0;
  int     result;
  
  result = timer_delete( timerid );
}
