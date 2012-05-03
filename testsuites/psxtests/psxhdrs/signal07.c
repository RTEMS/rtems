/*
 *  This test file is used to verify that the header files associated with
 *  invoking this function are correct.
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <signal.h>

#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_kill"
#endif

int test( void );

int test( void )
{
  pthread_t  thread;
  int        signal_number;
  int        result;

  thread = 0;
  signal_number = SIGALRM;

  result = pthread_kill( thread, signal_number );

  return result;
}
