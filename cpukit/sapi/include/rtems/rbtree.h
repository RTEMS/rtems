/**
 * @file rtems/rbtree.h
 *
 *  This include file contains all the constants and structures associated
 *  with the RBTree API in RTEMS. The rbtree is a Red Black Tree that
 *  is part of the Super Core. This is the published interface to that
 *  code.
 *
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_RBTREE_H
#define _RTEMS_RBTREE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/system.h>
#include <rtems/score/rbtree.h>

/**
 * @typedef rtems_rbtree_node
 *
 * A node that can be manipulated in the rbtree.
 */
typedef RBTree_Node rtems_rbtree_node;

/**
 * @typedef rtems_rbtree_control
 *
 * The rbtree's control anchors the rbtree.
 */
typedef RBTree_Control rtems_rbtree_control;

/**
 * @typedef rtems_rbtree_compare_function
 *
 * This type defines function pointers for user-provided comparison
 * function. The function compares two nodes in order to determine
 * the order in a red-black tree.
 */
typedef RBTree_Compare_function rtems_rbtree_compare_function;

/**
 *  @brief RBTree initializer for an empty rbtree with designator @a name.
 */
#define RTEMS_RBTREE_INITIALIZER_EMPTY(name) \
  RBTREE_INITIALIZER_EMPTY(name)

/**
 *  @brief RBTree definition for an empty rbtree with designator @a name.
 */
#define RTEMS_RBTREE_DEFINE_EMPTY(name) \
  RBTREE_DEFINE_EMPTY(name)

/**
  * @brief macro to return the structure containing the @a node.
  *
  * This macro returns a pointer of type @a object_type that points 
  * to the structure containing @a node, where @a object_member is the 
  * field name of the rtems_rbtree_node structure in objects of @a object_type.
  */
#define rtems_rbtree_container_of(node,object_type, object_member) \
  _RBTree_Container_of(node,object_type,object_member)

#include <rtems/rbtree.inl>

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
