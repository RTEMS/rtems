/*
 *  RTEMS API Initialization Support
 *
 *  NOTE:
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */

#include <assert.h>

#include <rtems/system.h>

#define INIT

#include <rtems/posix/cond.h>
#include <rtems/posix/condmp.h>
#include <rtems/posix/config.h>
#include <rtems/posix/key.h>
#include <rtems/posix/mutex.h>
#include <rtems/posix/mutexmp.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/pthreadmp.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_API_Initialize
 *
 *  XXX
 */

void _POSIX_API_Initialize(
  rtems_configuration_table *configuration_table
)
{
  posix_api_configuration_table *api_configuration;

  api_configuration = configuration_table->POSIX_api_configuration;

  assert( api_configuration );

  _RTEMS_tasks_Manager_initialization(
    api_configuration->maximum_tasks
#if 0
,
    api_configuration->number_of_initialization_tasks,
    api_configuration->User_initialization_tasks_table
#endif
  );
 
  _POSIX_Condition_variables_Manager_initialization(
    api_configuration->maximum_condition_variables
  );

  void _POSIX_Key_Manager_initialization( api_configuration->maximum_keys );

  _POSIX_Mutex_Manager_initialization( 
    api_configuration->maximum_mutexes
  );

}

/* end of file */
