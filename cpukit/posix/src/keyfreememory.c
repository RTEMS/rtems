/*
 *  COPYRIGHT (c) 1989-2010.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/rbtree.h>
#include <rtems/posix/key.h>

void _POSIX_Keys_Free_memory(
  POSIX_Keys_Control *the_key
)
{
  POSIX_Keys_Rbtree_node search_node;
  POSIX_Keys_Rbtree_node *p;
  RBTree_Node *iter, *next;

  search_node.key = the_key->object.id;
  search_node.thread_id = 0;
  iter = _RBTree_Find_unprotected( &_POSIX_Keys_Rbtree, &search_node.rb_node);
  if ( !iter )
    return;
  /**
   * find the smallest thread_id node in the rbtree.
   */
  next = _RBTree_Next_unprotected( iter, RBT_LEFT );
  p = _RBTree_Container_of( next, POSIX_Keys_Rbtree_node, rb_node );
  while ( p->key == the_key->object.id) {
    iter = next;
    next = _RBTree_Next_unprotected( iter, RBT_LEFT );
    p = _RBTree_Container_of( next, POSIX_Keys_Rbtree_node, rb_node );
  }
    
  /**
   * delete all nodes belongs to the_key from the rbtree and chain.
   */
  p = _RBTree_Container_of( iter, POSIX_Keys_Rbtree_node, rb_node );
  while ( p->key == the_key->object.id ) {
    next = _RBTree_Next_unprotected( iter, RBT_RIGHT );
    _RBTree_Extract_unprotected( &_POSIX_Keys_Rbtree, iter );
    _Chain_Extract_unprotected( &p->ch_node );
    _Workspace_Free( p );
    iter = next;
    p = _RBTree_Container_of( iter, POSIX_Keys_Rbtree_node, rb_node );
  }
}
