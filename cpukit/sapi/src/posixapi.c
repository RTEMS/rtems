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

#ifdef RTEMS_POSIX_API

#include <assert.h>

/*
 *  POSIX_API_INIT is defined so all of the POSIX API
 *  data will be included in this object file.
 */

#define POSIX_API_INIT

#include <rtems/system.h>

#include <sys/types.h>
#include <rtems/config.h>
#include <rtems/score/object.h>
#include <rtems/posix/cond.h>
#include <rtems/posix/config.h>
#include <rtems/posix/key.h>
#include <rtems/posix/mutex.h>
#include <rtems/posix/priority.h>
#include <rtems/posix/psignal.h>
#include <rtems/posix/pthread.h>
#include <rtems/posix/time.h>

/*PAGE
 *
 *  _POSIX_API_Initialize
 *
 *  XXX
 */

posix_api_configuration_table _POSIX_Default_configuration = {
  0,                             /* maximum_threads */
  0,                             /* maximum_mutexes */
  0,                             /* maximum_condition_variables */
  0,                             /* maximum_keys */
  0,                             /* number_of_initialization_tasks */
  NULL                           /* User_initialization_tasks_table */
};


void _POSIX_API_Initialize(
  rtems_configuration_table *configuration_table
)
{
  posix_api_configuration_table *api_configuration;

  /* XXX need to assert here based on size assumptions */

  assert( sizeof(pthread_t) == sizeof(Objects_Id) );

  api_configuration = configuration_table->POSIX_api_configuration;
  if ( !api_configuration ) 
    api_configuration = &_POSIX_Default_configuration;

  _POSIX_signals_Manager_Initialization();

  _POSIX_Threads_Manager_initialization(
    api_configuration->maximum_threads,
    api_configuration->number_of_initialization_tasks,
    api_configuration->User_initialization_tasks_table
  );
 
  _POSIX_Condition_variables_Manager_initialization(
    api_configuration->maximum_condition_variables
  );

  _POSIX_Key_Manager_initialization( api_configuration->maximum_keys );

  _POSIX_Mutex_Manager_initialization( 
    api_configuration->maximum_mutexes
  );

}

#endif
/* end of file */
