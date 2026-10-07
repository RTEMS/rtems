/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreRBTree which are only used by the implementation.
 */

/*
 * Copyright (C) 2010-2012 Gedare Bloom.
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

#ifndef _RTEMS_SCORE_RBTREEIMPL_H
#define _RTEMS_SCORE_RBTREEIMPL_H

#include <rtems/score/rbtree.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreRBTree
 *
 * @{
 */

/**
 * @brief Appends the node to the red-black tree.
 *
 * The appended node is the new maximum node of the tree.  The caller shall
 * ensure that the appended node is indeed the maximum node with respect to the
 * tree order.
 *
 * @param[in, out] the_rbtree is the red-black tree control.
 *
 * @param the_node[out] is the node to append.
 */
void _RBTree_Append( RBTree_Control *the_rbtree, RBTree_Node *the_node );

/**
 * @brief Prepends the node to the red-black tree.
 *
 * The prepended node is the new minimum node of the tree.  The caller shall
 * ensure that the prepended node is indeed the minimum node with respect to the
 * tree order.
 *
 * @param[in, out] the_rbtree is the red-black tree control.
 *
 * @param the_node[out] is the node to prepend.
 */
void _RBTree_Prepend( RBTree_Control *the_rbtree, RBTree_Node *the_node );

/**
 * @brief Red-black tree visitor.
 *
 * @param[in] node The node.
 * @param[in] visitor_arg The visitor argument.
 *
 * @retval true Stop the iteration.
 * @retval false Continue the iteration.
 *
 * @see _RBTree_Iterate().
 */
typedef bool ( *RBTree_Visitor )( const RBTree_Node *node, void *visitor_arg );

/**
 * @brief Red-black tree iteration.
 *
 * @param rbtree The red-black tree.
 * @param visitor The visitor.
 * @param visitor_arg The visitor argument.
 */
void _RBTree_Iterate(
  const RBTree_Control *rbtree,
  RBTree_Visitor        visitor,
  void                 *visitor_arg
);

/**
 * @brief Checks if the node is red.
 *
 * @param the_node is the node or NULL.
 *
 * @return Returns true, if the node is not NULL and red, otherwise false.
 */
static inline bool _RBTree_Is_red( const RBTree_Node *the_node )
{
  return the_node != NULL && _RBTree_Color( the_node ) == RTEMS_RB_RED;
}

/**
 * @brief Sets the color of the first node to black and the color of the
 *   second node to red.
 *
 * @param[out] black is the node to color black.
 *
 * @param[out] red is the node to color red.
 */
static inline void _RBTree_Set_black_red(
  RBTree_Node *black,
  RBTree_Node *red
)
{
  _RBTree_Set_color( black, RTEMS_RB_BLACK );
  _RBTree_Set_color( red, RTEMS_RB_RED );
}

/**
 * @brief Replaces the node by another node in the child link of the parent of
 *   the node.
 *
 * @param[in, out] head is the red-black tree control.
 *
 * @param out is the node to replace.
 *
 * @param in is the node which replaces the node.
 */
static inline void _RBTree_Swap_child(
  RBTree_Control *head,
  RBTree_Node    *out,
  RBTree_Node    *in
)
{
  if ( _RBTree_Parent( out ) == NULL ) {
    _RBTree_Set_root( head, in );
  } else if ( out == _RBTree_Left( _RBTree_Parent( out ) ) ) {
    _RBTree_Set_left( _RBTree_Parent( out ), in );
  } else {
    _RBTree_Set_right( _RBTree_Parent( out ), in );
  }
}

/**
 * @brief Rotates the right child of the node to the left.
 *
 * @param[in, out] head is the red-black tree control.
 *
 * @param[in, out] elm is the node.
 *
 * @return Returns the right child of the node before the rotation.
 */
static inline RBTree_Node *_RBTree_Rotate_left(
  RBTree_Control *head,
  RBTree_Node    *elm
)
{
  RBTree_Node *tmp;

  tmp = _RBTree_Right( elm );
  _RBTree_Set_right( elm, _RBTree_Left( tmp ) );

  if ( _RBTree_Right( elm ) != NULL ) {
    _RBTree_Set_parent( _RBTree_Right( elm ), elm );
  }

  _RBTree_Set_parent( tmp, _RBTree_Parent( elm ) );
  _RBTree_Swap_child( head, elm, tmp );
  _RBTree_Set_left( tmp, elm );
  _RBTree_Set_parent( elm, tmp );
  return tmp;
}

/**
 * @brief Rotates the left child of the node to the right.
 *
 * @param[in, out] head is the red-black tree control.
 *
 * @param[in, out] elm is the node.
 *
 * @return Returns the left child of the node before the rotation.
 */
static inline RBTree_Node *_RBTree_Rotate_right(
  RBTree_Control *head,
  RBTree_Node    *elm
)
{
  RBTree_Node *tmp;

  tmp = _RBTree_Left( elm );
  _RBTree_Set_left( elm, _RBTree_Right( tmp ) );

  if ( _RBTree_Left( elm ) != NULL ) {
    _RBTree_Set_parent( _RBTree_Left( elm ), elm );
  }

  _RBTree_Set_parent( tmp, _RBTree_Parent( elm ) );
  _RBTree_Swap_child( head, elm, tmp );
  _RBTree_Set_right( tmp, elm );
  _RBTree_Set_parent( elm, tmp );
  return tmp;
}

/**
 * @brief Rotates the right child of the left child of the parent to the left.
 *
 * The left child is the left child of the parent.
 *
 * @param[in, out] parent is the parent.
 *
 * @param[in, out] left is the left child of the parent.
 *
 * @return Returns the right child of the left child before the rotation.
 */
static inline RBTree_Node *_RBTree_Parent_rotate_left(
  RBTree_Node *parent,
  RBTree_Node *left
)
{
  RBTree_Node *tmp;

  tmp = _RBTree_Right( left );
  _RBTree_Set_right( left, _RBTree_Left( tmp ) );

  if ( _RBTree_Right( left ) != NULL ) {
    _RBTree_Set_parent( _RBTree_Right( left ), left );
  }

  _RBTree_Set_parent( tmp, parent );
  _RBTree_Set_left( parent, tmp );
  _RBTree_Set_left( tmp, left );
  _RBTree_Set_parent( left, tmp );
  return tmp;
}

/**
 * @brief Rotates the left child of the right child of the parent to the right.
 *
 * The right child is the right child of the parent.
 *
 * @param[in, out] parent is the parent.
 *
 * @param[in, out] right is the right child of the parent.
 *
 * @return Returns the left child of the right child before the rotation.
 */
static inline RBTree_Node *_RBTree_Parent_rotate_right(
  RBTree_Node *parent,
  RBTree_Node *right
)
{
  RBTree_Node *tmp;

  tmp = _RBTree_Left( right );
  _RBTree_Set_left( right, _RBTree_Right( tmp ) );

  if ( _RBTree_Left( right ) != NULL ) {
    _RBTree_Set_parent( _RBTree_Left( right ), right );
  }

  _RBTree_Set_parent( tmp, parent );
  _RBTree_Set_right( parent, tmp );
  _RBTree_Set_right( tmp, right );
  _RBTree_Set_parent( right, tmp );
  return tmp;
}

/**
 * @brief Rotates the red right child of the node to the left.
 *
 * The right child is red and has a left child.
 *
 * @param[in, out] head is the red-black tree control.
 *
 * @param[in, out] elm is the node.
 *
 * @return Returns the right child of the node before the rotation.
 */
static inline RBTree_Node *_RBTree_Red_rotate_left(
  RBTree_Control *head,
  RBTree_Node    *elm
)
{
  RBTree_Node *tmp;

  tmp = _RBTree_Right( elm );
  _RBTree_Set_right( elm, _RBTree_Left( tmp ) );
  _RBTree_Set_parent( _RBTree_Right( elm ), elm );
  _RBTree_Set_parent( tmp, _RBTree_Parent( elm ) );
  _RBTree_Swap_child( head, elm, tmp );
  _RBTree_Set_left( tmp, elm );
  _RBTree_Set_parent( elm, tmp );
  return tmp;
}

/**
 * @brief Rotates the red left child of the node to the right.
 *
 * The left child is red and has a right child.
 *
 * @param[in, out] head is the red-black tree control.
 *
 * @param[in, out] elm is the node.
 *
 * @return Returns the left child of the node before the rotation.
 */
static inline RBTree_Node *_RBTree_Red_rotate_right(
  RBTree_Control *head,
  RBTree_Node    *elm
)
{
  RBTree_Node *tmp;

  tmp = _RBTree_Left( elm );
  _RBTree_Set_left( elm, _RBTree_Right( tmp ) );
  _RBTree_Set_parent( _RBTree_Left( elm ), elm );
  _RBTree_Set_parent( tmp, _RBTree_Parent( elm ) );
  _RBTree_Swap_child( head, elm, tmp );
  _RBTree_Set_right( tmp, elm );
  _RBTree_Set_parent( elm, tmp );
  return tmp;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
