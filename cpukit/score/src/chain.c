/*
 *  COPYRIGHT (c) 1989-2007.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/isr.h>

/*
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
  size_t         number_nodes,
  size_t         node_size
)
{
  size_t count = number_nodes;
  Chain_Node *head = _Chain_Head( the_chain );
  Chain_Node *tail = _Chain_Tail( the_chain );
  Chain_Node *current = head;
  Chain_Node *next = starting_address;

  head->previous = NULL;

  while ( count-- ) {
    current->next  = next;
    next->previous = current;
    current        = next;
    next           = (Chain_Node *)
                        _Addresses_Add_offset( (void *) next, node_size );
  }

  current->next = tail;
  tail->previous = current;
}
