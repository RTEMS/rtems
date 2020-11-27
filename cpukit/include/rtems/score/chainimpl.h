/**
 * @file
 *
 * @ingroup RTEMSScoreChain
 *
 * @brief This header file provides interfaces of the
 *   @ref RTEMSScoreChain which are only used by the implementation.
 */

/*
 *  Copyright (c) 2010 embedded brains GmbH.
 *
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CHAINIMPL_H
#define _RTEMS_SCORE_CHAINIMPL_H

#include <rtems/score/chain.h>
#include <rtems/score/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSScoreChain
 *
 * @{
 */

/**
 *  @brief Chain initializer for an empty chain with designator @a name.
 */
#define CHAIN_INITIALIZER_EMPTY(name) \
  { { { &(name).Tail.Node, NULL }, &(name).Head.Node } }

/**
 *  @brief Chain initializer for a chain with one @a node.
 *
 *  @see CHAIN_NODE_INITIALIZER_ONE_NODE_CHAIN().
 */
#define CHAIN_INITIALIZER_ONE_NODE( node ) \
  { { { (node), NULL }, (node) } }

/**
 *  @brief Chain node initializer for a @a chain containing exactly this node.
 *
 *  @see CHAIN_INITIALIZER_ONE_NODE().
 */
#define CHAIN_NODE_INITIALIZER_ONE_NODE_CHAIN( chain ) \
  { &(chain)->Tail.Node, &(chain)->Head.Node }

/**
 *  @brief Chain definition for an empty chain with designator @a name.
 */
#define CHAIN_DEFINE_EMPTY(name) \
  Chain_Control name = CHAIN_INITIALIZER_EMPTY(name)

/**
 *  @brief Initializes a chain header.
 *
 *  This routine initializes @a the_chain structure to manage the
 *  contiguous array of @a number_nodes nodes which starts at
 *  @a starting_address.  Each node is of @a node_size bytes.
 *
 *  @param[out] the_chain Specifies the chain to initialize.
 *  @param starting_address The starting address of the array
 *         of elements.
 *  @param number_nodes The number of nodes that will be in the chain.
 *  @param node_size The size of each node.
 */
void _Chain_Initialize(
  Chain_Control *the_chain,
  void          *starting_address,
  size_t         number_nodes,
  size_t         node_size
);

/**
 * @brief Returns the node count of the chain.
 *
 * @param chain The chain to return the node count from.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 * operation.
 *
 * @return The node count of the chain.
 */
size_t _Chain_Node_count_unprotected( const Chain_Control *chain );

/**
 * @brief Sets off chain.
 *
 * This function sets the next field of the @a node to NULL indicating the @a
 * node is not part of a chain.
 *
 * @param[out] node The node to set off chain.
 */
RTEMS_INLINE_ROUTINE void _Chain_Set_off_chain(
  Chain_Node *node
)
{
  node->next = NULL;
#if defined(RTEMS_DEBUG)
  node->previous = NULL;
#endif
}

/**
 * @brief Initializes a chain node.
 *
 * In debug configurations, the node is set off chain.  In all other
 * configurations, this function does nothing.
 *
 * @param[out] the_node The chain node to initialize.
 */
RTEMS_INLINE_ROUTINE void _Chain_Initialize_node( Chain_Node *the_node )
{
#if defined(RTEMS_DEBUG)
  _Chain_Set_off_chain( the_node );
#else
  (void) the_node;
#endif
}

/**
 * @brief Checks if the node is off chain.
 *
 * This function returns true if the @a node is not on a chain.  A @a node is
 * off chain if the next field is set to NULL.
 *
 * @param node The node to check if it is off chain.
 *
 * @retval true The @a node is off chain.
 * @retval false The @a node is not off chain.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_node_off_chain(
  const Chain_Node *node
)
{
  return node->next == NULL;
}

/**
 * @brief Checks if two nodes are equal.
 *
 * This function returns true if @a left and @a right are equal,
 * and false otherwise.
 *
 * @param left The node on the left hand side of the comparison.
 * @param right The node on the right hand side of the comparison.
 *
 * @retval true @a left and @a right are equal.
 * @retval false @a left and @a right are not equal.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Are_nodes_equal(
  const Chain_Node *left,
  const Chain_Node *right
)
{
  return left == right;
}

/**
 * @brief Returns pointer to chain head.
 *
 * This function returns a pointer to the head node on the chain.
 *
 * @param[in] the_chain The chain to be operated upon.
 *
 * @return This method returns the permanent head node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Head(
  Chain_Control *the_chain
)
{
  return &the_chain->Head.Node;
}

/**
 * @brief Returns pointer to immutable chain head.
 *
 * This function returns a pointer to the head node on the chain.
 *
 * @param the_chain The chain to be operated upon.
 *
 * @return This method returns the permanent head node of the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_head(
  const Chain_Control *the_chain
)
{
  return &the_chain->Head.Node;
}

/**
 * @brief Returns pointer to chain tail.
 *
 * This function returns a pointer to the tail node on the chain.
 *
 * @param[in] the_chain The chain to be operated upon.
 *
 * @return This method returns the permanent tail node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Tail(
  Chain_Control *the_chain
)
{
  return &the_chain->Tail.Node;
}

/**
 * @brief Returns pointer to immutable chain tail.
 *
 * This function returns a pointer to the tail node on the chain.
 *
 * @param the_chain The chain to be operated upon.
 *
 * @return This method returns the permanent tail node of the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_tail(
  const Chain_Control *the_chain
)
{
  return &the_chain->Tail.Node;
}

/**
 * @brief Returns pointer to chain's first node.
 *
 * This function returns a pointer to the first node on the chain after the
 * head.
 *
 * @param the_chain The chain to be operated upon.
 *
 * @return This method returns the first node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_First(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_head( the_chain )->next;
}

/**
 * @brief Returns pointer to immutable chain's first node.
 *
 * This function returns a pointer to the first node on the chain after the
 * head.
 *
 * @param the_chain The chain to be operated upon.
 *
 * @return This method returns the first node of the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_first(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_head( the_chain )->next;
}

/**
 * @brief Returns pointer to chain's last node.
 *
 * This function returns a pointer to the last node on the chain just before
 * the tail.
 *
 * @param the_chain The chain to be operated upon.
 *
 * @return This method returns the last node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Last(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_tail( the_chain )->previous;
}

/**
 * @brief Returns pointer to immutable chain's last node.
 *
 * This function returns a pointer to the last node on the chain just before
 * the tail.
 *
 * @param the_chain The chain to be operated upon.
 *
 * @return This method returns the last node of the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_last(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_tail( the_chain )->previous;
}

/**
 * @brief Returns pointer to the next node from this node.
 *
 * This function returns a pointer to the next node after this node.
 *
 * @param the_node The node to be operated upon.
 *
 * @return This method returns the next node on the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Next(
  const Chain_Node *the_node
)
{
  return the_node->next;
}

/**
 * @brief Returns pointer to the immutable next node from this node.
 *
 * This function returns a pointer to the next node after this node.
 *
 * @param the_node The node to be operated upon.
 *
 * @return This method returns the next node on the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_next(
  const Chain_Node *the_node
)
{
  return the_node->next;
}

/**
 * @brief Returns pointer to the previous node from this node.
 *
 * This function returns a pointer to the previous node on this chain.
 *
 * @param the_node The node to be operated upon.
 *
 * @return This method returns the previous node on the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Previous(
  const Chain_Node *the_node
)
{
  return the_node->previous;
}

/**
 * @brief Returns pointer to the immutable previous node from this node.
 *
 * This function returns a pointer to the previous node on this chain.
 *
 * @param the_node The node to be operated upon.
 *
 * @return This method returns the previous node on the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_previous(
  const Chain_Node *the_node
)
{
  return the_node->previous;
}

/**
 * @brief Checks if the chain is empty.
 *
 * This function returns true if there are no nodes on @a the_chain and
 * false otherwise.
 *
 * @param the_chain The chain to check if it is empty.
 *
 * @retval true There are no nodes on @a the_chain.
 * @retval false There are nodes on @a the_chain.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_empty(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_first( the_chain )
    == _Chain_Immutable_tail( the_chain );
}

/**
 * @brief Checks if this is the first node on the chain.
 *
 * This function returns true if the_node is the first node on a chain and
 * false otherwise.
 *
 * @param the_node The node of which the caller wants to know if it is
 *            the first node on a chain.
 *
 * @retval true @a the_node is the first node on a chain.
 * @retval false @a the_node is not the first node on a chain.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_first(
  const Chain_Node *the_node
)
{
  return (the_node->previous->previous == NULL);
}

/**
 * @brief Checks if this is the last node on the chain.
 *
 * This function returns true if @a the_node is the last node on a chain and
 * false otherwise.
 *
 * @param the_node The node of which the caller wants to know if it is
 *            the last node on a chain.
 *
 * @retval true @a the_node is the last node on a chain.
 * @retval false @a the_node is not the last node on a chain.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_last(
  const Chain_Node *the_node
)
{
  return (the_node->next->next == NULL);
}

/**
 * @brief Checks if this chain has only one node.
 *
 * This function returns true if there is only one node on @a the_chain and
 * false otherwise.
 *
 * @param the_chain is the chain to be operated upon.
 *
 * @retval true There is only one node on @a the_chain.
 * @retval false There is more than one node on @a the_chain.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Has_only_one_node(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_first( the_chain )
    == _Chain_Immutable_last( the_chain );
}

/**
 * @brief Checks if this node is the chain head.
 *
 * This function returns true if @a the_node is the head of @a the_chain and
 * false otherwise.
 *
 * @param the_chain The chain to be operated upon.
 * @param the_node The node to check for being the Chain Head.
 *
 * @retval true @a the_node is the head of @a the_chain.
 * @retval false @a the_node is not the head of @a the_chain.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_head(
  const Chain_Control *the_chain,
  const Chain_Node    *the_node
)
{
  return (the_node == _Chain_Immutable_head( the_chain ));
}

/**
 * @brief Checks if this node is the chain tail.
 *
 * This function returns true if @a the_node is the tail of @a the_chain and
 * false otherwise.
 *
 * @param the_chain The chain to be operated upon.
 * @param the_node The node to check for being the Chain Tail.
 *
 * @retval true @a the_node is the tail of @a the_chain.
 * @retval false @a the_node is not the tail of @a the_chain.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_tail(
  const Chain_Control *the_chain,
  const Chain_Node    *the_node
)
{
  return (the_node == _Chain_Immutable_tail( the_chain ));
}

/**
 * @brief Initializes this chain as empty.
 *
 * This routine initializes the specified chain to contain zero nodes.
 *
 * @param[out] the_chain The chain to be initialized.
 */
RTEMS_INLINE_ROUTINE void _Chain_Initialize_empty(
  Chain_Control *the_chain
)
{
  Chain_Node *head;
  Chain_Node *tail;

  _Assert( the_chain != NULL );

  head = _Chain_Head( the_chain );
  tail = _Chain_Tail( the_chain );

  head->next = tail;
  head->previous = NULL;
  tail->previous = head;
}

/**
 * @brief Initializes this chain to contain exactly the specified node.
 *
 * @param[out] the_chain The chain to be initialized to contain exactly the specified node.
 * @param[out] the_node The one and only node of the chain to be initialized.
 */
RTEMS_INLINE_ROUTINE void _Chain_Initialize_one(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  Chain_Node *head;
  Chain_Node *tail;

  _Assert( _Chain_Is_node_off_chain( the_node ) );

  head = _Chain_Head( the_chain );
  tail = _Chain_Tail( the_chain );

  the_node->next = tail;
  the_node->previous = head;

  head->next = the_node;
  head->previous = NULL;
  tail->previous = the_node;
}

/**
 * @brief Extracts this node (unprotected).
 *
 * This routine extracts the_node from the chain on which it resides.
 * It does NOT disable interrupts to ensure the atomicity of the
 * extract operation.
 *
 * @param[out] the_node The node to be extracted.
 */
RTEMS_INLINE_ROUTINE void _Chain_Extract_unprotected(
  Chain_Node *the_node
)
{
  Chain_Node *next;
  Chain_Node *previous;

  _Assert( !_Chain_Is_node_off_chain( the_node ) );

  next           = the_node->next;
  previous       = the_node->previous;
  next->previous = previous;
  previous->next = next;

#if defined(RTEMS_DEBUG)
  _Chain_Set_off_chain( the_node );
#endif
}

/**
 * @brief Gets the first node (unprotected).
 *
 * This function removes the first node from the_chain and returns
 * a pointer to that node.  It does NOT disable interrupts to ensure
 * the atomicity of the get operation.
 *
 * @param[in, out] the_chain The chain to attempt to get the first node from.
 *
 * @return This method returns the first node on the chain even if it is
 *         the Chain Tail.
 *
 * @note This routine assumes that there is at least one node on the chain
 *       and always returns a node even if it is the Chain Tail.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Get_first_unprotected(
  Chain_Control *the_chain
)
{
  Chain_Node *head;
  Chain_Node *old_first;
  Chain_Node *new_first;

  _Assert( !_Chain_Is_empty( the_chain ) );

  head = _Chain_Head( the_chain );
  old_first = head->next;
  new_first = old_first->next;

  head->next = new_first;
  new_first->previous = head;

#if defined(RTEMS_DEBUG)
  _Chain_Set_off_chain( old_first );
#endif

  return old_first;
}

/**
 * @brief Gets the first node (unprotected).
 *
 * This function removes the first node from the_chain and returns
 * a pointer to that node.  If the_chain is empty, then NULL is returned.
 *
 * @param[in, out] the_chain The chain to attempt to get the first node from.
 *
 * @retval pointer Pointer to the first node.  The chain contained at least one node.
 * @retval NULL The chain is empty.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 *       get operation.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Get_unprotected(
  Chain_Control *the_chain
)
{
  if ( !_Chain_Is_empty(the_chain))
    return _Chain_Get_first_unprotected(the_chain);
  else
    return NULL;
}

/**
 * @brief Inserts a node (unprotected).
 *
 * This routine inserts the_node on a chain immediately following
 * after_node.
 *
 * @param[in, out] after_node The node which will precede @a the_node on the
 *            chain.
 * @param[out] the_node The node to be inserted after @a after_node.
 *
 * @note It does NOT disable interrupts to ensure the atomicity
 *       of the extract operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Insert_unprotected(
  Chain_Node *after_node,
  Chain_Node *the_node
)
{
  Chain_Node *before_node;

  _Assert( _Chain_Is_node_off_chain( the_node ) );

  the_node->previous    = after_node;
  before_node           = after_node->next;
  after_node->next      = the_node;
  the_node->next        = before_node;
  before_node->previous = the_node;
}

/**
 * @brief Appends a node (unprotected).
 *
 * This routine appends the_node onto the end of the_chain.
 *
 * @param[in, out] the_chain The chain to be operated upon.
 * @param[out] the_node The node to be appended to the end of @a the_chain.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 *       append operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Append_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  Chain_Node *tail;
  Chain_Node *old_last;

  _Assert( _Chain_Is_node_off_chain( the_node ) );

  tail = _Chain_Tail( the_chain );
  old_last = tail->previous;

  the_node->next = tail;
  tail->previous = the_node;
  old_last->next = the_node;
  the_node->previous = old_last;
}

/**
 * @brief Appends a node on the end of a chain if the node is in the off chain
 * state (unprotected).
 *
 * @param[in, out] the_chain The chain to be operated upon.
 * @param[in, out] the_node The node to be appended if it is in the off chain.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 *       append operation.
 *
 * @see _Chain_Append_unprotected() and _Chain_Is_node_off_chain().
 */
RTEMS_INLINE_ROUTINE void _Chain_Append_if_is_off_chain_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  if ( _Chain_Is_node_off_chain( the_node ) ) {
    _Chain_Append_unprotected( the_chain, the_node );
  }
}

/**
 * @brief Prepends a node (unprotected).
 *
 * This routine prepends the_node onto the front of the_chain.
 *
 * @param[in, out] the_chain The chain to be operated upon.
 * @param[in, out] the_node The node to be prepended to the front of @a the_chain.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 *       prepend operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Prepend_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  _Chain_Insert_unprotected(_Chain_Head(the_chain), the_node);
}

/**
 * @brief Appends a node and checks if the chain was empty before (unprotected).
 *
 * This routine appends the_node onto the end of the_chain.
 *
 * @param[in, out] the_chain The chain to be operated upon.
 * @param[out] the_node The node to be appended to the end of @a the_chain.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 *       append operation.
 *
 * @retval true The chain was empty before.
 * @retval false The chain contained at least one node before.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Append_with_empty_check_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  bool was_empty = _Chain_Is_empty( the_chain );

  _Chain_Append_unprotected( the_chain, the_node );

  return was_empty;
}

/**
 * @brief Prepends a node and checks if the chain was empty before (unprotected).
 *
 * This routine prepends the_node onto the front of the_chain.
 *
 * @param[in, out] the_chain The chain to be operated upon.
 * @param[out] the_node The node to be prepended to the front of @a the_chain.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 *       prepend operation.
 *
 * @retval true The chain was empty before.
 * @retval false The chain contained at least one node before.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Prepend_with_empty_check_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  bool was_empty = _Chain_Is_empty( the_chain );

  _Chain_Prepend_unprotected( the_chain, the_node );

  return was_empty;
}

/**
 * @brief Gets the first node and checks if the chain is empty afterwards
 * (unprotected).
 *
 * This function removes the first node from the_chain and returns
 * a pointer to that node in @a the_node.  If the_chain is empty, then NULL is
 * returned.
 *
 * @param[in, out] the_chain The chain to attempt to get the first node from.
 * @param[out] the_node The first node on the chain or NULL if the chain is
 * empty.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 *       get operation.
 *
 * @retval true The chain is empty now.
 * @retval false The chain contains at least one node now.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Get_with_empty_check_unprotected(
  Chain_Control *the_chain,
  Chain_Node **the_node
)
{
  bool is_empty_now = true;
  Chain_Node *head = _Chain_Head( the_chain );
  Chain_Node *tail = _Chain_Tail( the_chain );
  Chain_Node *old_first = head->next;

  if ( old_first != tail ) {
    Chain_Node *new_first = old_first->next;

    head->next = new_first;
    new_first->previous = head;

    *the_node = old_first;

    is_empty_now = new_first == tail;
  } else
    *the_node = NULL;

  return is_empty_now;
}

/**
 * @brief Chain node order.
 *
 * @param left The left hand side.
 * @param right The right hand side.
 *
 * @retval true According to the order the left node precedes the right node.
 * @retval false Otherwise.
 */
typedef bool ( *Chain_Node_order )(
  const void       *left,
  const Chain_Node *right
);

/**
 * @brief Inserts a node into the chain according to the order relation.
 *
 * After the operation the chain contains the node to insert and the order
 * relation holds for all nodes from the head up to the inserted node.  Nodes
 * after the inserted node are not moved.
 *
 * @param[in, out] the_chain The chain to be operated upon.
 * @param[out] to_insert The node to insert.
 * @param left The left hand side passed to the order relation.  It must
 *   correspond to the node to insert.  The separate left hand side parameter
 *   may help the compiler to generate better code if it is stored in a local
 *   variable.
 * @param order The order relation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Insert_ordered_unprotected(
  Chain_Control    *the_chain,
  Chain_Node       *to_insert,
  const void       *left,
  Chain_Node_order  order
)
{
  const Chain_Node *tail = _Chain_Immutable_tail( the_chain );
  Chain_Node *next = _Chain_First( the_chain );

  while ( next != tail && !( *order )( left, next ) ) {
    next = _Chain_Next( next );
  }

  _Chain_Insert_unprotected( _Chain_Previous( next ), to_insert );
}

/**
 * @brief The chain iterator direction.
 */
typedef enum {
  /**
   * @brief Iteration from head to tail.
   */
  CHAIN_ITERATOR_FORWARD,

  /**
   * @brief Iteration from tail to head.
   */
  CHAIN_ITERATOR_BACKWARD
} Chain_Iterator_direction;

/**
 * @brief A chain iterator which is updated during node extraction if it is
 * properly registered.
 *
 * @see _Chain_Iterator_initialize().
 */
typedef struct {
  /**
   * @brief Node for registration.
   *
   * Used during _Chain_Iterator_initialize() and _Chain_Iterator_destroy().
   */
  Chain_Node Registry_node;

  /**
   * @brief The direction of this iterator.
   *
   * Immutable after initialization via _Chain_Iterator_initialize().
   */
  Chain_Iterator_direction  direction;

  /**
   * @brief The current position of this iterator.
   *
   * The position is initialized via _Chain_Iterator_initialize().  It must be
   * explicitly set after one valid iteration step, e.g. in case a next node in
   * the iterator direction existed.  It is updated through the registration in
   * case a node is extracted via _Chain_Iterator_registry_update().
   */
  Chain_Node *position;
} Chain_Iterator;

/**
 * @brief A registry for chain iterators.
 *
 * Should be attached to a chain control to enable safe iteration through a
 * chain in case of concurrent node extractions.
 */
typedef struct {
  Chain_Control Iterators;
} Chain_Iterator_registry;

/**
 * @brief Chain iterator registry initializer for static initialization.
 *
 * @param name The designator of the chain iterator registry.
 */
#define CHAIN_ITERATOR_REGISTRY_INITIALIZER( name ) \
  { CHAIN_INITIALIZER_EMPTY( name.Iterators ) }

/**
 * @brief Initializes a chain iterator registry.
 *
 * @param[out] the_registry The chain iterator registry to be initialized.
 */
RTEMS_INLINE_ROUTINE void _Chain_Iterator_registry_initialize(
  Chain_Iterator_registry *the_registry
)
{
  _Chain_Initialize_empty( &the_registry->Iterators );
}

/**
 * @brief Updates all iterators present in the chain iterator registry in case
 * of a node extraction.
 *
 * Must be called before _Chain_Extract_unprotected().
 *
 * @warning This function will look at all registered chain iterators to
 *   determine if an update is necessary.
 *
 * @param[in, out] the_registry the chain iterator registry.
 * @param[out] the_node_to_extract The node that will be extracted.
 */
RTEMS_INLINE_ROUTINE void _Chain_Iterator_registry_update(
  Chain_Iterator_registry *the_registry,
  Chain_Node              *the_node_to_extract
)
{
  Chain_Node *iter_node;
  Chain_Node *iter_tail;

  iter_node = _Chain_Head( &the_registry->Iterators );
  iter_tail = _Chain_Tail( &the_registry->Iterators );

  while ( ( iter_node = _Chain_Next( iter_node ) ) != iter_tail ) {
    Chain_Iterator *iter;

    iter = (Chain_Iterator *) iter_node;

    if ( iter->position == the_node_to_extract ) {
      if ( iter->direction == CHAIN_ITERATOR_FORWARD ) {
        iter->position = _Chain_Previous( the_node_to_extract );
      } else {
        iter->position = _Chain_Next( the_node_to_extract );
      }
    }
  }
}

/**
 * @brief Initializes the chain iterator.
 *
 * In the following example nodes inserted during the iteration are visited in
 * case they are inserted after the current position in iteration order.
 *
 * @code
 * #include <rtems/score/chainimpl.h>
 * #include <rtems/score/isrlock.h>
 *
 * typedef struct {
 *   Chain_Control           Chain;
 *   Chain_Iterator_registry Iterators;
 *   ISR_LOCK_MEMBER(        Lock )
 * } Some_Control;
 *
 * void iterate(
 *   Some_Control   *the_some,
 *   void         ( *visitor )( Chain_Node * )
 * )
 * {
 *   ISR_lock_Context  lock_context;
 *   Chain_Iterator    iter;
 *   Chain_Node       *node;
 *   const Chain_Node *end;
 *
 *   end = _Chain_Immutable_tail( &the_some->Chain );
 *
 *   _ISR_lock_ISR_disable_and_acquire( &the_some->Lock, &lock_context );
 *
 *   _Chain_Iterator_initialize(
 *     &the_some->Chain,
 *     &the_some->Iterators,
 *     &iter,
 *     CHAIN_ITERATOR_FORWARD
 *   );
 *
 *   while ( ( node = _Chain_Iterator_next( &iter ) ) != end ) {
 *     _Chain_Iterator_set_position( &iter, node );
 *     _ISR_lock_Release_and_ISR_enable( &the_some->Lock, &lock_context );
 *     ( *visitor )( node );
 *     _ISR_lock_ISR_disable_and_acquire( &the_some->Lock, &lock_context );
 *   }
 *
 *   _Chain_Iterator_destroy( &iter );
 *   _ISR_lock_Release_and_ISR_enable( &the_some->Lock, &lock_context );
 * }
 * @endcode
 *
 * @param[out] the_chain The chain to iterate.
 * @param[in, out] the_registry The registry for the chain iterator.
 * @param[out] the_iterator The chain iterator to initialize.
 * @param[out] direction The iteration direction.
 *
 * @see _Chain_Iterator_next(), _Chain_Iterator_set_position() and
 * Chain_Iterator_destroy().
 *
 * @warning Think twice before you use a chain iterator.  Its current
 *   implementation is unfit for use in performance relevant components, due to
 *   the linear time complexity in _Chain_Iterator_registry_update().
 */
RTEMS_INLINE_ROUTINE void _Chain_Iterator_initialize(
  Chain_Control            *the_chain,
  Chain_Iterator_registry  *the_registry,
  Chain_Iterator           *the_iterator,
  Chain_Iterator_direction  direction
)
{
  _Chain_Initialize_node( &the_iterator->Registry_node );
  _Chain_Append_unprotected(
    &the_registry->Iterators,
    &the_iterator->Registry_node
  );

  the_iterator->direction = direction;

  if ( direction == CHAIN_ITERATOR_FORWARD ) {
    the_iterator->position = _Chain_Head( the_chain );
  } else {
    the_iterator->position = _Chain_Tail( the_chain );
  }
}

/**
 * @brief Returns the next node in the iterator direction.
 *
 * In case a next node exists, then the iterator should be updated via
 * _Chain_Iterator_set_position() to continue with the next iteration step.
 *
 * @param[in, out] the_iterator The chain iterator.
 *
 * @return The next node in the iterator direction
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Iterator_next(
  const Chain_Iterator *the_iterator
)
{
  if ( the_iterator->direction == CHAIN_ITERATOR_FORWARD ) {
    return _Chain_Next( the_iterator->position );
  } else {
    return _Chain_Previous( the_iterator->position );
  }
}

/**
 * @brief Sets the iterator position.
 *
 * @param[out] the_iterator The chain iterator.
 * @param[out] the_node The new iterator position.
 */
RTEMS_INLINE_ROUTINE void _Chain_Iterator_set_position(
  Chain_Iterator *the_iterator,
  Chain_Node     *the_node
)
{
  the_iterator->position = the_node;
}

/**
 * @brief Destroys the iterator.
 *
 * Removes the iterator from its registry.
 *
 * @param[out] the_iterator The chain iterator.
 */
RTEMS_INLINE_ROUTINE void _Chain_Iterator_destroy(
  Chain_Iterator *the_iterator
)
{
  _Chain_Extract_unprotected( &the_iterator->Registry_node );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
