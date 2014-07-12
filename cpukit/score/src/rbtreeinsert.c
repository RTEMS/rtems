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
#include <rtems/score/isr.h>

/** @brief Validate and fix-up tree properties for a new insert/colored node
 *
 *  This routine checks and fixes the Red-Black Tree properties based on
 *  @a the_node being just added to the tree.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *        append operation.
 */
static void _RBTree_Validate_insert(
    RBTree_Node    *the_node
    )
{
  RBTree_Node *u,*g;

  /* note: the insert root case is handled already */
  /* if the parent is black, nothing needs to be done
   * otherwise may need to loop a few times */
  while (_RBTree_Is_red(_RBTree_Parent(the_node))) {
    u = _RBTree_Parent_sibling(the_node);
    g = the_node->parent->parent;

    /* if uncle is red, repaint uncle/parent black and grandparent red */
    if(_RBTree_Is_red(u)) {
      the_node->parent->color = RBT_BLACK;
      u->color = RBT_BLACK;
      g->color = RBT_RED;
      the_node = g;
    } else { /* if uncle is black */
      RBTree_Direction dir = the_node != the_node->parent->child[0];
      RBTree_Direction pdir = the_node->parent != g->child[0];

      /* ensure node is on the same branch direction as parent */
      if (dir != pdir) {
        _RBTree_Rotate(the_node->parent, pdir);
        the_node = the_node->child[pdir];
      }
      the_node->parent->color = RBT_BLACK;
      g->color = RBT_RED;

      /* now rotate grandparent in the other branch direction (toward uncle) */
      _RBTree_Rotate(g, (1-pdir));
    }
  }
  if(!the_node->parent->parent) the_node->color = RBT_BLACK;
}

RBTree_Node *_RBTree_Insert(
  RBTree_Control *the_rbtree,
  RBTree_Node    *the_node,
  RBTree_Compare  compare,
  bool            is_unique
)
{
  if(!the_node) return (RBTree_Node*)-1;

  RBTree_Node *iter_node = the_rbtree->root;
  int compare_result;

  if (!iter_node) { /* special case: first node inserted */
    the_node->color = RBT_BLACK;
    the_rbtree->root = the_node;
    the_rbtree->first[0] = the_rbtree->first[1] = the_node;
    the_node->parent = (RBTree_Node *) the_rbtree;
    the_node->child[RBT_LEFT] = the_node->child[RBT_RIGHT] = NULL;
  } else {
    /* typical binary search tree insert, descend tree to leaf and insert */
    while (iter_node) {
      compare_result = ( *compare )( the_node, iter_node );
      if ( is_unique && _RBTree_Is_equal( compare_result ) )
        return iter_node;
      RBTree_Direction dir = !_RBTree_Is_lesser( compare_result );
      if (!iter_node->child[dir]) {
        the_node->child[RBT_LEFT] = the_node->child[RBT_RIGHT] = NULL;
        the_node->color = RBT_RED;
        iter_node->child[dir] = the_node;
        the_node->parent = iter_node;
        /* update min/max */
        compare_result = ( *compare )(
          the_node,
          _RBTree_First( the_rbtree, dir )
        );
        if ( (!dir && _RBTree_Is_lesser(compare_result)) ||
              (dir && _RBTree_Is_greater(compare_result)) ) {
          the_rbtree->first[dir] = the_node;
        }
        break;
      } else {
        iter_node = iter_node->child[dir];
      }

    } /* while(iter_node) */

    /* verify red-black properties */
    _RBTree_Validate_insert(the_node);
  }
  return (RBTree_Node*)0;
}
