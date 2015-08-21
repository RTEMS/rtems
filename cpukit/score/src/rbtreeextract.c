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

RB_GENERATE_REMOVE_COLOR( RBTree_Control, RBTree_Node, Node, static )

RB_GENERATE_REMOVE( RBTree_Control, RBTree_Node, Node, static )

void _RBTree_Extract(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node
)
{
  RB_REMOVE( RBTree_Control, the_rbtree, the_node );
}
