/**
 *  @file
 *
 *  @brief Dual Ported Memory Manager Initialization
 *  @ingroup ClassicDPMEM
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
#include <rtems/rtems/dpmemimpl.h>

Objects_Information _Dual_ported_memory_Information;

static void _Dual_ported_memory_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Dual_ported_memory_Information, /* object information table */
    OBJECTS_CLASSIC_API,              /* object API */
    OBJECTS_RTEMS_PORTS,              /* object class */
    Configuration_RTEMS_API.maximum_ports,
                                  /* maximum objects of this class */
    sizeof( Dual_ported_memory_Control ),
                                  /* size of this object's control block */
    false,                        /* true if names of this object are strings */
    RTEMS_MAXIMUM_NAME_LENGTH,    /* maximum length of each object's name */
    NULL                          /* Proxy extraction support callout */
  );
}

RTEMS_SYSINIT_ITEM(
  _Dual_ported_memory_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_DUAL_PORTED_MEMORY,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
