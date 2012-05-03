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

#include <pthread.h>

#ifndef _POSIX_THREADS
#error "rtems is supposed to have pthread_key_create"
#endif

int test( void );
void key_destructor( void *argument );

void key_destructor(
  void *argument
)
{
}

int test( void )
{
  pthread_key_t    key;
  int              result;

  result = pthread_key_create( &key, key_destructor );

  return result;
}
