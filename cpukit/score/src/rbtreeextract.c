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
 *  Copyright (c) 2010 Gedare Bloom.
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
