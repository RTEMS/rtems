/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicRBTrees
 *
 * @brief This source file contains the implementation of
 *   rtems_rbtree_find().
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

#include <rtems/rbtree.h>

rtems_rbtree_node *rtems_rbtree_find(
  const rtems_rbtree_control *the_rbtree,
  const rtems_rbtree_node    *the_node,
  rtems_rbtree_compare        compare,
  bool                  is_unique
)
{
  rtems_rbtree_node *iter_node = rtems_rbtree_root( the_rbtree );
  rtems_rbtree_node *found = NULL;

  while ( iter_node != NULL ) {
    rtems_rbtree_compare_result compare_result =
      ( *compare )( the_node, iter_node );

    if ( rtems_rbtree_is_equal( compare_result ) ) {
      found = iter_node;

      if ( is_unique )
        break;
    }

    if ( rtems_rbtree_is_greater( compare_result ) ) {
      iter_node = rtems_rbtree_right( iter_node );
    } else {
      iter_node = rtems_rbtree_left( iter_node );
    }
  }

  return found;
}
