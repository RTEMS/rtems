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
    &_POSIX_Condition_variables_Information,
    OBJECTS_POSIX_CONDITION_VARIABLES,
    TRUE,
    maximum_condition_variables,
    sizeof( POSIX_Condition_variables_Control ),
    FALSE,
    0,
    FALSE
  );
}
