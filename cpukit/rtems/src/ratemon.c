/*
 *  Rate Monotonic Manager
 *
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
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>

/*
 *  _Rate_monotonic_Manager_initialization
 *
 *  This routine initializes all Rate Monotonic Manager related
 *  data structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 *
 *  NOTE: The Rate Monotonic Manager is built on top of the Watchdog
 *        Handler.
 */

void _Rate_monotonic_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Rate_monotonic_Information,    /* object information table */
    OBJECTS_CLASSIC_API,             /* object API */
    OBJECTS_RTEMS_PERIODS,           /* object class */
    Configuration_RTEMS_API.maximum_periods,
                                     /* maximum objects of this class */
    sizeof( Rate_monotonic_Control ),/* size of this object's control block */
    false,                           /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH        /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                           /* true if this is a global object class */
    NULL                             /* Proxy extraction support callout */
#endif
  );
}
