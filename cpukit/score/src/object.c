/*
 *  Object Handler
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
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
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Objects_Handler_initialization(void)
{
#if defined(RTEMS_MULTIPROCESSING)
  uint32_t   node;
  uint32_t   maximum_nodes;
  uint32_t   maximum_global_objects;

  node                   = _Configuration_MP_table->node;
  maximum_nodes          = _Configuration_MP_table->maximum_nodes;
  maximum_global_objects = _Configuration_MP_table->maximum_global_objects;
  
  if ( node < 1 || node > maximum_nodes )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_INVALID_NODE
    );

  _Objects_Local_node    = node;
  _Objects_Maximum_nodes = maximum_nodes;

  _Objects_MP_Handler_initialization(
    node,
    maximum_nodes,
    maximum_global_objects
  );
#endif
}
