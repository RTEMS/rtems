/*
 *  Thread Handler / Thread Initialize
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
#include <rtems/score/scheduler.h>
#include <rtems/score/states.h>
#include <rtems/score/sysstate.h>
#include <rtems/score/thread.h>
#include <rtems/score/threadq.h>
#include <rtems/score/userext.h>
#include <rtems/score/watchdog.h>
#include <rtems/score/wkspace.h>

/*
 *  _Thread_Initialize
 *
 *  This routine initializes the specified the thread.  It allocates
 *  all memory associated with this thread.  It completes by adding
 *  the thread to the local object table so operations on this
 *  thread id are allowed.
 */

bool _Thread_Initialize(
  Objects_Information                  *information,
  Thread_Control                       *the_thread,
  void                                 *stack_area,
  size_t                                stack_size,
  bool                                  is_fp,
  Priority_Control                      priority,
  bool                                  is_preemptible,
  Thread_CPU_budget_algorithms          budget_algorithm,
  Thread_CPU_budget_algorithm_callout   budget_callout,
  uint32_t                              isr_level,
  Objects_Name                          name
)
{
  size_t               actual_stack_size = 0;
  void                *stack = NULL;
  #if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    void              *fp_area;
  #endif
  void                *sched = NULL;
  void                *extensions_area;
  bool                 extension_status;
  int                  i;

  /*
   *  Initialize the Ada self pointer
   */
  #if __RTEMS_ADA__
    the_thread->rtems_ada_self = NULL;
  #endif

  /*
   *  Zero out all the allocated memory fields
   */
  for ( i=0 ; i <= THREAD_API_LAST ; i++ )
    the_thread->API_Extensions[i] = NULL;

  extensions_area = NULL;
  the_thread->libc_reent = NULL;

  #if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    fp_area = NULL;
  #endif

  /*
   *  Allocate and Initialize the stack for this thread.
   */
  #if !defined(RTEMS_SCORE_THREAD_ENABLE_USER_PROVIDED_STACK_VIA_API)
    actual_stack_size = _Thread_Stack_Allocate( the_thread, stack_size );
    if ( !actual_stack_size || actual_stack_size < stack_size )
      return false;                     /* stack allocation failed */

    stack = the_thread->Start.stack;
  #else
    if ( !stack_area ) {
      actual_stack_size = _Thread_Stack_Allocate( the_thread, stack_size );
      if ( !actual_stack_size || actual_stack_size < stack_size )
        return false;                     /* stack allocation failed */

      stack = the_thread->Start.stack;
      the_thread->Start.core_allocated_stack = true;
    } else {
      stack = stack_area;
      actual_stack_size = stack_size;
      the_thread->Start.core_allocated_stack = false;
    }
  #endif

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
      if ( !fp_area )
        goto failed;
      fp_area = _Context_Fp_start( fp_area, 0 );
    }
    the_thread->fp_context       = fp_area;
    the_thread->Start.fp_context = fp_area;
  #endif

  /*
   *  Initialize the thread timer
   */
  _Watchdog_Initialize( &the_thread->Timer, NULL, 0, NULL );

  #ifdef __RTEMS_STRICT_ORDER_MUTEX__
    /* Initialize the head of chain of held mutexes */
    _Chain_Initialize_empty(&the_thread->lock_mutex);
  #endif

  /*
   *  Allocate the extensions area for this thread
   */
  if ( _Thread_Maximum_extensions ) {
    extensions_area = _Workspace_Allocate(
      (_Thread_Maximum_extensions + 1) * sizeof( void * )
    );
    if ( !extensions_area )
      goto failed;
  }
  the_thread->extensions = (void **) extensions_area;

  /*
   * Clear the extensions area so extension users can determine
   * if they are linked to the thread. An extension user may
   * create the extension long after tasks have been created
   * so they cannot rely on the thread create user extension
   * call.
   */
  if ( the_thread->extensions ) {
    for ( i = 0; i <= _Thread_Maximum_extensions ; i++ )
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
    #if defined(RTEMS_SCORE_THREAD_ENABLE_EXHAUST_TIMESLICE)
      case THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE:
        the_thread->cpu_time_budget = _Thread_Ticks_per_timeslice;
        break;
    #endif
    #if defined(RTEMS_SCORE_THREAD_ENABLE_SCHEDULER_CALLOUT)
      case THREAD_CPU_BUDGET_ALGORITHM_CALLOUT:
	break;
    #endif
  }

  the_thread->Start.isr_level         = isr_level;

  the_thread->current_state           = STATES_DORMANT;
  the_thread->Wait.queue              = NULL;
  the_thread->resource_count          = 0;
  the_thread->real_priority           = priority;
  the_thread->Start.initial_priority  = priority;
  sched =_Scheduler_Allocate( the_thread );
  if ( !sched )
    goto failed;
  _Thread_Set_priority( the_thread, priority );

  /*
   *  Initialize the CPU usage statistics
   */
  #ifndef __RTEMS_USE_TICKS_FOR_STATISTICS__
    _Timestamp_Set_to_zero( &the_thread->cpu_time_used );
  #else
    the_thread->cpu_time_used = 0;
  #endif

  /*
   *  Open the object
   */
  _Objects_Open( information, &the_thread->Object, name );

  /*
   *  We assume the Allocator Mutex is locked and dispatching is
   *  enabled when we get here.  We want to be able to run the
   *  user extensions with dispatching enabled.  The Allocator
   *  Mutex provides sufficient protection to let the user extensions
   *  run safely.
   */
  extension_status = _User_extensions_Thread_create( the_thread );
  if ( extension_status )
    return true;

failed:
  _Workspace_Free( the_thread->libc_reent );

  for ( i=0 ; i <= THREAD_API_LAST ; i++ )
    _Workspace_Free( the_thread->API_Extensions[i] );

  _Workspace_Free( extensions_area );

  #if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    _Workspace_Free( fp_area );
  #endif

   _Workspace_Free( sched );

   _Thread_Stack_Free( the_thread );
  return false;
}
