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

#include <pthread.h>
 
#ifndef _POSIX_THREAD_PRIORITY_SCHEDULING
#error "RTEMS is supposed to have pthread_attr_setschedpolicy"
#endif

void test( void )
{
  pthread_attr_t  attr;
  int             policy;
  int             result;

  policy = SCHED_FIFO;
  policy = SCHED_RR;
  policy = SCHED_OTHER;
#ifdef _POSIX_SPORADIC_SERVER
  policy = SCHED_SPORADIC;
#endif
  
  result = pthread_attr_setschedpolicy( &attr, policy );
}
