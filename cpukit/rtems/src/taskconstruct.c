/**
 * @file
 *
 * @ingroup RTEMSImplClassicTask
 *
 * @brief This source file contains the implementation of
 *   rtems_task_construct() and the Task Manager system initialization.
 */

/*
 *  COPYRIGHT (c) 1989-2014,2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/modesimpl.h>
#include <rtems/rtems/support.h>
#include <rtems/rtems/statusimpl.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/sysinit.h>

#include <string.h>

static rtems_status_code _RTEMS_tasks_Prepare_user_stack(
  Thread_Configuration    *thread_config,
  const rtems_task_config *config
)
{
  size_t size;

  size = _TLS_Get_allocation_size();

  if ( config->maximum_thread_local_storage_size < size ) {
    return RTEMS_INVALID_SIZE;
  }

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( thread_config->is_fp ) {
    size += CONTEXT_FP_SIZE;
  }
#endif

  size += _Stack_Minimum();

  if ( config->storage_size < size ) {
    return RTEMS_INVALID_SIZE;
  }

  thread_config->stack_size = config->storage_size;
  thread_config->stack_area = config->storage_area;

  if ( config->storage_free != NULL ) {
    thread_config->stack_free = config->storage_free;
  } else {
    thread_config->stack_free = _Stack_Free_nothing;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code rtems_task_construct(
  const rtems_task_config *config,
  rtems_id                *id
)
{
  return _RTEMS_tasks_Create( config, id, _RTEMS_tasks_Prepare_user_stack );
}

rtems_status_code _RTEMS_tasks_Create(
  const rtems_task_config   *config,
  rtems_id                  *id,
  RTEMS_tasks_Prepare_stack  prepare_stack
)
{
  Thread_Control          *the_thread;
  Thread_Configuration     thread_config;
#if defined(RTEMS_MULTIPROCESSING)
  Objects_MP_Control      *the_global_object = NULL;
  bool                     is_global;
#endif
  rtems_status_code        status;
  rtems_attribute          attributes;
  bool                     valid;
  RTEMS_API_Control       *api;
  ASR_Information         *asr;

  if ( !rtems_is_name_valid( config->name ) ) {
    return RTEMS_INVALID_NAME;
  }

  if ( id == NULL ) {
    return RTEMS_INVALID_ADDRESS;
  }

  /*
   *  Core Thread Initialize insures we get the minimum amount of
   *  stack space.
   */

  /*
   *  Fix the attribute set to match the attributes which
   *  this processor (1) requires and (2) is able to support.
   *  First add in the required flags for attributes
   *  Typically this might include FP if the platform
   *  or application required all tasks to be fp aware.
   *  Then turn off the requested bits which are not supported.
   */

  attributes = _Attributes_Set( config->attributes, ATTRIBUTES_REQUIRED );
  attributes = _Attributes_Clear( attributes, ATTRIBUTES_NOT_SUPPORTED );

  memset( &thread_config, 0, sizeof( thread_config ) );
  thread_config.budget_algorithm = _Modes_Is_timeslice( config->initial_modes ) ?
    THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE
      : THREAD_CPU_BUDGET_ALGORITHM_NONE,
  thread_config.isr_level =  _Modes_Get_interrupt_level( config->initial_modes );
  thread_config.name = config->name;
  thread_config.is_fp = _Attributes_Is_floating_point( attributes );
  thread_config.is_preemptible = _Modes_Is_preempt( config->initial_modes );

  /*
   *  Validate the RTEMS API priority and convert it to the core priority range.
   */

  if ( !_Attributes_Is_system_task( attributes ) ) {
    if ( config->initial_priority == PRIORITY_MINIMUM ) {
      return RTEMS_INVALID_PRIORITY;
    }
  }

  thread_config.scheduler =
    _Thread_Scheduler_get_home( _Thread_Get_executing() );

  thread_config.priority = _RTEMS_Priority_To_core(
    thread_config.scheduler,
    config->initial_priority,
    &valid
  );
  if ( !valid ) {
    return RTEMS_INVALID_PRIORITY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( !_System_state_Is_multiprocessing ) {
    attributes = _Attributes_Clear( attributes, RTEMS_GLOBAL );
  }

  is_global = _Attributes_Is_global( attributes );
#endif

  /*
   *  Allocate the thread control block and -- if the task is global --
   *  allocate a global object control block.
   *
   *  NOTE:  This routine does not use the combined allocate and open
   *         global object routine (_Objects_MP_Allocate_and_open) because
   *         this results in a lack of control over when memory is allocated
   *         and can be freed in the event of an error.
   */
  the_thread = _RTEMS_tasks_Allocate();

  if ( !the_thread ) {
    _Objects_Allocator_unlock();
    return RTEMS_TOO_MANY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( is_global ) {
    the_global_object = _Objects_MP_Allocate_global_object();

    if ( the_global_object == NULL ) {
      _Objects_Free( &_RTEMS_tasks_Information.Objects, &the_thread->Object );
      _Objects_Allocator_unlock();
      return RTEMS_TOO_MANY;
    }
  }
#endif

  status = ( *prepare_stack )( &thread_config, config );

  /*
   *  Initialize the core thread for this task.
   */

  if ( status == RTEMS_SUCCESSFUL ) {
    Status_Control score_status;

    score_status = _Thread_Initialize(
      &_RTEMS_tasks_Information,
      the_thread,
      &thread_config
    );
    status = _Status_Get( score_status );
  } else {
    _Objects_Free( &_RTEMS_tasks_Information.Objects, &the_thread->Object );
  }

  if ( status != RTEMS_SUCCESSFUL ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( is_global )
      _Objects_MP_Free_global_object( the_global_object );
#endif
    _Objects_Allocator_unlock();
    return status;
  }

  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  asr->is_enabled = !_Modes_Is_asr_disabled( config->initial_modes );

  *id = the_thread->Object.id;

#if defined(RTEMS_MULTIPROCESSING)
  the_thread->is_global = is_global;
  if ( is_global ) {

    _Objects_MP_Open(
      &_RTEMS_tasks_Information.Objects,
      the_global_object,
      config->name,
      the_thread->Object.id
    );

    _RTEMS_tasks_MP_Send_process_packet(
      RTEMS_TASKS_MP_ANNOUNCE_CREATE,
      the_thread->Object.id,
      config->name
    );

   }
#endif

  _Objects_Allocator_unlock();
  return RTEMS_SUCCESSFUL;
}

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

#if defined(RTEMS_MULTIPROCESSING)
static void _RTEMS_tasks_Terminate_extension( Thread_Control *executing )
{
  if ( executing->is_global ) {
    _Objects_MP_Close(
      &_RTEMS_tasks_Information.Objects,
      executing->Object.id
    );
    _RTEMS_tasks_MP_Send_process_packet(
      RTEMS_TASKS_MP_ANNOUNCE_DELETE,
      executing->Object.id,
      0                                /* Not used */
    );
  }
}
#endif

static User_extensions_Control _RTEMS_tasks_User_extensions = {
  .Callouts = {
#if defined(RTEMS_MULTIPROCESSING)
    .thread_terminate = _RTEMS_tasks_Terminate_extension,
#endif
    .thread_start     = _RTEMS_tasks_Start_extension,
    .thread_restart   = _RTEMS_tasks_Start_extension
  }
};

static void _RTEMS_tasks_Manager_initialization( void )
{
  _Thread_Initialize_information( &_RTEMS_tasks_Information );
  _User_extensions_Add_API_set( &_RTEMS_tasks_User_extensions );
}

RTEMS_SYSINIT_ITEM(
  _RTEMS_tasks_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_TASKS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
