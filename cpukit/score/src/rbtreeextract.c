/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This source file contains the implementation of
 *   _RBTree_Extract().
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
 * Copyright (C) 2010 Gedare Bloom.
 * Copyright (C) 2002 Niels Provos <provos@citi.umich.edu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * The red-black tree code of this file derives from the macros of the
 * FreeBSD <sys/tree.h> file of 2020.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/rbtreeimpl.h>

static void _RBTree_Change_child(
  RBTree_Control *head,
  RBTree_Node    *old_node,
  RBTree_Node    *new_node,
  RBTree_Node    *parent
)
{
  if ( parent == NULL ) {
    _RBTree_Set_root( head, new_node );
  } else if ( _RBTree_Left( parent ) == old_node ) {
    _RBTree_Set_left( parent, new_node );
  } else {
    _RBTree_Set_right( parent, new_node );
  }
}

/*
 * The new node takes the parent and the color of the old node.  The old node
 * gets the new node as its parent and the specified color.
 */
static void _RBTree_Rotate_set_parents(
  RBTree_Control *head,
  RBTree_Node    *old_node,
  RBTree_Node    *new_node,
  int             color
)
{
  RBTree_Node *parent;

  parent = _RBTree_Parent( old_node );
  _RBTree_Set_parent( new_node, parent );
  _RBTree_Set_color( new_node, _RBTree_Color( old_node ) );
  _RBTree_Set_parent( old_node, new_node );
  _RBTree_Set_color( old_node, color );
  _RBTree_Change_child( head, old_node, new_node, parent );
}

/*
 * The removal of a black node without a child leaves the paths through the
 * parent one black node short.
 */
static void _RBTree_Remove_color( RBTree_Control *head, RBTree_Node *parent )
{
  RBTree_Node *elm;
  RBTree_Node *sibling;
  RBTree_Node *tmp1;
  RBTree_Node *tmp2;

  elm = NULL;

  while ( true ) {
    sibling = _RBTree_Right( parent );

    if ( elm != sibling ) {
      /* The node is the left child.  The sibling is the right child. */
      if ( _RBTree_Color( sibling ) == RTEMS_RB_RED ) {
        tmp1 = _RBTree_Left( sibling );
        _RBTree_Set_right( parent, tmp1 );
        _RBTree_Set_left( sibling, parent );
        _RBTree_Set_parent( tmp1, parent );
        _RBTree_Set_color( tmp1, RTEMS_RB_BLACK );
        _RBTree_Rotate_set_parents( head, parent, sibling, RTEMS_RB_RED );
        sibling = tmp1;
      }

      tmp1 = _RBTree_Right( sibling );

      if ( !_RBTree_Is_red( tmp1 ) ) {
        tmp2 = _RBTree_Left( sibling );

        if ( !_RBTree_Is_red( tmp2 ) ) {
          _RBTree_Set_color( sibling, RTEMS_RB_RED );

          if ( _RBTree_Color( parent ) == RTEMS_RB_RED ) {
            _RBTree_Set_color( parent, RTEMS_RB_BLACK );
            return;
          }

          elm = parent;
          parent = _RBTree_Parent( elm );

          if ( parent == NULL ) {
            return;
          }

          continue;
        }

        tmp1 = _RBTree_Right( tmp2 );
        _RBTree_Set_left( sibling, tmp1 );
        _RBTree_Set_right( tmp2, sibling );
        _RBTree_Set_right( parent, tmp2 );

        if ( tmp1 != NULL ) {
          _RBTree_Set_parent( tmp1, sibling );
          _RBTree_Set_color( tmp1, RTEMS_RB_BLACK );
        }

        tmp1 = sibling;
        sibling = tmp2;
      }

      tmp2 = _RBTree_Left( sibling );
      _RBTree_Set_right( parent, tmp2 );
      _RBTree_Set_left( sibling, parent );
      _RBTree_Set_parent( tmp1, sibling );
      _RBTree_Set_color( tmp1, RTEMS_RB_BLACK );

      if ( tmp2 != NULL ) {
        _RBTree_Set_parent( tmp2, parent );
      }

      _RBTree_Rotate_set_parents( head, parent, sibling, RTEMS_RB_BLACK );
      return;
    }

    sibling = _RBTree_Left( parent );

    if ( _RBTree_Color( sibling ) == RTEMS_RB_RED ) {
      tmp1 = _RBTree_Right( sibling );
      _RBTree_Set_left( parent, tmp1 );
      _RBTree_Set_right( sibling, parent );
      _RBTree_Set_parent( tmp1, parent );
      _RBTree_Set_color( tmp1, RTEMS_RB_BLACK );
      _RBTree_Rotate_set_parents( head, parent, sibling, RTEMS_RB_RED );
      sibling = tmp1;
    }

    tmp1 = _RBTree_Left( sibling );

    if ( !_RBTree_Is_red( tmp1 ) ) {
      tmp2 = _RBTree_Right( sibling );

      if ( !_RBTree_Is_red( tmp2 ) ) {
        _RBTree_Set_color( sibling, RTEMS_RB_RED );

        if ( _RBTree_Color( parent ) == RTEMS_RB_RED ) {
          _RBTree_Set_color( parent, RTEMS_RB_BLACK );
          return;
        }

        elm = parent;
        parent = _RBTree_Parent( elm );

        if ( parent == NULL ) {
          return;
        }

        continue;
      }

      tmp1 = _RBTree_Left( tmp2 );
      _RBTree_Set_right( sibling, tmp1 );
      _RBTree_Set_left( tmp2, sibling );
      _RBTree_Set_left( parent, tmp2 );

      if ( tmp1 != NULL ) {
        _RBTree_Set_parent( tmp1, sibling );
        _RBTree_Set_color( tmp1, RTEMS_RB_BLACK );
      }

      tmp1 = sibling;
      sibling = tmp2;
    }

    tmp2 = _RBTree_Right( sibling );
    _RBTree_Set_left( parent, tmp2 );
    _RBTree_Set_right( sibling, parent );
    _RBTree_Set_parent( tmp1, sibling );
    _RBTree_Set_color( tmp1, RTEMS_RB_BLACK );

    if ( tmp2 != NULL ) {
      _RBTree_Set_parent( tmp2, parent );
    }

    _RBTree_Rotate_set_parents( head, parent, sibling, RTEMS_RB_BLACK );
    return;
  }
}

static void _RBTree_Remove( RBTree_Control *head, RBTree_Node *elm )
{
  RBTree_Node *child;
  RBTree_Node *tmp;
  RBTree_Node *parent;
  RBTree_Node *rebalance;
  int          color;

  /*
   * Each case needs the parent and the color of the node.  The loads at the
   * start overlap with the loads of the children.
   */
  child = _RBTree_Right( elm );
  tmp = _RBTree_Left( elm );
  parent = _RBTree_Parent( elm );
  color = _RBTree_Color( elm );

  if ( tmp == NULL ) {
    /*
     * The node has at most a right child.  A child is red, so it takes the
     * place and the color of the node.
     */
    _RBTree_Change_child( head, elm, child, parent );

    if ( child != NULL ) {
      _RBTree_Set_parent( child, parent );
      _RBTree_Set_color( child, color );
      rebalance = NULL;
    } else if ( color == RTEMS_RB_BLACK ) {
      rebalance = parent;
    } else {
      rebalance = NULL;
    }
  } else if ( child == NULL ) {
    /* The node has only a left child.  It is red and takes the place. */
    _RBTree_Set_parent( tmp, parent );
    _RBTree_Set_color( tmp, color );
    _RBTree_Change_child( head, elm, tmp, parent );
    rebalance = NULL;
  } else {
    RBTree_Node *successor;
    RBTree_Node *child2;

    /*
     * The node has two children.  Its successor takes its place, its children
     * and its color.
     */
    successor = child;
    tmp = _RBTree_Left( child );

    if ( tmp == NULL ) {
      parent = successor;
      child2 = _RBTree_Right( successor );
    } else {
      do {
        parent = successor;
        successor = tmp;
        tmp = _RBTree_Left( tmp );
      } while ( tmp != NULL );

      child2 = _RBTree_Right( successor );
      _RBTree_Set_left( parent, child2 );
      _RBTree_Set_right( successor, child );
      _RBTree_Set_parent( child, successor );
    }

    tmp = _RBTree_Left( elm );
    _RBTree_Set_left( successor, tmp );
    _RBTree_Set_parent( tmp, successor );
    tmp = _RBTree_Parent( elm );
    _RBTree_Change_child( head, elm, successor, tmp );

    if ( child2 != NULL ) {
      _RBTree_Set_parent( child2, parent );
      _RBTree_Set_color( child2, RTEMS_RB_BLACK );
      rebalance = NULL;
    } else if ( _RBTree_Color( successor ) == RTEMS_RB_BLACK ) {
      rebalance = parent;
    } else {
      rebalance = NULL;
    }

    _RBTree_Set_parent( successor, tmp );
    _RBTree_Set_color( successor, color );
  }

  if ( rebalance != NULL ) {
    _RBTree_Remove_color( head, rebalance );
  }
}

#if defined( RTEMS_DEBUG )
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

void _RBTree_Extract( RBTree_Control *the_rbtree, RBTree_Node *the_node )
{
  _Assert( _RBTree_Find_root( the_node ) == _RBTree_Root( the_rbtree ) );
  _RBTree_Remove( the_rbtree, the_node );
  _RBTree_Initialize_node( the_node );
}
