/**
 * @file
 *
 * @brief Chain Handler API
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
#include <rtems/score/address.h>
#include <rtems/score/assert.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup ScoreChain
 */
/**@{**/

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
 *  @brief Initialize a chain header.
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
 *  @brief Extract the specified node from a chain.
 *
 *  This routine extracts @a the_node from the chain on which it resides.
 *  It disables interrupts to ensure the atomicity of the extract operation.
 *
 *  @param[in] the_node is the node to be extracted
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
void _Chain_Extract(
  Chain_Node *the_node
);

/**
 *  @brief Obtain the first node on a chain.
 *
 *  This function removes the first node from @a the_chain and returns
 *  a pointer to that node.  If @a the_chain is empty, then NULL is returned.
 *
 *  @retval This method returns a pointer a node.  If a node was removed,
 *          then a pointer to that node is returned.  If @a the_chain was
 *          empty, then NULL is returned.
 *
 *  @note It disables interrupts to ensure the atomicity of the get operation.
 */
Chain_Node *_Chain_Get(
  Chain_Control *the_chain
);

/**
 *  @brief Insert a node on a chain.
 *
 *  This routine inserts @a the_node on a chain immediately following
 *  @a after_node.
 *
 *  @param[in] after_node is the pointer to the node in chain to be
 *             inserted after
 *  @param[in] the_node is the pointer to the node to be inserted
 *
 *  @note It disables interrupts to ensure the atomicity
 *  of the insert operation.
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
void _Chain_Insert(
  Chain_Node *after_node,
  Chain_Node *the_node
);

/**
 *  @brief Append a node on the end of a chain.
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
 *
 *  - INTERRUPT LATENCY:
 *    + single case
 */
bool _Chain_Get_with_empty_check(
  Chain_Control *the_chain,
  Chain_Node **the_node
);

/**
 * @brief Returns the node count of the chain.
 *
 * @param[in] chain The chain.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 * operation.
 *
 * @retval The node count of the chain.
 */
size_t _Chain_Node_count_unprotected( const Chain_Control *chain );

/**
 * @brief Set off chain.
 *
 * This function sets the next field of the @a node to NULL indicating the @a
 * node is not part of a chain.
 *
 * @param[in] node the node set to off chain.
 */
RTEMS_INLINE_ROUTINE void _Chain_Set_off_chain(
  Chain_Node *node
)
{
  node->next = NULL;
}

/**
 * @brief Is the node off chain.
 *
 * This function returns true if the @a node is not on a chain.  A @a node is
 * off chain if the next field is set to NULL.
 *
 * @param[in] node is the node off chain.
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
 * @brief Are two nodes equal.
 *
 * This function returns true if @a left and @a right are equal,
 * and false otherwise.
 *
 * @param[in] left is the node on the left hand side of the comparison.
 * @param[in] right is the node on the left hand side of the comparison.
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
 * @brief Is the chain node pointer NULL.
 *
 * This function returns true if the_node is NULL and false otherwise.
 *
 * @param[in] the_node is the node pointer to check.
 *
 * @retval true @a the_node is @c NULL.
 * @retval false @a the_node is not @c NULL.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_null_node(
  const Chain_Node *the_node
)
{
  return (the_node == NULL);
}

/**
 * @brief Return pointer to chain head.
 *
 * This function returns a pointer to the head node on the chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
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
 * @brief Return pointer to immutable chain head.
 *
 * This function returns a pointer to the head node on the chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
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
 * @brief Return pointer to chain tail.
 *
 * This function returns a pointer to the tail node on the chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
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
 * @brief Return pointer to immutable chain tail.
 *
 * This function returns a pointer to the tail node on the chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
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
 * @brief Return pointer to chain's first node.
 *
 * This function returns a pointer to the first node on the chain after the
 * head.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the first node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_First(
  Chain_Control *the_chain
)
{
  return _Chain_Head( the_chain )->next;
}

/**
 * @brief Return pointer to immutable chain's first node.
 *
 * This function returns a pointer to the first node on the chain after the
 * head.
 *
 * @param[in] the_chain is the chain to be operated upon.
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
 * @brief Return pointer to chain's last node.
 *
 * This function returns a pointer to the last node on the chain just before
 * the tail.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the last node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Last(
  Chain_Control *the_chain
)
{
  return _Chain_Tail( the_chain )->previous;
}

/**
 * @brief Return pointer to immutable chain's last node.
 *
 * This function returns a pointer to the last node on the chain just before
 * the tail.
 *
 * @param[in] the_chain is the chain to be operated upon.
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
 * @brief Return pointer the next node from this node.
 *
 * This function returns a pointer to the next node after this node.
 *
 * @param[in] the_node is the node to be operated upon.
 *
 * @return This method returns the next node on the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Next(
  Chain_Node *the_node
)
{
  return the_node->next;
}

/**
 * @brief Return pointer the immutable next node from this node.
 *
 * This function returns a pointer to the next node after this node.
 *
 * @param[in] the_node is the node to be operated upon.
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
 * @brief Return pointer the previous node from this node.
 *
 * This function returns a pointer to the previous node on this chain.
 *
 * @param[in] the_node is the node to be operated upon.
 *
 * @return This method returns the previous node on the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Previous(
  Chain_Node *the_node
)
{
  return the_node->previous;
}

/**
 * @brief Return pointer the immutable previous node from this node.
 *
 * This function returns a pointer to the previous node on this chain.
 *
 * @param[in] the_node is the node to be operated upon.
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
 * @brief Is the chain empty.
 *
 * This function returns true if there a no nodes on @a the_chain and
 * false otherwise.
 *
 * @param[in] the_chain is the chain to be operated upon.
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
 * @brief Is this the first node on the chain.
 *
 * This function returns true if the_node is the first node on a chain and
 * false otherwise.
 *
 * @param[in] the_node is the node the caller wants to know if it is
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
 * @brief Is this the last node on the chain.
 *
 * This function returns true if @a the_node is the last node on a chain and
 * false otherwise.
 *
 * @param[in] the_node is the node to check as the last node.
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
 * @brief Does this chain have only one node.
 *
 * This function returns true if there is only one node on @a the_chain and
 * false otherwise.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This function returns true if there is only one node on
 *         @a the_chain and false otherwise.
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
 * @brief Is this node the chain head.
 *
 * This function returns true if @a the_node is the head of @a the_chain and
 * false otherwise.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to check for being the Chain Head.
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
 * @brief Is this node the chail tail.
 *
 * This function returns true if @a the_node is the tail of @a the_chain and
 * false otherwise.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to check for being the Chain Tail.
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
 * @brief Initialize this chain as empty.
 *
 * This routine initializes the specified chain to contain zero nodes.
 *
 * @param[in] the_chain is the chain to be initialized.
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
 * @brief Extract this node (unprotected).
 *
 * This routine extracts the_node from the chain on which it resides.
 * It does NOT disable interrupts to ensure the atomicity of the
 * extract operation.
 *
 * @param[in] the_node is the node to be extracted.
 */
RTEMS_INLINE_ROUTINE void _Chain_Extract_unprotected(
  Chain_Node *the_node
)
{
  Chain_Node *next;
  Chain_Node *previous;

  next           = the_node->next;
  previous       = the_node->previous;
  next->previous = previous;
  previous->next = next;
}

/**
 * @brief Get the first node (unprotected).
 *
 * This function removes the first node from the_chain and returns
 * a pointer to that node.  It does NOT disable interrupts to ensure
 * the atomicity of the get operation.
 *
 * @param[in] the_chain is the chain to attempt to get the first node from.
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
  Chain_Node *head = _Chain_Head( the_chain );
  Chain_Node *old_first = head->next;
  Chain_Node *new_first = old_first->next;

  head->next = new_first;
  new_first->previous = head;

  return old_first;
}

/**
 * @brief Get the first node (unprotected).
 *
 * This function removes the first node from the_chain and returns
 * a pointer to that node.  If the_chain is empty, then NULL is returned.
 *
 * @param[in] the_chain is the chain to attempt to get the first node from.
 *
 * @return This method returns the first node on the chain or NULL if the
 *         chain is empty.
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
 * @brief Insert a node (unprotected).
 *
 * This routine inserts the_node on a chain immediately following
 * after_node.
 *
 * @param[in] after_node is the node which will precede @a the_node on the
 *            chain.
 * @param[in] the_node is the node to be inserted.
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

  the_node->previous    = after_node;
  before_node           = after_node->next;
  after_node->next      = the_node;
  the_node->next        = before_node;
  before_node->previous = the_node;
}

/**
 * @brief Append a node (unprotected).
 *
 * This routine appends the_node onto the end of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be appended.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 *       append operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Append_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  Chain_Node *tail = _Chain_Tail( the_chain );
  Chain_Node *old_last = tail->previous;

  the_node->next = tail;
  tail->previous = the_node;
  old_last->next = the_node;
  the_node->previous = old_last;
}

/**
 * @brief Append a node on the end of a chain if the node is in the off chain
 * state (unprotected).
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
 * @brief Prepend a node (unprotected).
 *
 * This routine prepends the_node onto the front of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be prepended.
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
 * @brief Prepend a node (protected).
 *
 * This routine prepends the_node onto the front of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be prepended.
 *
 * @note It disables interrupts to ensure the atomicity of the
 *       prepend operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Prepend(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  _Chain_Insert(_Chain_Head(the_chain), the_node);
}

/**
 * @brief Append a node and check if the chain was empty before (unprotected).
 *
 * This routine appends the_node onto the end of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be appended.
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
 * @brief Prepend a node and check if the chain was empty before (unprotected).
 *
 * This routine prepends the_node onto the front of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be prepended.
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
 * @brief Get the first node and check if the chain is empty afterwards
 * (unprotected).
 *
 * This function removes the first node from the_chain and returns
 * a pointer to that node in @a the_node.  If the_chain is empty, then NULL is
 * returned.
 *
 * @param[in] the_chain is the chain to attempt to get the first node from.
 * @param[out] the_node is the first node on the chain or NULL if the chain is
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
 * @param[in] left The left node.
 * @param[in] right The right node.
 *
 * @retval true According to the order the left node precedes the right node.
 * @retval false Otherwise.
 */
typedef bool ( *Chain_Node_order )(
  const Chain_Node *left,
  const Chain_Node *right
);

/**
 * @brief Inserts a node into the chain according to the order relation.
 *
 * After the operation the chain contains the node to insert and the order
 * relation holds for all nodes from the head up to the inserted node.  Nodes
 * after the inserted node are not moved.
 *
 * @param[in,out] chain The chain.
 * @param[in,out] to_insert The node to insert.
 * @param[in] order The order relation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Insert_ordered_unprotected(
  Chain_Control *chain,
  Chain_Node *to_insert,
  Chain_Node_order order
)
{
  const Chain_Node *tail = _Chain_Immutable_tail( chain );
  Chain_Node *next = _Chain_First( chain );

  while ( next != tail && !( *order )( to_insert, next ) ) {
    next = _Chain_Next( next );
  }

  _Chain_Insert_unprotected( _Chain_Previous( next ), to_insert );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
