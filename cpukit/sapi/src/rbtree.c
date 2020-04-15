/**
 *  @file
 *
 *  @brief Initialize a Red-Black Tree
 *  @ingroup ClassicRBTrees
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
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
