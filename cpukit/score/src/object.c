/*
 *  Object Handler
 *
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

#include <rtems/system.h>
#include <rtems/score/address.h>
#include <rtems/score/chain.h>
#include <rtems/score/object.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/objectmp.h>
#endif
#include <rtems/score/thread.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/isr.h>

/*PAGE
 *
 *  _Objects_Handler_initialization
 *
 *  This routine initializes the object handler.
 *
 *  Input parameters:
 *    node                   - local node
 *    maximum_nodes          - number of nodes in the system
 *    maximum_global_objects - number of configured global objects
 *
 *  Output parameters:  NONE
 */

void _Objects_Handler_initialization(
  unsigned32 node,
  unsigned32 maximum_nodes,
  unsigned32 maximum_global_objects
)
{
  if ( node < 1 || node > maximum_nodes )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_INVALID_NODE
    );

  _Objects_Local_node    = node;
  _Objects_Maximum_nodes = maximum_nodes;

#if defined(RTEMS_MULTIPROCESSING)
  _Objects_MP_Handler_initialization(
    node,
    maximum_nodes,
    maximum_global_objects
  );
#endif
}
