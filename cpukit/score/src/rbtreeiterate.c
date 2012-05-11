/**
 * @file
 *
 * @ingroup ScoreRBTree
 *
 * @brief _RBTree_Iterate_unprotected() implementation.
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

void _RBTree_Iterate_unprotected(
  const RBTree_Control *rbtree,
  RBTree_Direction dir,
  RBTree_Visitor visitor,
  void *visitor_arg
)
{
  RBTree_Direction opp_dir = _RBTree_Opposite_direction( dir );
  const RBTree_Node *current = _RBTree_First( rbtree, opp_dir );
  bool stop = false;

  while ( !stop && current != NULL ) {
    stop = (*visitor)( current, dir, visitor_arg );

    current = _RBTree_Next_unprotected( current, dir );
  }
}
