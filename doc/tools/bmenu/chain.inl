/*  inline/chain.inl
 * 
 *  This include file contains the bodies of the routines which are
 *  associated with doubly linked chains and inlined.
 *
 *  COPYRIGHT (c) 1988-2002.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 */

#ifndef __INLINE_CHAIN_inl
#define __INLINE_CHAIN_inl

/*PAGE
 *
 *  _Chain_Is_null
 */

STATIC INLINE boolean _Chain_Is_null(
  Chain_Control *the_chain
)
{
  return ( the_chain == NULL );
}

/*PAGE
 *
 *  _Chain_Is_null_node
 */

STATIC INLINE boolean _Chain_Is_null_node(
  Chain_Node *the_node
)
{
  return ( the_node == NULL );
}

/*PAGE
 *
 *  _Chain_Head
 */

STATIC INLINE Chain_Node *_Chain_Head(
  Chain_Control *the_chain
)
{
   return (Chain_Node *) the_chain;
}

/*PAGE
 *
 *  _Chain_Tail
 */

STATIC INLINE Chain_Node *_Chain_Tail(
  Chain_Control *the_chain
)
{
   return (Chain_Node *) &the_chain->permanent_null;
}

/*PAGE
 *
 *  _Chain_Is_empty
 */

STATIC INLINE boolean _Chain_Is_empty(
  Chain_Control *the_chain
)
{
  return ( the_chain->first == _Chain_Tail( the_chain ) );
}

/*PAGE
 *
 *  _Chain_Is_first
 */

STATIC INLINE boolean _Chain_Is_first(
  Chain_Node *the_node
)
{
  return ( the_node->previous == NULL );
}

/*PAGE
 *
 *  _Chain_Is_last
 */

STATIC INLINE boolean _Chain_Is_last(
  Chain_Node *the_node
)
{
  return ( the_node->next == NULL );
}

/*PAGE
 *
 *  _Chain_Has_only_one_node
 */

STATIC INLINE boolean _Chain_Has_only_one_node(
  Chain_Control *the_chain
)
{
  return ( the_chain->first == the_chain->last );
}

/*PAGE
 *
 *  _Chain_Is_head
 */

STATIC INLINE boolean _Chain_Is_head(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
   return ( the_node == _Chain_Head( the_chain ) );
}

/*PAGE
 *
 *  _Chain_Is_tail
 */

STATIC INLINE boolean _Chain_Is_tail(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
   return ( the_node == _Chain_Tail( the_chain ) );
}

/*PAGE
 *
 *  Chain_Initialize_empty
 */

STATIC INLINE void _Chain_Initialize_empty(
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
 */

STATIC INLINE void _Chain_Extract_unprotected(
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
 */

STATIC INLINE Chain_Node *_Chain_Get_first_unprotected( 
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
 */

STATIC INLINE Chain_Node *_Chain_Get_unprotected( 
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
 */

STATIC INLINE void _Chain_Insert_unprotected(
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
 */

STATIC INLINE void _Chain_Append_unprotected(
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
 */

STATIC INLINE void _Chain_Prepend_unprotected(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  _Chain_Insert_unprotected( _Chain_Head( the_chain ), the_node );

}

/*PAGE
 *
 *  _Chain_Prepend
 */

STATIC INLINE void _Chain_Prepend(
  Chain_Control *the_chain,
  Chain_Node    *the_node
)
{
  _Chain_Insert( _Chain_Head( the_chain ), the_node );
}

#endif
/* end of include file */
