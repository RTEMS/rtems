/*
* @file
* @brief pthread_once() API Conformance Test
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
#error "rtems is supposed to have pthread_once"
#endif

int test( void );
void init_routine( void );

void init_routine( void )
{
}

int test( void )
{
  pthread_once_t   once_control = PTHREAD_ONCE_INIT;
  int              result;

  result = pthread_once( &once_control, init_routine );

  return result;
}
