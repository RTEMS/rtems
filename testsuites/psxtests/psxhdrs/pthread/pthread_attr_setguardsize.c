/*
* @file
* @brief pthread_attr_setguardsize() API Conformance Test
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
#include <limits.h>  /* only for PTHREAD_STACK_MIN */

#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_setstacksize"
#endif

int test( void );

int test( void )
{
  pthread_attr_t  attribute;
  size_t          size;
  int             result;

  size = PTHREAD_STACK_MIN;

  result = pthread_attr_setguardsize( &attribute, size );

  return result;
}
