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

#include <itron.h>

#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

#include <rtems/itron/task.h>

/*
 *  _ITRON_Task_Create_extension
 *
 *  This routine is an extension routine that is invoked as part
 *  of creating any type of task or thread in the system.  If the
 *  task is created via another API, then this routine is invoked
 *  and this API given the opportunity to initialize its extension
 *  area.
 */

boolean _ITRON_Task_Create_extension(
  Thread_Control *executing,
  Thread_Control *created
)
{
  ITRON_API_Control *api;

  api = _Workspace_Allocate( sizeof( ITRON_API_Control ) );

  if ( !api )
    return FALSE;

  created->API_Extensions[ THREAD_API_ITRON ] = api;

  /*
   *  Initialize the ITRON API extension
   */

  return TRUE;
}

/*
 *  _ITRON_Task_Delete_extension
 *
 *  This extension routine is invoked when a task is deleted.
 */

User_extensions_routine _ITRON_Task_Delete_extension(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  (void) _Workspace_Free( deleted->API_Extensions[ THREAD_API_ITRON ] );
 
  deleted->API_Extensions[ THREAD_API_ITRON ] = NULL;
}

/*
 *  _ITRON_Task_Initialize_user_tasks
 *
 *  This routine creates and starts all configured user
 *  initialzation threads.
 */

void _ITRON_Task_Initialize_user_tasks( void )
{
  unsigned32                        index;
  unsigned32                        maximum;
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

/*PAGE
 *
 *  _ITRON_Delete_task
 */

ER _ITRON_Delete_task(
  Thread_Control *the_thread
)
{
  Objects_Information     *the_information;

  the_information = _Objects_Get_information( the_thread->Object.id );
  if ( !the_information ) {
    return E_OBJ;             /* XXX - should never happen */
  }

  _Thread_Close( the_information, the_thread );

  _ITRON_Task_Free( the_thread );

  return E_OK;
}

/*
 *  At this point in time, the ITRON API does not need any other 
 *  extensions.  See the POSIX and RTEMS API extensions for 
 *  examples of how they can be used.
 */

/*
 *  Extension Tables
 */

API_extensions_Control _ITRON_Task_API_extensions = {
  { NULL, NULL },
  NULL,                                     /* predriver */
  _ITRON_Task_Initialize_user_tasks,       /* postdriver */
  NULL                                      /* post switch */
};

User_extensions_Control _ITRON_Task_User_extensions = {
  { NULL, NULL },
  { { NULL, NULL }, NULL },
  { _ITRON_Task_Create_extension,             /* create */
    NULL,                                     /* start */
    NULL,                                     /* restart */
    _ITRON_Task_Delete_extension,             /* delete */
    NULL,                                     /* switch */
    NULL,                                     /* begin */
    NULL,                                     /* exitted */
    NULL                                      /* fatal */
  }
};

/*
 *  _ITRON_Task_Manager_initialization
 *
 *  This routine initializes all Task Manager related data structures.
 *
 *  Input parameters:
 *    maximum_tasks       - number of tasks to initialize
 *
 *  Output parameters:  NONE
 */

void _ITRON_Task_Manager_initialization(
  unsigned32                        maximum_tasks,
  unsigned32                        number_of_initialization_tasks,
  itron_initialization_tasks_table *user_tasks
)
{

  _ITRON_Task_Number_of_initialization_tasks = number_of_initialization_tasks;
  _ITRON_Task_User_initialization_tasks = user_tasks;

  /*
   *  There may not be any ITRON_initialization tasks configured.
   */

#if 0
  if ( user_tasks == NULL || number_of_initialization_tasks == 0 )
    _Internal_error_Occurred( INTERNAL_ERROR_ITRON_API, TRUE, -1 );
#endif

  _Objects_Initialize_information(
    &_ITRON_Task_Information,   /* object information table */
    OBJECTS_ITRON_API,          /* object API */
    OBJECTS_ITRON_TASKS,        /* object class */
    maximum_tasks,              /* maximum objects of this class */
    sizeof( Thread_Control ),   /* size of this object's control block */
    FALSE,                      /* TRUE if names for this object are strings */
    ITRON_MAXIMUM_NAME_LENGTH   /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                      /* TRUE if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );

  /*
   *  Add all the extensions for this API
   */

  _User_extensions_Add_API_set( &_ITRON_Task_User_extensions );

  _API_extensions_Add( &_ITRON_Task_API_extensions );

  /*
   *  XXX MP not supported
   *  Register the MP Process Packet routine.
   */

}
