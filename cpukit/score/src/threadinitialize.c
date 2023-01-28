/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreThread
 *
 * @brief This source file contains the implementation of
 *   _Thread_Initialize().
 */

/*
 *  COPYRIGHT (c) 1989-2014.
 *  On-Line Applications Research Corporation (OAR).
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/threadimpl.h>
#include <rtems/score/freechainimpl.h>
#include <rtems/score/schedulerimpl.h>
#include <rtems/score/stackimpl.h>
#include <rtems/score/tls.h>
#include <rtems/score/userextimpl.h>
#include <rtems/score/watchdogimpl.h>

void _Thread_Free(
  Thread_Information *information,
  Thread_Control     *the_thread
)
{
#if defined(RTEMS_SMP)
  Scheduler_Node *scheduler_node;
  size_t          scheduler_index;
#endif

  _User_extensions_Thread_delete( the_thread );
  _User_extensions_Destroy_iterators( the_thread );
  _ISR_lock_Destroy( &the_thread->Keys.Lock );

#if defined(RTEMS_SMP)
  scheduler_node = the_thread->Scheduler.nodes;
  scheduler_index = 0;

  while ( scheduler_index < _Scheduler_Count ) {
    _Scheduler_Node_destroy(
      &_Scheduler_Table[ scheduler_index ],
      scheduler_node
    );
    scheduler_node = (Scheduler_Node *)
      ( (uintptr_t) scheduler_node + _Scheduler_Node_size );
    ++scheduler_index;
  }
#else
  _Scheduler_Node_destroy(
    _Thread_Scheduler_get_home( the_thread ),
    _Thread_Scheduler_get_home_node( the_thread )
  );
#endif

  _ISR_lock_Destroy( &the_thread->Timer.Lock );

  /*
   *  The thread might have been FP.  So deal with that.
   */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
#if ( CPU_USE_DEFERRED_FP_SWITCH == TRUE )
  if ( _Thread_Is_allocated_fp( the_thread ) )
    _Thread_Deallocate_fp();
#endif
#endif

  _Freechain_Push(
    &information->Thread_queue_heads.Free,
    the_thread->Wait.spare_heads
  );

  /*
   *  Free the rest of the memory associated with this task
   *  and set the associated pointers to NULL for safety.
   */
  ( *the_thread->Start.stack_free )( the_thread->Start.Initial_stack.area );

#if defined(RTEMS_SMP)
  _ISR_lock_Destroy( &the_thread->Scheduler.Lock );
  _ISR_lock_Destroy( &the_thread->Wait.Lock.Default );
  _SMP_lock_Stats_destroy( &the_thread->Potpourri_stats );
#endif

  _Thread_queue_Destroy( &the_thread->Join_queue );
  _Context_Destroy( the_thread, &the_thread->Registers );
  _Objects_Free( &information->Objects, &the_thread->Object );
}

static void _Thread_Initialize_scheduler_and_wait_nodes(
  Thread_Control             *the_thread,
  const Thread_Configuration *config
)
{
  Scheduler_Node          *home_scheduler_node;
#if defined(RTEMS_SMP)
  Scheduler_Node          *scheduler_node;
  const Scheduler_Control *scheduler;
  size_t                   scheduler_index;
#endif

#if defined(RTEMS_SMP)
  home_scheduler_node = NULL;
  scheduler_node = the_thread->Scheduler.nodes;
  scheduler = &_Scheduler_Table[ 0 ];
  scheduler_index = 0;

  /*
   * In SMP configurations, the thread has exactly one scheduler node for each
   * configured scheduler.  Initialize the scheduler nodes of each scheduler.
   * The application configuration ensures that we have at least one scheduler
   * configured.
   */

  _Assert ( _Scheduler_Count >= 1 );

  do {
    Priority_Control priority;

    if ( scheduler == config->scheduler ) {
      priority = config->priority;
      home_scheduler_node = scheduler_node;
    } else {
      /*
       * Use the idle thread priority for the non-home scheduler instances by
       * default.
       */
      priority = _Scheduler_Map_priority(
        scheduler,
        scheduler->maximum_priority
      );
    }

    _Scheduler_Node_initialize(
      scheduler,
      scheduler_node,
      the_thread,
      priority
    );

    /*
     * Since the size of a scheduler node depends on the application
     * configuration, the _Scheduler_Node_size constant is used to get the next
     * scheduler node.  Using sizeof( Scheduler_Node ) would be wrong.
     */
    scheduler_node = (Scheduler_Node *)
      ( (uintptr_t) scheduler_node + _Scheduler_Node_size );
    ++scheduler;
    ++scheduler_index;
  } while ( scheduler_index < _Scheduler_Count );

  /*
   * The thread is initialized to use exactly one scheduler node which is
   * provided by its home scheduler.
   */
  _Assert( home_scheduler_node != NULL );
  _Chain_Initialize_one(
    &the_thread->Scheduler.Wait_nodes,
    &home_scheduler_node->Thread.Wait_node
  );
  _Chain_Initialize_one(
    &the_thread->Scheduler.Scheduler_nodes,
    &home_scheduler_node->Thread.Scheduler_node.Chain
  );
#else
  /*
   * In uniprocessor configurations, the thread has exactly one scheduler node.
   */
  home_scheduler_node = _Thread_Scheduler_get_home_node( the_thread );
  _Scheduler_Node_initialize(
    config->scheduler,
    home_scheduler_node,
    the_thread,
    config->priority
  );
#endif

  /*
   * The current priority of the thread is initialized to exactly the real
   * priority of the thread.  During the lifetime of the thread, it may gain
   * more priority nodes, for example through locking protocols such as
   * priority inheritance or priority ceiling.
   */
  _Priority_Node_initialize( &the_thread->Real_priority, config->priority );
  _Priority_Initialize_one(
    &home_scheduler_node->Wait.Priority,
    &the_thread->Real_priority
  );

#if defined(RTEMS_SMP)
  RTEMS_STATIC_ASSERT( THREAD_SCHEDULER_BLOCKED == 0, Scheduler_state );
  the_thread->Scheduler.home_scheduler = config->scheduler;
  _ISR_lock_Initialize( &the_thread->Scheduler.Lock, "Thread Scheduler" );
  _ISR_lock_Initialize( &the_thread->Wait.Lock.Default, "Thread Wait Default" );
  _Thread_queue_Gate_open( &the_thread->Wait.Lock.Tranquilizer );
  _RBTree_Initialize_node( &the_thread->Wait.Link.Registry_node );
#endif
}

static bool _Thread_Try_initialize(
  Thread_Information         *information,
  Thread_Control             *the_thread,
  const Thread_Configuration *config
)
{
  uintptr_t                tls_size;
  size_t                   i;
  char                    *stack_begin;
  char                    *stack_end;
  uintptr_t                stack_align;
  Per_CPU_Control         *cpu = _Per_CPU_Get_by_index( 0 );

  memset(
    &the_thread->Join_queue,
    0,
    information->Objects.object_size - offsetof( Thread_Control, Join_queue )
  );

  for ( i = 0 ; i < _Thread_Control_add_on_count ; ++i ) {
    const Thread_Control_add_on *add_on = &_Thread_Control_add_ons[ i ];

    *(void **) ( (char *) the_thread + add_on->destination_offset ) =
      (char *) the_thread + add_on->source_offset;
  }

  /* Set up the properly aligned stack area begin and end */
  stack_begin = config->stack_area;
  stack_end = stack_begin + config->stack_size;
  stack_align = CPU_STACK_ALIGNMENT;
  stack_end = (char *) RTEMS_ALIGN_DOWN( (uintptr_t) stack_end, stack_align );

  /* Allocate floating-point context in stack area */
#if ( CPU_HARDWARE_FP == TRUE ) || ( CPU_SOFTWARE_FP == TRUE )
  if ( config->is_fp ) {
    stack_end -= CONTEXT_FP_SIZE;
    the_thread->fp_context = (Context_Control_fp *) stack_end;
    the_thread->Start.fp_context = (Context_Control_fp *) stack_end;
  }
#endif

  tls_size = _TLS_Get_allocation_size();

  /* Allocate thread-local storage (TLS) area in stack area */
  if ( tls_size > 0 ) {
    stack_end -= tls_size;
    the_thread->Start.tls_area = stack_end;
  }

  _Stack_Initialize(
    &the_thread->Start.Initial_stack,
    stack_begin,
    stack_end - stack_begin
  );

  /*
   *  Get thread queue heads
   */
  the_thread->Wait.spare_heads = _Freechain_Pop(
    &information->Thread_queue_heads.Free
  );
  _Thread_queue_Heads_initialize( the_thread->Wait.spare_heads );

  /*
   *  General initialization
   */

  the_thread->is_fp                       = config->is_fp;
  the_thread->Start.isr_level             = config->isr_level;
  the_thread->Start.is_preemptible        = config->is_preemptible;
  the_thread->Start.cpu_budget_operations = config->cpu_budget_operations;
  the_thread->Start.stack_free            = config->stack_free;
  the_thread->Join_queue.Queue.owner      = the_thread;

  _Thread_Timer_initialize( &the_thread->Timer, cpu );
  _Thread_Initialize_scheduler_and_wait_nodes( the_thread, config );

#if defined(RTEMS_SMP)
  _Processor_mask_Assign(
    &the_thread->Scheduler.Affinity,
    _SMP_Get_online_processors()
   );
  _SMP_lock_Stats_initialize( &the_thread->Potpourri_stats, "Thread Potpourri" );
  _SMP_lock_Stats_initialize( &the_thread->Join_queue.Lock_stats, "Thread State" );
#endif

  /* Initialize the CPU for the non-SMP schedulers */
  _Thread_Set_CPU( the_thread, cpu );

  the_thread->current_state           = STATES_DORMANT;
  the_thread->Wait.operations         = &_Thread_queue_Operations_default;
  the_thread->Start.initial_priority  = config->priority;

  RTEMS_STATIC_ASSERT( THREAD_WAIT_STATE_READY == 0, Wait_flags );

  /* POSIX Keys */
  _RBTree_Initialize_empty( &the_thread->Keys.Key_value_pairs );
  _ISR_lock_Initialize( &the_thread->Keys.Lock, "POSIX Key Value Pairs" );

  _Thread_Action_control_initialize( &the_thread->Post_switch_actions );

  _Objects_Open_u32( &information->Objects, &the_thread->Object, config->name );

  /*
   * We do following checks of simple error conditions after the thread is
   * fully initialized to simplify the clean up in case of an error.  With a
   * fully initialized thread we can simply use _Thread_Free() and do not have
   * to bother with partially initialized threads.
   */

#if defined(RTEMS_SMP)
  if (
    !config->is_preemptible
      && !_Scheduler_Is_non_preempt_mode_supported( config->scheduler )
  ) {
    return false;
  }
#endif

#if defined(RTEMS_SMP) || CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  if (
    config->isr_level != 0
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == FALSE
      && _SMP_Need_inter_processor_interrupts()
#endif
  ) {
    return false;
  }
#endif

  /*
   *  We assume the Allocator Mutex is locked and dispatching is
   *  enabled when we get here.  We want to be able to run the
   *  user extensions with dispatching enabled.  The Allocator
   *  Mutex provides sufficient protection to let the user extensions
   *  run safely.
   */
  return _User_extensions_Thread_create( the_thread );
}

Status_Control _Thread_Initialize(
  Thread_Information         *information,
  Thread_Control             *the_thread,
  const Thread_Configuration *config
)
{
  bool ok;

  ok = _Thread_Try_initialize( information, the_thread, config );

  if ( !ok ) {
    _Objects_Close( &information->Objects, &the_thread->Object );
    _Thread_Free( information, the_thread );

    return STATUS_UNSATISFIED;
  }

  return STATUS_SUCCESSFUL;
}
