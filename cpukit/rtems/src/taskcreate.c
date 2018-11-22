/**
 *  @file
 *
 *  @brief RTEMS Task Create
 *  @ingroup ClassicTasks
 */

/*
 *  COPYRIGHT (c) 1989-2014,2016.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/rtems/tasksimpl.h>
#include <rtems/rtems/attrimpl.h>
#include <rtems/rtems/eventimpl.h>
#include <rtems/rtems/modesimpl.h>
#include <rtems/rtems/support.h>
#include <rtems/score/apimutex.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/userextimpl.h>
#include <rtems/sysinit.h>

rtems_status_code rtems_task_create(
  rtems_name           name,
  rtems_task_priority  initial_priority,
  size_t               stack_size,
  rtems_mode           initial_modes,
  rtems_attribute      attribute_set,
  rtems_id            *id
)
{
  Thread_Control          *the_thread;
  const Scheduler_Control *scheduler;
  bool                     is_fp;
#if defined(RTEMS_MULTIPROCESSING)
  Objects_MP_Control      *the_global_object = NULL;
  bool                     is_global;
#endif
  bool                     status;
  rtems_attribute          the_attribute_set;
  bool                     valid;
  Priority_Control         priority;
  RTEMS_API_Control       *api;
  ASR_Information         *asr;


  if ( !id )
   return RTEMS_INVALID_ADDRESS;

  if ( !rtems_is_name_valid( name ) )
    return RTEMS_INVALID_NAME;

  /*
   *  Core Thread Initialize insures we get the minimum amount of
   *  stack space.
   */

  /*
   *  Fix the attribute set to match the attributes which
   *  this processor (1) requires and (2) is able to support.
   *  First add in the required flags for attribute_set
   *  Typically this might include FP if the platform
   *  or application required all tasks to be fp aware.
   *  Then turn off the requested bits which are not supported.
   */

  the_attribute_set = _Attributes_Set( attribute_set, ATTRIBUTES_REQUIRED );
  the_attribute_set =
    _Attributes_Clear( the_attribute_set, ATTRIBUTES_NOT_SUPPORTED );

  if ( _Attributes_Is_floating_point( the_attribute_set ) )
    is_fp = true;
  else
    is_fp = false;

  /*
   *  Validate the RTEMS API priority and convert it to the core priority range.
   */

  if ( !_Attributes_Is_system_task( the_attribute_set ) ) {
    if ( initial_priority == PRIORITY_MINIMUM ) {
      return RTEMS_INVALID_PRIORITY;
    }
  }

  scheduler = _Thread_Scheduler_get_home( _Thread_Get_executing() );

  priority = _RTEMS_Priority_To_core( scheduler, initial_priority, &valid );
  if ( !valid ) {
    return RTEMS_INVALID_PRIORITY;
  }

#if defined(RTEMS_MULTIPROCESSING)
  if ( _Attributes_Is_global( the_attribute_set ) ) {

    is_global = true;

    if ( !_System_state_Is_multiprocessing )
      return RTEMS_MP_NOT_CONFIGURED;

  } else
    is_global = false;
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

    if ( _Objects_MP_Is_null_global_object( the_global_object ) ) {
      _RTEMS_tasks_Free( the_thread );
      _Objects_Allocator_unlock();
      return RTEMS_TOO_MANY;
    }
  }
#endif

  /*
   *  Initialize the core thread for this task.
   */

  status = _Thread_Initialize(
    &_RTEMS_tasks_Information,
    the_thread,
    scheduler,
    NULL,
    stack_size,
    is_fp,
    priority,
    _Modes_Is_preempt(initial_modes)   ? true : false,
    _Modes_Is_timeslice(initial_modes) ?
      THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE :
      THREAD_CPU_BUDGET_ALGORITHM_NONE,
    NULL,        /* no budget algorithm callout */
    _Modes_Get_interrupt_level(initial_modes),
    (Objects_Name) name
  );

  if ( !status ) {
#if defined(RTEMS_MULTIPROCESSING)
    if ( is_global )
      _Objects_MP_Free_global_object( the_global_object );
#endif
    _RTEMS_tasks_Free( the_thread );
    _Objects_Allocator_unlock();
    return RTEMS_UNSATISFIED;
  }

  api = the_thread->API_Extensions[ THREAD_API_RTEMS ];
  asr = &api->Signal;

  asr->is_enabled = _Modes_Is_asr_disabled(initial_modes) ? false : true;

  *id = the_thread->Object.id;

#if defined(RTEMS_MULTIPROCESSING)
  the_thread->is_global = is_global;
  if ( is_global ) {

    _Objects_MP_Open(
      &_RTEMS_tasks_Information.Objects,
      the_global_object,
      name,
      the_thread->Object.id
    );

    _RTEMS_tasks_MP_Send_process_packet(
      RTEMS_TASKS_MP_ANNOUNCE_CREATE,
      the_thread->Object.id,
      name
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

static void _RTEMS_tasks_Manager_initialization(void)
{
  _Thread_Initialize_information( &_RTEMS_tasks_Information );
  _User_extensions_Add_API_set( &_RTEMS_tasks_User_extensions );

#if defined(RTEMS_MULTIPROCESSING)
  _MPCI_Register_packet_processor(
    MP_PACKET_TASKS,
    _RTEMS_tasks_MP_Process_packet
  );
#endif
}

RTEMS_SYSINIT_ITEM(
  _RTEMS_tasks_Manager_initialization,
  RTEMS_SYSINIT_CLASSIC_TASKS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);
