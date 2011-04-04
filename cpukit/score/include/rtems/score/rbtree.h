
/**
 *  @file  rtems/score/rbtree.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Red-Black Tree Handler.
 */

/*
 *  Copyright (c) 2010 Gedare Bloom.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_SCORE_RBTREE_H
#define _RTEMS_SCORE_RBTREE_H

/**
 *  @defgroup ScoreRBTree Red-Black Tree Handler
 *
 *  The Red-Black Tree Handler is used to manage sets of entities.  This handler
 *  provides two data structures.  The rbtree Node data structure is included
 *  as the first part of every data structure that will be placed on
 *  a RBTree.  The second data structure is rbtree Control which is used
 *  to manage a set of rbtree Nodes.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/address.h>

  /**
   *  @typedef RBTree_Node
   *
   *  This type definition promotes the name for the RBTree Node used by
   *  all RTEMS code.  It is a separate type definition because a forward
   *  reference is required to define it.  See @ref RBTree_Node_struct for
   *  detailed information.
   */
  typedef struct RBTree_Node_struct RBTree_Node;

  /**
   * This enum type defines the colors available for the RBTree Nodes
   */
  typedef enum {
    RBT_BLACK,
    RBT_RED
  } RBTree_Color;

  /**
   *  @struct RBTree_Node_struct
   *
   *  This is used to manage each element (node) which is placed
   *  on a RBT.
   *
   *  @note Typically, a more complicated structure will use the
   *        rbtree package.  The more complicated structure will
   *        include a rbtree node as the first element in its
   *        control structure.  It will then call the rbtree package
   *        with a pointer to that node element.  The node pointer
   *        and the higher level structure start at the same address
   *        so the user can cast the pointers back and forth.
   *
   */
  struct RBTree_Node_struct {
    /** This points to the node's parent */
    RBTree_Node *parent;
    /** child[0] points to the left child, child[1] points to the right child */
    RBTree_Node *child[2];
    /** This is the integer value stored by this node, used for sorting */
    unsigned int value;
    /** The color of the node. Either red or black */
    RBTree_Color color;
  };

  /**
   * @brief macro to return the structure containing the @a node.
   *
   * This macro returns a pointer of type @a container_type that points 
   * to the structure containing @a node, where @a node_field_name is the 
   * field name of the RBTree_Node structure in @a container_type.
   *
   */
  
#define _RBTree_Container_of(node,container_type, node_field_name) \
  ((container_type*) \
   ((size_t)node - ((size_t)(&((container_type *)0)->node_field_name))))


  typedef enum {
    RBT_LEFT=0,
    RBT_RIGHT=1
  } RBTree_Direction;

  /**
   *  @struct RBTree_Control
   *
   * This is used to manage a RBT.  A rbtree consists of a tree of zero or more
   * nodes.
   *
   * @note This implementation does not require special checks for
   *   manipulating the root element of the RBT.
   *   To accomplish this the @a RBTree_Control structure can be overlaid 
   *   with a @ref RBTree_Node structure to act as a "dummy root", 
   *   which has a NULL parent and its left child is the root.
   */

  /* the RBTree_Control is actually part of the RBTree structure as an 
   * RBTree_Node. The mapping of fields from RBTree_Control to RBTree_Node are:
   *   permanent_null == parent
   *   root == left
   *   first[0] == right
   */
  typedef struct {
    /** This points to a NULL. Useful for finding the root. */
    RBTree_Node *permanent_null;
    /** This points to the root node of the RBT. */
    RBTree_Node *root;
    /** This points to the min and max nodes of this RBT. */
    RBTree_Node *first[2];
  } RBTree_Control;

  /**
   *  @brief RBTree initializer for an empty rbtree with designator @a name.
   */
#define RBTREE_INITIALIZER_EMPTY(name) \
  { \
    .permanent_null = NULL, \
    .root = NULL, \
    .first[0] = NULL, \
    .first[1] = NULL, \
  }

  /**
   *  @brief RBTree definition for an empty rbtree with designator @a name.
   */
#define RBTREE_DEFINE_EMPTY(name) \
  RBTree_Control name = RBTREE_INITIALIZER_EMPTY(name)

  /**
   *  @brief RBTree_Node initializer for an empty node with designator @a name.
   */
#define RBTREE_NODE_INITIALIZER_EMPTY(name) \
  { \
    .parent = NULL, \
    .child[0] = NULL, \
    .child[1] = NULL, \
    .value = -1, \
    RBT_RED \
  }

  /**
   *  @brief RBTree definition for an empty rbtree with designator @a name.
   */
#define RBTREE_NODE_DEFINE_EMPTY(name) \
  RBTree_Node name = RBTREE_NODE_INITIALIZER_EMPTY(name)




  /**
   *  @brief Initialize a RBTree Header
   *
   *  This routine initializes @a the_rbtree structure to manage the
   *  contiguous array of @a number_nodes nodes which starts at
   *  @a starting_address.  Each node is of @a node_size bytes.
   */
  void _RBTree_Initialize(
      RBTree_Control *the_rbtree,
      void          *starting_address,
      size_t         number_nodes,
      size_t         node_size
      );

  /**
   *  @brief Obtain the min or max node of a rbtree
   *
   *  This function removes the min or max node from @a the_rbtree and returns
   *  a pointer to that node.  If @a the_rbtree is empty, then NULL is returned.
   *  @a dir specifies whether to return the min (0) or max (1).
   *
   *  @return This method returns a pointer to a node.  If a node was removed,
   *          then a pointer to that node is returned.  If @a the_rbtree was
   *          empty, then NULL is returned.
   *
   *  @note It disables interrupts to ensure the atomicity of the get operation.
   */
  RBTree_Node *_RBTree_Get(
      RBTree_Control *the_rbtree,
      RBTree_Direction dir
      );

  /**
   *  @brief Check the min or max node on a rbtree
   *
   *  This function returns a pointer to the min or max node of @a the_rbtree.
   *  If @a the_rbtree is empty, then NULL is returned. @a dir specifies 
   *  whether to return the min (0) or max (1).
   *
   *  @return This method returns a pointer to a node.
   *          If @a the_rbtree was empty, then NULL is returned.
   *
   *  @note It disables interrupts to ensure the atomicity of the get operation.
   */
  RBTree_Node *_RBTree_Peek(
      RBTree_Control *the_rbtree,
      RBTree_Direction dir
      );

  /** @brief Find the node with given value in the tree
   *
   *  This function returns a pointer to the node with value equal to @a value 
   *  if it exists in the Red-Black Tree @a the_rbtree, and NULL if not. 
   */
  RBTree_Node *_RBTree_Find(
      RBTree_Control *the_rbtree, 
      unsigned int value
      );

  /** @brief Find the control structure of the tree containing the given node 
   *
   *  This function returns a pointer to the control structure of the tree 
   *  containing @a the_node, if it exists, and NULL if not. 
   */
  RBTree_Control *_RBTree_Find_header(
      RBTree_Node *the_node
      );

  /** @brief Insert a Node (unprotected)
   *
   *  This routine inserts @a the_node on the Red-Black Tree @a the_rbtree.
   *
   *  @retval 0 Successfully inserted.
   *  @retval -1 NULL @a the_node.
   *  @retval RBTree_Node* if one with equal value to @a the_node->value exists 
   *          in @a the_rbtree.
   *
   *  @note It does NOT disable interrupts to ensure the atomicity
   *        of the extract operation.
   */
  RBTree_Node *_RBTree_Insert_unprotected(
      RBTree_Control *the_rbtree,
      RBTree_Node *the_node
  );

  /**
   *  @brief Insert a node on a rbtree
   *
   *  This routine inserts @a the_node on the tree @a the_rbtree.
   *
   *  @note It disables interrupts to ensure the atomicity
   *  of the extract operation.
   */
  void _RBTree_Insert(
      RBTree_Control *the_rbtree,
      RBTree_Node *the_node
      );


  /** @brief Extract a Node (unprotected)
   *
   *  This routine extracts (removes) @a the_node from @a the_rbtree.
   *
   *  @note It does NOT disable interrupts to ensure the atomicity
   *        of the extract operation.
   */

  void _RBTree_Extract_unprotected(
      RBTree_Control *the_rbtree,
      RBTree_Node *the_node
  );


  /**
   *  @brief Delete a node from the rbtree
   *
   *  This routine deletes @a the_node from @a the_rbtree.
   *
   *  @note It disables interrupts to ensure the atomicity of the
   *  append operation.
   */
  void _RBTree_Extract(
      RBTree_Control *the_rbtree,
      RBTree_Node    *the_node
      );

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/rbtree.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
