/**
 * @file
 *
 * @ingroup ScoreRBTree
 *
 * @brief _RBTree_Next_unprotected() and _RBTree_Next() implementation.
 */

/*
 * Copyright (c) 2012 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/rbtree.h>
#include <rtems/score/isr.h>

RBTree_Node *_RBTree_Next_unprotected(
  const RBTree_Node *node,
  RBTree_Direction dir
)
{
  RBTree_Direction opp_dir = _RBTree_Opposite_direction( dir );
  RBTree_Node *current = node->child [dir];
  RBTree_Node *next = NULL;

  if ( current != NULL ) {
    next = current;
    while ( (current = current->child [opp_dir]) != NULL ) {
      next = current;
    }
  } else {
    RBTree_Node *parent = node->parent;

    if ( parent->parent && node == parent->child [opp_dir] ) {
      next = parent;
    } else {
      while ( parent->parent && node == parent->child [dir] ) {
        node = parent;
        parent = parent->parent;
      }

      if ( parent->parent ) {
        next = parent;
      }
    }
  }

  return next;
}

RBTree_Node *_RBTree_Next(
  const RBTree_Node *node,
  RBTree_Direction dir
)
{
  RBTree_Node *next;
  ISR_Level level;

  _ISR_Disable( level );
  next = _RBTree_Next_unprotected( node, dir );
  _ISR_Enable( level );

  return next;
}
