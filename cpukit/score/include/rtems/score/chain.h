/**
 *  @file  rtems/score/chain.h
 *
 *  This include file contains all the constants and structures associated
 *  with the Doubly-Linked Chain Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2004.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __RTEMS_CHAIN_h
#define __RTEMS_CHAIN_h

/**
 *  @defgroup ScoreChain Chain Handler
 *
 *  The Chain Handler contains XXX
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
 *  reference is required to define it.
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
  /** This points to the node after to this one on this chain. */
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
 *   The permanent head of the chain overlays a node structure on the
 *   @a first and @a permanent_null fields.  The permanent tail
 *   of the chain overlays a node structure on the
 *   @a permanent_null and @a last elements of the structure.
 *
 */
typedef struct {
  /** This points to the first node on this chain. */
  Chain_Node *first;
  /** This field is always 0. */
  Chain_Node *permanent_null;
  /** This points to the last node on this chain. */
  Chain_Node *last;
} Chain_Control;

/**
 *  @brief Initialize a Chain Header
 *
 *  This routine initializes @a the_chain structure to manage the
 *  contiguous array of @a number_nodes nodes which starts at
 *  @a starting_address.  Each node is of @a node_size bytes.
 *
 *  @param the_chain (in) specifies the chain to initialize
 *  @param starting_address (in) is the starting address of the array
 *         of elements
 *  @param number_nodes (in) is the numebr of nodes that will be in the chain
 *  @param node_size (in) is the size of each node
 */
void _Chain_Initialize(
  Chain_Control *the_chain,
  void          *starting_address,
  uint32_t       number_nodes,
  uint32_t       node_size
);

#ifndef RTEMS_INLINES
/**
 *  @brief Get the first node (do not disable interrupts)
 *
 *  This method attempts to obtain the first node from \a the_chain.
 *
 *  @param the_chain (in) points to the chain to operate upon
 *  @return If successful, a chain node is returned.  Otherwise, NULL
 *  is returned.
 */
Chain_Node *_Chain_Get_first_unprotected(
  Chain_Control *the_chain
);
#endif

/**
 *  @brief Extract the specified node from a chain
 *
 *  This routine extracts \a the_node from the chain on which it resides.
 *  It disables interrupts to insure the atomicity of the
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
 *  This function removes the first node from \a the_chain and returns
 *  a pointer to that node.  If \a the_chain is empty, then NULL is returned.
 *
 *  @note It disables interrupts to insure the atomicity of the
 *  get operation.
 */
Chain_Node *_Chain_Get(
  Chain_Control *the_chain
);

/**
 *  @brief Insert a node on a chain
 *
 *  This routine inserts \a the_node on a chain immediately following
 *  \a after_node.  
 *
 *  @note It disables interrupts to insure the atomicity
 *  of the extract operation.
 */
void _Chain_Insert(
  Chain_Node *after_node,
  Chain_Node *the_node
);

/**
 *  @brief Append a node on the end of a chain
 *
 *  This routine appends \a the_node onto the end of \a the_chain.
 *
 *  @note It disables interrupts to insure the atomicity of the
 *  append operation.
 */
void _Chain_Append(
  Chain_Control *the_chain,
  Chain_Node    *the_node
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
