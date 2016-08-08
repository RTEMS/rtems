/**
 * @file
 *
 * @brief Set Specific Key
 * @ingroup POSIXAPI
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2014.
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

static int _POSIX_Keys_Set_value(
  POSIX_Keys_Key_value_pair *key_value_pair,
  const void                *value
)
{
  key_value_pair->value = RTEMS_DECONST( void *, value );

  return 0;
}

static int _POSIX_Keys_Create_value(
  pthread_key_t       key,
  const void         *value,
  Thread_Control     *executing
)
{
  POSIX_Keys_Control *the_key;
  int                 eno;

  _Objects_Allocator_lock();

  the_key = _POSIX_Keys_Get( key );
  if ( the_key != NULL ) {
    POSIX_Keys_Key_value_pair *key_value_pair;

    key_value_pair = _POSIX_Keys_Key_value_allocate();
    if ( key_value_pair != NULL ) {
      ISR_lock_Context lock_context;

      key_value_pair->key = key;
      key_value_pair->thread = executing;
      key_value_pair->value = RTEMS_DECONST( void *, value );

      _RBTree_Initialize_node( &key_value_pair->Lookup_node );

      _Chain_Initialize_node( &key_value_pair->Key_node );
      _Chain_Append_unprotected(
        &the_key->Key_value_pairs,
        &key_value_pair->Key_node
      );

      _POSIX_Keys_Key_value_acquire( executing, &lock_context );
      _POSIX_Keys_Key_value_insert( key, key_value_pair, executing );
      _POSIX_Keys_Key_value_release( executing, &lock_context );
      eno = 0;
    } else {
      eno = ENOMEM;
    }

  } else {
    eno = EINVAL;
  }

  _Objects_Allocator_unlock();

  return eno;
}

static int _POSIX_Keys_Delete_value(
  pthread_key_t   key,
  Thread_Control *executing
)
{
  POSIX_Keys_Control *the_key;
  int                 eno;

  _Objects_Allocator_lock();

  the_key = _POSIX_Keys_Get( key );
  if ( the_key != NULL ) {
    POSIX_Keys_Key_value_pair *key_value_pair;
    ISR_lock_Context           lock_context;

    _POSIX_Keys_Key_value_acquire( executing, &lock_context );

    key_value_pair = _POSIX_Keys_Key_value_find( key, executing );
    if ( key_value_pair != NULL ) {
      _RBTree_Extract(
        &executing->Keys.Key_value_pairs,
        &key_value_pair->Lookup_node
      );

      _POSIX_Keys_Key_value_release( executing, &lock_context );

      _POSIX_Keys_Key_value_free( key_value_pair );
    } else {
      _POSIX_Keys_Key_value_release( executing, &lock_context );
    }

    eno = 0;
  } else {
    eno = EINVAL;
  }

  _Objects_Allocator_unlock();

  return eno;
}

/*
 *  17.1.2 Thread-Specific Data Management, P1003.1c/Draft 10, p. 165
 */

int pthread_setspecific(
  pthread_key_t  key,
  const void    *value
)
{
  Thread_Control   *executing;
  int               eno;

  executing = _Thread_Get_executing();

  if ( value != NULL ) {
    ISR_lock_Context           lock_context;
    POSIX_Keys_Key_value_pair *key_value_pair;

    _POSIX_Keys_Key_value_acquire( executing, &lock_context );

    key_value_pair = _POSIX_Keys_Key_value_find( key, executing );
    if ( key_value_pair != NULL ) {
      eno = _POSIX_Keys_Set_value( key_value_pair, value );
      _POSIX_Keys_Key_value_release( executing, &lock_context );
    } else {
      _POSIX_Keys_Key_value_release( executing, &lock_context );
      eno = _POSIX_Keys_Create_value( key, value, executing );
    }
  } else {
    eno = _POSIX_Keys_Delete_value( key, executing );
  }

  return eno;
}
