/*
 *  This test file is used to verify that the header files associated with
 *  the callout are correct.
 *
 *  COPYRIGHT (c) 1989-1997. 1995, 1996.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <pthread.h>
 
#ifndef _POSIX_THREAD_CPUTIME
#error "rtems is supposed to have pthread_getcpuclockid"
#endif

void test( void )
{
  pthread_t  thread;
  clockid_t  clock_id;
  int        result;

  thread = 0;

  result = pthread_getcpuclockid( thread, &clock_id );
}
