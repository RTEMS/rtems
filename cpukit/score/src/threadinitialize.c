/**
 *  @file
 *
 *  @brief Initialize Thread
 *
 *  @ingroup ScoreThread
 */
/*
 *  COPYRIGHT (c) 1989-2014.
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
#include <rtems/score/resourceimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/tls.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/cpusetimpl.h>
#include <rtems/config.h>

bool _Thread_Initialize(
  Thread_Information                   *information,
  Thread_Control                       *the_thread,
  const Scheduler_Control              *scheduler,
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
  uintptr_t                tls_size = _TLS_Get_size();
  size_t                   actual_stack_size = 0;
  void                    *stack = NULL;
  #if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    void                  *fp_area = NULL;
  #endif
  bool                     extension_status;
  size_t                   i;
  bool                     scheduler_node_initialized = false;
  Per_CPU_Control         *cpu = _Per_CPU_Get_by_index( 0 );

#if defined( RTEMS_SMP )
  if ( rtems_configuration_is_smp_enabled() && !is_preemptible ) {
    return false;
  }
#endif

  for ( i = 0 ; i < _Thread_Control_add_on_count ; ++i ) {
    const Thread_Control_add_on *add_on = &_Thread_Control_add_ons[ i ];

    *(void **) ( (char *) the_thread + add_on->destination_offset ) =
      (char *) the_thread + add_on->source_offset;
  }

  /*
   *  Initialize the Ada self pointer
   */
  #if __RTEMS_ADA__
    the_thread->rtems_ada_self = NULL;
  #endif

  the_thread->Start.tls_area = NULL;
  the_thread->Wait.spare_heads = NULL;

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

  /* Thread-local storage (TLS) area allocation */
  if ( tls_size > 0 ) {
    uintptr_t tls_align = _TLS_Heap_align_up( (uintptr_t) _TLS_Alignment );
    uintptr_t tls_alloc = _TLS_Get_allocation_size( tls_size, tls_align );

    the_thread->Start.tls_area =
      _Workspace_Allocate_aligned( tls_alloc, tls_align );

    if ( the_thread->Start.tls_area == NULL ) {
      goto failed;
    }
  }

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
   *  Get thread queue heads
   */
  the_thread->Wait.spare_heads = _Freechain_Get(
    &information->Free_thread_queue_heads,
    _Workspace_Allocate,
    _Objects_Extend_size( &information->Objects ),
    sizeof( *the_thread->Wait.spare_heads )
  );
  if ( the_thread->Wait.spare_heads == NULL ) {
    goto failed;
  }
  _Chain_Initialize_empty( &the_thread->Wait.spare_heads->Free_chain );

  /*
   *  Initialize the thread timer
   */
  _Watchdog_Preinitialize( &the_thread->Timer );

  #ifdef __RTEMS_STRICT_ORDER_MUTEX__
    /* Initialize the head of chain of held mutexes */
    _Chain_Initialize_empty(&the_thread->lock_mutex);
  #endif

  /*
   * Clear the extensions area so extension users can determine
   * if they are linked to the thread. An extension user may
   * create the extension long after tasks have been created
   * so they cannot rely on the thread create user extension
   * call.  The object index starts with one, so the first extension context is
   * unused.
   */
  for ( i = 1 ; i <= rtems_configuration_get_maximum_extensions() ; ++i )
    the_thread->extensions[ i ] = NULL;

  /*
   *  General initialization
   */

  the_thread->is_fp                  = is_fp;
  the_thread->Start.isr_level        = isr_level;
  the_thread->Start.is_preemptible   = is_preemptible;
  the_thread->Start.budget_algorithm = budget_algorithm;
  the_thread->Start.budget_callout   = budget_callout;

  switch ( budget_algorithm ) {
    case THREAD_CPU_BUDGET_ALGORITHM_NONE:
    case THREAD_CPU_BUDGET_ALGORITHM_RESET_TIMESLICE:
      break;
    #if defined(RTEMS_SCORE_THREAD_ENABLE_EXHAUST_TIMESLICE)
      case THREAD_CPU_BUDGET_ALGORITHM_EXHAUST_TIMESLICE:
        the_thread->cpu_time_budget =
          rtems_configuration_get_ticks_per_timeslice();
        break;
    #endif
    #if defined(RTEMS_SCORE_THREAD_ENABLE_SCHEDULER_CALLOUT)
      case THREAD_CPU_BUDGET_ALGORITHM_CALLOUT:
	break;
    #endif
  }

#if defined(RTEMS_SMP)
  the_thread->Scheduler.state = THREAD_SCHEDULER_BLOCKED;
  the_thread->Scheduler.own_control = scheduler;
  the_thread->Scheduler.control = scheduler;
  the_thread->Scheduler.own_node = the_thread->Scheduler.node;
  _Resource_Node_initialize( &the_thread->Resource_node );
  _CPU_Context_Set_is_executing( &the_thread->Registers, false );
  the_thread->Lock.current = &the_thread->Lock.Default;
  _SMP_ticket_lock_Initialize( &the_thread->Lock.Default );
  _SMP_lock_Stats_initialize( &the_thread->Lock.Stats, "Thread Lock" );
  _SMP_lock_Stats_initialize( &the_thread->Potpourri_stats, "Thread Potpourri" );
  _Atomic_Init_uint(&the_thread->Lock.generation, 0);
#endif

  _Thread_Debug_set_real_processor( the_thread, cpu );

  /* Initialize the CPU for the non-SMP schedulers */
  _Thread_Set_CPU( the_thread, cpu );

  the_thread->current_state           = STATES_DORMANT;
  the_thread->Wait.queue              = NULL;
  the_thread->Wait.operations         = &_Thread_queue_Operations_default;
  the_thread->resource_count          = 0;
  the_thread->current_priority        = priority;
  the_thread->real_priority           = priority;
  the_thread->priority_generation     = 0;
  the_thread->Start.initial_priority  = priority;

  _Thread_Wait_flags_set( the_thread, THREAD_WAIT_FLAGS_INITIAL );

  _Scheduler_Node_initialize( scheduler, the_thread );
  scheduler_node_initialized = true;

  _Scheduler_Update_priority( the_thread, priority );

  /*
   *  Initialize the CPU usage statistics
   */
  _Timestamp_Set_to_zero( &the_thread->cpu_time_used );

  /*
   * initialize thread's key vaule node chain
   */
  _Chain_Initialize_empty( &the_thread->Key_Chain );

  _Thread_Action_control_initialize( &the_thread->Post_switch_actions );

  _Thread_Action_initialize(
    &the_thread->Life.Action,
    _Thread_Life_action_handler
  );
  the_thread->Life.state = THREAD_LIFE_NORMAL;
  the_thread->Life.terminator = NULL;

  the_thread->Capture.flags = 0;
  the_thread->Capture.control = NULL;

  /*
   *  Open the object
   */
  _Objects_Open( &information->Objects, &the_thread->Object, name );

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

  if ( scheduler_node_initialized ) {
    _Scheduler_Node_destroy( scheduler, the_thread );
  }

  _Workspace_Free( the_thread->Start.tls_area );

  _Freechain_Put(
    &information->Free_thread_queue_heads,
    the_thread->Wait.spare_heads
  );

  #if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
    _Workspace_Free( fp_area );
  #endif

   _Thread_Stack_Free( the_thread );
  return false;
}
