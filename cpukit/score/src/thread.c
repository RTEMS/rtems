/*
 *  Thread Handler
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
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
 *  _Thread_Handler_initialization
 *
 *  This routine initializes all thread manager related data structures.
 *
 *  Input parameters:
 *    ticks_per_timeslice - clock ticks per quantum
 *    maximum_proxies     - number of proxies to initialize
 *
 *  Output parameters:  NONE
 */

void _Thread_Handler_initialization(
  unsigned32   ticks_per_timeslice,
  unsigned32   maximum_extensions,
  unsigned32   maximum_proxies
)
{
  unsigned32      index;

  /*
   * BOTH stacks hooks must be set or both must be NULL.
   * Do not allow mixture.
   */

  if ( !( ( _CPU_Table.stack_allocate_hook == 0 )
       == ( _CPU_Table.stack_free_hook == 0 ) ) )
    _Internal_error_Occurred(
      INTERNAL_ERROR_CORE,
      TRUE,
      INTERNAL_ERROR_BAD_STACK_HOOK
    );

  _Context_Switch_necessary = FALSE;
  _Thread_Executing         = NULL;
  _Thread_Heir              = NULL;
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  _Thread_Allocated_fp      = NULL;
#endif

  _Thread_Do_post_task_switch_extension = 0;

  _Thread_Maximum_extensions = maximum_extensions;

  _Thread_Ticks_per_timeslice  = ticks_per_timeslice;

  _Thread_Ready_chain = (Chain_Control *) _Workspace_Allocate_or_fatal_error(
    (PRIORITY_MAXIMUM + 1) * sizeof(Chain_Control)
  );

  for ( index=0; index <= PRIORITY_MAXIMUM ; index++ )
    _Chain_Initialize_empty( &_Thread_Ready_chain[ index ] );

#if defined(RTEMS_MULTIPROCESSING)
  _Thread_MP_Handler_initialization( maximum_proxies );
#endif

  /*
   *  Initialize this class of objects.
   */
 
  _Objects_Initialize_information(
    &_Thread_Internal_information,
    OBJECTS_INTERNAL_API,
    OBJECTS_INTERNAL_THREADS,
    ( _System_state_Is_multiprocessing ) ?  2 : 1,
    sizeof( Thread_Control ),
                                /* size of this object's control block */
    TRUE,                       /* TRUE if names for this object are strings */
    8                           /* maximum length of each object's name */
#if defined(RTEMS_MULTIPROCESSING)
    ,
    FALSE,                      /* TRUE if this is a global object class */
    NULL                        /* Proxy extraction support callout */
#endif
  );

}

