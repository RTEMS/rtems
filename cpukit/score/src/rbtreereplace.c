/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief _RBTree_Replace_node() implementation.
 */

/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
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

#include <rtems/score/rbtree.h>

void _RBTree_Replace_node(
  RBTree_Control *the_rbtree,
  RBTree_Node    *victim,
  RBTree_Node    *replacement
)
{
  RBTree_Node  *parent = _RBTree_Parent( victim );
  RBTree_Node **link;
  RBTree_Node  *child;

  if (parent != NULL) {
    if ( victim == _RBTree_Left( parent ) ) {
      link = _RBTree_Left_reference( parent );
    } else {
      link = _RBTree_Right_reference( parent );
    }
  } else {
    link = _RBTree_Root_reference( the_rbtree );
  }
  *link = replacement;

  child = _RBTree_Left( victim );
  if ( child != NULL ) {
    RB_PARENT( child, Node ) = replacement;
  }

  child = _RBTree_Right( victim );
  if ( child != NULL ) {
    RB_PARENT( child, Node ) = replacement;
  }

  *replacement = *victim;
}
