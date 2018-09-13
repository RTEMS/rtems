/*
 *  Copyright (c) 2010-2012 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rbtree.h>
#include <rtems/score/rbtreeimpl.h>

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
