/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This source file contains the implementation of
 *   _RBTree_Postorder_next() and _RBTree_Postorder_first().
 */

/*
 * Copyright (c) 2018 embedded brains GmbH & Co. KG
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
