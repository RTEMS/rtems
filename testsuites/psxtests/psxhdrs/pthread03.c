/*
 *  This test file is used to verify that the header files associated with
 *  the callout are correct.
 *
 *  COPYRIGHT (c) 1989-1998. 1995, 1996.
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
#error "rtems is supposed to have pthread_attr_setdetachstate"
#endif

void test( void )
{
  pthread_attr_t  attribute;
  int             state;
  int             result;

  state = PTHREAD_CREATE_DETACHED;
  state = PTHREAD_CREATE_JOINABLE;

  result = pthread_attr_setdetachstate( &attribute, state );
}
