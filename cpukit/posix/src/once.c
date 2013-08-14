/**
 * @file
 *
 * @brief POSIX Once Manager Initialization
 * @ingroup POSIX_ONCE POSIX Once Support
 */

/*
 *  COPYRIGHT (c) 2013
 *  Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/posix/onceimpl.h>

pthread_mutex_t _POSIX_Once_Lock;

void _POSIX_Once_Manager_initialization(void)
{
  pthread_mutexattr_t mattr;
  int r;

  _POSIX_Once_Lock = PTHREAD_MUTEX_INITIALIZER;

  r = pthread_mutexattr_init( &mattr );
  if ( r != 0 )
    rtems_fatal( RTEMS_FATAL_SOURCE_ASSERT, 0x80aa0000 | r );

  r = pthread_mutexattr_settype( &mattr, PTHREAD_MUTEX_RECURSIVE );
   if ( r != 0 )
    rtems_fatal( RTEMS_FATAL_SOURCE_ASSERT, 0x80aa1000 | r );

  r = pthread_mutex_init( &_POSIX_Once_Lock, &mattr );
  if ( r != 0 )
    rtems_fatal( RTEMS_FATAL_SOURCE_ASSERT, 0x80aa2000 | r );

  pthread_mutexattr_destroy( &mattr );
}
