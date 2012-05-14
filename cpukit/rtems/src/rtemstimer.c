/*
 *  Timer Manager
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
#include <rtems/score/object.h>
#include <rtems/score/thread.h>
#include <rtems/rtems/timer.h>
#include <rtems/score/tod.h>
#include <rtems/score/watchdog.h>

/*
 *  _Timer_Manager_initialization
 *
 *  This routine initializes all timer manager related data structures.
 *
 *  Input parameters: NONE
 *
 *  Output parameters:  NONE
 */

void _Timer_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_Timer_Information,       /* object information table */
    OBJECTS_CLASSIC_API,       /* object API */
    OBJECTS_RTEMS_TIMERS,      /* object class */
    Configuration_RTEMS_API.maximum_timers ,
                               /* maximum objects of this class */
    sizeof( Timer_Control ),   /* size of this object's control block */
    false,                     /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH  /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                     /* true if this is a global object class */
    NULL                       /* Proxy extraction support callout */
#endif
  );

  /*
   *  Initialize the pointer to the default timer server control block to NULL
   *  indicating that task-based timer support is not initialized.
   */

  _Timer_server = NULL;
}
