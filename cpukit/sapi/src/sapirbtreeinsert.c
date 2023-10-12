/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicRBTrees
 *
 * @brief This source file contains the implementation of
 *   rtems_rbtree_insert().
 */

/*
 *  Copyright (c) 2010-2012 Gedare Bloom.
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

#include <rtems/rbtree.h>

RTEMS_STATIC_ASSERT(
  sizeof( rtems_rbtree_compare_result ) >= sizeof( intptr_t ),
  rtems_rbtree_compare_result_intptr_t
);

RTEMS_STATIC_ASSERT(
  sizeof( rtems_rbtree_compare_result ) >= sizeof( int32_t ),
  rtems_rbtree_compare_result_int32_t
);

rtems_rbtree_node *rtems_rbtree_insert(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_node    *the_node,
  rtems_rbtree_compare  compare,
  bool                  is_unique
)
{
  rtems_rbtree_node **which = _RBTree_Root_reference( the_rbtree );
  rtems_rbtree_node  *parent = NULL;

  while ( *which != NULL ) {
    rtems_rbtree_compare_result compare_result;

    parent = *which;
    compare_result = ( *compare )( the_node, parent );

    if ( is_unique && rtems_rbtree_is_equal( compare_result ) ) {
      return parent;
    }

    if ( rtems_rbtree_is_lesser( compare_result ) ) {
      which = _RBTree_Left_reference( parent );
    } else {
      which = _RBTree_Right_reference( parent );
    }
  }

  _RBTree_Initialize_node( the_node );
  _RBTree_Add_child( the_node, parent, which );
  _RBTree_Insert_color( the_rbtree, the_node );

  return NULL;
}
