/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Deletes Thread-specific Data Key Previously Returned by keycreate.c
 */

/*
 * COPYRIGHT (c) 1989-2007.
 * On-Line Applications Research Corporation (OAR).
 * Copyright (c) 2016 embedded brains GmbH.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/keyimpl.h>

#include <errno.h>

static void _POSIX_Keys_Destroy( POSIX_Keys_Control *the_key )
{
  _Objects_Close( &_POSIX_Keys_Information, &the_key->Object );

  while ( !_Chain_Is_empty( &the_key->Key_value_pairs ) ) {
    POSIX_Keys_Key_value_pair *key_value_pair;
    ISR_lock_Context           lock_context;
    Thread_Control            *the_thread;

    key_value_pair = (POSIX_Keys_Key_value_pair *)
      _Chain_First( &the_key->Key_value_pairs );

    the_thread = key_value_pair->thread;
    _POSIX_Keys_Key_value_acquire( the_thread, &lock_context );
    _RBTree_Extract(
      &the_thread->Keys.Key_value_pairs,
      &key_value_pair->Lookup_node
    );
    _POSIX_Keys_Key_value_release( the_thread, &lock_context );

    _POSIX_Keys_Key_value_free( key_value_pair );
  }

  _Objects_Free( &_POSIX_Keys_Information, &the_key->Object );
}

/*
 *  17.1.3 Thread-Specific Data Key Deletion, P1003.1c/Draft 10, p. 167
 */
int pthread_key_delete(
  pthread_key_t  key
)
{
  POSIX_Keys_Control *the_key;
  int                 eno;

  _Objects_Allocator_lock();

  the_key = _POSIX_Keys_Get( key );
  if ( the_key != NULL ) {
    _POSIX_Keys_Destroy( the_key );
    eno = 0;
  } else {
    eno = EINVAL;
  }

  _Objects_Allocator_unlock();

  return eno;
}
