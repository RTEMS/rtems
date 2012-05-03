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
# error "Never use <rtems/score/chain.inl> directly; include <rtems/score/chain.h> instead."
#endif

#ifndef _RTEMS_SCORE_CHAIN_INL
#define _RTEMS_SCORE_CHAIN_INL

/**
 *  @addtogroup ScoreChain 
 *  @{
 */

/** @brief Set off chain
 *
 *  This function sets the next and previous fields of the @a node to NULL
 *  indicating the @a node is not part of a chain.
 *
 *  @param[in] node the node set to off chain.
 */
RTEMS_INLINE_ROUTINE void _Chain_Set_off_chain(
  Chain_Node *node
)
{
  node->next = node->previous = NULL;
}

/** @brief Is the Node off Chain
 *
 *  This function returns true if the @a node is not on a chain. A @a node is
 *  off chain if the next and previous fields are set to NULL.
 *
 *  @param[in] node is the node off chain.
 *
 *  @return This function returns true if the @a node is off chain.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_node_off_chain(
  const Chain_Node *node
)
{
  return (node->next == NULL) && (node->previous == NULL);
}

/** @brief Are Two Nodes Equal
 *
 *  This function returns true if @a left and @a right are equal,
 *  and false otherwise.
 *
 *  @param[in] left is the node on the left hand side of the comparison.
 *  @param[in] right is the node on the left hand side of the comparison.
 *
 *  @return This function returns true if @a left and @a right are equal,
 *          and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Are_nodes_equal(
  const Chain_Node *left,
  const Chain_Node *right
)
{
  return left == right;
}

/** @brief Is this Chain Control Pointer Null
 *
 *  This function returns true if the_chain is NULL and false otherwise.
 *
 *  @param[in] the_chain is the chain to be checked for empty status.
 *
 *  @return This method returns true if the_chain is NULL and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_null(
  const Chain_Control *the_chain
)
{
  return (the_chain == NULL);
}

/** @brief Is the Chain Node Pointer NULL
 *
 *  This function returns true if the_node is NULL and false otherwise.
 *
 *  @param[in] the_node is the node pointer to check.
 *
 *  @return This method returns true if the_node is NULL and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_null_node(
  const Chain_Node *the_node
)
{
  return (the_node == NULL);
}

/** @brief Return pointer to Chain Head
 *
 *  This function returns a pointer to the head node on the chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the permanent head node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Head(
  Chain_Control *the_chain
)
{
  return &the_chain->Head.Node;
}

/** @brief Return pointer to immutable Chain Head
 *
 *  This function returns a pointer to the head node on the chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the permanent head node of the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_head(
  const Chain_Control *the_chain
)
{
  return &the_chain->Head.Node;
}

/** @brief Return pointer to Chain Tail
 *
 *  This function returns a pointer to the tail node on the chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the permanent tail node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Tail(
  Chain_Control *the_chain
)
{
  return &the_chain->Tail.Node;
}

/** @brief Return pointer to immutable Chain Tail
 *
 *  This function returns a pointer to the tail node on the chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the permanent tail node of the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_tail(
  const Chain_Control *the_chain
)
{
  return &the_chain->Tail.Node;
}

/** @brief Return pointer to Chain's First node
 *
 *  This function returns a pointer to the first node on the chain after the
 *  head.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the first node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_First(
  Chain_Control *the_chain
)
{
  return _Chain_Head( the_chain )->next;
}

/** @brief Return pointer to immutable Chain's First node
 *
 *  This function returns a pointer to the first node on the chain after the
 *  head.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the first node of the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_first(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_head( the_chain )->next;
}

/** @brief Return pointer to Chain's Last node
 *
 *  This function returns a pointer to the last node on the chain just before
 *  the tail.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the last node of the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Last(
  Chain_Control *the_chain
)
{
  return _Chain_Tail( the_chain )->previous;
}

/** @brief Return pointer to immutable Chain's Last node
 *
 *  This function returns a pointer to the last node on the chain just before
 *  the tail.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the last node of the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_last(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_tail( the_chain )->previous;
}

/** @brief Return pointer the next node from this node
 *
 *  This function returns a pointer to the next node after this node.
 *
 *  @param[in] the_node is the node to be operated upon.
 *
 *  @return This method returns the next node on the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Next(
  Chain_Node *the_node
)
{
  return the_node->next;
}

/** @brief Return pointer the immutable next node from this node
 *
 *  This function returns a pointer to the next node after this node.
 *
 *  @param[in] the_node is the node to be operated upon.
 *
 *  @return This method returns the next node on the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_next(
  const Chain_Node *the_node
)
{
  return the_node->next;
}

/** @brief Return pointer the previous node from this node
 *
 *  This function returns a pointer to the previous node on this chain.
 *
 *  @param[in] the_node is the node to be operated upon.
 *
 *  @return This method returns the previous node on the chain.
 */
RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Previous(
  Chain_Node *the_node
)
{
  return the_node->previous;
}

/** @brief Return pointer the immutable previous node from this node
 *
 *  This function returns a pointer to the previous node on this chain.
 *
 *  @param[in] the_node is the node to be operated upon.
 *
 *  @return This method returns the previous node on the chain.
 */
RTEMS_INLINE_ROUTINE const Chain_Node *_Chain_Immutable_previous(
  const Chain_Node *the_node
)
{
  return the_node->previous;
}

/** @brief Is the Chain Empty
 *
 *  This function returns true if there a no nodes on @a the_chain and
 *  false otherwise.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This function returns true if there a no nodes on @a the_chain and
 *  false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_empty(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_first( the_chain )
    == _Chain_Immutable_tail( the_chain );
}

/** @brief Is this the First Node on the Chain
 *
 *  This function returns true if the_node is the first node on a chain and
 *  false otherwise.
 *
 *  @param[in] the_node is the node the caller wants to know if it is
 *             the first node on a chain.
 *
 *  @return This function returns true if @a the_node is the first node on
 *          a chain and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_first(
  const Chain_Node *the_node
)
{
  return (the_node->previous->previous == NULL);
}

/** @brief Is this the Last Node on the Chain
 *
 *  This function returns true if @a the_node is the last node on a chain and
 *  false otherwise.
 *
 *  @param[in] the_node is the node to check as the last node.
 *
 *  @return This function returns true if @a the_node is the last node on
 *          a chain and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_last(
  const Chain_Node *the_node
)
{
  return (the_node->next->next == NULL);
}

/** @brief Does this Chain have only One Node
 *
 *  This function returns true if there is only one node on @a the_chain and
 *  false otherwise.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This function returns true if there is only one node on
 *          @a the_chain and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Has_only_one_node(
  const Chain_Control *the_chain
)
{
  return _Chain_Immutable_first( the_chain )
    == _Chain_Immutable_last( the_chain );
}

/** @brief Is this Node the Chain Head
 *
 *  This function returns true if @a the_node is the head of the_chain and
 *  false otherwise.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to check for being the Chain Head.
 *
 *  @return This function returns true if @a the_node is the head of
 *          @a the_chain and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_head(
  const Chain_Control *the_chain,
  const Chain_Node    *the_node
)
{
  return (the_node == _Chain_Immutable_head( the_chain ));
}

/** @brief Is this Node the Chail Tail
 *
 *  This function returns true if the_node is the tail of the_chain and
 *  false otherwise.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to check for being the Chain Tail.
 */
RTEMS_INLINE_ROUTINE bool _Chain_Is_tail(
  const Chain_Control *the_chain,
  const Chain_Node    *the_node
)
{
  return (the_node == _Chain_Immutable_tail( the_chain ));
}

/** @brief Initialize this Chain as Empty
 *
 *  This routine initializes the specified chain to contain zero nodes.
 *
 *  @param[in] the_chain is the chain to be initialized.
 */
RTEMS_INLINE_ROUTINE void _Chain_Initialize_empty(
  Chain_Control *the_chain
)
{
  Chain_Node *head = _Chain_Head( the_chain );
  Chain_Node *tail = _Chain_Tail( the_chain );

  head->next = tail;
  head->previous = NULL;
  tail->previous = head;
}

/** @brief Extract this Node (unprotected)
 *
 *  This routine extracts the_node from the chain on which it resides.
 *  It does NOT disable interrupts to ensure the atomicity of the
 *  extract operation.
 *
 *  @param[in] the_node is the node to be extracted.
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

/** @brief Get the First Node (unprotected)
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  It does NOT disable interrupts to ensure
 *  the atomicity of the get operation.
 *
 *  @param[in] the_chain is the chain to attempt to get the first node from.
 *
 *  @return This method returns the first node on the chain even if it is
 *          the Chain Tail.
 *
 *  @note This routine assumes that there is at least one node on the chain
 *        and always returns a node even if it is the Chain Tail.
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

/** @brief Get the First Node (unprotected)
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *
 *  @param[in] the_chain is the chain to attempt to get the first node from.
 *
 *  @return This method returns the first node on the chain or NULL if the
 *          chain is empty.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *        get operation.
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

/** @brief Insert a Node (unprotected)
 *
 *  This routine inserts the_node on a chain immediately following
 *  after_node.
 *
 *  @param[in] after_node is the node which will precede @a the_node on the
 *             chain.
 *  @param[in] the_node is the node to be inserted.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity
 *        of the extract operation.
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

/** @brief Append a Node (unprotected) 
 *
 *  This routine appends the_node onto the end of the_chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to be appended.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *        append operation.
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

/** @brief Prepend a Node (unprotected)
 *
 *  This routine prepends the_node onto the front of the_chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to be prepended.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *        prepend operation.
 */
RTEMS_INLINE_ROUTINE void _Chain_Prepend_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  _Chain_Insert_unprotected(_Chain_Head(the_chain), the_node);
}

/** @brief Prepend a Node (protected)
 *
 *  This routine prepends the_node onto the front of the_chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to be prepended.
 *
 *  @note It disables interrupts to ensure the atomicity of the
 *        prepend operation.
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

/**@}*/

#endif
/* end of include file */
