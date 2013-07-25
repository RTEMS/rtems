/**
 *  @file
 *
 *  @brief RTEMS Task API Extensions
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2009.
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
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/stack.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/score/thread.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>

/*
 *  _RTEMS_tasks_Create_extension
 *
 *  This routine is an extension routine that is invoked as part
 *  of creating any type of task or thread in the system.  If the
 *  task is created via another API, then this routine is invoked
 *  and this API given the opportunity to initialize its extension
 *  area.
 */

static bool _RTEMS_tasks_Create_extension(
  Thread_Control *executing,
  Thread_Control *created
)
{
  RTEMS_API_Control *api;
  int                i;
  size_t             to_allocate;

  /*
   *  Notepads must be the last entry in the structure and they
   *  can be left off if disabled in the configuration.
   */
  to_allocate = sizeof( RTEMS_API_Control );
  if ( !rtems_configuration_get_notepads_enabled() )
    to_allocate -= (RTEMS_NUMBER_NOTEPADS * sizeof(uint32_t));

  api = _Workspace_Allocate( to_allocate );

  if ( !api )
    return false;

  created->API_Extensions[ THREAD_API_RTEMS ] = api;

  _Event_Initialize( &api->Event );
  _Event_Initialize( &api->System_event );
  _ASR_Initialize( &api->Signal );
  created->task_variables = NULL;

  if ( rtems_configuration_get_notepads_enabled() ) {
    for (i=0; i < RTEMS_NUMBER_NOTEPADS; i++)
      api->Notepads[i] = 0;
  }

  return true;
}

/*
 *  _RTEMS_tasks_Start_extension
 *
 *  This extension routine is invoked when a task is started for the
 *  first time.
 */

static void _RTEMS_tasks_Start_extension(
  Thread_Control *executing,
  Thread_Control *started
)
{
  RTEMS_API_Control *api;

  api = started->API_Extensions[ THREAD_API_RTEMS ];

  _Event_Initialize( &api->Event );
  _Event_Initialize( &api->System_event );
}

/*
 *  _RTEMS_tasks_Delete_extension
 *
 *  This extension routine is invoked when a task is deleted.
 */

static void _RTEMS_tasks_Delete_extension(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  rtems_task_variable_t *tvp, *next;

  /*
   *  Free per task variable memory
   */

  tvp = deleted->task_variables;
  deleted->task_variables = NULL;
  while (tvp) {
    next = (rtems_task_variable_t *)tvp->next;
    _RTEMS_Tasks_Invoke_task_variable_dtor( deleted, tvp );
    tvp = next;
  }

  /*
   *  Free API specific memory
   */

  (void) _Workspace_Free( deleted->API_Extensions[ THREAD_API_RTEMS ] );
  deleted->API_Extensions[ THREAD_API_RTEMS ] = NULL;
}

/*
 *  _RTEMS_tasks_Switch_extension
 *
 *  This extension routine is invoked at each context switch.
 */

static void _RTEMS_tasks_Switch_extension(
  Thread_Control *executing,
  Thread_Control *heir
)
{
  rtems_task_variable_t *tvp;

  /*
   *  Per Task Variables
   */

  tvp = executing->task_variables;
  while (tvp) {
    tvp->tval = *tvp->ptr;
    *tvp->ptr = tvp->gval;
    tvp = (rtems_task_variable_t *)tvp->next;
  }

  tvp = heir->task_variables;
  while (tvp) {
    tvp->gval = *tvp->ptr;
    *tvp->ptr = tvp->tval;
    tvp = (rtems_task_variable_t *)tvp->next;
  }
}

API_extensions_Control _RTEMS_tasks_API_extensions = {
  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    .predriver_hook = NULL,
  #endif
  .postdriver_hook = _RTEMS_tasks_Initialize_user_tasks
};

User_extensions_Control _RTEMS_tasks_User_extensions = {
  { NULL, NULL },
  { { NULL, NULL }, _RTEMS_tasks_Switch_extension },
  { _RTEMS_tasks_Create_extension,            /* create */
    _RTEMS_tasks_Start_extension,             /* start */
    _RTEMS_tasks_Start_extension,             /* restart */
    _RTEMS_tasks_Delete_extension,            /* delete */
    _RTEMS_tasks_Switch_extension,            /* switch */
    NULL,                                     /* begin */
    NULL,                                     /* exitted */
    NULL                                      /* fatal */
  }
};

void _RTEMS_tasks_Manager_initialization(void)
{
  _Objects_Initialize_information(
    &_RTEMS_tasks_Information, /* object information table */
    OBJECTS_CLASSIC_API,       /* object API */
    OBJECTS_RTEMS_TASKS,       /* object class */
    Configuration_RTEMS_API.maximum_tasks,
                               /* maximum objects of this class */
    sizeof( Thread_Control ),  /* size of this object's control block */
    false,                     /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH  /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    true,                      /* true if this is a global object class */
    NULL                       /* Proxy extraction support callout */
#endif
  );

  /*
   *  Add all the extensions for this API
   */

  _User_extensions_Add_API_set( &_RTEMS_tasks_User_extensions );

  _API_extensions_Add( &_RTEMS_tasks_API_extensions );

  /*
   *  Register the MP Process Packet routine.
   */

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_TASKS,
    _RTEMS_tasks_MP_Process_packet
  );
#endif

}

void _RTEMS_tasks_Initialize_user_tasks( void )
{
  if ( _RTEMS_tasks_Initialize_user_tasks_p )
    (*_RTEMS_tasks_Initialize_user_tasks_p)();
}
