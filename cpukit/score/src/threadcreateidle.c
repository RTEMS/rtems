/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

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

/*PAGE
 *
 *  _Thread_Create_idle
 */

char *_Thread_Idle_name = "IDLE";

void _Thread_Create_idle( void )
{
  void       *idle;
  unsigned32  idle_task_stack_size;

  /*
   *  The entire workspace is zeroed during its initialization.  Thus, all
   *  fields not explicitly assigned were explicitly zeroed by
   *  _Workspace_Initialization.
   */
 
  _Thread_Idle = _Thread_Internal_allocate();
 
  /*
   *  Initialize the IDLE task.
   */
 
#if (CPU_PROVIDES_IDLE_THREAD_BODY == TRUE)
  idle = (void *) _CPU_Thread_Idle_body;
#else
  idle = (void *) _Thread_Idle_body;
#endif
 
  if ( _CPU_Table.idle_task )
    idle = _CPU_Table.idle_task;
 
  idle_task_stack_size =  _CPU_Table.idle_task_stack_size;
  if ( idle_task_stack_size < STACK_MINIMUM_SIZE )
    idle_task_stack_size = STACK_MINIMUM_SIZE;
 
  _Thread_Initialize(
    &_Thread_Internal_information,
    _Thread_Idle,
    NULL,        /* allocate the stack */
    idle_task_stack_size,
    CPU_IDLE_TASK_IS_FP,
    PRIORITY_MAXIMUM,
    TRUE,        /* preemptable */
    THREAD_CPU_BUDGET_ALGORITHM_NONE,
    NULL,        /* no budget algorithm callout */
    0,           /* all interrupts enabled */
    _Thread_Idle_name
  );
 
  /*
   *  WARNING!!! This is necessary to "kick" start the system and
   *             MUST be done before _Thread_Start is invoked.
   */
 
  _Thread_Heir      =
  _Thread_Executing = _Thread_Idle;
 
  _Thread_Start(
    _Thread_Idle,
    THREAD_START_NUMERIC,
    idle,
    NULL,
    0
  );
 
}
