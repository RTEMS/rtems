/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This source file contains the implementation of
 *   _RBTree_Extract().
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>

RB_GENERATE_REMOVE_COLOR( RBTree_Control, RBTree_Node, Node, static )

RB_GENERATE_REMOVE( RBTree_Control, RBTree_Node, Node, static )

#if defined(RTEMS_DEBUG)
static const RBTree_Node *_RBTree_Find_root( const RBTree_Node *the_node )
{
  while ( true ) {
    const RBTree_Node *potential_root;

    potential_root = the_node;
    the_node = _RBTree_Parent( the_node );

    if ( the_node == NULL ) {
      return potential_root;
    }
  }
}
#endif

void _RBTree_Extract(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node
)
{
  _Assert( _RBTree_Find_root( the_node ) == _RBTree_Root( the_rbtree ) );
  RB_REMOVE( RBTree_Control, the_rbtree, the_node );
  _RBTree_Initialize_node( the_node );
}
