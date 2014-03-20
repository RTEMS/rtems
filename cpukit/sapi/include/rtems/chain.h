/**
 * @file
 * 
 * @brief Chain API
 */

/*
 *  Copyright (c) 2010-2014 embedded brains GmbH.
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_CHAIN_H
#define _RTEMS_CHAIN_H

#include <rtems/score/chainimpl.h>
#include <rtems/rtems/event.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup ClassicChains Chains
 *
 * @ingroup ClassicRTEMS
 *
 * @brief Chain API
 */
/**@{**/

typedef Chain_Node rtems_chain_node;

typedef Chain_Control rtems_chain_control;

/**
 *  @brief Chain initializer for an empty chain with designator @a name.
 */
#define RTEMS_CHAIN_INITIALIZER_EMPTY( name ) \
  CHAIN_INITIALIZER_EMPTY( name )

/**
 *  @brief Chain initializer for a chain with one @a node.
 *
 *  @see RTEMS_CHAIN_NODE_INITIALIZER_ONE_NODE_CHAIN().
 */
#define RTEMS_CHAIN_INITIALIZER_ONE_NODE( node ) \
  CHAIN_INITIALIZER_ONE_NODE( node )

/**
 *  @brief Chain node initializer for a @a chain containing exactly this node.
 *
 *  @see RTEMS_CHAIN_INITIALIZER_ONE_NODE().
 */
#define RTEMS_CHAIN_NODE_INITIALIZER_ONE_NODE_CHAIN( chain ) \
  CHAIN_NODE_INITIALIZER_ONE_NODE_CHAIN( chain )

/**
 *  @brief Chain definition for an empty chain with designator @a name.
 */
#define RTEMS_CHAIN_DEFINE_EMPTY( name ) \
  rtems_chain_control name = RTEMS_CHAIN_INITIALIZER_EMPTY( name )

/**
 * @brief Appends the @a node to the @a chain and sends the @a events to the
 * @a task if the @a chain was empty before the append.
 *
 * @see rtems_chain_append_with_empty_check() and rtems_event_send().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID No such task.
 */
rtems_status_code rtems_chain_append_with_notification(
  rtems_chain_control *chain,
  rtems_chain_node *node,
  rtems_id task,
  rtems_event_set events
);

/**
 * @brief Prepends the @a node to the @a chain and sends the @a events to the
 * @a task if the @a chain was empty before the prepend.
 *
 * @see rtems_chain_prepend_with_empty_check() and rtems_event_send().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID No such task.
 */
rtems_status_code rtems_chain_prepend_with_notification(
  rtems_chain_control *chain,
  rtems_chain_node *node,
  rtems_id task,
  rtems_event_set events
);

/**
 * @brief Gets the first @a node of the @a chain and sends the @a events to the
 * @a task if the @a chain is empty after the get.
 *
 * @see rtems_chain_get_with_empty_check() and rtems_event_send().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_INVALID_ID No such task.
 */
rtems_status_code rtems_chain_get_with_notification(
  rtems_chain_control *chain,
  rtems_id task,
  rtems_event_set events,
  rtems_chain_node **node
);

/**
 * @brief Gets the first @a node of the @a chain and sends the @a events to the
 * @a task if the @a chain is empty afterwards.
 *
 * @see rtems_chain_get() and rtems_event_receive().
 *
 * @retval RTEMS_SUCCESSFUL Successful operation.
 * @retval RTEMS_TIMEOUT Timeout.
 */
rtems_status_code rtems_chain_get_with_wait(
  rtems_chain_control *chain,
  rtems_event_set events,
  rtems_interval timeout,
  rtems_chain_node **node
);

/**
 * @brief Initialize a chain Header.
 *
 * This routine initializes @a the_chain structure to manage the
 * contiguous array of @a number_nodes nodes which starts at
 * @a starting_address.  Each node is of @a node_size bytes.
 *
 * @param[in] the_chain specifies the chain to initialize
 * @param[in] starting_address is the starting address of the array
 *        of elements
 * @param[in] number_nodes is the number of nodes that will be in the chain
 * @param[in] node_size is the size of each node
 */
RTEMS_INLINE_ROUTINE void rtems_chain_initialize(
  rtems_chain_control *the_chain,
  void                *starting_address,
  size_t               number_nodes,
  size_t               node_size
)
{
  _Chain_Initialize(
    the_chain,
    starting_address,
    number_nodes,
    node_size
  );
}

/**
 * @brief Initialize this chain as empty.
 *
 * This routine initializes the specified chain to contain zero nodes.
 *
 * @param[in] the_chain is the chain to be initialized.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_initialize_empty(
  rtems_chain_control *the_chain
)
{
  _Chain_Initialize_empty( the_chain );
}

/**
 * @brief Set off chain.
 *
 * This function sets the next and previous fields of the @a node to NULL
 * indicating the @a node is not part of a chain.
 *
 * @param[in] node the node set to off chain.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_set_off_chain(
  rtems_chain_node *node
)
{
  _Chain_Set_off_chain( node );
}

/**
 * @brief Is the node off chain.
 *
 * This function returns true if the @a node is not on a chain. A @a node is
 * off chain if the next and previous fields are set to NULL.
 *
 * @param[in] node is the node off chain.
 *
 * @retval true The node is off chain.
 * @retval false The node is not off chain.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_node_off_chain(
  const rtems_chain_node *node
)
{
  return _Chain_Is_node_off_chain( node );
}

/**
 * @brief Is the chain node pointer NULL.
 *
 * This function returns true if the_node is NULL and false otherwise.
 *
 * @param[in] the_node is the node pointer to check.
 * 
 * @retval true The chain node pointer is NULL.
 * @retval false The chain node pointer is not NULL.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_null_node(
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_null_node( the_node );
}

/**
 * @brief Return pointer to Chain Head
 *
 * This function returns a pointer to the first node on the chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the permanent node of the chain.
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_head(
  rtems_chain_control *the_chain
)
{
  return _Chain_Head( the_chain );
}

/**
 * @brief Return pointer to immutable Chain Head
 *
 * This function returns a pointer to the head node on the chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the permanent head node of the chain.
 */
RTEMS_INLINE_ROUTINE const rtems_chain_node *rtems_chain_immutable_head(
  const rtems_chain_control *the_chain
)
{
  return _Chain_Immutable_head( the_chain );
}

/**
 * @brief Return pointer to Chain Tail
 *
 * This function returns a pointer to the tail node on the chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the permanent tail node of the chain.
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_tail(
  rtems_chain_control *the_chain
)
{
  return _Chain_Tail( the_chain );
}

/** 
 * @brief Return pointer to immutable Chain Tail
 *
 * This function returns a pointer to the tail node on the chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the permanent tail node of the chain.
 */
RTEMS_INLINE_ROUTINE const rtems_chain_node *rtems_chain_immutable_tail(
  const rtems_chain_control *the_chain
)
{
  return _Chain_Immutable_tail( the_chain );
}

/**
 * @brief Return pointer to Chain's First node after the permanent head.
 *
 * This function returns a pointer to the first node on the chain after the
 * head.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the first node of the chain.
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_first(
  rtems_chain_control *the_chain
)
{
  return _Chain_First( the_chain );
}

/** 
 * @brief Return pointer to immutable Chain's First node
 *
 * This function returns a pointer to the first node on the chain after the
 * head.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the first node of the chain.
 */
RTEMS_INLINE_ROUTINE const rtems_chain_node *rtems_chain_immutable_first(
  const rtems_chain_control *the_chain
)
{
  return _Chain_Immutable_first( the_chain );
}

/**
 * @brief Return pointer to Chain's Last node before the permanent tail.
 *
 * This function returns a pointer to the last node on the chain just before
 * the tail.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the last node of the chain.
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_last(
  rtems_chain_control *the_chain
)
{
  return _Chain_Last( the_chain );
}

/**
 * @brief Return pointer to immutable Chain's Last node
 *
 * This function returns a pointer to the last node on the chain just before
 * the tail.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @return This method returns the last node of the chain.
 */
RTEMS_INLINE_ROUTINE const rtems_chain_node *rtems_chain_immutable_last(
  const rtems_chain_control *the_chain
)
{
  return _Chain_Immutable_last( the_chain );
}

/**
 * @brief Return pointer the next node from this node
 *
 * This function returns a pointer to the next node after this node.
 *
 * @param[in] the_node is the node to be operated upon.
 *
 * @return This method returns the next node on the chain.
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_next(
  rtems_chain_node *the_node
)
{
  return _Chain_Next( the_node );
}

/** 
 * @brief Return pointer the immutable next node from this node
 *
 * This function returns a pointer to the next node after this node.
 *
 * @param[in] the_node is the node to be operated upon.
 *
 * @return This method returns the next node on the chain.
 */
RTEMS_INLINE_ROUTINE const rtems_chain_node *rtems_chain_immutable_next(
  const rtems_chain_node *the_node
)
{
  return _Chain_Immutable_next( the_node );
}

/**
 * @brief Return pointer the previous node from this node
 *
 * This function returns a pointer to the previous node on this chain.
 *
 * @param[in] the_node is the node to be operated upon.
 *
 * @return This method returns the previous node on the chain.
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_previous(
  rtems_chain_node *the_node
)
{
  return _Chain_Previous( the_node );
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
RTEMS_INLINE_ROUTINE const rtems_chain_node *rtems_chain_immutable_previous(
  const rtems_chain_node *the_node
)
{
  return _Chain_Immutable_previous( the_node );
}

/**
 * @brief Are Two nodes equal.
 *
 * This function returns true if @a left and @a right are equal,
 * and false otherwise.
 *
 * @param[in] left is the node on the left hand side of the comparison.
 * @param[in] right is the node on the left hand side of the comparison.
 *
 * @retval true @a left is equal to @a right.
 * @retval false @a left is not equal to @a right
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_are_nodes_equal(
  const rtems_chain_node *left,
  const rtems_chain_node *right
)
{
  return _Chain_Are_nodes_equal( left, right );
}

/**
 * @brief Is the chain empty
 *
 * This function returns true if there a no nodes on @a the_chain and
 * false otherwise.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @retval true The chain is empty.
 * @retval false The chain is not empty.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_empty(
  const rtems_chain_control *the_chain
)
{
  return _Chain_Is_empty( the_chain );
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
RTEMS_INLINE_ROUTINE bool rtems_chain_is_first(
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_first( the_node );
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
 * @retval false @a the_node is not the last node on a chain
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_last(
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_last( the_node );
}

/**
 * @brief Does this chain have only one node.
 *
 * This function returns true if there is only one node on @a the_chain and
 * false otherwise.
 *
 * @param[in] the_chain is the chain to be operated upon.
 *
 * @retval true The chain has only one node.
 * @retval false The chain has more than one nodes.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_has_only_one_node(
  const rtems_chain_control *the_chain
)
{
  return _Chain_Has_only_one_node( the_chain );
}

/**
 * @brief Is this node the chain head.
 *
 * This function returns true if @a the_node is the head of the_chain and
 * false otherwise.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to check for being the Chain Head.
 *
 * @retval true @a the_node is the head of @a the_chain.
 * @retval false @a the_node is not the head of @a the_chain.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_head(
  const rtems_chain_control *the_chain,
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_head( the_chain, the_node );
}

/**
 * @brief Is this node the chain tail.
 *
 * This function returns true if the_node is the tail of the_chain and
 * false otherwise.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to check for being the Chain Tail.
 * 
 * @retval true @a the_node is the tail of @a the_chain.
 * @retval false @a the_node is not the tail of @a the_chain.
 */
RTEMS_INLINE_ROUTINE bool rtems_chain_is_tail(
  const rtems_chain_control *the_chain,
  const rtems_chain_node *the_node
)
{
  return _Chain_Is_tail( the_chain, the_node );
}

/**
 * @brief Extract the specified node from a chain.
 *
 * This routine extracts @a the_node from the chain on which it resides.
 * It disables interrupts to ensure the atomicity of the
 * extract operation.
 *
 * @arg the_node specifies the node to extract
 */
#if defined( RTEMS_SMP )
void rtems_chain_extract(
  rtems_chain_node *the_node
);
#else
RTEMS_INLINE_ROUTINE void rtems_chain_extract(
  rtems_chain_node *the_node
)
{
  _Chain_Extract( the_node );
}
#endif

/**
 * @brief Extract the specified node from a chain (unprotected).
 *
 * This routine extracts @a the_node from the chain on which it resides.
 *
 * NOTE: It does NOT disable interrupts to ensure the atomicity of the
 * append operation.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_extract_unprotected(
  rtems_chain_node *the_node
)
{
  _Chain_Extract_unprotected( the_node );
}

/**
 * @brief Obtain the first node on a chain.
 *
 * This function removes the first node from @a the_chain and returns
 * a pointer to that node.  If @a the_chain is empty, then NULL is returned.
 *
 * @return This method returns a pointer a node.  If a node was removed,
 *         then a pointer to that node is returned.  If @a the_chain was
 *         empty, then NULL is returned.
 *
 *  NOTE: It disables interrupts to ensure the atomicity of the get operation.
 */
#if defined( RTEMS_SMP )
rtems_chain_node *rtems_chain_get(
  rtems_chain_control *the_chain
);
#else
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_get(
  rtems_chain_control *the_chain
)
{
  return _Chain_Get( the_chain );
}
#endif

/**
 * @brief See _Chain_Get_unprotected().
 */
RTEMS_INLINE_ROUTINE rtems_chain_node *rtems_chain_get_unprotected(
  rtems_chain_control *the_chain
)
{
  return _Chain_Get_unprotected( the_chain );
}

/**
 * @brief Insert a node on a chain
 *
 * This routine inserts @a the_node on a chain immediately following
 * @a after_node.  
 *
 * NOTE: It disables interrupts to ensure the atomicity
 * of the extract operation.
 */
#if defined( RTEMS_SMP )
void rtems_chain_insert(
  rtems_chain_node *after_node,
  rtems_chain_node *the_node
);
#else
RTEMS_INLINE_ROUTINE void rtems_chain_insert(
  rtems_chain_node *after_node,
  rtems_chain_node *the_node
)
{
  _Chain_Insert( after_node, the_node );
}
#endif

/**
 * @brief See _Chain_Insert_unprotected().
 */
RTEMS_INLINE_ROUTINE void rtems_chain_insert_unprotected(
  rtems_chain_node *after_node,
  rtems_chain_node *the_node
)
{
  _Chain_Insert_unprotected( after_node, the_node );
}

/**
 * @brief Append a node on the end of a chain.
 *
 * This routine appends @a the_node onto the end of @a the_chain.
 *
 * NOTE: It disables interrupts to ensure the atomicity of the
 * append operation.
 */
#if defined( RTEMS_SMP )
void rtems_chain_append(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
);
#else
RTEMS_INLINE_ROUTINE void rtems_chain_append(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
)
{
  _Chain_Append( the_chain, the_node );
}
#endif

/**
 * @brief Append a node on the end of a chain (unprotected).
 *
 * This routine appends @a the_node onto the end of @a the_chain.
 *
 * NOTE: It does NOT disable interrupts to ensure the atomicity of the
 * append operation.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_append_unprotected(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
)
{
  _Chain_Append_unprotected( the_chain, the_node );
}

/** 
 * @brief Prepend a node.
 *
 * This routine prepends the_node onto the front of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be prepended.
 *
 * NOTE: It disables interrupts to ensure the atomicity of the
 *       prepend operation.
 */
#if defined( RTEMS_SMP )
void rtems_chain_prepend(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
);
#else
RTEMS_INLINE_ROUTINE void rtems_chain_prepend(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
)
{
  _Chain_Prepend( the_chain, the_node );
}
#endif

/** 
 * @brief Prepend a node (unprotected).
 *
 * This routine prepends the_node onto the front of the_chain.
 *
 * @param[in] the_chain is the chain to be operated upon.
 * @param[in] the_node is the node to be prepended.
 *
 * NOTE: It does NOT disable interrupts to ensure the atomicity of the
 *       prepend operation.
 */
RTEMS_INLINE_ROUTINE void rtems_chain_prepend_unprotected(
  rtems_chain_control *the_chain,
  rtems_chain_node    *the_node
)
{
  _Chain_Prepend_unprotected( the_chain, the_node );
}

/**
 * @brief Checks if the @a chain is empty and appends the @a node.
 *
 * Interrupts are disabled to ensure the atomicity of the operation.
 *
 * @retval true The chain was empty before the append.
 * @retval false The chain contained at least one node before the append.
 */
#if defined( RTEMS_SMP )
bool rtems_chain_append_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
);
#else
RTEMS_INLINE_ROUTINE bool rtems_chain_append_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  return _Chain_Append_with_empty_check( chain, node );
}
#endif

/**
 * @brief Checks if the @a chain is empty and prepends the @a node.
 *
 * Interrupts are disabled to ensure the atomicity of the operation.
 *
 * @retval true The chain was empty before the prepend.
 * @retval false The chain contained at least one node before the prepend.
 */
#if defined( RTEMS_SMP )
bool rtems_chain_prepend_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
);
#else
RTEMS_INLINE_ROUTINE bool rtems_chain_prepend_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node *node
)
{
  return _Chain_Prepend_with_empty_check( chain, node );
}
#endif

/**
 * @brief Tries to get the first @a node and check if the @a chain is empty
 * afterwards.
 *
 * This function removes the first node from the @a chain and returns a pointer
 * to that node in @a node.  If the @a chain is empty, then @c NULL is returned.
 *
 * Interrupts are disabled to ensure the atomicity of the operation.
 *
 * @retval true The chain is empty after the node removal.
 * @retval false The chain contained at least one node after the node removal.
 */
#if defined( RTEMS_SMP )
bool rtems_chain_get_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node **node
);
#else
RTEMS_INLINE_ROUTINE bool rtems_chain_get_with_empty_check(
  rtems_chain_control *chain,
  rtems_chain_node **node
)
{
  return _Chain_Get_with_empty_check( chain, node );
}
#endif

/**
 * @brief Returns the node count of the chain.
 *
 * @param[in] chain The chain.
 *
 * @note It does NOT disable interrupts to ensure the atomicity of the
 * operation.
 *
 * @return The node count of the chain.
 */
RTEMS_INLINE_ROUTINE size_t rtems_chain_node_count_unprotected(
  const rtems_chain_control *chain
)
{
  return _Chain_Node_count_unprotected( chain );
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
