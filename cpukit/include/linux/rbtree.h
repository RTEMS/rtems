/*
 * Copyright (c) 2015 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _LINUX_RBTREE_H
#define _LINUX_RBTREE_H

#include <rtems/score/rbtree.h>

#define rb_node RBTree_Node

#define rb_left Node.rbe_left

#define rb_right Node.rbe_right

/*
 * Getting rid of this placeholder structure is a bit difficult.  The use of
 * this placeholder struct may lead to bugs with link-time optimization due to
 * a strict aliasing violation.
 *
 * A common use of this API is a direct access of the rb_node member to get the
 * root node of the tree. So, this cannot be changed.
 *
 * The red-black tree implementation is provided by <sys/tree.h> and we have
 *
 * struct RBTree_Control {
 *   struct RBTree_Node *rbh_root;
 * };
 *
 * The member name rbh_root is fixed by the <sys/tree.h> API.  To use
 * RBTree_Control directly we would need two defines:
 *
 * #define rb_root RBTree_Control
 * #define rb_node rbh_root
 *
 * We already have an rb_node define to RBTree_Node, see above.
 */
struct rb_root {
  RBTree_Node *rb_node;
};

RTEMS_STATIC_ASSERT(
  sizeof( struct rb_root ) == sizeof( RBTree_Control ),
  rb_root_size
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_root, rb_node ) == offsetof( RBTree_Control, rbh_root ),
  rb_root_node
);

#undef RB_ROOT
#define RB_ROOT ( (struct rb_root) { NULL } )

#define rb_entry( p, container, field ) RTEMS_CONTAINER_OF( p, container, field )

static inline void rb_insert_color( struct rb_node *node, struct rb_root *root)
{
  _RBTree_Insert_color( (RBTree_Control *) root, node );
}

static inline void rb_erase( struct rb_node *node, struct rb_root *root )
{
  _RBTree_Extract( (RBTree_Control *) root, node );
}

static inline struct rb_node *rb_next( struct rb_node *node )
{
  return _RBTree_Successor( node );
}

static inline struct rb_node *rb_prev( struct rb_node *node )
{
  return _RBTree_Predecessor( node );
}

static inline struct rb_node *rb_first( struct rb_root *root )
{
  return _RBTree_Minimum( (RBTree_Control *) root );
}

static inline struct rb_node *rb_last( struct rb_root *root )
{
  return _RBTree_Maximum( (RBTree_Control *) root );
}

static inline void rb_replace_node(
  struct rb_node *victim,
  struct rb_node *replacement, 
  struct rb_root *root
)
{
  _RBTree_Replace_node(
    (RBTree_Control *) root,
    victim,
    replacement
  );
}

static inline void rb_link_node(
  struct rb_node *node,
  struct rb_node *parent,
  struct rb_node **link
)
{
  _RBTree_Initialize_node( node );
  _RBTree_Add_child( node, parent, link );
}

static inline struct rb_node *rb_parent( struct rb_node *node )
{
  return _RBTree_Parent( node );
}

#define rbtree_postorder_for_each_entry_safe( node, next, root, field ) \
  for ( \
    node = _RBTree_Postorder_first( \
      (RBTree_Control *) root, \
      offsetof( __typeof__( *node ), field ) \
    ); \
    node != NULL && ( \
      next = _RBTree_Postorder_next( \
        &node->field, \
        offsetof( __typeof__( *node ), field ) \
      ), \
      node != NULL \
    ); \
    node = next \
  )

#endif /* _LINUX_RBTREE_H */
