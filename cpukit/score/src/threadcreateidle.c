/**
 *  @file
 *
 *  @brief Create Idle Thread
 *  @ingroup ScoreThread
 */

/*
 *  COPYRIGHT (c) 1989-2011.
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
#include <rtems/score/apiext.h>
#include <rtems/score/context.h>
#include <rtems/score/interr.h>
#include <rtems/score/isr.h>
#include <rtems/score/object.h>
#include <rtems/score/priority.h>
#include <rtems/score/smp.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/states.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>

static void _Thread_Create_idle_for_cpu( Per_CPU_Control *per_cpu )
{
  Objects_Name    name;
  Thread_Control *idle;

  name.name_u32 = _Objects_Build_name( 'I', 'D', 'L', 'E' );

  /*
   *  The entire workspace is zeroed during its initialization.  Thus, all
   *  fields not explicitly assigned were explicitly zeroed by
   *  _Workspace_Initialization.
   */
  idle = _Thread_Internal_allocate();

  _Thread_Initialize(
    &_Thread_Internal_information,
    idle,
    NULL,        /* allocate the stack */
    _Stack_Ensure_minimum( rtems_configuration_get_idle_task_stack_size() ),
    CPU_IDLE_TASK_IS_FP,
    PRIORITY_MAXIMUM,
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
  per_cpu->heir      =
  per_cpu->executing = idle;

  _Thread_Start(
    idle,
    THREAD_START_NUMERIC,
    rtems_configuration_get_idle_task(),
    NULL,
    0,
    per_cpu
  );
}

void _Thread_Create_idle( void )
{
  uint32_t processor_count = _SMP_Get_processor_count();
  uint32_t processor;

  for ( processor = 0 ; processor < processor_count ; ++processor ) {
    Per_CPU_Control *per_cpu = &_Per_CPU_Information[ processor ];

    _Thread_Create_idle_for_cpu( per_cpu );
  }
}
