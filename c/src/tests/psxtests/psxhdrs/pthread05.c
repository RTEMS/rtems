/*
 *  This test file is used to verify that the header files associated with
 *  the callout are correct.
 *
 *  COPYRIGHT (c) 1989-1997. 1995, 1996.
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
#include <limits.h>  /* only for PTHREAD_STACK_MIN */

#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_setstacksize"
#endif

#ifndef _POSIX_THREAD_ATTR_STACKSIZE
#error "rtems is supposed to have pthread_setstacksize"
#endif

void test( void )
{
  pthread_attr_t  attribute;
  size_t          size;
  int             result;

  size = PTHREAD_STACK_MIN;

  result = pthread_attr_setstacksize( &attribute, size );
}
