/*
* @file
* @brief pthread_mutex_destroy() API Conformance Test
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
#error "rtems is supposed to have pthread_mutex_destroy"
#endif

int test( void );

int test( void )
{
  pthread_mutex_t  mutex = PTHREAD_MUTEX_INITIALIZER;
  int result;

  result = pthread_mutex_destroy( &mutex );

  return result;
}
