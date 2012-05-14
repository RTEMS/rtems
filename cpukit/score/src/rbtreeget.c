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
 *  _RBTree_Get
 *
 *  This kernel routine returns a pointer to a node taken from the
 *  given rbtree.
 *
 *  Input parameters:
 *    the_rbtree - pointer to rbtree header
 *    dir - specifies min (0) or max (1)
 *
 *  Output parameters:
 *    return_node - pointer to node in rbtree allocated
 *    NULL   - if no nodes available
 *
 *  INTERRUPT LATENCY:
 *    only case
 */

RBTree_Node *_RBTree_Get(
  RBTree_Control *the_rbtree,
  RBTree_Direction dir
)
{
  ISR_Level          level;
  RBTree_Node *return_node;

  return_node = NULL;
  _ISR_Disable( level );
      return_node = _RBTree_Get_unprotected( the_rbtree, dir );
  _ISR_Enable( level );
  return return_node;
}

