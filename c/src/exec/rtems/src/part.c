/*
 *  Partition Manager
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
#include <rtems/score/address.h>
#include <rtems/score/object.h>
#include <rtems/rtems/part.h>
#include <rtems/score/thread.h>
#include <rtems/score/sysstate.h>

/*PAGE
 *
 *  _Partition_Manager_initialization
 *
 *  This routine initializes all partition manager related
 *  data structures.
 *
 *  Input parameters:
 *    maximum_partitions - number of partitions to initialize
 *
 *  Output parameters:  NONE
 */

void _Partition_Manager_initialization(
  unsigned32 maximum_partitions
)
{
  _Objects_Initialize_information(
    &_Partition_Information,
    OBJECTS_RTEMS_PARTITIONS,
    TRUE,
    maximum_partitions,
    sizeof( Partition_Control ),
    FALSE,
    RTEMS_MAXIMUM_NAME_LENGTH,
    FALSE
  );

  /*
   *  Register the MP Process Packet routine.
   */

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_PARTITION,
    _Partition_MP_Process_packet
  );
#endif

}
