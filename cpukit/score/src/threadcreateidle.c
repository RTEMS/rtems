/**
 *  @file
 *
 *  @brief Create Idle Thread
 *  @ingroup RTEMSScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userextimpl.h>
#include <rtems/config.h>

static void _Thread_Create_idle_for_CPU( Per_CPU_Control *cpu )
{
  Objects_Name             name;
  Thread_Control          *idle;
  const Scheduler_Control *scheduler;

  scheduler = _Scheduler_Get_by_CPU( cpu );

#if defined(RTEMS_SMP)
  if (scheduler == NULL) {
    return;
  }
#endif

  name.name_u32 = _Objects_Build_name( 'I', 'D', 'L', 'E' );

  /*
   *  The entire workspace is zeroed during its initialization.  Thus, all
   *  fields not explicitly assigned were explicitly zeroed by
   *  _Workspace_Initialization.
   */
  idle = _Thread_Internal_allocate();
  _Assert( idle != NULL );

  _Thread_Initialize(
    &_Thread_Information,
    idle,
    scheduler,
    NULL,        /* allocate the stack */
    _Stack_Ensure_minimum( rtems_configuration_get_idle_task_stack_size() ),
    CPU_IDLE_TASK_IS_FP,
    _Scheduler_Map_priority( scheduler, scheduler->maximum_priority ),
    true,        /* preemptable */
    THREAD_CPU_BUDGET_ALGORITHM_NONE,
    NULL,        /* no budget algorithm callout */
    0,           /* all interrupts enabled */
    name
  );

  /*
   *  WARNING!!! This is necessary to "kick" start the system and
   *             MUST be done before _Thread_Start is invoked.
   */
  cpu->heir      =
  cpu->executing = idle;

  idle->is_idle = true;
  idle->Start.Entry.adaptor = _Thread_Entry_adaptor_idle;
  idle->Start.Entry.Kinds.Idle.entry = rtems_configuration_get_idle_task();

  _Thread_Load_environment( idle );

  idle->current_state = STATES_READY;
  _Scheduler_Start_idle( scheduler, idle, cpu );
  _User_extensions_Thread_start( idle );
}

void _Thread_Create_idle( void )
{
  uint32_t cpu_max;
  uint32_t cpu_index;

  _System_state_Set( SYSTEM_STATE_BEFORE_MULTITASKING );
  cpu_max = _SMP_Get_processor_maximum();

  for ( cpu_index = 0 ; cpu_index < cpu_max ; ++cpu_index ) {
    Per_CPU_Control *cpu = _Per_CPU_Get_by_index( cpu_index );

    if ( _Per_CPU_Is_processor_online( cpu ) ) {
      _Thread_Create_idle_for_CPU( cpu );
    }
  }
}
