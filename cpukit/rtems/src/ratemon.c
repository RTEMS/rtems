/*
 *  Rate Monotonic Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/rtems/ratemon.h>
#include <rtems/score/thread.h>

/*PAGE
 *
 *  _Rate_monotonic_Manager_initialization
 *
 *  This routine initializes all Rate Monotonic Manager related
 *  data structures.
 *
 *  Input parameters:
 *    maximum_periods - number of periods timers to initialize
 *
 *  Output parameters:  NONE
 *
 *  NOTE: The Rate Monotonic Manager is built on top of the Watchdog
 *        Handler.
 */

void _Rate_monotonic_Manager_initialization(
  unsigned32 maximum_periods
)
{
  _Objects_Initialize_information(
    &_Rate_monotonic_Information,     /* object information table */
    OBJECTS_CLASSIC_API,              /* object API */
    OBJECTS_RTEMS_PERIODS,            /* object class */
    maximum_periods,                  /* maximum objects of this class */
    sizeof( Rate_monotonic_Control ), /* size of this object's control block */
    FALSE,                            /* TRUE if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH         /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                     /* TRUE if this is a global object class */
    NULL                       /* Proxy extraction support callout */
#endif
  );
}
