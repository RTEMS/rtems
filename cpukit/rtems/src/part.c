/*
 *  Partition Manager
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
    &_Partition_Information,     /* object information table */
    OBJECTS_CLASSIC_API,         /* object API */
    OBJECTS_RTEMS_PARTITIONS,    /* object class */
    maximum_partitions,          /* maximum objects of this class */
    sizeof( Partition_Control ), /* size of this object's control block */
    FALSE,                       /* TRUE if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH    /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    TRUE,                        /* TRUE if this is a global object class */
    _Partition_MP_Send_extract_proxy  /* Proxy extraction support callout */
#endif
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
