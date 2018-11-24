/*
* @file
* @brief pthread_getschedparam() API Conformance Test
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

#ifndef _POSIX_THREAD_PRIORITY_SCHEDULING
#error "RTEMS is supposed to have pthread_getschedparam"
#endif

int test( void );

int test( void )
{
  pthread_t           thread = 0;
  int                 policy;
  struct sched_param  param;
  int                 result;

  result = pthread_getschedparam( thread, &policy, &param );

  return result;
}
