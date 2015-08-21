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
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>

RBTree_Node *_RBTree_Find(
  const RBTree_Control *the_rbtree,
  const RBTree_Node    *the_node,
  RBTree_Compare        compare,
  bool                  is_unique
)
{
  RBTree_Node *iter_node = _RBTree_Root( the_rbtree );
  RBTree_Node *found = NULL;

  while ( iter_node != NULL ) {
    RBTree_Compare_result compare_result = ( *compare )( the_node, iter_node );

    if ( _RBTree_Is_equal( compare_result ) ) {
      found = iter_node;

      if ( is_unique )
        break;
    }

    if ( _RBTree_Is_greater( compare_result ) ) {
      iter_node = _RBTree_Right( iter_node );
    } else {
      iter_node = _RBTree_Left( iter_node );
    }
  }

  return found;
}
