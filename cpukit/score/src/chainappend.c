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
