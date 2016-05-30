/**
 *  @file
 *
 *  @brief Semaphore Manager Initialization
 *  @ingroup ClassicSem
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

#include <rtems/config.h>
#include <rtems/sysinit.h>
#include <rtems/rtems/semimpl.h>

Objects_Information _Semaphore_Information;

static void _Semaphore_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Semaphore_Information,     /* object information table */
    OBJECTS_CLASSIC_API,         /* object API */
    OBJECTS_RTEMS_SEMAPHORES,    /* object class */
     Configuration_RTEMS_API.maximum_semaphores,
                                 /* maximum objects of this class */
    sizeof( Semaphore_Control ), /* size of this object's control block */
    false,                       /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH,   /* maximum length of an object name */
    _Semaphore_MP_Send_extract_proxy /* Proxy extraction support callout */
  );

  /*
   *  Register the MP Process Packet routine.
   */

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_SEMAPHORE,
    _Semaphore_MP_Process_packet
  );
#endif

}

RTEMS_SYSINIT_ITEM(
  _Semaphore_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_SEMAPHORE,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
