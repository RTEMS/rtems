/**
 * @file
 *
 * @brief Thread-Specific Data Key Create
 * @ingroup POSIXAPI
 */

/*
 * COPYRIGHT (c) 1989-2010.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/posix/keyimpl.h>

/**
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 */
int pthread_key_create(
  pthread_key_t  *key,
  void          (*destructor)( void * )
)
{
  POSIX_Keys_Control  *the_key;

  _Thread_Disable_dispatch();

  the_key = _POSIX_Keys_Allocate();

  if ( !the_key ) {
    _Thread_Enable_dispatch();
    return EAGAIN;
  }

  the_key->destructor = destructor;
  _Objects_Open_u32( &_POSIX_Keys_Information, &the_key->Object, 0 );
  *key = the_key->Object.id;
  _Thread_Enable_dispatch();
  return 0;
}
