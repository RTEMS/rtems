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

#include <signal.h>
 
#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_sigmask"
#endif

void test( void )
{
  int       how;
  sigset_t  set;
  sigset_t  oset;
  int       result;

  how = SIG_BLOCK;
  how = SIG_UNBLOCK;
  how = SIG_SETMASK;

  result = pthread_sigmask( how, &set, &oset );
}
