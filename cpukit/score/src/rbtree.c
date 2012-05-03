/*
 *  Copyright (c) 2010 Gedare Bloom.
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
#include <rtems/score/rbtree.h>
#include <rtems/score/isr.h>

/*
 *  _RBTree_Initialize
 *
 *  This kernel routine initializes a Red-Black Tree.
 *
 *  Input parameters:
 *    the_rbtree        - pointer to rbtree header
 *    starting_address - starting address of first node
 *    number_nodes     - number of nodes in rbtree
 *    node_size        - size of node in bytes
 *
 *  Output parameters:  NONE
 */

void _RBTree_Initialize(
  RBTree_Control          *the_rbtree,
  RBTree_Compare_function  compare_function,
  void                    *starting_address,
  size_t                   number_nodes,
  size_t                   node_size,
  bool                     is_unique
)
{
  size_t      count;
  RBTree_Node *next;

  /* TODO: Error message? */
  if (!the_rbtree) return;

  /* could do sanity checks here */
  _RBTree_Initialize_empty(the_rbtree, compare_function, is_unique);

  count = number_nodes;
  next  = starting_address;
  while ( count-- ) {
    _RBTree_Insert_unprotected(the_rbtree, next);
    next           = (RBTree_Node *)
                        _Addresses_Add_offset( (void *) next, node_size );
  }
}
