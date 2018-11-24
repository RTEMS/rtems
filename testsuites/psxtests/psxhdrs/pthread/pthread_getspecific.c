/*
* @file
* @brief pthread_getspecific() API Conformance Test
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
#error "rtems is supposed to have pthread_getspecific"
#endif

int test( void );

int test( void )
{
  pthread_key_t    key;
  void            *value;

  key = 0;

  value = pthread_getspecific( key );

  return (value != 0);
}
