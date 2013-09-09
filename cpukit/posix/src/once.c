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
#include <rtems/posix/posixapi.h>
#include <rtems/posix/onceimpl.h>

void _POSIX_Once_Manager_initialization(void)
{
  pthread_mutexattr_t mattr;
  int eno;

  _POSIX_Once_Lock = PTHREAD_MUTEX_INITIALIZER;

  eno = pthread_mutexattr_init( &mattr );
  _Assert( eno == 0 );
  eno = pthread_mutexattr_settype( &mattr, PTHREAD_MUTEX_RECURSIVE );
  _Assert( eno == 0 );

  eno = pthread_mutex_init( &_POSIX_Once_Lock, &mattr );
  if ( eno != 0 )
    _POSIX_Fatal_error( POSIX_FD_PTHREAD_ONCE, eno );

  eno = pthread_mutexattr_destroy( &mattr );
  _Assert( eno == 0 );
}
