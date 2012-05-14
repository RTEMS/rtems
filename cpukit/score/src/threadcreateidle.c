/*
 *  Thread Handler
 *
 *
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
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/wkspace.h>
#include <rtems/config.h>
#if defined(RTEMS_SMP)
  #include <rtems/score/smp.h>
#endif

static inline void _Thread_Create_idle_helper(
  uint32_t name_u32,
  int      cpu
)
{
  Objects_Name    name;
  Thread_Control *idle;

  name.name_u32 = name_u32;

  /*
   *  The entire workspace is zeroed during its initialization.  Thus, all
   *  fields not explicitly assigned were explicitly zeroed by
   *  _Workspace_Initialization.
   */
  idle = _Thread_Internal_allocate();

  /*
   *  This is only called during initialization and we better be sure
   *  that when _Thread_Initialize unnests dispatch that we do not
   *  do anything stupid.
   */
  _Thread_Disable_dispatch();

  _Thread_Initialize(
    &_Thread_Internal_information,
    idle,
    NULL,        /* allocate the stack */
    _Stack_Ensure_minimum( Configuration.idle_task_stack_size ),
    CPU_IDLE_TASK_IS_FP,
    PRIORITY_MAXIMUM,
    true,        /* preemptable */
    THREAD_CPU_BUDGET_ALGORITHM_NONE,
    NULL,        /* no budget algorithm callout */
    0,           /* all interrupts enabled */
    name
  );

  _Thread_Unnest_dispatch();

  /*
   *  WARNING!!! This is necessary to "kick" start the system and
   *             MUST be done before _Thread_Start is invoked.
   */
  _Per_CPU_Information[ cpu ].idle      =
  _Per_CPU_Information[ cpu ].heir      =
  _Per_CPU_Information[ cpu ].executing = idle;

  _Thread_Start(
    idle,
    THREAD_START_NUMERIC,
    Configuration.idle_task,
    NULL,
    0
  );
}

void _Thread_Create_idle( void )
{
  #if defined(RTEMS_SMP)
    int cpu;

    for ( cpu=0 ; cpu < _SMP_Processor_count ; cpu++ ) {
      _Thread_Create_idle_helper(
        _Objects_Build_name( 'I', 'D', 'L', 'E' ),
        cpu
      );
    }
  #else
    _Thread_Create_idle_helper(_Objects_Build_name( 'I', 'D', 'L', 'E' ), 0);
  #endif
}
