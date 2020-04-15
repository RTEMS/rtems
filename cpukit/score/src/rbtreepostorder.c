/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief _RBTree_Postorder_first() and _RBTree_Postorder_next()
 * implementation.
 */

/*
 * Copyright (c) 2018 embedded brains GmbH.  All rights reserved.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtree.h>

static void *_RBTree_Postorder_dive_left(
  const RBTree_Node *the_node,
  size_t             offset
)
{
  while ( true ) {
    if ( _RBTree_Left( the_node ) != NULL ) {
      the_node = _RBTree_Left( the_node );
    } else if ( _RBTree_Right( the_node ) != NULL ) {
      the_node = _RBTree_Right( the_node );
    } else {
      return (void *) ( (uintptr_t) the_node - offset );
    }
  }
}

void *_RBTree_Postorder_next(
  const RBTree_Node *the_node,
  size_t             offset
)
{
  const RBTree_Node *parent;

  parent = _RBTree_Parent( the_node );
  if ( parent == NULL ) {
    return NULL;
  }

  if (
    the_node == _RBTree_Left( parent )
      && _RBTree_Right( parent ) != NULL
  ) {
    return _RBTree_Postorder_dive_left( _RBTree_Right( parent ), offset );
  }

  return (void *) ( (uintptr_t) parent - offset );
}

void *_RBTree_Postorder_first(
  const RBTree_Control *the_rbtree,
  size_t                offset
)
{
  const RBTree_Node *the_node;

  the_node = _RBTree_Root( the_rbtree );
  if ( the_node == NULL ) {
    return NULL;
  }

  return _RBTree_Postorder_dive_left( the_node, offset );
}
