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

static void _RBTree_Remove_color( RBTree_Control *head, RBTree_Node *parent )
{
  RBTree_Node *elm;
  RBTree_Node *tmp;

  elm = NULL;

  do {
    if ( _RBTree_Left( parent ) == elm ) {
      tmp = _RBTree_Right( parent );

      if ( _RBTree_Color( tmp ) == RTEMS_RB_RED ) {
        _RBTree_Set_black_red( tmp, parent );
        (void) _RBTree_Red_rotate_left( head, parent );
        tmp = _RBTree_Right( parent );
      }

      if ( _RBTree_Is_red( _RBTree_Right( tmp ) ) ) {
        _RBTree_Set_color( _RBTree_Right( tmp ), RTEMS_RB_BLACK );
      } else if ( _RBTree_Is_red( _RBTree_Left( tmp ) ) ) {
        RBTree_Node *oleft;

        oleft = _RBTree_Parent_rotate_right( parent, tmp );
        _RBTree_Set_color( oleft, RTEMS_RB_BLACK );
        tmp = oleft;
      } else {
        _RBTree_Set_color( tmp, RTEMS_RB_RED );
        elm = parent;
        parent = _RBTree_Parent( elm );
        continue;
      }

      _RBTree_Set_color( tmp, _RBTree_Color( parent ) );
      _RBTree_Set_color( parent, RTEMS_RB_BLACK );
      (void) _RBTree_Rotate_left( head, parent );
      elm = _RBTree_Root( head );
      break;
    } else {
      tmp = _RBTree_Left( parent );

      if ( _RBTree_Color( tmp ) == RTEMS_RB_RED ) {
        _RBTree_Set_black_red( tmp, parent );
        (void) _RBTree_Red_rotate_right( head, parent );
        tmp = _RBTree_Left( parent );
      }

      if ( _RBTree_Is_red( _RBTree_Left( tmp ) ) ) {
        _RBTree_Set_color( _RBTree_Left( tmp ), RTEMS_RB_BLACK );
      } else if ( _RBTree_Is_red( _RBTree_Right( tmp ) ) ) {
        RBTree_Node *oright;

        oright = _RBTree_Parent_rotate_left( parent, tmp );
        _RBTree_Set_color( oright, RTEMS_RB_BLACK );
        tmp = oright;
      } else {
        _RBTree_Set_color( tmp, RTEMS_RB_RED );
        elm = parent;
        parent = _RBTree_Parent( elm );
        continue;
      }

      _RBTree_Set_color( tmp, _RBTree_Color( parent ) );
      _RBTree_Set_color( parent, RTEMS_RB_BLACK );
      (void) _RBTree_Rotate_right( head, parent );
      elm = _RBTree_Root( head );
      break;
    }
  } while ( _RBTree_Color( elm ) == RTEMS_RB_BLACK && parent != NULL );

  _RBTree_Set_color( elm, RTEMS_RB_BLACK );
}

static void _RBTree_Remove( RBTree_Control *head, RBTree_Node *elm )
{
  RBTree_Node *child;
  RBTree_Node *old;
  RBTree_Node *parent;
  RBTree_Node *right;
  int          color;

  old = elm;
  parent = _RBTree_Parent( elm );
  right = _RBTree_Right( elm );
  color = _RBTree_Color( elm );

  if ( _RBTree_Left( elm ) == NULL ) {
    elm = child = right;
  } else if ( right == NULL ) {
    elm = child = _RBTree_Left( elm );
  } else {
    if ( ( child = _RBTree_Left( right ) ) == NULL ) {
      child = _RBTree_Right( right );
      _RBTree_Set_right( old, child );
      parent = elm = right;
    } else {
      do {
        elm = child;
      } while ( ( child = _RBTree_Left( elm ) ) != NULL );

      child = _RBTree_Right( elm );
      parent = _RBTree_Parent( elm );
      _RBTree_Set_left( parent, child );
      _RBTree_Set_parent( _RBTree_Right( old ), elm );
    }

    _RBTree_Set_parent( _RBTree_Left( old ), elm );
    color = _RBTree_Color( elm );
    *elm = *old;
  }

  _RBTree_Swap_child( head, old, elm );

  if ( child != NULL ) {
    _RBTree_Set_parent( child, parent );
    _RBTree_Set_color( child, RTEMS_RB_BLACK );
  } else if ( color != RTEMS_RB_RED && parent != NULL ) {
    _RBTree_Remove_color( head, parent );
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
