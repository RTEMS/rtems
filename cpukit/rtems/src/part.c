/**
 *  @file
 *
 *  @brief Partition Manager Initialization
 *  @ingroup ClassicPart
 */

/*
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/sysinit.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/partimpl.h>

Objects_Information _Partition_Information;

static void _Partition_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Partition_Information,     /* object information table */
    OBJECTS_CLASSIC_API,         /* object API */
    OBJECTS_RTEMS_PARTITIONS,    /* object class */
    Configuration_RTEMS_API.maximum_partitions,
                                 /* maximum objects of this class */
    sizeof( Partition_Control ), /* size of this object's control block */
    false,                       /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH,   /* maximum length of an object name */
    _Partition_MP_Send_extract_proxy  /* Proxy extraction support callout */
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

RTEMS_SYSINIT_ITEM(
  _Partition_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_PARTITION,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
