/*  inline/chain.inl
 *
 *  This include file contains the bodies of the routines which are
 *  associated with doubly linked chains and inlined.
 *
 *  NOTE:  The routines in this file are ordered from simple
 *         to complex.  No other Chain Handler routine is referenced
 *         unless it has already been defined.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#ifndef __INLINE_CHAIN_inl
#define __INLINE_CHAIN_inl

/*PAGE
 *
 *  _Chain_Are_nodes_equal
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if LEFT and RIGHT are equal,
 *  and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Are_nodes_equal(
  Chain_Node *left,
  Chain_Node *right
)
{
  return left == right;
}

/*PAGE
 *
 *  _Chain_Is_null
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_chain is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Is_null(
  Chain_Control *the_chain
)
{
  return ( the_chain == NULL );
}

/*PAGE
 *
 *  _Chain_Is_null_node
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is NULL and FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Is_null_node(
  Chain_Node *the_node
)
{
  return ( the_node == NULL );
}

/*PAGE
 *
 *  _Chain_Head
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the first node on the chain.
 */

RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Head(
  Chain_Control *the_chain
)
{
   return (Chain_Node *) the_chain;
}

/*PAGE
 *
 *  _Chain_Tail
 *
 *  DESCRIPTION:
 *
 *  This function returns a pointer to the last node on the chain.
 */

RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Tail(
  Chain_Control *the_chain
)
{
   return (Chain_Node *) &the_chain->permanent_null;
}

/*PAGE
 *
 *  _Chain_Is_empty
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if there a no nodes on the_chain and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Is_empty(
  Chain_Control *the_chain
)
{
  return ( the_chain->first == _Chain_Tail( the_chain ) );
}

/*PAGE
 *
 *  _Chain_Is_first
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is the first node on a chain and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Is_first(
  Chain_Node *the_node
)
{
  return ( the_node->previous == NULL );
}

/*PAGE
 *
 *  _Chain_Is_last
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is the last node on a chain and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Is_last(
  Chain_Node *the_node
)
{
  return ( the_node->next == NULL );
}

/*PAGE
 *
 *  _Chain_Has_only_one_node
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if there is only one node on the_chain and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Has_only_one_node(
  Chain_Control *the_chain
)
{
  return ( the_chain->first == the_chain->last );
}

/*PAGE
 *
 *  _Chain_Is_head
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is the head of the_chain and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Is_head(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
   return ( the_node == _Chain_Head( the_chain ) );
}

/*PAGE
 *
 *  _Chain_Is_tail
 *
 *  DESCRIPTION:
 *
 *  This function returns TRUE if the_node is the tail of the_chain and
 *  FALSE otherwise.
 */

RTEMS_INLINE_ROUTINE boolean _Chain_Is_tail(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
   return ( the_node == _Chain_Tail( the_chain ) );
}

/*PAGE
 *
 *  Chain_Initialize_empty
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the specified chain to contain zero nodes.
 */

RTEMS_INLINE_ROUTINE void _Chain_Initialize_empty(
  Chain_Control *the_chain
)
{
  the_chain->first          = _Chain_Tail( the_chain );
  the_chain->permanent_null = NULL;
  the_chain->last           = _Chain_Head( the_chain );
}

/*PAGE
 *
 *  _Chain_Extract_unprotected
 *
 *  DESCRIPTION:
 *
 *  This routine extracts the_node from the chain on which it resides.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  extract operation.
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

/*PAGE
 *
 *  _Chain_Get_first_unprotected
 *
 *  DESCRIPTION:
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  It does NOT disable interrupts to insure
 *  the atomicity of the get operation.
 */

RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Get_first_unprotected(
  Chain_Control *the_chain
)
{
  Chain_Node  *return_node;
  Chain_Node  *new_first;

  return_node         = the_chain->first;
  new_first           = return_node->next;
  the_chain->first    = new_first;
  new_first->previous = _Chain_Head( the_chain );

  return return_node;
}

/*PAGE
 *
 *  Chain_Get_unprotected
 *
 *  DESCRIPTION:
 *
 *  This function removes the first node from the_chain and returns
 *  a pointer to that node.  If the_chain is empty, then NULL is returned.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  get operation.
 */

RTEMS_INLINE_ROUTINE Chain_Node *_Chain_Get_unprotected(
  Chain_Control *the_chain
)
{
  if ( !_Chain_Is_empty( the_chain ) )
    return _Chain_Get_first_unprotected( the_chain );
  else
    return NULL;
}

/*PAGE
 *
 *  _Chain_Insert_unprotected
 *
 *  DESCRIPTION:
 *
 *  This routine inserts the_node on a chain immediately following
 *  after_node.  It does NOT disable interrupts to insure the atomicity
 *  of the extract operation.
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

/*PAGE
 *
 *  _Chain_Append_unprotected
 *
 *  DESCRIPTION:
 *
 *  This routine appends the_node onto the end of the_chain.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  append operation.
 */

RTEMS_INLINE_ROUTINE void _Chain_Append_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  Chain_Node *old_last_node;

  the_node->next      = _Chain_Tail( the_chain );
  old_last_node       = the_chain->last;
  the_chain->last     = the_node;
  old_last_node->next = the_node;
  the_node->previous  = old_last_node;
}

/*PAGE
 *
 *  _Chain_Prepend_unprotected
 *
 *  DESCRIPTION:
 *
 *  This routine prepends the_node onto the front of the_chain.
 *  It does NOT disable interrupts to insure the atomicity of the
 *  prepend operation.
 */

RTEMS_INLINE_ROUTINE void _Chain_Prepend_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  _Chain_Insert_unprotected( _Chain_Head( the_chain ), the_node );

}

/*PAGE
 *
 *  _Chain_Prepend
 *
 *  DESCRIPTION:
 *
 *  This routine prepends the_node onto the front of the_chain.
 *  It disables interrupts to insure the atomicity of the
 *  prepend operation.
 */

RTEMS_INLINE_ROUTINE void _Chain_Prepend(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  _Chain_Insert( _Chain_Head( the_chain ), the_node );
}

#endif
/* end of include file */
