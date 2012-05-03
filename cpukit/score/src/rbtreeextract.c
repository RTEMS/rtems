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

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/rbtree.h>
#include <rtems/score/isr.h>

/** @brief  Validate and fix-up tree properties after deleting a node
 *
 *  This routine is called on a black node, @a the_node, after its deletion.
 *  This function maintains the properties of the red-black tree.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity
 *        of the extract operation.
 */
static void _RBTree_Extract_validate_unprotected(
    RBTree_Node *the_node
    )
{
  RBTree_Node *parent, *sibling;
  RBTree_Direction dir;

  parent = the_node->parent;
  if(!parent->parent) return;

  sibling = _RBTree_Sibling(the_node);

  /* continue to correct tree as long as the_node is black and not the root */
  while (!_RBTree_Is_red(the_node) && parent->parent) {

    /* if sibling is red, switch parent (black) and sibling colors,
     * then rotate parent left, making the sibling be the_node's grandparent.
     * Now the_node has a black sibling and red parent. After rotation,
     * update sibling pointer.
     */
    if (_RBTree_Is_red(sibling)) {
      parent->color = RBT_RED;
      sibling->color = RBT_BLACK;
      dir = the_node != parent->child[0];
      _RBTree_Rotate(parent, dir);
      sibling = parent->child[_RBTree_Opposite_direction(dir)];
    }

    /* sibling is black, see if both of its children are also black. */
    if (!_RBTree_Is_red(sibling->child[RBT_RIGHT]) &&
        !_RBTree_Is_red(sibling->child[RBT_LEFT])) {
        sibling->color = RBT_RED;
        if (_RBTree_Is_red(parent)) {
          parent->color = RBT_BLACK;
          break;
        }
        the_node = parent; /* done if parent is red */
        parent = the_node->parent;
        sibling = _RBTree_Sibling(the_node);
    } else {
      /* at least one of sibling's children is red. we now proceed in two
       * cases, either the_node is to the left or the right of the parent.
       * In both cases, first check if one of sibling's children is black,
       * and if so rotate in the proper direction and update sibling pointer.
       * Then switch the sibling and parent colors, and rotate through parent.
       */
      dir = the_node != parent->child[0];
      if (!_RBTree_Is_red(sibling->child[_RBTree_Opposite_direction(dir)])) {
        sibling->color = RBT_RED;
        sibling->child[dir]->color = RBT_BLACK;
        _RBTree_Rotate(sibling, _RBTree_Opposite_direction(dir));
        sibling = parent->child[_RBTree_Opposite_direction(dir)];
      }
      sibling->color = parent->color;
      parent->color = RBT_BLACK;
      sibling->child[_RBTree_Opposite_direction(dir)]->color = RBT_BLACK;
      _RBTree_Rotate(parent, dir);
      break; /* done */
    }
  } /* while */
  if(!the_node->parent->parent) the_node->color = RBT_BLACK;
}

/** @brief Extract a Node (unprotected)
 *
 *  This routine extracts (removes) @a the_node from @a the_rbtree.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity
 *        of the extract operation.
 */
void _RBTree_Extract_unprotected(
    RBTree_Control *the_rbtree,
    RBTree_Node *the_node
    )
{
  RBTree_Node *leaf, *target;
  RBTree_Color victim_color;
  RBTree_Direction dir;

  if (!the_node) return;

  /* check if min needs to be updated */
  if (the_node == the_rbtree->first[RBT_LEFT]) {
    RBTree_Node *next;
    next = _RBTree_Successor_unprotected(the_node);
    the_rbtree->first[RBT_LEFT] = next;
  }

  /* Check if max needs to be updated. min=max for 1 element trees so
   * do not use else if here. */
  if (the_node == the_rbtree->first[RBT_RIGHT]) {
    RBTree_Node *previous;
    previous = _RBTree_Predecessor_unprotected(the_node);
    the_rbtree->first[RBT_RIGHT] = previous;
  }

  /* if the_node has at most one non-null child then it is safe to proceed
   * check if both children are non-null, if so then we must find a target node
   * either max in node->child[RBT_LEFT] or min in node->child[RBT_RIGHT],
   * and replace the_node with the target node. This maintains the binary
   * search tree property, but may violate the red-black properties.
   */

  if (the_node->child[RBT_LEFT] && the_node->child[RBT_RIGHT]) {
    target = the_node->child[RBT_LEFT]; /* find max in node->child[RBT_LEFT] */
    while (target->child[RBT_RIGHT]) target = target->child[RBT_RIGHT];

    /* if the target node has a child, need to move it up the tree into
     * target's position (target is the right child of target->parent)
     * when target vacates it. if there is no child, then target->parent
     * should become NULL. This may cause the coloring to be violated.
     * For now we store the color of the node being deleted in victim_color.
     */
    leaf = target->child[RBT_LEFT];
    if(leaf) {
      leaf->parent = target->parent;
    } else {
      /* fix the tree here if the child is a null leaf. */
      _RBTree_Extract_validate_unprotected(target);
    }
    victim_color = target->color;
    dir = target != target->parent->child[0];
    target->parent->child[dir] = leaf;

    /* now replace the_node with target */
    dir = the_node != the_node->parent->child[0];
    the_node->parent->child[dir] = target;

    /* set target's new children to the original node's children */
    target->child[RBT_RIGHT] = the_node->child[RBT_RIGHT];
    if (the_node->child[RBT_RIGHT])
      the_node->child[RBT_RIGHT]->parent = target;
    target->child[RBT_LEFT] = the_node->child[RBT_LEFT];
    if (the_node->child[RBT_LEFT])
      the_node->child[RBT_LEFT]->parent = target;

    /* finally, update the parent node and recolor. target has completely
     * replaced the_node, and target's child has moved up the tree if needed.
     * the_node is no longer part of the tree, although it has valid pointers
     * still.
     */
    target->parent = the_node->parent;
    target->color = the_node->color;
  } else {
    /* the_node has at most 1 non-null child. Move the child in to
     * the_node's location in the tree. This may cause the coloring to be
     * violated. We will fix it later.
     * For now we store the color of the node being deleted in victim_color.
     */
    leaf = the_node->child[RBT_LEFT] ?
              the_node->child[RBT_LEFT] : the_node->child[RBT_RIGHT];
    if( leaf ) {
      leaf->parent = the_node->parent;
    } else {
      /* fix the tree here if the child is a null leaf. */
      _RBTree_Extract_validate_unprotected(the_node);
    }
    victim_color = the_node->color;

    /* remove the_node from the tree */
    dir = the_node != the_node->parent->child[0];
    the_node->parent->child[dir] = leaf;
  }

  /* fix coloring. leaf has moved up the tree. The color of the deleted
   * node is in victim_color. There are two cases:
   *   1. Deleted a red node, its child must be black. Nothing must be done.
   *   2. Deleted a black node, its child must be red. Paint child black.
   */
  if (victim_color == RBT_BLACK) { /* eliminate case 1 */
    if (leaf) {
      leaf->color = RBT_BLACK; /* case 2 */
    }
  }

  /* Wipe the_node */
  _RBTree_Set_off_rbtree(the_node);

  /* set root to black, if it exists */
  if (the_rbtree->root) the_rbtree->root->color = RBT_BLACK;
}


/*
 *  _RBTree_Extract
 *
 *  This kernel routine deletes the given node from a rbtree.
 *
 *  Input parameters:
 *    node - pointer to node in rbtree to be deleted
 *
 *  Output parameters:  NONE
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

void _RBTree_Extract(
  RBTree_Control *the_rbtree,
  RBTree_Node *the_node
)
{
  ISR_Level level;

  _ISR_Disable( level );
    _RBTree_Extract_unprotected( the_rbtree, the_node );
  _ISR_Enable( level );
}
