/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief _RBTree_Iterate() implementation.
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
 * http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>

void _RBTree_Iterate(
  const RBTree_Control *rbtree,
  RBTree_Visitor        visitor,
  void                 *visitor_arg
)
{
  const RBTree_Node *current = _RBTree_Minimum( rbtree );
  bool               stop = false;

  while ( !stop && current != NULL ) {
    stop = ( *visitor )( current, visitor_arg );

    current = _RBTree_Successor( current );
  }
}
