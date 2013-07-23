/**
 * @file
 *
 * @brief Find the control structure of the tree containing the given node
 * @ingroup ScoreRBTree
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>
#include <rtems/score/isr.h>

RBTree_Node *_RBTree_Find(
  const RBTree_Control *the_rbtree,
  const RBTree_Node *search_node
)
{
  ISR_Level          level;
  RBTree_Node *return_node;

  return_node = NULL;
  _ISR_Disable( level );
      return_node = _RBTree_Find_unprotected( the_rbtree, search_node );
  _ISR_Enable( level );
  return return_node;
}

RBTree_Node *_RBTree_Find_unprotected(
  const RBTree_Control *the_rbtree,
  const RBTree_Node *the_node
)
{
  RBTree_Node* iter_node = the_rbtree->root;
  RBTree_Node* found = NULL;
  int compare_result;
  while (iter_node) {
    compare_result = the_rbtree->compare_function(the_node, iter_node);
    if ( _RBTree_Is_equal( compare_result ) ) {
      found = iter_node;
      if ( the_rbtree->is_unique )
        break;
    }

    RBTree_Direction dir =
      (RBTree_Direction) _RBTree_Is_greater( compare_result );
    iter_node = iter_node->child[dir];
  } /* while(iter_node) */

  return found;
}
