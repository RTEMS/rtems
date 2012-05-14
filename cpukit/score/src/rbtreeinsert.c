/*
 *  Copyright (c) 2010-2012 Gedare Bloom.
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

/** @brief Validate and fix-up tree properties for a new insert/colored node
 *
 *  This routine checks and fixes the Red-Black Tree properties based on
 *  @a the_node being just added to the tree.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *        append operation.
 */
static void _RBTree_Validate_insert_unprotected(
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



/** @brief Insert a Node (unprotected)
 *
 *  This routine inserts @a the_node on the Red-Black Tree @a the_rbtree.
 *
 *  @retval 0 Successfully inserted.
 *  @retval -1 NULL @a the_node.
 *  @retval RBTree_Node* if one with equal key to the key of @a the_node exists
 *          in an unique @a the_rbtree.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity
 *        of the extract operation.
 */
RBTree_Node *_RBTree_Insert_unprotected(
    RBTree_Control *the_rbtree,
    RBTree_Node *the_node
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
      compare_result = the_rbtree->compare_function(the_node, iter_node);
      if ( the_rbtree->is_unique && _RBTree_Is_equal( compare_result ) )
        return iter_node;
      RBTree_Direction dir = !_RBTree_Is_lesser( compare_result );
      if (!iter_node->child[dir]) {
        the_node->child[RBT_LEFT] = the_node->child[RBT_RIGHT] = NULL;
        the_node->color = RBT_RED;
        iter_node->child[dir] = the_node;
        the_node->parent = iter_node;
        /* update min/max */
        compare_result = the_rbtree->compare_function(
            the_node,
            _RBTree_First(the_rbtree, dir)
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
    _RBTree_Validate_insert_unprotected(the_node);
  }
  return (RBTree_Node*)0;
}


/*
 *  _RBTree_Insert
 *
 *  This kernel routine inserts a given node after a specified node
 *  a requested rbtree.
 *
 *  Input parameters:
 *    tree - pointer to RBTree Control for tree to insert to
 *    node       - pointer to node to be inserted
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

RBTree_Node *_RBTree_Insert(
  RBTree_Control *tree,
  RBTree_Node *node
)
{
  ISR_Level level;
  RBTree_Node *return_node;

  _ISR_Disable( level );
  return_node = _RBTree_Insert_unprotected( tree, node );
  _ISR_Enable( level );
  return return_node;
}
