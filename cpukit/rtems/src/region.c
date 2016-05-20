/**
 * @file
 *
 * @brief _Region_Manager_initialization
 * @ingroup ClassicRegion Regions
 */

/*
 *  COPYRIGHT (c) 1989-2013.
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
#include <rtems/rtems/options.h>
#include <rtems/rtems/regionimpl.h>

Objects_Information _Region_Information;

/*
 *  _Region_Manager_initialization
 *
 *  This routine initializes all region manager related data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

static void _Region_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Region_Information,      /* object information table */
    OBJECTS_CLASSIC_API,       /* object API */
    OBJECTS_RTEMS_REGIONS,     /* object class */
    Configuration_RTEMS_API.maximum_regions,
                               /* maximum objects of this class */
    sizeof( Region_Control ),  /* size of this object's control block */
    false,                     /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH, /* maximum length of an object name */
    NULL                       /* Proxy extraction support callout */
  );
}

RTEMS_SYSINIT_ITEM(
  _Region_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_REGION,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
