/**
 * @file
 *
 * @brief Thread-Specific Data Key Create
 * @ingroup POSIXAPI
 */

/*
 * COPYRIGHT (c) 1989-2010.
 * On-Line Applications Research Corporation (OAR).
 * Copyright (c) 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/keyimpl.h>

#include <errno.h>

/**
 *  17.1.1 Thread-Specific Data Key Create, P1003.1c/Draft 10, p. 163
 */
int pthread_key_create(
  pthread_key_t  *key,
  void          (*destructor)( void * )
)
{
  POSIX_Keys_Control  *the_key;

  the_key = _POSIX_Keys_Allocate();

  if ( !the_key ) {
    _Objects_Allocator_unlock();
    return EAGAIN;
  }

  the_key->destructor = destructor;
  _Chain_Initialize_empty( &the_key->Key_value_pairs );
  _Objects_Open_u32( &_POSIX_Keys_Information, &the_key->Object, 0 );
  *key = the_key->Object.id;
  _Objects_Allocator_unlock();
  return 0;
}
