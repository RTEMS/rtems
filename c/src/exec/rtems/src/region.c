/*
 *  Region Manager
 *
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *  Copyright assigned to U.S. Government, 1994.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/rtems/options.h>
#include <rtems/rtems/region.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>

/*PAGE
 *
 *  _Region_Manager_initialization
 *
 *  This routine initializes all region manager related data structures.
 *
 *  Input parameters:
 *    maximum_regions - number of regions to initialize
 *
 *  Output parameters:  NONE
 */

void _Region_Manager_initialization(
  unsigned32 maximum_regions
)
{
  _Objects_Initialize_information(
    &_Region_Information,
    OBJECTS_RTEMS_REGIONS,
    FALSE,
    maximum_regions,
    sizeof( Region_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );

  /*
   *  Register the MP Process Packet routine.
   */

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_REGION,
    0  /* XXX _Region_MP_Process_packet */
  );
#endif

}

