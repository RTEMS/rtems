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
 
#ifndef _POSIX_THREAD_PRIO_INHERIT
#error "rtems is supposed to have pthread_mutexattr_setprotocol"
#endif
#ifndef _POSIX_THREAD_PRIO_PROTECT
#error "rtems is supposed to have pthread_mutexattr_setprotocol"
#endif

void test( void )
{
  pthread_mutexattr_t attribute;
  int                 protocol;
  int                 result;

  protocol = PTHREAD_PRIO_NONE;
  protocol = PTHREAD_PRIO_INHERIT;
  protocol = PTHREAD_PRIO_PROTECT;

  result = pthread_mutexattr_setprotocol( &attribute, protocol );
}
