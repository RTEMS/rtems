/**
 * @file
 *
 * @ingroup ScoreChain
 *
 * @brief Chain Handler API.
 */

/*
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  COPYRIGHT (c) 1989-2006.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CHAIN_H
#define _RTEMS_SCORE_CHAIN_H

/**
 *  @defgroup ScoreChain Chain Handler
 *
 *  @ingroup Score
 *
 *  The Chain Handler is used to manage sets of entities.  This handler
 *  provides two data structures.  The Chain Node data structure is included
 *  as the first part of every data structure that will be placed on
 *  a chain.  The second data structure is Chain Control which is used
 *  to manage a set of Chain Nodes.
 */
/**@{*/

#ifdef __cplusplus
extern "C" {
#endif

#include <rtems/score/address.h>

/**
 *  @typedef Chain_Node
 *
 *  This type definition promotes the name for the Chain Node used by
 *  all RTEMS code.  It is a separate type definition because a forward
 *  reference is required to define it.  See @ref Chain_Node_struct for
 *  detailed information.
 */
typedef struct Chain_Node_struct Chain_Node;

/**
 *  @struct Chain_Node_struct
 *
 *  This is used to manage each element (node) which is placed
 *  on a chain.
 *
 *  @note Typically, a more complicated structure will use the
 *        chain package.  The more complicated structure will
 *        include a chain node as the first element in its
 *        control structure.  It will then call the chain package
 *        with a pointer to that node element.  The node pointer
 *        and the higher level structure start at the same address
 *        so the user can cast the pointers back and forth.
 *
 */
struct Chain_Node_struct {
  /** This points to the node after this one on this chain. */
  Chain_Node *next;
  /** This points to the node immediate prior to this one on this chain. */
  Chain_Node *previous;
};

/**
 *  @struct Chain_Control
 *
 * This is used to manage a chain.  A chain consists of a doubly
 * linked list of zero or more nodes.
 *
 * @note This implementation does not require special checks for
 *   manipulating the first and last elements on the chain.
 *   To accomplish this the @a Chain_Control structure is
 *   treated as two overlapping @ref Chain_Node structures.
 */
typedef union {
  struct {
    Chain_Node Node;
    Chain_Node *fill;
  } Head;

  struct {
    Chain_Node *fill;
    Chain_Node Node;
  } Tail;
} Chain_Control;

/**
 *  @brief Chain initializer for an empty chain with designator @a name.
 */
#define CHAIN_INITIALIZER_EMPTY(name) \
  { { { &(name).Tail.Node, NULL }, &(name).Head.Node } }

/**
 *  @brief Chain definition for an empty chain with designator @a name.
 */
#define CHAIN_DEFINE_EMPTY(name) \
  Chain_Control name = CHAIN_INITIALIZER_EMPTY(name)

/**
 *  @brief Initialize a Chain Header
 *
 *  This routine initializes @a the_chain structure to manage the
 *  contiguous array of @a number_nodes nodes which starts at
 *  @a starting_address.  Each node is of @a node_size bytes.
 *
 *  @param[in] the_chain specifies the chain to initialize
 *  @param[in] starting_address is the starting address of the array
 *         of elements
 *  @param[in] number_nodes is the numebr of nodes that will be in the chain
 *  @param[in] node_size is the size of each node
 */
void _Chain_Initialize(
  Chain_Control *the_chain,
  void          *starting_address,
  size_t         number_nodes,
  size_t         node_size
);

/**
 *  @brief Extract the specified node from a chain
 *
 *  This routine extracts @a the_node from the chain on which it resides.
 *  It disables interrupts to ensure the atomicity of the
 *  extract operation.
 *
 *  @arg the_node specifies the node to extract
 */
void _Chain_Extract(
  Chain_Node *the_node
);

/**
 *  @brief Obtain the first node on a chain
 *
 *  This function removes the first node from @a the_chain and returns
 *  a pointer to that node.  If @a the_chain is empty, then NULL is returned.
 *
 *  @return This method returns a pointer a node.  If a node was removed,
 *          then a pointer to that node is returned.  If @a the_chain was
 *          empty, then NULL is returned.
 *
 *  @note It disables interrupts to ensure the atomicity of the get operation.
 */
Chain_Node *_Chain_Get(
  Chain_Control *the_chain
);

/**
 *  @brief Insert a node on a chain
 *
 *  This routine inserts @a the_node on a chain immediately following
 *  @a after_node.
 *
 *  @note It disables interrupts to ensure the atomicity
 *  of the extract operation.
 */
void _Chain_Insert(
  Chain_Node *after_node,
  Chain_Node *the_node
);

/**
 *  @brief Append a node on the end of a chain
 *
 *  This routine appends @a the_node onto the end of @a the_chain.
 *
 *  @note It disables interrupts to ensure the atomicity of the
 *  append operation.
 */
void _Chain_Append(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/**
 * @brief Append a node and check if the chain was empty before.
 *
 * This routine appends the_node onto the end of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be appended.
 *
 * @note It disables interrupts to ensure the atomicity of the append
 * operation.
 *
 * @retval true The chain was empty before.
 * @retval false The chain contained at least one node before.
 */
bool _Chain_Append_with_empty_check(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/**
 * @brief Prepend a node and check if the chain was empty before.
 *
 * This routine prepends the_node onto the front of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be prepended.
 *
 * @note It disables interrupts to ensure the atomicity of the append
 * operation.
 *
 * @retval true The chain was empty before.
 * @retval false The chain contained at least one node before.
 */
bool _Chain_Prepend_with_empty_check(
  Chain_Control *the_chain,
  Chain_Node    *the_node
);

/**
 * @brief Get the first node and check if the chain is empty afterwards.
 *
 * This function removes the first node from the_chain and returns
 * a pointer to that node in @a the_node.  If the_chain is empty, then NULL is
 * returned.
 *
 * @param[in] the_chain is the chain to attempt to get the first node from.
 * @param[out] the_node is the first node on the chain or NULL if the chain is
 * empty.
 *
 * @note It disables interrupts to ensure the atomicity of the append
 * operation.
 *
 * @retval true The chain is empty now.
 * @retval false The chain contains at least one node now.
 */
bool _Chain_Get_with_empty_check(
  Chain_Control *the_chain,
  Chain_Node **the_node
);

#ifndef __RTEMS_APPLICATION__
#include <rtems/score/chain.inl>
#endif

#ifdef __cplusplus
}
#endif

/**@}*/

#endif
/* end of include file */
