/**
 *  @file
 *
 *  @brief RTEMS Task API Extensions
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/system.h>
#include <rtems/config.h>
#include <rtems/rtems/asrimpl.h>
#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/signalimpl.h>
#include <rtems/rtems/status.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/modes.h>
#include <rtems/rtems/tasksimpl.h>
#include <rtems/posix/keyimpl.h>
#include <rtems/score/stack.h>
#include <rtems/score/threadimpl.h>
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
  size_t             i;

  api = created->API_Extensions[ THREAD_API_RTEMS ];

  _ASR_Create( &api->Signal );
  _Thread_Action_initialize( &api->Signal_action, _Signal_Action_handler );
#if !defined(RTEMS_SMP)
  created->task_variables = NULL;
#endif

  /*
   * We know this is deprecated and don't want a warning on every BSP built.
   */
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
  if ( rtems_configuration_get_notepads_enabled() ) {
    for (i=0; i < RTEMS_NUMBER_NOTEPADS; i++)
      api->Notepads[i] = 0;
  }
  #pragma GCC diagnostic pop

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

static void _RTEMS_tasks_Delete_extension(
  Thread_Control *executing,
  Thread_Control *deleted
)
{
  RTEMS_API_Control *api;

  api = deleted->API_Extensions[ THREAD_API_RTEMS ];

  _ASR_Destroy( &api->Signal );
}

static void _RTEMS_tasks_Terminate_extension(
  Thread_Control *executing
)
{
  /*
   *  Free per task variable memory
   *
   *  Per Task Variables are only enabled in uniprocessor configurations.
   */
  #if !defined(RTEMS_SMP)
    /*
     * We know this is deprecated and don't want a warning on every BSP built.
     */
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    do { 
      rtems_task_variable_t *tvp, *next;

      tvp = executing->task_variables;
      executing->task_variables = NULL;
      while (tvp) {
	next = (rtems_task_variable_t *)tvp->next;
	_RTEMS_Tasks_Invoke_task_variable_dtor( executing, tvp );
	tvp = next;
      }
    } while (0);
    #pragma GCC diagnostic pop
  #endif

  /*
   *  Run all the key destructors
   */
  _POSIX_Keys_Run_destructors( executing );
}

#if !defined(RTEMS_SMP)
/*
 *  _RTEMS_tasks_Switch_extension
 *
 *  This extension routine is invoked at each context switch.
 *
 *  @note Since this only needs to address per-task variables, it is
 *        disabled entirely for SMP configurations.
 */
static void _RTEMS_tasks_Switch_extension(
  Thread_Control *executing,
  Thread_Control *heir
)
{
  rtems_task_variable_t *tvp;

  /*
   *  Per Task Variables are only enabled in uniprocessor configurations
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
#define RTEMS_TASKS_SWITCH_EXTENSION _RTEMS_tasks_Switch_extension
#else 
#define RTEMS_TASKS_SWITCH_EXTENSION NULL
#endif

API_extensions_Control _RTEMS_tasks_API_extensions = {
  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    .predriver_hook = NULL,
  #endif
  .postdriver_hook = _RTEMS_tasks_Initialize_user_tasks
};

User_extensions_Control _RTEMS_tasks_User_extensions = {
  { NULL, NULL },
  { { NULL, NULL }, RTEMS_TASKS_SWITCH_EXTENSION },
  { _RTEMS_tasks_Create_extension,            /* create */
    _RTEMS_tasks_Start_extension,             /* start */
    _RTEMS_tasks_Start_extension,             /* restart */
    _RTEMS_tasks_Delete_extension,            /* delete */
    RTEMS_TASKS_SWITCH_EXTENSION,             /* switch */
    NULL,                                     /* begin */
    NULL,                                     /* exitted */
    NULL,                                     /* fatal */
    _RTEMS_tasks_Terminate_extension          /* terminate */
  }
};

void _RTEMS_tasks_Manager_initialization(void)
{
  _Thread_Initialize_information(
    &_RTEMS_tasks_Information, /* object information table */
    OBJECTS_CLASSIC_API,       /* object API */
    OBJECTS_RTEMS_TASKS,       /* object class */
    Configuration_RTEMS_API.maximum_tasks,
                               /* maximum objects of this class */
    false,                     /* true if the name is a string */
    RTEMS_MAXIMUM_NAME_LENGTH  /* maximum length of an object name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    true                       /* true if this is a global object class */
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
