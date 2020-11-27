/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This source file contains the implementation of
 *   _RBTree_Insert_color().
 */

/*
 *  Copyright (c) 2010-2012 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>

RB_GENERATE_INSERT_COLOR( RBTree_Control, RBTree_Node, Node, static inline )

void _RBTree_Insert_color(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node
)
{
  RBTree_Control_RB_INSERT_COLOR( the_rbtree, the_node );
}
