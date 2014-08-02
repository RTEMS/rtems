/**
 * @file
 *
 * @brief POSIX Function Keys Free Memory
 * @ingroup POSIXAPI
 */

/*
 * Copyright (c) 2012 Zhongwei Yao.
 * COPYRIGHT (c) 1989-2010.
 * On-Line Applications Research Corporation (OAR).
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/posix/keyimpl.h>
#include <rtems/score/chainimpl.h>

void _POSIX_Keys_Free_memory(
  POSIX_Keys_Control *the_key
)
{
  POSIX_Keys_Key_value_pair search_node;
  POSIX_Keys_Key_value_pair *p;
  RBTree_Node *iter, *next;
  Objects_Id key_id;

  key_id = the_key->Object.id;
  iter = _POSIX_Keys_Find( key_id, 0, &search_node );
  if ( !iter )
    return;
  /**
   * find the smallest thread_id node in the rbtree.
   */
  next = _RBTree_Next( iter, RBT_LEFT );
  p = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( next );
  while ( next != NULL && p->key == key_id) {
    iter = next;
    next = _RBTree_Next( iter, RBT_LEFT );
    p = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( next );
  }

  /**
   * delete all nodes belongs to the_key from the rbtree and chain.
   */
  p = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( iter );
  while ( iter != NULL && p->key == key_id ) {
    next = _RBTree_Next( iter, RBT_RIGHT );
    _RBTree_Extract( &_POSIX_Keys_Key_value_lookup_tree, iter );
    _Chain_Extract_unprotected( &p->Key_values_per_thread_node );
    _POSIX_Keys_Key_value_pair_free( p );

    iter = next;
    p = POSIX_KEYS_RBTREE_NODE_TO_KEY_VALUE_PAIR( iter );
  }
}
