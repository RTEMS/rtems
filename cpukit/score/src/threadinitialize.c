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
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/tls.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/watchdogimpl.h>
#include <rtems/score/wkspace.h>
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
#if defined(RTEMS_SMP)
  Scheduler_Node          *scheduler_node_for_index;
  const Scheduler_Control *scheduler_for_index;
#endif
  size_t                   scheduler_index;
  Per_CPU_Control         *cpu = _Per_CPU_Get_by_index( 0 );

#if defined( RTEMS_SMP )
  if ( rtems_configuration_is_smp_enabled() ) {
    if ( !is_preemptible ) {
      return false;
    }

    if ( isr_level != 0 ) {
      return false;
    }
  }
#endif

  memset(
    &the_thread->Join_queue,
    0,
    information->Objects.size - offsetof( Thread_Control, Join_queue )
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

  scheduler_index = 0;

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

#if defined(RTEMS_SMP)
  scheduler_node = NULL;
  scheduler_node_for_index = the_thread->Scheduler.nodes;
  scheduler_for_index = &_Scheduler_Table[ 0 ];

  while ( scheduler_index < _Scheduler_Count ) {
    Priority_Control priority_for_index;

    if ( scheduler_for_index == scheduler ) {
      priority_for_index = priority;
      scheduler_node = scheduler_node_for_index;
    } else {
      /*
       * Use the idle thread priority for the non-home scheduler instances by
       * default.
       */
      priority_for_index = _Scheduler_Map_priority(
        scheduler_for_index,
        scheduler_for_index->maximum_priority
      );
    }

    _Scheduler_Node_initialize(
      scheduler_for_index,
      scheduler_node_for_index,
      the_thread,
      priority_for_index
    );
    scheduler_node_for_index = (Scheduler_Node *)
      ( (uintptr_t) scheduler_node_for_index + _Scheduler_Node_size );
    ++scheduler_for_index;
    ++scheduler_index;
  }

  _Assert( scheduler_node != NULL );
  _Chain_Initialize_one(
    &the_thread->Scheduler.Wait_nodes,
    &scheduler_node->Thread.Wait_node
  );
  _Chain_Initialize_one(
    &the_thread->Scheduler.Scheduler_nodes,
    &scheduler_node->Thread.Scheduler_node.Chain
  );
#else
  scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  _Scheduler_Node_initialize(
    scheduler,
    scheduler_node,
    the_thread,
    priority
  );
  scheduler_index = 1;
#endif

  _Priority_Node_initialize( &the_thread->Real_priority, priority );
  _Priority_Initialize_one(
    &scheduler_node->Wait.Priority,
    &the_thread->Real_priority
  );

#if defined(RTEMS_SMP)
  RTEMS_STATIC_ASSERT( THREAD_SCHEDULER_BLOCKED == 0, Scheduler_state );
  the_thread->Scheduler.home_scheduler = scheduler;
  _ISR_lock_Initialize( &the_thread->Scheduler.Lock, "Thread Scheduler" );
  _Processor_mask_Assign(
    &the_thread->Scheduler.Affinity,
    _SMP_Get_online_processors()
   );
  _ISR_lock_Initialize( &the_thread->Wait.Lock.Default, "Thread Wait Default" );
  _Thread_queue_Gate_open( &the_thread->Wait.Lock.Tranquilizer );
  _RBTree_Initialize_node( &the_thread->Wait.Link.Registry_node );
  _SMP_lock_Stats_initialize( &the_thread->Potpourri_stats, "Thread Potpourri" );
  _SMP_lock_Stats_initialize( &the_thread->Join_queue.Lock_stats, "Thread State" );
#endif

  /* Initialize the CPU for the non-SMP schedulers */
  _Thread_Set_CPU( the_thread, cpu );

  the_thread->current_state           = STATES_DORMANT;
  the_thread->Wait.operations         = &_Thread_queue_Operations_default;
  the_thread->Start.initial_priority  = priority;

  RTEMS_STATIC_ASSERT( THREAD_WAIT_FLAGS_INITIAL == 0, Wait_flags );

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

#if defined(RTEMS_SMP)
  while ( scheduler_index > 0 ) {
    scheduler_node_for_index = (Scheduler_Node *)
      ( (uintptr_t) scheduler_node_for_index - _Scheduler_Node_size );
    --scheduler_for_index;
    --scheduler_index;
    _Scheduler_Node_destroy( scheduler_for_index, scheduler_node_for_index );
  }
#else
  if ( scheduler_index > 0 ) {
    _Scheduler_Node_destroy( scheduler, scheduler_node );
  }
#endif

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
