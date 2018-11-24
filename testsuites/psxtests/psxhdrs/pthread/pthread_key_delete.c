/*
* @file
* @brief pthread_key_delete() API Conformance Test
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
#error "rtems is supposed to have pthread_key_delete"
#endif

int test( void );

int test( void )
{
  pthread_key_t    key = 0;
  int              result;

  result = pthread_key_delete( key );

  return result;
}
