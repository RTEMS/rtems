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
 
#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_cond_timedwait"
#endif

void test( void )
{
  pthread_cond_t  cond  = PTHREAD_COND_INITIALIZER;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  struct timespec abstime;
  int result;

  result = pthread_cond_timedwait( &cond, &mutex, &abstime );
}
