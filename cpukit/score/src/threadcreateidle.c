/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-2008.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
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

/*PAGE
 *
 *  _Thread_Create_idle
 */

void _Thread_Create_idle( void )
{
  Objects_Name name;

  name.name_p = "IDLE";

  /*
   *  The entire workspace is zeroed during its initialization.  Thus, all
   *  fields not explicitly assigned were explicitly zeroed by
   *  _Workspace_Initialization.
   */
  _Thread_Idle = _Thread_Internal_allocate();

  /*
   *  This is only called during initialization and we better be sure
   *  that when _Thread_Initialize unnests dispatch that we do not
   *  do anything stupid.
   */
  _Thread_Disable_dispatch();

  _Thread_Initialize(
    &_Thread_Internal_information,
    _Thread_Idle,
    NULL,        /* allocate the stack */
    _Stack_Ensure_minimum( _Configuration_Table->idle_task_stack_size ),
    CPU_IDLE_TASK_IS_FP,
    PRIORITY_MAXIMUM,
    TRUE,        /* preemptable */
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
  _Thread_Heir      =
  _Thread_Executing = _Thread_Idle;

  _Thread_Start(
    _Thread_Idle,
    THREAD_START_NUMERIC,
    _Configuration_Table->idle_task,
    NULL,
    0
  );

}
