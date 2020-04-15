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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadidledata.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/assert.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/userextimpl.h>

#include <string.h>

static void _Thread_Create_idle_for_CPU( Per_CPU_Control *cpu )
{
  Thread_Configuration  config;
  Thread_Control       *idle;
  bool                  ok;

  memset( &config, 0, sizeof( config ) );
  config.scheduler = _Scheduler_Get_by_CPU( cpu );

#if defined(RTEMS_SMP)
  if ( config.scheduler == NULL ) {
    return;
  }
#endif

  config.priority = _Scheduler_Map_priority(
    config.scheduler,
    config.scheduler->maximum_priority
  );
  config.budget_algorithm = THREAD_CPU_BUDGET_ALGORITHM_NONE;
  config.name.name_u32 = _Objects_Build_name( 'I', 'D', 'L', 'E' );
  config.is_fp = CPU_IDLE_TASK_IS_FP;
  config.is_preemptible = true;
  config.stack_size = _Thread_Idle_stack_size
    + CPU_IDLE_TASK_IS_FP * CONTEXT_FP_SIZE;
  config.stack_area = &_Thread_Idle_stacks[
    _Per_CPU_Get_index( cpu ) * config.stack_size
  ];

  /*
   *  The entire workspace is zeroed during its initialization.  Thus, all
   *  fields not explicitly assigned were explicitly zeroed by
   *  _Workspace_Initialization.
   */
  idle = _Thread_Internal_allocate();
  _Assert( idle != NULL );

  ok = _Thread_Initialize( &_Thread_Information, idle, &config );
  _Assert( ok );
  (void) ok;

  /*
   *  WARNING!!! This is necessary to "kick" start the system and
   *             MUST be done before _Thread_Start is invoked.
   */
  cpu->heir      =
  cpu->executing = idle;
#if defined(RTEMS_SMP)
  cpu->ancestor = idle;
#endif

  idle->is_idle = true;
  idle->Start.Entry.adaptor = _Thread_Entry_adaptor_idle;
  idle->Start.Entry.Kinds.Idle.entry = _Thread_Idle_body;

  _Thread_Load_environment( idle );

  idle->current_state = STATES_READY;
  _Scheduler_Start_idle( config.scheduler, idle, cpu );
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
