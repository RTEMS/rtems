/*
* @file
* @brief pthread_cond_signal() API Conformance Test
*/

/*
* COPYRIGHT (c) 1989-2009.
* On-Line Applications Research Corporation (OAR).
*
* The license and distribution terms for this file may be
* found in the file LICENSE in this distribution or at
* http://www.rtems.org/license/LICENSE.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>

#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_cond_signal"
#endif

int test( void );

int test( void )
{
  pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
  int result;

  result = pthread_cond_signal( &cond );

  return result;
}
