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
  Scheduler_Node          *scheduler_node;
  bool                     scheduler_node_initialized = false;
  Per_CPU_Control         *cpu = _Per_CPU_Get_by_index( 0 );

#if defined( RTEMS_SMP )
  if ( rtems_configuration_is_smp_enabled() && !is_preemptible ) {
    return false;
  }
#endif

  memset(
    &the_thread->current_state,
    0,
    information->Objects.size - offsetof( Thread_Control, current_state )
  );

  for ( i = 0 ; i < _Thread_Control_add_on_count ; ++i ) {
    const Thread_Control_add_on *add_on = &_Thread_Control_add_ons[ i ];

    *(void **) ( (char *) the_thread + add_on->destination_offset ) =
      (char *) the_thread + add_on->source_offset;
  }

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
    THREAD_QUEUE_HEADS_SIZE( _Scheduler_Count )
  );
  if ( the_thread->Wait.spare_heads == NULL ) {
    goto failed;
  }
  _Thread_queue_Heads_initialize( the_thread->Wait.spare_heads );

  /*
   *  General initialization
   */

  the_thread->is_fp                  = is_fp;
  the_thread->Start.isr_level        = isr_level;
  the_thread->Start.is_preemptible   = is_preemptible;
  the_thread->Start.budget_algorithm = budget_algorithm;
  the_thread->Start.budget_callout   = budget_callout;

  _Thread_Timer_initialize( &the_thread->Timer, cpu );

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

  scheduler_node = the_thread->Scheduler.node;

#if defined(RTEMS_SMP)
  RTEMS_STATIC_ASSERT( THREAD_SCHEDULER_BLOCKED == 0, Scheduler_state );
  the_thread->Scheduler.own_control = scheduler;
  the_thread->Scheduler.control = scheduler;
  the_thread->Scheduler.own_node = scheduler_node;
  _Resource_Node_initialize( &the_thread->Resource_node );
  _Atomic_Store_uintptr(
    &the_thread->Lock.current.atomic,
    (uintptr_t) &the_thread->Lock.Default,
    ATOMIC_ORDER_RELAXED
  );
  _SMP_ticket_lock_Initialize( &the_thread->Lock.Default );
  _SMP_lock_Stats_initialize( &the_thread->Lock.Stats, "Thread Lock" );
  _SMP_lock_Stats_initialize( &the_thread->Potpourri_stats, "Thread Potpourri" );
#endif

  _Thread_Debug_set_real_processor( the_thread, cpu );

  /* Initialize the CPU for the non-SMP schedulers */
  _Thread_Set_CPU( the_thread, cpu );

  _Thread_queue_Initialize( &the_thread->Join_queue );

  the_thread->current_state           = STATES_DORMANT;
  the_thread->Wait.operations         = &_Thread_queue_Operations_default;
  the_thread->current_priority        = priority;
  the_thread->real_priority           = priority;
  the_thread->Start.initial_priority  = priority;

  RTEMS_STATIC_ASSERT( THREAD_WAIT_FLAGS_INITIAL == 0, Wait_flags );

  _Scheduler_Node_initialize( scheduler, scheduler_node, the_thread, priority );
  scheduler_node_initialized = true;

  /* POSIX Keys */
  _RBTree_Initialize_empty( &the_thread->Keys.Key_value_pairs );
  _ISR_lock_Initialize( &the_thread->Keys.Lock, "POSIX Key Value Pairs" );

  _Thread_Action_control_initialize( &the_thread->Post_switch_actions );

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
    _Scheduler_Node_destroy( scheduler, scheduler_node );
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
