/*
 *  Chain Handler
 *
 *  COPYRIGHT (c) 1988-2002.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights reserved.
 *
 *  $Id$
 */

#include "system.h"
#include "address.h"
#include "chain.h"
#include "isr.h"

/*PAGE
 *
 *  _Chain_Initialize
 *
 *  This kernel routine initializes a doubly linked chain. 
 *
 *  Input parameters:
 *    the_chain        - pointer to chain header
 *    starting_address - starting address of first node
 *    number_nodes     - number of nodes in chain
 *    node_size        - size of node in bytes
 *
 *  Output parameters:  NONE
 */

void _Chain_Initialize( 
  Chain_Control *the_chain,
  void           *starting_address,
  unsigned32     number_nodes,
  unsigned32     node_size
)
{
  unsigned32  count; 
  Chain_Node *current;
  Chain_Node *next;

  count                     = number_nodes;
  current                   = _Chain_Head( the_chain );
  the_chain->permanent_null = NULL;
  next                      = (Chain_Node *)starting_address;
  while ( count-- ) {
    current->next  = next;
    next->previous = current;
    current        = next;
    next           = (Chain_Node *)
                        _Addresses_Add_offset( (void *) next, node_size );
  }
  current->next    = _Chain_Tail( the_chain );
  the_chain->last  = current;
}

/*PAGE
 *
 *  _Chain_Get
 *
 *  This kernel routine returns a pointer to a node taken from the
 *  given chain.
 *
 *  Input parameters:
 *    the_chain - pointer to chain header
 * 
 *  Output parameters: 
 *    return_node - pointer to node in chain allocated
 *    CHAIN_END   - if no nodes available
 * 
 *  INTERRUPT LATENCY:
 *    only case
 */

Chain_Node *_Chain_Get(
  Chain_Control *the_chain
)
{
  ISR_Level          level;
  Chain_Node *return_node;

  return_node = NULL;
  _ISR_Disable( level );
    if ( !_Chain_Is_empty( the_chain ) ) 
      return_node = _Chain_Get_first_unprotected( the_chain );
  _ISR_Enable( level ); 
  return return_node;
}

/*PAGE
 *
 *  _Chain_Append
 *
 *  This kernel routine puts a node on the end of the specified chain.
 *
 *  Input parameters:
 *    the_chain - pointer to chain header
 *    node      - address of node to put at rear of chain
 * 
 *  Output parameters:  NONE
 * 
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Chain_Append( 
  Chain_Control *the_chain,
  Chain_Node    *node
)
{
  ISR_Level level;

  _ISR_Disable( level );
    _Chain_Append_unprotected( the_chain, node );
  _ISR_Enable( level ); 
}

/*PAGE
 *
 *  _Chain_Extract
 *
 *  This kernel routine deletes the given node from a chain.
 *
 *  Input parameters:
 *    node - pointer to node in chain to be deleted
 * 
 *  Output parameters:  NONE
 * 
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Chain_Extract( 
  Chain_Node *node
)
{ 
  ISR_Level level;

  _ISR_Disable( level );
    _Chain_Extract_unprotected( node );
  _ISR_Enable( level );
}

/*PAGE
 *
 *  _Chain_Insert
 *
 *  This kernel routine inserts a given node after a specified node
 *  a requested chain.
 *
 *  Input parameters:
 *    after_node - pointer to node in chain to be inserted after
 *    node       - pointer to node to be inserted
 * 
 *  Output parameters:  NONE
 * 
 *  INTERRUPT LATENCY:
 *    only case
 */

void _Chain_Insert( 
  Chain_Node *after_node,
  Chain_Node *node
)
{
  ISR_Level level;

  _ISR_Disable( level );
    _Chain_Insert_unprotected( after_node, node );
  _ISR_Enable( level ); 
}

/*PAGE
 *
 *  _Chain_Insert_chain 
 *
 *  This routine inserts a chain after the specified node in another
 *  chain. It is assumed that the insert after node is not on the 
 *  second chain. 
 *
 *  Input parameters:
 *    insert_after - insert the chain after this node
 *    to_insert    - the chain to insert
 */ 
 
void _Chain_Insert_chain(
  Chain_Node    *insert_after,
  Chain_Control *to_insert
)
{
  Chain_Node *first;
  Chain_Node *last;
  Chain_Node *insert_after_next;

  first = to_insert->first;
  last = to_insert->last;

  insert_after_next = insert_after->next;

  insert_after->next = first;
  first->previous    = insert_after;

  insert_after_next->previous = last;
  last->next                  = insert_after_next;

  _Chain_Initialize_empty( to_insert );
}
