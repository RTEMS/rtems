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
 
#ifndef _POSIX_THREAD_PRIO_INHERIT
#error "rtems is supposed to have pthread_mutex_getprioceiling"
#endif
#ifndef _POSIX_THREAD_PRIO_PROTECT
#error "rtems is supposed to have pthread_mutex_getprioceiling"
#endif

void test( void )
{
  pthread_mutex_t mutex;
  int             prioceiling;
  int             result;

  result = pthread_mutex_getprioceiling( &mutex, &prioceiling );
}
