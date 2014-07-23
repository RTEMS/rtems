/*
 *  Copyright (c) 2010-2012 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>

RTEMS_STATIC_ASSERT(
  sizeof( RBTree_Compare_result ) >= sizeof( intptr_t ),
  RBTree_Compare_result_intptr_t
);

RTEMS_STATIC_ASSERT(
  sizeof( RBTree_Compare_result ) >= sizeof( int32_t ),
  RBTree_Compare_result_int32_t
);

/** @brief Validate and fix-up tree properties for a new insert/colored node
 *
 *  This routine checks and fixes the Red-Black Tree properties based on
 *  @a the_node being just added to the tree.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *        append operation.
 */
static void _RBTree_Validate_insert( RBTree_Node *the_node )
{
  RBTree_Node *parent = _RBTree_Parent( the_node );
  RBTree_Node *grandparent = _RBTree_Parent( parent );

  /* note: the insert root case is handled already */
  /* if the parent is black, nothing needs to be done
   * otherwise may need to loop a few times */
  while ( parent->color == RBT_RED ) {
    /* The root is black, so the grandparent must exist */
    RBTree_Node *uncle = _RBTree_Sibling( parent, grandparent );

    /*
     * If uncle exists and is red, repaint uncle/parent black and grandparent
     * red.
     */
    if ( uncle != NULL && uncle->color == RBT_RED ) {
      parent->color = RBT_BLACK;
      uncle->color = RBT_BLACK;
      grandparent->color = RBT_RED;
      the_node = grandparent;
      parent = _RBTree_Parent( the_node );
      grandparent = _RBTree_Parent( parent );

      if ( grandparent == NULL )
        break;
    } else { /* If uncle does not exist or is black */
      RBTree_Direction dir = _RBTree_Direction( the_node, parent );
      RBTree_Direction parentdir = _RBTree_Direction( parent, grandparent );

      /* ensure node is on the same branch direction as parent */
      if ( dir != parentdir ) {
        RBTree_Node *oldparent = parent;

        parent = the_node;
        the_node = oldparent;
        _RBTree_Rotate( oldparent, parentdir );
      }

      parent->color = RBT_BLACK;
      grandparent->color = RBT_RED;

      /* now rotate grandparent in the other branch direction (toward uncle) */
      _RBTree_Rotate( grandparent, _RBTree_Opposite_direction( parentdir ) );

      grandparent = _RBTree_Parent( parent );
      break;
    }
  }

  if ( grandparent == NULL )
    the_node->color = RBT_BLACK;
}

RBTree_Node *_RBTree_Insert(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node,
  RBTree_Compare  compare,
  bool            is_unique
)
{
  RBTree_Node *iter_node = the_rbtree->root;

  if ( !iter_node ) { /* special case: first node inserted */
    the_node->color = RBT_BLACK;
    the_rbtree->root = the_node;
    the_rbtree->first[ 0 ] = the_rbtree->first[ 1 ] = the_node;
    the_node->parent = (RBTree_Node *) the_rbtree;
    the_node->child[ RBT_LEFT ] = the_node->child[ RBT_RIGHT ] = NULL;
  } else {
    /* typical binary search tree insert, descend tree to leaf and insert */
    while ( iter_node ) {
      RBTree_Compare_result compare_result =
        ( *compare )( the_node, iter_node );

      if ( is_unique && _RBTree_Is_equal( compare_result ) )
        return iter_node;

      RBTree_Direction dir = !_RBTree_Is_lesser( compare_result );

      if ( !iter_node->child[ dir ] ) {
        the_node->child[ RBT_LEFT ] = the_node->child[ RBT_RIGHT ] = NULL;
        the_node->color = RBT_RED;
        iter_node->child[ dir ] = the_node;
        the_node->parent = iter_node;
        /* update min/max */
        compare_result = ( *compare )(
          the_node,
          _RBTree_First( the_rbtree, dir )
        );

        if (
          ( dir == RBT_LEFT && _RBTree_Is_lesser( compare_result ) )
            || ( dir == RBT_RIGHT && !_RBTree_Is_lesser( compare_result ) )
        ) {
          the_rbtree->first[ dir ] = the_node;
        }

        break;
      } else {
        iter_node = iter_node->child[ dir ];
      }
    } /* while(iter_node) */

    /* verify red-black properties */
    _RBTree_Validate_insert( the_node );
  }

  return (RBTree_Node *) 0;
}
