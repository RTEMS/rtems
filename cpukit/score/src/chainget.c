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
