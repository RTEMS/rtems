/*
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <pthread.h>
#include <errno.h>

#include <rtems/system.h>
#include <rtems/score/object.h>
#include <rtems/score/states.h>
#include <rtems/score/watchdog.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/time.h>
#include <rtems/posix/mutex.h>

/*PAGE
 *
 *  _POSIX_Condition_variables_Manager_initialization
 *
 *  This routine initializes all condition variable manager related data 
 *  structures.
 *
 *  Input parameters:
 *    maximum_condition_variables - maximum configured condition_variables
 *
 *  Output parameters:  NONE
 */
 
void _POSIX_Condition_variables_Manager_initialization(
  unsigned32 maximum_condition_variables
)
{
  _Objects_Initialize_information(
    &_POSIX_Condition_variables_Information, /* object information table */
    OBJECTS_POSIX_API,                       /* object API */
    OBJECTS_POSIX_CONDITION_VARIABLES,       /* object class */
    maximum_condition_variables,             /* maximum objects of this class */
    sizeof( POSIX_Condition_variables_Control ),
                                /* size of this object's control block */
    FALSE,                      /* TRUE if names for this object are strings */
    0                           /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                      /* TRUE if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  ); 
}
