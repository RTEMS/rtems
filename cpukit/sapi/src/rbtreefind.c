/**
 * @file
 *
 * @brief Find the control structure of the tree containing the given node
 * @ingroup RTEMSScorertems_rbtree
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
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
