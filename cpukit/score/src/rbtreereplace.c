/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreRBTree
 *
 * @brief This source file contains the implementation of
 *   _RBTree_Replace_node().
 */

/*
 * Copyright (c) 2015 embedded brains GmbH & Co. KG
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

void _RBTree_Replace_node(
  RBTree_Control *the_rbtree,
  RBTree_Node    *victim,
  RBTree_Node    *replacement
)
{
  RBTree_Node  *parent = _RBTree_Parent( victim );
  RBTree_Node **link;
  RBTree_Node  *child;

  if (parent != NULL) {
    if ( victim == _RBTree_Left( parent ) ) {
      link = _RBTree_Left_reference( parent );
    } else {
      link = _RBTree_Right_reference( parent );
    }
  } else {
    link = _RBTree_Root_reference( the_rbtree );
  }
  *link = replacement;

  child = _RBTree_Left( victim );
  if ( child != NULL ) {
    RB_PARENT( child, Node ) = replacement;
  }

  child = _RBTree_Right( victim );
  if ( child != NULL ) {
    RB_PARENT( child, Node ) = replacement;
  }

  *replacement = *victim;
}
