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

#include <pthread.h>
 
#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_setcancelstate"
#endif

void init_routine( void )
{
}

void test( void )
{
  int         state;
  int         oldstate;
  int         status;

  state = PTHREAD_CANCEL_ENABLE;
  state = PTHREAD_CANCEL_DISABLE;

  status = pthread_setcancelstate( state, &oldstate );
}
