/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief Private Inlined Routines for POSIX Key's
 *
 * This include file contains the static inline implementation of the private
 * inlined routines for POSIX key's.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright (c) 2016 embedded brains GmbH & Co. KG
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

#include <rtems/posix/key.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/freechainimpl.h>
#include <rtems/score/objectimpl.h>
#include <rtems/score/percpu.h>

#ifndef _RTEMS_POSIX_KEYIMPL_H
#define _RTEMS_POSIX_KEYIMPL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup POSIX_KEY
 *
 * @{
 */

/**
 * @brief This freechain is used as a memory pool for POSIX_Keys_Key_value_pair.
 */
extern Freechain_Control _POSIX_Keys_Keypool;

#define POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( node ) \
  RTEMS_CONTAINER_OF( node, POSIX_Keys_Key_value_pair, Lookup_node )

/**
 * @brief Allocate a keys control block.
 *
 * This function allocates a keys control block from
 * the inactive chain of free keys control blocks.
 */

static inline POSIX_Keys_Control *_POSIX_Keys_Allocate( void )
{
  return (POSIX_Keys_Control *) _Objects_Allocate( &_POSIX_Keys_Information );
}

/**
 * @brief Free a keys control block.
 *
 * This routine frees a keys control block to the
 * inactive chain of free keys control blocks.
 */
static inline void _POSIX_Keys_Free(
  POSIX_Keys_Control *the_key
)
{
  _Objects_Free( &_POSIX_Keys_Information, &the_key->Object );
}

static inline POSIX_Keys_Control *_POSIX_Keys_Get( pthread_key_t key )
{
  return (POSIX_Keys_Control *)
    _Objects_Get_no_protection( (Objects_Id) key, &_POSIX_Keys_Information );
}

static inline void _POSIX_Keys_Key_value_acquire(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable_and_acquire( &the_thread->Keys.Lock, lock_context );
}

static inline void _POSIX_Keys_Key_value_release(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &the_thread->Keys.Lock, lock_context );
}

POSIX_Keys_Key_value_pair * _POSIX_Keys_Key_value_allocate( void );

static inline void _POSIX_Keys_Key_value_free(
  POSIX_Keys_Key_value_pair *key_value_pair
)
{
  _Chain_Extract_unprotected( &key_value_pair->Key_node );
  _Freechain_Put( &_POSIX_Keys_Keypool, key_value_pair );
}

static inline bool _POSIX_Keys_Key_value_equal(
  const void        *left,
  const RBTree_Node *right
)
{
  const pthread_key_t             *the_left;
  const POSIX_Keys_Key_value_pair *the_right;

  the_left = left;
  the_right = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( right );

  return *the_left == the_right->key;
}

static inline bool _POSIX_Keys_Key_value_less(
  const void        *left,
  const RBTree_Node *right
)
{
  const pthread_key_t             *the_left;
  const POSIX_Keys_Key_value_pair *the_right;

  the_left = left;
  the_right = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( right );

  return *the_left < the_right->key;
}

static inline void *_POSIX_Keys_Key_value_map( RBTree_Node *node )
{
  return POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( node );
}

static inline POSIX_Keys_Key_value_pair *_POSIX_Keys_Key_value_find(
  pthread_key_t         key,
  const Thread_Control *the_thread
)
{
  return _RBTree_Find_inline(
    &the_thread->Keys.Key_value_pairs,
    &key,
    _POSIX_Keys_Key_value_equal,
    _POSIX_Keys_Key_value_less,
    _POSIX_Keys_Key_value_map
  );
}

static inline void _POSIX_Keys_Key_value_insert(
  pthread_key_t              key,
  POSIX_Keys_Key_value_pair *key_value_pair,
  Thread_Control            *the_thread
)
{
  _RBTree_Insert_inline(
    &the_thread->Keys.Key_value_pairs,
    &key_value_pair->Lookup_node,
    &key,
    _POSIX_Keys_Key_value_less
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
