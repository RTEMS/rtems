/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSAPIClassicRBTrees
 *
 * @brief This source file contains the implementation of
 *   rtems_rbtree_initialize().
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
#include <rtems/score/address.h>

void rtems_rbtree_initialize(
  rtems_rbtree_control *the_rbtree,
  rtems_rbtree_compare  compare,
  void                 *starting_address,
  size_t                number_nodes,
  size_t                node_size,
  bool                  is_unique
)
{
  size_t             count;
  rtems_rbtree_node *next;

  /* could do sanity checks here */
  rtems_rbtree_initialize_empty( the_rbtree );

  count = number_nodes;
  next = starting_address;

  while ( count-- ) {
    rtems_rbtree_insert( the_rbtree, next, compare, is_unique );
    next = (rtems_rbtree_node *) _Addresses_Add_offset( next, node_size );
  }
}
