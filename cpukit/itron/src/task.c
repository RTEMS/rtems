/*
 *  COPYRIGHT (c) 1989-2008.
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
#include <rtems/config.h>
#include <rtems/score/thread.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/sysstate.h>

#include <rtems/itron/task.h>

#if 0
/*
 *  _ITRON_Task_Create_extension
 *
 *  This routine is an extension routine that is invoked as part
 *  of creating any type of task or thread in the system.  If the
 *  task is created via another API, then this routine is invoked
 *  and this API given the opportunity to initialize its extension
 *  area.
 */

bool _ITRON_Task_Create_extension(
  Thread_Control *executing,
  Thread_Control *created
)
{
  /*
   *  Until we actually put data in this structure, do not even
   *  allocate it.
   */
#if 0
  ITRON_API_Control *api;

  api = _Workspace_Allocate( sizeof( ITRON_API_Control ) );

  if ( !api )
    return false;

  created->API_Extensions[ THREAD_API_ITRON ] = api;
#else
  created->API_Extensions[ THREAD_API_ITRON ] = NULL;
#endif

  /*
   *  Initialize the ITRON API extension
   */

  return true;
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
  /*
   *  Until we actually put data in this structure, do not even
   *  allocate it.
   */
#if 0
  (void) _Workspace_Free( deleted->API_Extensions[ THREAD_API_ITRON ] );

  deleted->API_Extensions[ THREAD_API_ITRON ] = NULL;
#endif
}
#endif

/*
 *  _ITRON_Task_Initialize_user_tasks
 *
 *  This routine creates and starts all configured user
 *  initialzation threads.
 */

void _ITRON_Task_Initialize_user_tasks( void )
{
  if ( _ITRON_Initialize_user_tasks_p )
    (*_ITRON_Initialize_user_tasks_p)();
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

/*
 *  Until ITRON needs this, do not even declare it.
 */
#if 0
User_extensions_Control _ITRON_Task_User_extensions = {
  { NULL, NULL },
  { { NULL, NULL }, NULL },
  {
    _ITRON_Task_Create_extension,             /* create */
    NULL,                                     /* start */
    NULL,                                     /* restart */
    _ITRON_Task_Delete_extension,             /* delete */
    NULL,                                     /* switch */
    NULL,                                     /* begin */
    NULL,                                     /* exitted */
    NULL                                      /* fatal */
  }
};
#endif

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

void _ITRON_Task_Manager_initialization(void)
{

  _Objects_Initialize_information(
    &_ITRON_Task_Information,   /* object information table */
    OBJECTS_ITRON_API,          /* object API */
    OBJECTS_ITRON_TASKS,        /* object class */
    Configuration_ITRON_API.maximum_tasks,
                                /* maximum objects of this class */
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
   *  Until ITRON needs this, do not even declare it.
   */
  #if 0
    /*
     *  Add all the extensions for this API
     */
    _User_extensions_Add_API_set( &_ITRON_Task_User_extensions );
  #endif

  _API_extensions_Add( &_ITRON_Task_API_extensions );

}
