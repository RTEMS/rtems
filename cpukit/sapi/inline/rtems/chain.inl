/**
 * @file rtems/chain.inl
 *
 *  This include file contains all the constants and structures associated
 *  with the Chain API in RTEMS. The chain is a double linked list that
 *  is part of the Super Core. This is the published interface to that
 *  code.
 *
 *  Iterate the node of a chain can be performed with the following code:
 *
 *     rtems_chain_control* cc = &object->pending;
 *     rtems_chain_node*    cn = cc->first;
 *     while (!rtems_chain_is_tail (cc, cn))
 *     {
 *       cn = cn->next;
 *     }
 */
 
/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef _RTEMS_CHAIN_H
# error "Never use <rtems/chain.inl> directly; include <rtems/chain.h> instead."
#endif

#ifndef _RTEMS_CHAIN_INL
#define _RTEMS_CHAIN_INL

#include <rtems/score/chain.inl>

/**
 *  @brief Initialize this Chain as Empty
 *
 *  This routine initializes the specified chain to contain zero nodes.
 *
 *  @param[in] the_chain is the chain to be initialized.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_initialize_empty(
  rtems_chain_control *the_chain
)
{
  _Chain_Initialize_empty( the_chain );
}

/**
 *  @brief Is the Chain Node Pointer NULL
 *
 *  This function returns true if the_node is NULL and false otherwise.
 *
 *  @param[in] the_node is the node pointer to check.
 *
 *  @return This method returns true if the_node is NULL and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_null_node(
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_null_node( the_node );
}

/**
 *  @brief Return pointer to Chain Head
 *
 *  This function returns a pointer to the first node on the chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the permanent head node of the chain.
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_head(
  rtems_chain_control *the_chain
)
{
  return _Chain_Head( the_chain );
}

/**
 *  @brief Return pointer to Chain Tail
 *
 *  This function returns a pointer to the last node on the chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This method returns the permanent tail node of the chain.
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_tail(
  rtems_chain_control *the_chain
)
{
  return _Chain_Tail( the_chain );
}

/**
 *  @brief Are Two Nodes Equal
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
RTEMS_INLINE_ROUTINE bool rtems_chain_are_nodes_equal(
  const rtems_chain_node *left,
  const rtems_chain_node *right
)
{
  return _Chain_Are_nodes_equal( left, right );
}

/**
 *  @brief Is the Chain Empty
 *
 *  This function returns true if there a no nodes on @a the_chain and
 *  false otherwise.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This function returns true if there a no nodes on @a the_chain and
 *  false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_empty(
  rtems_chain_control *the_chain
)
{
  return _Chain_Is_empty( the_chain );
}

/**
 *  @brief Is this the First Node on the Chain
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
RTEMS_INLINE_ROUTINE bool rtems_chain_is_first(
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_first( the_node );
}

/**
 *  @brief Is this the Last Node on the Chain
 *
 *  This function returns true if @a the_node is the last node on a chain and
 *  false otherwise.
 *
 *  @param[in] the_node is the node to check as the last node.
 *
 *  @return This function returns true if @a the_node is the last node on
 *          a chain and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_last(
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_last( the_node );
}

/**
 *  @brief Does this Chain have only One Node
 *
 *  This function returns true if there is only one node on @a the_chain and
 *  false otherwise.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *
 *  @return This function returns true if there is only one node on
 *          @a the_chain and false otherwise.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_has_only_one_node(
  const rtems_chain_control *the_chain
)
{
  return _Chain_Has_only_one_node( the_chain );
}

/**
 *  @brief Is this Node the Chain Head
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
RTEMS_INLINE_ROUTINE bool rtems_chain_is_head(
  rtems_chain_control    *the_chain,
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_head( the_chain, the_node );
}

/**
 *  @brief Is this Node the Chail Tail
 *
 *  This function returns true if the_node is the tail of the_chain and
 *  false otherwise.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to check for being the Chain Tail.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_tail(
  rtems_chain_control    *the_chain,
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_tail( the_chain, the_node );
}

/**
 *  @brief Extract the specified node from a chain
 *
 *  This routine extracts @a the_node from the chain on which it resides.
 *  It disables interrupts to ensure the atomicity of the
 *  extract operation.
 *
 *  @arg the_node specifies the node to extract
 */
RTEMS_INLINE_ROUTINE void rtems_chain_extract(
  rtems_chain_node *the_node
)
{
  _Chain_Extract( the_node );
}

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
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_get(
  rtems_chain_control *the_chain
)
{
  return _Chain_Get( the_chain );
}

/**
 *  @brief Insert a node on a chain
 *
 *  This routine inserts @a the_node on a chain immediately following
 *  @a after_node.  
 *
 *  @note It disables interrupts to ensure the atomicity
 *  of the extract operation.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_insert(
  rtems_chain_node *after_node,
  rtems_chain_node *the_node
)
{
  _Chain_Insert( after_node, the_node );
}

/**
 *  @brief Append a node on the end of a chain
 *
 *  This routine appends @a the_node onto the end of @a the_chain.
 *
 *  @note It disables interrupts to ensure the atomicity of the
 *  append operation.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_append(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
)
{
  _Chain_Append( the_chain, the_node );
}

/**
 *  @brief Append a node on the end of a chain (unprotected)
 *
 *  This routine appends @a the_node onto the end of @a the_chain.
 *
 *  @note It does NOT disable interrupts to ensure the atomicity of the
 *  append operation.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_append_unprotected(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
)
{
  _Chain_Append_unprotected( the_chain, the_node );
}

/** @brief Prepend a Node
 *
 *  This routine prepends the_node onto the front of the_chain.
 *
 *  @param[in] the_chain is the chain to be operated upon.
 *  @param[in] the_node is the node to be prepended.
 *
 *  @note It disables interrupts to ensure the atomicity of the
 *        prepend operation.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_prepend(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
)
{
  _Chain_Prepend( the_chain, the_node );
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
RTEMS_INLINE_ROUTINE void rtems_chain_prepend_unprotected(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
)
{
  _Chain_Prepend_unprotected( the_chain, the_node );
}

#endif
/* end of include file */
