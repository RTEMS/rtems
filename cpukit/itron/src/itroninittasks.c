/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

#include <rtems/itron/task.h>

/*
 *  _ITRON_Task_Initialize_user_tasks
 *
 *  This routine creates and starts all configured user
 *  initialzation threads.
 */

void _ITRON_Task_Initialize_user_tasks_body( void )
{
  uint32_t                          index;
  uint32_t                          maximum;
  ER                                return_value;
  itron_initialization_tasks_table *user_tasks;

  /*
   *  NOTE:  This is slightly different from the Ada implementation.
   */

  user_tasks = _ITRON_Task_User_initialization_tasks;
  maximum    = _ITRON_Task_Number_of_initialization_tasks;

  if ( !user_tasks || maximum == 0 )
    return;

  for ( index=0 ; index < maximum ; index++ ) {

    return_value = cre_tsk(
       user_tasks[ index ].id,
       &user_tasks[ index ].attributes
    );

    if ( return_value != E_OK )
      _Internal_error_Occurred( INTERNAL_ERROR_ITRON_API, TRUE, return_value );

    return_value = sta_tsk( user_tasks[ index ].id, 0 );

    if ( return_value != E_OK )
      _Internal_error_Occurred( INTERNAL_ERROR_ITRON_API, TRUE, return_value );

  }
}
