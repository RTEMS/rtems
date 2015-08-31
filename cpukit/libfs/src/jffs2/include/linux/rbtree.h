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

struct rb_node {
  struct rb_node *rb_left;
  struct rb_node *rb_right;
  struct rb_node *rb_parent;
  int rb_color;
};

RTEMS_STATIC_ASSERT(
  sizeof( struct rb_node ) == sizeof( RBTree_Node ),
  rb_node_size
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_node, rb_left ) == offsetof( RBTree_Node, Node.rbe_left ),
  rb_node_left
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_node, rb_right ) == offsetof( RBTree_Node, Node.rbe_right ),
  rb_node_right
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_node, rb_parent ) == offsetof( RBTree_Node, Node.rbe_parent ),
  rb_node_parent
);

RTEMS_STATIC_ASSERT(
  offsetof( struct rb_node, rb_color ) == offsetof( RBTree_Node, Node.rbe_color ),
  rb_node_color
);

struct rb_root {
  struct rb_node *rb_node;
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
  _RBTree_Insert_color( (RBTree_Control *) root, (RBTree_Node *) node );
}

static inline void rb_erase( struct rb_node *node, struct rb_root *root )
{
  _RBTree_Extract( (RBTree_Control *) root, (RBTree_Node *) node );
}

static inline struct rb_node *rb_next( struct rb_node *node )
{
  return (struct rb_node *) _RBTree_Successor( (RBTree_Node *) node );
}

static inline struct rb_node *rb_prev( struct rb_node *node )
{
  return (struct rb_node *) _RBTree_Predecessor( (RBTree_Node *) node );
}

static inline struct rb_node *rb_first( struct rb_root *root )
{
  return (struct rb_node *) _RBTree_Minimum( (RBTree_Control *) root );
}

static inline struct rb_node *rb_last( struct rb_root *root )
{
  return (struct rb_node *) _RBTree_Maximum( (RBTree_Control *) root );
}

static inline void rb_replace_node(
  struct rb_node *victim,
  struct rb_node *replacement, 
  struct rb_root *root
)
{
  _RBTree_Replace_node(
    (RBTree_Control *) root,
    (RBTree_Node *) victim,
    (RBTree_Node *) replacement
  );
}

static inline void rb_link_node(
  struct rb_node *node,
  struct rb_node *parent,
  struct rb_node **link
)
{
  _RBTree_Add_child(
    (RBTree_Node *) node,
    (RBTree_Node *) parent,
    (RBTree_Node **) link
  );
}

static inline struct rb_node *rb_parent( struct rb_node *node )
{
  return (struct rb_node *) _RBTree_Parent( (RBTree_Node *) node );
}

#endif /* _LINUX_RBTREE_H */
