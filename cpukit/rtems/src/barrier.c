/*
 *  Barrier Manager
 *
 *  DESCRIPTION:
 *
 *  This package is the implementation of the Barrier Manager.
 *
 *  Directives provided are:
 *
 *     + create a barrier
 *     + get an ID of a barrier
 *     + delete a barrier
 *     + acquire a barrier
 *     + release a barrier
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/score/object.h>
#include <rtems/rtems/barrier.h>
#if defined(RTEMS_MULTIPROCESSING)
#include <rtems/score/mpci.h>
#endif

/**
 *  @brief _Barrier_Manager_initialization
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _Barrier_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Barrier_Information,         /* object information table */
    OBJECTS_CLASSIC_API,           /* object API */
    OBJECTS_RTEMS_BARRIERS,        /* object class */
    Configuration_RTEMS_API.maximum_barriers,
                                   /* maximum objects of this class */
    sizeof( Barrier_Control ),     /* size of this object's control block */
    false,                         /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH      /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                         /* true if this is a global object class */
    NULL                           /* Proxy extraction support callout */
#endif
  );
}
