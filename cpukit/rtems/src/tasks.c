/*
 *  RTEMS Task Manager
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#include <rtems/system.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/score/object.h>
#include <rtems/score/stack.h>
#include <rtems/score/states.h>
#include <rtems/rtems/tasks.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/tod.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/apiext.h>
#include <rtems/score/sysstate.h>

/*PAGE
 *
 *  _RTEMS_tasks_Create_extension
 *
 *  This routine is an extension routine that is invoked as part
 *  of creating any type of task or thread in the system.  If the
 *  task is created via another API, then this routine is invoked
 *  and this API given the opportunity to initialize its extension
 *  area.
 */

boolean _RTEMS_tasks_Create_extension(
  Thread_Control *executing, 
  Thread_Control *created
)
{
  RTEMS_API_Control *api;

  api = _Workspace_Allocate( sizeof( RTEMS_API_Control ) );

  if ( !api )
    return FALSE;

  created->API_Extensions[ THREAD_API_RTEMS ] = api;
 
  api->pending_events = EVENT_SETS_NONE_PENDING;
  _ASR_Initialize( &api->Signal );
  created->task_variables = NULL;
  return TRUE;
}

/*PAGE
 *
 *  _RTEMS_tasks_Start_extension
 *
 *  This extension routine is invoked when a task is started for the
 *  first time.
 */
 
User_extensions_routine _RTEMS_tasks_Start_extension(
  Thread_Control *executing,
  Thread_Control *started
)
{
  RTEMS_API_Control *api;

  api = started->API_Extensions[ THREAD_API_RTEMS ];
 
  api->pending_events = EVENT_SETS_NONE_PENDING;

  _ASR_Initialize( &api->Signal );
}

/*PAGE
 *
 *  _RTEMS_tasks_Delete_extension
 *
 *  This extension routine is invoked when a task is deleted.
 */
 
User_extensions_routine _RTEMS_tasks_Delete_extension(
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
    next = tvp->next;
    if  (tvp->dtor)
        (*tvp->dtor)(*tvp->ptr );
    if (executing == deleted)
        *tvp->ptr = tvp->gval;
    _Workspace_Free( tvp );
    tvp = next;
  }

  /*
   *  Free API specific memory
   */

  (void) _Workspace_Free( deleted->API_Extensions[ THREAD_API_RTEMS ] );
  deleted->API_Extensions[ THREAD_API_RTEMS ] = NULL;
}

/*PAGE
 *
 *  _RTEMS_tasks_Switch_extension
 *
 *  This extension routine is invoked at each context switch.
 */
 
void _RTEMS_tasks_Switch_extension(
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
    tvp = tvp->next;
  }

  tvp = heir->task_variables;
  while (tvp) {
    tvp->gval = *tvp->ptr;
    *tvp->ptr = tvp->tval;
    tvp = tvp->next;
  } 
}

/*PAGE
 *
 *  _RTEMS_tasks_Post_switch_extension
 *
 *  This extension routine is invoked at each context switch.
 */
 
void _RTEMS_tasks_Post_switch_extension(
  Thread_Control *executing
)
{
  ISR_Level          level;
  RTEMS_API_Control *api;
  ASR_Information   *asr;
  rtems_signal_set   signal_set;
  Modes_Control      prev_mode;

  api = executing->API_Extensions[ THREAD_API_RTEMS ];

  /*
   *  Signal Processing
   */

  asr = &api->Signal;
 
  _ISR_Disable( level );
    signal_set = asr->signals_posted;
    asr->signals_posted = 0;
  _ISR_Enable( level );
 
 
  if ( !signal_set ) /* similar to _ASR_Are_signals_pending( asr ) */
    return;
 
  asr->nest_level += 1;
  rtems_task_mode( asr->mode_set, RTEMS_ALL_MODE_MASKS, &prev_mode );
 
  (*asr->handler)( signal_set );

  asr->nest_level -= 1;
  rtems_task_mode( prev_mode, RTEMS_ALL_MODE_MASKS, &prev_mode );

}

API_extensions_Control _RTEMS_tasks_API_extensions = {
  { NULL, NULL },
  NULL,                                     /* predriver */
  _RTEMS_tasks_Initialize_user_tasks,       /* postdriver */
  _RTEMS_tasks_Post_switch_extension        /* post switch */
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

/*PAGE
 *
 *  _RTEMS_tasks_Manager_initialization
 *
 *  This routine initializes all Task Manager related data structures.
 *
 *  Input parameters:
 *    maximum_tasks       - number of tasks to initialize
 *
 *  Output parameters:  NONE
 */

void _RTEMS_tasks_Manager_initialization(
  unsigned32                        maximum_tasks,
  unsigned32                        number_of_initialization_tasks,
  rtems_initialization_tasks_table *user_tasks
)
{

  _RTEMS_tasks_Number_of_initialization_tasks = number_of_initialization_tasks;
  _RTEMS_tasks_User_initialization_tasks = user_tasks;

  /*
   *  There may not be any RTEMS initialization tasks configured.
   */

#if 0
  if ( user_tasks == NULL || number_of_initialization_tasks == 0 )
    _Internal_error_Occurred( INTERNAL_ERROR_RTEMS_API, TRUE, RTEMS_TOO_MANY );
#endif

  _Objects_Initialize_information(
    &_RTEMS_tasks_Information, /* object information table */
    OBJECTS_CLASSIC_API,       /* object API */
    OBJECTS_RTEMS_TASKS,       /* object class */
    maximum_tasks,             /* maximum objects of this class */
    sizeof( Thread_Control ),  /* size of this object's control block */
    FALSE,                     /* TRUE if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH  /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    TRUE,                      /* TRUE if this is a global object class */
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

