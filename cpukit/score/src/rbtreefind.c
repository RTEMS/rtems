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

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/rbtree.h>
#include <rtems/score/isr.h>

RBTree_Node *_RBTree_Find(
  RBTree_Control *the_rbtree,
  RBTree_Node *search_node
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
