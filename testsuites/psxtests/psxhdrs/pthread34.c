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
