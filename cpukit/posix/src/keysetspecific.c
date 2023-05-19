/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup POSIXAPI
 *
 * @brief Set Specific Key
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2014.
 * On-Line Applications Research Corporation (OAR).
 * Copyright (c) 2016 embedded brains GmbH & Co. KG
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
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
