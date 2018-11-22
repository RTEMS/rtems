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
 *  Copyright (c) 2016 embedded brains GmbH.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/posix/key.h>
#include <rtems/score/chainimpl.h>
#include <rtems/score/freechain.h>
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

RTEMS_INLINE_ROUTINE POSIX_Keys_Control *_POSIX_Keys_Allocate( void )
{
  return (POSIX_Keys_Control *) _Objects_Allocate( &_POSIX_Keys_Information );
}

/**
 * @brief Free a keys control block.
 *
 * This routine frees a keys control block to the
 * inactive chain of free keys control blocks.
 */
RTEMS_INLINE_ROUTINE void _POSIX_Keys_Free(
  POSIX_Keys_Control *the_key
)
{
  _Objects_Free( &_POSIX_Keys_Information, &the_key->Object );
}

RTEMS_INLINE_ROUTINE POSIX_Keys_Control *_POSIX_Keys_Get( pthread_key_t key )
{
  return (POSIX_Keys_Control *)
    _Objects_Get_no_protection( (Objects_Id) key, &_POSIX_Keys_Information );
}

RTEMS_INLINE_ROUTINE void _POSIX_Keys_Key_value_acquire(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_ISR_disable_and_acquire( &the_thread->Keys.Lock, lock_context );
}

RTEMS_INLINE_ROUTINE void _POSIX_Keys_Key_value_release(
  Thread_Control   *the_thread,
  ISR_lock_Context *lock_context
)
{
  _ISR_lock_Release_and_ISR_enable( &the_thread->Keys.Lock, lock_context );
}

POSIX_Keys_Key_value_pair * _POSIX_Keys_Key_value_allocate( void );

RTEMS_INLINE_ROUTINE void _POSIX_Keys_Key_value_free(
  POSIX_Keys_Key_value_pair *key_value_pair
)
{
  _Chain_Extract_unprotected( &key_value_pair->Key_node );
  _Freechain_Put( &_POSIX_Keys_Keypool, key_value_pair );
}

RTEMS_INLINE_ROUTINE bool _POSIX_Keys_Key_value_equal(
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

RTEMS_INLINE_ROUTINE bool _POSIX_Keys_Key_value_less(
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

RTEMS_INLINE_ROUTINE void *_POSIX_Keys_Key_value_map( RBTree_Node *node )
{
  return POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( node );
}

RTEMS_INLINE_ROUTINE POSIX_Keys_Key_value_pair *_POSIX_Keys_Key_value_find(
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

RTEMS_INLINE_ROUTINE void _POSIX_Keys_Key_value_insert(
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
