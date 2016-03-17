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
 * @brief The information control block used to manage this class of objects.
 */
extern Objects_Information _POSIX_Keys_Information;

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
  Objects_Locations location;

  return (POSIX_Keys_Control *) _Objects_Get_no_protection(
    &_POSIX_Keys_Information,
    (Objects_Id) key,
    &location
  );
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

RTEMS_INLINE_ROUTINE RBTree_Node *_POSIX_Keys_Key_value_find(
  pthread_key_t     key,
  Thread_Control   *the_thread
)
{
  RBTree_Node **link;
  RBTree_Node  *parent;

  link = _RBTree_Root_reference( &the_thread->Keys.Key_value_pairs );
  parent = NULL;

  while ( *link != NULL ) {
    POSIX_Keys_Key_value_pair *parent_key_value_pair;
    pthread_key_t              parent_key;

    parent = *link;
    parent_key_value_pair = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( parent );
    parent_key = parent_key_value_pair->key;

    if ( key == parent_key ) {
      return parent;
    } else if ( key < parent_key ) {
      link = _RBTree_Left_reference( parent );
    } else {
      link = _RBTree_Right_reference( parent );
    }
  }

  return NULL;
}

RTEMS_INLINE_ROUTINE void _POSIX_Keys_Key_value_insert(
  pthread_key_t              key,
  POSIX_Keys_Key_value_pair *key_value_pair,
  Thread_Control            *the_thread
)
{
  RBTree_Node **link;
  RBTree_Node  *parent;

  link = _RBTree_Root_reference( &the_thread->Keys.Key_value_pairs );
  parent = NULL;

  while ( *link != NULL ) {
    POSIX_Keys_Key_value_pair *parent_key_value_pair;

    parent = *link;
    parent_key_value_pair = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( parent );

    if ( key < parent_key_value_pair->key ) {
      link = _RBTree_Left_reference( parent );
    } else {
      link = _RBTree_Right_reference( parent );
    }
  }

  _RBTree_Add_child( &key_value_pair->Lookup_node, parent, link );
  _RBTree_Insert_color(
    &the_thread->Keys.Key_value_pairs,
    &key_value_pair->Lookup_node
  );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/*  end of include file */
