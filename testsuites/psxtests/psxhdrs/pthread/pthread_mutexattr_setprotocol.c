/*
* @file
* @brief pthread_mutexattr_setprotocol() API Conformance Test
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

#ifndef _POSIX_THREAD_PRIO_INHERIT
#error "rtems is supposed to have pthread_mutexattr_setprotocol"
#endif
#ifndef _POSIX_THREAD_PRIO_PROTECT
#error "rtems is supposed to have pthread_mutexattr_setprotocol"
#endif

int test( void );

int test( void )
{
  pthread_mutexattr_t attribute;
  int                 protocol;
  int                 result;

  protocol = PTHREAD_PRIO_NONE;
  protocol = PTHREAD_PRIO_INHERIT;
  protocol = PTHREAD_PRIO_PROTECT;

  result = pthread_mutexattr_setprotocol( &attribute, protocol );

  return result;
}
