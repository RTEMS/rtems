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
 *  _Thread_Initialize
 *
 *  This routine initializes the specified the thread.  It allocates
 *  all memory associated with this thread.  It completes by adding
 *  the thread to the local object table so operations on this
 *  thread id are allowed.
 */

boolean _Thread_Initialize(
  Objects_Information                  *information,
  Thread_Control                       *the_thread,
  void                                 *stack_area,
  unsigned32                            stack_size,
  boolean                               is_fp,
  Priority_Control                      priority,
  boolean                               is_preemptible,
  Thread_CPU_budget_algorithms          budget_algorithm,
  Thread_CPU_budget_algorithm_callout   budget_callout,
  unsigned32                            isr_level,
  Objects_Name                          name
)
{
  unsigned32           actual_stack_size = 0;
  void                *stack = NULL;
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  void                *fp_area;
#endif
  void                *extensions_area;

  /*
   *  Initialize the Ada self pointer
   */

  the_thread->rtems_ada_self = NULL;

  /*
   *  Allocate and Initialize the stack for this thread.
   */


  if ( !stack_area ) {
    if ( !_Stack_Is_enough( stack_size ) )
      actual_stack_size = STACK_MINIMUM_SIZE;
    else
      actual_stack_size = stack_size;

    actual_stack_size = _Thread_Stack_Allocate( the_thread, actual_stack_size );
 
    if ( !actual_stack_size ) 
      return FALSE;                     /* stack allocation failed */

    stack = the_thread->Start.stack;
    the_thread->Start.core_allocated_stack = TRUE;
  } else {
    stack = stack_area;
    actual_stack_size = stack_size;
    the_thread->Start.core_allocated_stack = FALSE;
  }

  _Stack_Initialize(
     &the_thread->Start.Initial_stack,
     stack,
     actual_stack_size
  );

  /*
   *  Allocate the floating point area for this thread
   */
  
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( is_fp ) {

    fp_area = _Workspace_Allocate( CONTEXT_FP_SIZE );
    if ( !fp_area ) {
      _Thread_Stack_Free( the_thread );
      return FALSE;
    }
    fp_area = _Context_Fp_start( fp_area, 0 );

  } else
    fp_area = NULL;

  the_thread->fp_context       = fp_area;
  the_thread->Start.fp_context = fp_area;
#endif

  /*
   *  Allocate the extensions area for this thread
   */

  if ( _Thread_Maximum_extensions ) {
    extensions_area = _Workspace_Allocate(
      (_Thread_Maximum_extensions + 1) * sizeof( void * )
    );

    if ( !extensions_area ) {
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
      if ( fp_area )
        (void) _Workspace_Free( fp_area );
#endif

      _Thread_Stack_Free( the_thread );

      return FALSE;
    }
  } else 
    extensions_area = NULL;
  
  the_thread->extensions = (void **) extensions_area;

  /*
   * Clear the extensions area so extension users can determine
   * if they are linked to the thread. An extension user may
   * create the extension long after tasks have been created
   * so they cannot rely on the thread create user extension
   * call.
   */

  if ( the_thread->extensions ) {
    int i;
    for ( i = 0; i < (_Thread_Maximum_extensions + 1); i++ )
      the_thread->extensions[i] = NULL;
  }

  /*
   *  General initialization
   */

  the_thread->Start.is_preemptible   = is_preemptible;
  the_thread->Start.budget_algorithm = budget_algorithm;
  the_thread->Start.budget_callout   = budget_callout;

  switch ( budget_algorithm ) {
    case THREAD_CPU_BUDGET_ALGORITHM_NONE:
    case THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE:
      break;
    case THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE:
      the_thread->cpu_time_budget = _Thread_Ticks_per_timeslice;
      break;
    case THREAD_CPU_BUDGET_ALGORITHM_CALLOUT:
      break;
  }

  the_thread->Start.isr_level        = isr_level;

  the_thread->current_state          = STATES_DORMANT;
  the_thread->resource_count         = 0;
  the_thread->suspend_count          = 0;
  the_thread->real_priority          = priority;
  the_thread->Start.initial_priority = priority;
  the_thread->ticks_executed         = 0;
 
  _Thread_Set_priority( the_thread, priority );

  /*
   *  Open the object
   */

  _Objects_Open( information, &the_thread->Object, name );

  /*
   *  Invoke create extensions
   */

  if ( !_User_extensions_Thread_create( the_thread ) ) {

    if ( extensions_area )
      (void) _Workspace_Free( extensions_area );

#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    if ( fp_area )
      (void) _Workspace_Free( fp_area );
#endif

    _Thread_Stack_Free( the_thread );

    return FALSE;
  }

  return TRUE;
   
}
