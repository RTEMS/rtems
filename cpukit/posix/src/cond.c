/*
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

#include <pthread.h>
#include <errno.h>
#include <limits.h>

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/time.h>
#include <rtems/posix/mutex.h>

/*
 *  _POSIX_Condition_variables_Manager_initialization
 *
 *  This routine initializes all condition variable manager related data
 *  structures.
 *
 *  Input parameters:   NONE
 *
 *  Output parameters:  NONE
 */

void _POSIX_Condition_variables_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_POSIX_Condition_variables_Information, /* object information table */
    OBJECTS_POSIX_API,                       /* object API */
    OBJECTS_POSIX_CONDITION_VARIABLES,       /* object class */
    Configuration_POSIX_API.maximum_condition_variables,
                                /* maximum objects of this class */
    sizeof( POSIX_Condition_variables_Control ),
                                /* size of this object's control block */
    true,                       /* true if names for this object are strings */
    _POSIX_PATH_MAX             /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    false,                      /* true if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );
}
