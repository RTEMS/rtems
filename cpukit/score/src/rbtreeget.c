/**
 *  @file
 *
 *  @brief Obtain the min or max node of a rbtree
 *  @ingroup ScoreRBTree
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

RBTree_Node *_RBTree_Get(
  RBTree_Control *the_rbtree,
  RBTree_Direction dir
)
{
  ISR_Level          level;
  RBTree_Node *return_node;

  return_node = NULL;
  _ISR_Disable( level );
      return_node = _RBTree_Get_unprotected( the_rbtree, dir );
  _ISR_Enable( level );
  return return_node;
}

