/**
 * @file
 *
 * @brief RTEMS Barrier Manager Initialization
 * @ingroup ClassicBarrier Barriers
 */

/*
 *  COPYRIGHT (c) 1989-2015.
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
#include <rtems/rtems/barrierimpl.h>

Objects_Information _Barrier_Information;

/**
 *  @brief _Barrier_Manager_initialization
 */
static void _Barrier_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Barrier_Information,         /* object information table */
    OBJECTS_CLASSIC_API,           /* object API */
    OBJECTS_RTEMS_BARRIERS,        /* object class */
    Configuration_RTEMS_API.maximum_barriers,
                                   /* maximum objects of this class */
    sizeof( Barrier_Control ),     /* size of this object's control block */
    false,                         /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH,     /* maximum length of an object name */
    NULL                           /* Proxy extraction support callout */
  );
}

RTEMS_SYSINIT_ITEM(
  _Barrier_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_BARRIER,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
