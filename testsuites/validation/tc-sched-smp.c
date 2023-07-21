/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSchedSmpValSmp
 */

/*
 * Copyright (C) 2021, 2022 embedded brains GmbH & Co. KG
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

/*
 * This file is part of the RTEMS quality process and was automatically
 * generated.  If you find something that needs to be fixed or
 * worded better please post a report or patch to an RTEMS mailing list
 * or raise a bug report:
 *
 * https://www.rtems.org/bugs.html
 *
 * For information on updating and regenerating please refer to the How-To
 * section in the Software Requirements Engineering chapter of the
 * RTEMS Software Engineering manual.  The manual is provided as a part of
 * a release.  For development sources please refer to the online
 * documentation at:
 *
 * https://docs.rtems.org
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems.h>
#include <rtems/test-scheduler.h>
#include <rtems/score/percpu.h>
#include <rtems/score/schedulersmp.h>
#include <rtems/score/threadimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreSchedSmpValSmp spec:/score/sched/smp/val/smp
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests SMP-specific scheduler behaviour.
 *
 * This test case performs the following actions:
 *
 * - Construct a system state in which a sticky thread is blocked while an idle
 *   thread executes on behalf of the thread.
 *
 *   - Block the sticky worker A while it uses an idle thread in the home
 *     scheduler.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a thread is preempted while it is
 *   blocked.
 *
 *   - Block worker A and preempt it before the withdraw node operations are
 *     performed for worker A.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a thread is rescheduled  while it is not
 *   scheduled on another scheduler.
 *
 *   - Reschedule worker A by the home scheduler while worker A is not
 *     scheduled on another scheduler.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which an ask for help request is cancelled
 *   while it is processed on another processor.
 *
 *   - Unblock worker A.  It cannot be scheduled on its home scheduler.
 *     Intercept the ask for help request.  Block the worker A.  This will
 *     cancel the ask for help request.  Remove the request while the other
 *     processor tries to cancel the request.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is already scheduled during a block operation.
 *
 *   - Block the runner thread while the owner thread of the highest priority
 *     ready node is already scheduled.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is blocked during a block operation.
 *
 *   - Block the runner thread while the owner thread of the highest priority
 *     ready node is blocked.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is already scheduled during a set affinity operation.
 *
 *   - Set the affinity of the runner thread while the owner thread of the
 *     highest priority ready node is already scheduled.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is already scheduled during a set affinity operation
 *   while a sticky node is involved.
 *
 *   - Set the affinity of the runner thread while the owner thread of the
 *     highest priority ready node is already scheduled.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is blocked during a set affinity operation.
 *
 *   - Set the affinity of the runner thread while the owner thread of the
 *     highest priority ready node is blocked.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is blocked during a set affinity operation while a
 *   sticky node is involved.
 *
 *   - Set the affinity of the runner thread while the owner thread of the
 *     highest priority ready node is blocked.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is already scheduled during a set priority operation.
 *
 *   - Set the priority of the runner thread while the owner thread of the
 *     highest priority ready node is already scheduled.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is already scheduled during a set priority operation
 *   while a sticky node is involved.
 *
 *   - Set the priority of the runner thread while the owner thread of the
 *     highest priority ready node is already scheduled.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is blocked during a set priority operation.
 *
 *   - Set the priority of the runner thread while the owner thread of the
 *     highest priority ready node is blocked.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is already scheduled during a yield operation.
 *
 *   - Yield while the owner thread of the highest priority ready node is
 *     already scheduled.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is already scheduled during a yield operation while a
 *   sticky node is involved.
 *
 *   - Yield while the owner thread of the highest priority ready node is
 *     already scheduled.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is blocked during a yield operation.
 *
 *   - Yield while the owner thread of the highest priority ready node is
 *     blocked.
 *
 *   - Clean up all used resources.
 *
 * - Construct a system state in which a scheduler tries to schedule a node
 *   those owner thread is blocked during a yield operation while a sticky node
 *   is involved.
 *
 *   - Yield while the owner thread of the highest priority ready node is
 *     blocked.
 *
 *   - Clean up all used resources.
 *
 * - Create three worker threads and a mutex.  Use the mutex and the worker to
 *   check that a not scheduled thread does not get removed from the set of
 *   ready threads of a scheduler when a help request is reconsidered.
 *
 *   - Prevent that worker B can perform a post-switch cleanup.
 *
 *   - Give worker C a lower priority than worker B.  Worker B will try to
 *     finish the thread dispatch by doing a post-switch cleanup.  The
 *     post-switch cleanup cannot progress since the runner owns the thread
 *     state lock.  Wait until the other processor waits on the thread state
 *     lock of worker B.
 *
 *   - Give worker C a higher priority than worker B.  Let worker B do its
 *     post-switch cleanup which will carry out the reconsider help requests
 *     for a not scheduled thread.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

typedef enum {
  WORKER_A,
  WORKER_B,
  WORKER_C,
  WORKER_COUNT
} WorkerIndex;

/**
 * @brief Test context for spec:/score/sched/smp/val/smp test case.
 */
typedef struct {
  /**
   * @brief This member contains the runner identifier.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the worker identifiers.
   */
  rtems_id worker_id[ WORKER_COUNT ];

  /**
   * @brief This member contains the mutex identifier.
   */
  rtems_id mutex_id;

  /**
   * @brief This member contains the sticky mutex identifier.
   */
  rtems_id sticky_id;

  /**
   * @brief This member contains the worker busy status.
   */
  volatile bool busy[ WORKER_COUNT ];

  /**
   * @brief This member contains an ISR lock context.
   */
  ISR_lock_Context lock_context;

  /**
   * @brief This member contains a counter.
   */
  uint32_t counter;

  /**
   * @brief If this member is true, then the worker shall be in the busy loop.
   */
  volatile bool is_busy[ WORKER_COUNT ];

  /**
   * @brief This member contains the per-CPU jobs.
   */
  Per_CPU_Job job[ 2 ];

  /**
   * @brief This member contains the per-CPU job contexts.
   */
  Per_CPU_Job_context job_context[ 2 ];

  /**
   * @brief This member contains the call within ISR request.
   */
  CallWithinISRRequest request;
} ScoreSchedSmpValSmp_Context;

static ScoreSchedSmpValSmp_Context
  ScoreSchedSmpValSmp_Instance;

#define EVENT_OBTAIN RTEMS_EVENT_0

#define EVENT_RELEASE RTEMS_EVENT_1

#define EVENT_STICKY_OBTAIN RTEMS_EVENT_2

#define EVENT_STICKY_RELEASE RTEMS_EVENT_3

#define EVENT_SYNC_RUNNER RTEMS_EVENT_4

#define EVENT_BUSY RTEMS_EVENT_5

typedef ScoreSchedSmpValSmp_Context Context;

static void SendAndSync(
  Context        *ctx,
  WorkerIndex     worker,
  rtems_event_set event
)
{
  SendEvents( ctx->worker_id[ worker ], EVENT_SYNC_RUNNER | event );
  ReceiveAllEvents( EVENT_SYNC_RUNNER );
  WaitForExecutionStop( ctx->worker_id[ worker ] );
}

static void MakeBusy( Context *ctx, WorkerIndex worker )
{
  ctx->is_busy[ worker ] = false;
  ctx->busy[ worker ] = true;
  SendEvents( ctx->worker_id[ worker ], EVENT_BUSY );
}

static void WaitForBusy( Context *ctx, WorkerIndex worker )
{
  while ( !ctx->is_busy[ worker ] ) {
    /* Wait */
  }
}

static void StopBusy( Context *ctx, WorkerIndex worker )
{
  ctx->busy[ worker ] = false;
  WaitForExecutionStop( ctx->worker_id[ worker ] );
}

static void MakeSticky( const Context *ctx )
{
  ObtainMutex( ctx->sticky_id );
}

static void CleanSticky( const Context *ctx )
{
  ReleaseMutex( ctx->sticky_id );
}

static void Block( void *arg )
{
  Context *ctx;

  ctx = arg;
  SuspendTask( ctx->runner_id );
  ResumeTask( ctx->runner_id );
}

static void OperationStopBusyC(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when,
  T_scheduler_operation    op
)
{
  Context *ctx;

  ctx = arg;

  if ( when == T_SCHEDULER_BEFORE && event->operation == op ) {
    T_scheduler_set_event_handler( NULL, NULL );
    StopBusy( ctx, WORKER_C );
  }
}

static void BlockStopBusyC(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  OperationStopBusyC( arg, event, when, T_SCHEDULER_BLOCK );
}

static void SetAffinityStopBusyC(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  OperationStopBusyC( arg, event, when, T_SCHEDULER_SET_AFFINITY );
}

static void UpdatePriorityStopBusyC(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  OperationStopBusyC( arg, event, when, T_SCHEDULER_UPDATE_PRIORITY );
}

static void YieldStopBusyC(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  OperationStopBusyC( arg, event, when, T_SCHEDULER_YIELD );
}

static void SuspendA( void *arg )
{
  Context *ctx;

  ctx = arg;
  SuspendTask( ctx->worker_id[ WORKER_A ] );
}

static void OperationSuspendA(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when,
  T_scheduler_operation    op
)
{
  Context *ctx;

  ctx = arg;

  if ( when == T_SCHEDULER_BEFORE && event->operation == op ) {
    const rtems_tcb *worker_a;

    T_scheduler_set_event_handler( NULL, NULL );
    ctx->job_context[ 0 ].handler = SuspendA;
    _Per_CPU_Submit_job( _Per_CPU_Get_by_index( 1 ), &ctx->job[ 0 ] );

    worker_a = GetThread( ctx->worker_id[ WORKER_A ] );

    while ( worker_a->Scheduler.state != THREAD_SCHEDULER_BLOCKED ) {
      RTEMS_COMPILER_MEMORY_BARRIER();
    }
  }
}

static void BlockSuspendA(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  OperationSuspendA( arg, event, when, T_SCHEDULER_BLOCK );
}

static void SetAffinitySuspendA(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  OperationSuspendA( arg, event, when, T_SCHEDULER_SET_AFFINITY );
}

static void UpdatePrioritySuspendA(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  OperationSuspendA( arg, event, when, T_SCHEDULER_UPDATE_PRIORITY );
}

static void YieldSuspendA(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  OperationSuspendA( arg, event, when, T_SCHEDULER_YIELD );
}

static void GuideAskForHelp( void *arg )
{
  Context         *ctx;
  Per_CPU_Control *cpu;
  ISR_lock_Context lock_context;

  ctx = arg;
  cpu = _Per_CPU_Get_by_index( 0 );

  _ISR_lock_ISR_disable( &lock_context );
  _Per_CPU_Acquire( cpu, &lock_context );

  ISRLockWaitForOthers( &cpu->Lock, 1 );

  ctx->job_context[ 0 ].handler = SuspendA;
  _Per_CPU_Submit_job( _Per_CPU_Get_by_index( 1 ), &ctx->job[ 0 ] );
  ISRLockWaitForOthers( &cpu->Lock, 2 );

  _Per_CPU_Release( cpu, &lock_context );
  _ISR_lock_ISR_enable( &lock_context );
}

static void InterceptAskForHelp( void *arg )
{
  Context         *ctx;
  Per_CPU_Control *cpu_self;

  ctx = arg;
  cpu_self = _Per_CPU_Get();

  if ( rtems_scheduler_get_processor_maximum() > 2 ) {
    ctx->job_context[ 1 ].handler = GuideAskForHelp;
    _Per_CPU_Submit_job( _Per_CPU_Get_by_index( 2 ), &ctx->job[ 1 ] );
    ISRLockWaitForOwned( &cpu_self->Lock );
  } else {
    ISR_lock_Context lock_context;
    Chain_Node      *node;
    Thread_Control  *thread;

    _ISR_lock_ISR_disable( &lock_context );
    _Per_CPU_Acquire( cpu_self, &lock_context );
    ctx->job_context[ 0 ].handler = SuspendA;
    _Per_CPU_Submit_job( _Per_CPU_Get_by_index( 1 ), &ctx->job[ 0 ] );
    ISRLockWaitForOthers( &cpu_self->Lock, 1 );

    /* See _Thread_Preemption_intervention() */
    node = _Chain_Get_first_unprotected( &cpu_self->Threads_in_need_for_help );
    thread = THREAD_OF_SCHEDULER_HELP_NODE( node );
    T_assert_eq_ptr( thread, GetThread( ctx->worker_id[ WORKER_A ] ) );
    thread->Scheduler.ask_for_help_cpu = NULL;

    _Per_CPU_Release( cpu_self, &lock_context );
    _ISR_lock_ISR_enable( &lock_context );
  }
}

static void UnblockAskForHelp(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  ctx = arg;

  if (
    when == T_SCHEDULER_BEFORE &&
    event->operation == T_SCHEDULER_UNBLOCK
  ) {
    T_scheduler_set_event_handler( NULL, NULL );
    ctx->request.handler = InterceptAskForHelp;
    ctx->request.arg = ctx;
    CallWithinISRSubmit( &ctx->request );
  }
}

static void RaiseWorkerPriorityWithIdleRunner( void *arg )
{
  Context *ctx;

  ctx = arg;
  SuspendTask( ctx->runner_id );
  T_scheduler_set_event_handler( UpdatePriorityStopBusyC, ctx );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_HIGH );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_NORMAL );
  ResumeTask( ctx->runner_id );
}

static void MakeReady( void *arg )
{
  Context *ctx;

  ctx = arg;
  MakeBusy( ctx, WORKER_C );
}

static void UpdatePriorityMakeReady(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  ctx = arg;

  if (
    when == T_SCHEDULER_BEFORE &&
    event->operation == T_SCHEDULER_UPDATE_PRIORITY
  ) {
    Thread_Control  *thread;

    T_scheduler_set_event_handler( NULL, NULL );

    thread = GetThread( ctx->worker_id[ WORKER_A ] );
    T_eq_int( thread->Scheduler.state, THREAD_SCHEDULER_SCHEDULED );

    ctx->job_context[ 0 ].handler = MakeReady;
    _Per_CPU_Submit_job( _Per_CPU_Get_by_index( 1 ), &ctx->job[ 0 ] );

    while ( thread->Scheduler.state != THREAD_SCHEDULER_READY ) {
      RTEMS_COMPILER_MEMORY_BARRIER();
    }
  }
}

static void ReadyToScheduled( void *arg )
{
  Context *ctx;

  ctx = arg;
  SuspendTask( ctx->runner_id );

  T_scheduler_set_event_handler( UpdatePriorityMakeReady, ctx );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_HIGH );

  SetPriority( ctx->runner_id, PRIO_VERY_HIGH );
  ResumeTask( ctx->runner_id );
}

static void BlockAndReuseIdle( void *arg )
{
  Context *ctx;

  ctx = arg;
  SuspendTask( ctx->runner_id );
  SuspendTask( ctx->worker_id[ WORKER_A ] );
  ResumeTask( ctx->worker_id[ WORKER_A ] );
  SetPriority( ctx->runner_id, PRIO_HIGH );
  ResumeTask( ctx->runner_id );
}

static void Preempt( void *arg )
{
  Context *ctx;

  ctx = arg;
  MakeBusy( ctx, WORKER_C );
}

static void BlockAndPreempt(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  ctx = arg;

  if ( when == T_SCHEDULER_AFTER && event->operation == T_SCHEDULER_BLOCK ) {
    Thread_Control  *thread;

    T_scheduler_set_event_handler( NULL, NULL );

    thread = GetThread( ctx->worker_id[ WORKER_A ] );
    T_eq_int( thread->Scheduler.state, THREAD_SCHEDULER_BLOCKED );

    ctx->job_context[ 0 ].handler = Preempt;
    _Per_CPU_Submit_job( _Per_CPU_Get_by_index( 1 ), &ctx->job[ 0 ] );
    _Per_CPU_Wait_for_job( _Per_CPU_Get_by_index( 1 ), &ctx->job[ 0 ] );
  }
}

static void PrepareOwnerScheduled( Context *ctx )
{
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_B_ID, PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_OBTAIN );
  SendAndSync( ctx, WORKER_B, EVENT_OBTAIN );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_B_ID, PRIO_HIGH );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_NORMAL );
  MakeBusy( ctx, WORKER_C );
  MakeBusy( ctx, WORKER_A );
}

static void CleanupOwnerScheduled( Context *ctx )
{
  StopBusy( ctx, WORKER_A );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_HIGH );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_RELEASE );
  SendAndSync( ctx, WORKER_B, EVENT_RELEASE );
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_A_ID, PRIO_HIGH );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_A_ID, PRIO_HIGH );
}

static void PrepareOwnerBlocked( Context *ctx )
{
  SetScheduler( ctx->worker_id[ WORKER_A ], SCHEDULER_B_ID, PRIO_NORMAL );
  SendAndSync( ctx, WORKER_A, EVENT_OBTAIN );
  SendEvents( ctx->worker_id[ WORKER_B ], EVENT_OBTAIN );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_B_ID, PRIO_HIGH );
  MakeBusy( ctx, WORKER_C );
  SetPriority( ctx->worker_id[ WORKER_B ], PRIO_LOW );
  MakeBusy( ctx, WORKER_A );
  SetPriority( ctx->worker_id[ WORKER_B ], PRIO_NORMAL );
}

static void CleanupOwnerBlocked( Context *ctx )
{
  StopBusy( ctx, WORKER_C );
  ResumeTask( ctx->worker_id[ WORKER_A ] );
  StopBusy( ctx, WORKER_A );
  SendAndSync( ctx, WORKER_A, EVENT_RELEASE );
  SetPriority( ctx->worker_id[ WORKER_B ], PRIO_HIGH );
  SendEvents( ctx->worker_id[ WORKER_B ], EVENT_RELEASE );
  SetScheduler( ctx->worker_id[ WORKER_A ], SCHEDULER_A_ID, PRIO_HIGH );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_A_ID, PRIO_HIGH );
}

static void ReconsiderHelpRequestB(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  (void) when;
  ctx = arg;

  if ( event->operation == T_SCHEDULER_RECONSIDER_HELP_REQUEST ) {
    Scheduler_SMP_Node *node;

    node = (Scheduler_SMP_Node *) event->node;
    T_eq_int( node->state, SCHEDULER_SMP_NODE_READY );
    ++ctx->counter;
  }
}

static void ReleaseThreadLockB(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  ctx = arg;

  if (
    when == T_SCHEDULER_AFTER &&
    event->operation == T_SCHEDULER_UPDATE_PRIORITY
  ) {
    Thread_Control *worker_b;

    T_scheduler_set_event_handler( ReconsiderHelpRequestB, ctx );

    worker_b = GetThread( ctx->worker_id[ WORKER_B ] );
    T_eq_int( worker_b->Scheduler.state, THREAD_SCHEDULER_READY );

    _Thread_State_release_critical( worker_b, &ctx->lock_context );
  }
}

static void Worker( rtems_task_argument arg, WorkerIndex worker )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_SYNC_RUNNER ) != 0 ) {
      SendEvents( ctx->runner_id, EVENT_SYNC_RUNNER );
    }

    if ( ( events & EVENT_OBTAIN ) != 0 ) {
      ObtainMutex( ctx->mutex_id );
    }

    if ( ( events & EVENT_RELEASE ) != 0 ) {
      ReleaseMutex( ctx->mutex_id );
    }

    if ( ( events & EVENT_STICKY_OBTAIN ) != 0 ) {
      ObtainMutex( ctx->sticky_id );
    }

    if ( ( events & EVENT_STICKY_RELEASE ) != 0 ) {
      ReleaseMutex( ctx->sticky_id );
    }

    if ( ( events & EVENT_BUSY ) != 0 ) {
      ctx->is_busy[ worker ] = true;

      while ( ctx->busy[ worker ] ) {
        /* Wait */
      }

      ctx->is_busy[ worker ] = false;
    }
  }
}

static void WorkerA( rtems_task_argument arg )
{
  Worker( arg, WORKER_A );
}

static void WorkerB( rtems_task_argument arg )
{
  Worker( arg, WORKER_B );
}

static void WorkerC( rtems_task_argument arg )
{
  Worker( arg, WORKER_C );
}

static void ScoreSchedSmpValSmp_Setup( ScoreSchedSmpValSmp_Context *ctx )
{
  rtems_status_code sc;
  size_t            i;

  ctx->runner_id = rtems_task_self();
  ctx->mutex_id = CreateMutex();

  for ( i = 0; i < RTEMS_ARRAY_SIZE( ctx->job ); ++i ) {
    ctx->job_context[ i ].arg = ctx;
    ctx->job[ i ].context = &ctx->job_context[ i ];
  }

  sc = rtems_semaphore_create(
    rtems_build_name( 'S', 'T', 'K', 'Y' ),
    1,
    RTEMS_BINARY_SEMAPHORE | RTEMS_PRIORITY |
      RTEMS_MULTIPROCESSOR_RESOURCE_SHARING,
    PRIO_NORMAL,
    &ctx->sticky_id
  );
  T_rsc_success( sc );

  SetSelfPriority( PRIO_NORMAL );

  ctx->worker_id[ WORKER_A ] = CreateTask( "WRKA", PRIO_HIGH );
  StartTask( ctx->worker_id[ WORKER_A ], WorkerA, ctx );

  ctx->worker_id[ WORKER_B ] = CreateTask( "WRKB", PRIO_HIGH );
  StartTask( ctx->worker_id[ WORKER_B ], WorkerB, ctx );

  ctx->worker_id[ WORKER_C ] = CreateTask( "WRKC", PRIO_HIGH );
  StartTask( ctx->worker_id[ WORKER_C ], WorkerC, ctx );
}

static void ScoreSchedSmpValSmp_Setup_Wrap( void *arg )
{
  ScoreSchedSmpValSmp_Context *ctx;

  ctx = arg;
  ScoreSchedSmpValSmp_Setup( ctx );
}

static void ScoreSchedSmpValSmp_Teardown( ScoreSchedSmpValSmp_Context *ctx )
{
  DeleteTask( ctx->worker_id[ WORKER_A ] );
  DeleteTask( ctx->worker_id[ WORKER_B ] );
  DeleteTask( ctx->worker_id[ WORKER_C ] );
  DeleteMutex( ctx->mutex_id );
  DeleteMutex( ctx->sticky_id );
  RestoreRunnerPriority();
}

static void ScoreSchedSmpValSmp_Teardown_Wrap( void *arg )
{
  ScoreSchedSmpValSmp_Context *ctx;

  ctx = arg;
  ScoreSchedSmpValSmp_Teardown( ctx );
}

static T_fixture ScoreSchedSmpValSmp_Fixture = {
  .setup = ScoreSchedSmpValSmp_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreSchedSmpValSmp_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &ScoreSchedSmpValSmp_Instance
};

/**
 * @brief Construct a system state in which a sticky thread is blocked while an
 *   idle thread executes on behalf of the thread.
 */
static void ScoreSchedSmpValSmp_Action_0( ScoreSchedSmpValSmp_Context *ctx )
{
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_B_ID, PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_OBTAIN );
  SendAndSync( ctx, WORKER_B, EVENT_OBTAIN );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_STICKY_OBTAIN );
  MakeBusy( ctx, WORKER_A );
  WaitForBusy( ctx, WORKER_A );

  /*
   * Block the sticky worker A while it uses an idle thread in the home
   * scheduler.
   */
  CallWithinISR( BlockAndReuseIdle, ctx );

  /*
   * Clean up all used resources.
   */
  StopBusy( ctx, WORKER_A );
  SendAndSync( ctx, WORKER_A, EVENT_STICKY_RELEASE );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_HIGH );
  SetSelfPriority( PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_RELEASE );
  SendAndSync( ctx, WORKER_B, EVENT_RELEASE );
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_A_ID, PRIO_HIGH );
}

/**
 * @brief Construct a system state in which a thread is preempted while it is
 *   blocked.
 */
static void ScoreSchedSmpValSmp_Action_1( ScoreSchedSmpValSmp_Context *ctx )
{
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_B_ID, PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_OBTAIN );
  SendAndSync( ctx, WORKER_B, EVENT_OBTAIN );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_B_ID, PRIO_HIGH );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_NORMAL );
  MakeBusy( ctx, WORKER_A );
  WaitForBusy( ctx, WORKER_A );

  /*
   * Block worker A and preempt it before the withdraw node operations are
   * performed for worker A.
   */
  T_scheduler_set_event_handler( BlockAndPreempt, ctx );
  SuspendTask( ctx->worker_id[ WORKER_A ] );

  /*
   * Clean up all used resources.
   */
  ResumeTask( ctx->worker_id[ WORKER_A ] );
  StopBusy( ctx, WORKER_C );
  StopBusy( ctx, WORKER_A );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_HIGH );
  SetSelfPriority( PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_RELEASE );
  SendAndSync( ctx, WORKER_B, EVENT_RELEASE );
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_A_ID, PRIO_HIGH );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_A_ID, PRIO_HIGH );
}

/**
 * @brief Construct a system state in which a thread is rescheduled  while it
 *   is not scheduled on another scheduler.
 */
static void ScoreSchedSmpValSmp_Action_2( ScoreSchedSmpValSmp_Context *ctx )
{
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_B_ID, PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_OBTAIN );
  SendAndSync( ctx, WORKER_B, EVENT_OBTAIN );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_B_ID, PRIO_HIGH );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_STICKY_OBTAIN );
  MakeBusy( ctx, WORKER_A );
  WaitForBusy( ctx, WORKER_A );

  /*
   * Reschedule worker A by the home scheduler while worker A is not scheduled
   * on another scheduler.
   */
  CallWithinISR( ReadyToScheduled, ctx );

  /*
   * Clean up all used resources.
   */
  StopBusy( ctx, WORKER_C );
  StopBusy( ctx, WORKER_A );
  SendAndSync( ctx, WORKER_A, EVENT_STICKY_RELEASE );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_HIGH );
  SetSelfPriority( PRIO_NORMAL );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_RELEASE );
  SendAndSync( ctx, WORKER_B, EVENT_RELEASE );
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_A_ID, PRIO_HIGH );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_A_ID, PRIO_HIGH );
}

/**
 * @brief Construct a system state in which an ask for help request is
 *   cancelled while it is processed on another processor.
 */
static void ScoreSchedSmpValSmp_Action_3( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerScheduled( ctx );

  /*
   * Unblock worker A.  It cannot be scheduled on its home scheduler. Intercept
   * the ask for help request.  Block the worker A.  This will cancel the ask
   * for help request.  Remove the request while the other processor tries to
   * cancel the request.
   */
  SuspendTask( ctx->worker_id[ WORKER_A ] );
  T_scheduler_set_event_handler( UnblockAskForHelp, ctx );
  ResumeTask( ctx->worker_id[ WORKER_A ] );

  /*
   * Clean up all used resources.
   */
  ResumeTask( ctx->worker_id[ WORKER_A ] );
  StopBusy( ctx, WORKER_C );
  CleanupOwnerScheduled( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is already scheduled during a block operation.
 */
static void ScoreSchedSmpValSmp_Action_4( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerScheduled( ctx );

  /*
   * Block the runner thread while the owner thread of the highest priority
   * ready node is already scheduled.
   */
  T_scheduler_set_event_handler( BlockStopBusyC, ctx );
  CallWithinISR( Block, ctx );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerScheduled( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is blocked during a block operation.
 */
static void ScoreSchedSmpValSmp_Action_5( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerBlocked( ctx );

  /*
   * Block the runner thread while the owner thread of the highest priority
   * ready node is blocked.
   */
  T_scheduler_set_event_handler( BlockSuspendA, ctx );
  CallWithinISR( Block, ctx );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerBlocked( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is already scheduled during a set affinity
 *   operation.
 */
static void ScoreSchedSmpValSmp_Action_6( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerScheduled( ctx );

  /*
   * Set the affinity of the runner thread while the owner thread of the
   * highest priority ready node is already scheduled.
   */
  T_scheduler_set_event_handler( SetAffinityStopBusyC, ctx );
  SetSelfAffinityAll();

  /*
   * Clean up all used resources.
   */
  CleanupOwnerScheduled( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is already scheduled during a set affinity
 *   operation while a sticky node is involved.
 */
static void ScoreSchedSmpValSmp_Action_7( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerScheduled( ctx );

  /*
   * Set the affinity of the runner thread while the owner thread of the
   * highest priority ready node is already scheduled.
   */
  MakeSticky( ctx );
  T_scheduler_set_event_handler( SetAffinityStopBusyC, ctx );
  SetSelfAffinityAll();
  CleanSticky( ctx );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerScheduled( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is blocked during a set affinity operation.
 */
static void ScoreSchedSmpValSmp_Action_8( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerBlocked( ctx );

  /*
   * Set the affinity of the runner thread while the owner thread of the
   * highest priority ready node is blocked.
   */
  T_scheduler_set_event_handler( SetAffinitySuspendA, ctx );
  SetSelfAffinityAll();

  /*
   * Clean up all used resources.
   */
  CleanupOwnerBlocked( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is blocked during a set affinity operation while a
 *   sticky node is involved.
 */
static void ScoreSchedSmpValSmp_Action_9( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerBlocked( ctx );

  /*
   * Set the affinity of the runner thread while the owner thread of the
   * highest priority ready node is blocked.
   */
  MakeSticky( ctx );
  T_scheduler_set_event_handler( SetAffinitySuspendA, ctx );
  SetSelfAffinityAll();
  CleanSticky( ctx );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerBlocked( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is already scheduled during a set priority
 *   operation.
 */
static void ScoreSchedSmpValSmp_Action_10( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerScheduled( ctx );

  /*
   * Set the priority of the runner thread while the owner thread of the
   * highest priority ready node is already scheduled.
   */
  SetSelfPriority( PRIO_HIGH );
  T_scheduler_set_event_handler( UpdatePriorityStopBusyC, ctx );
  SetSelfPriority( PRIO_NORMAL );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerScheduled( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is already scheduled during a set priority
 *   operation while a sticky node is involved.
 */
static void ScoreSchedSmpValSmp_Action_11( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerScheduled( ctx );

  /*
   * Set the priority of the runner thread while the owner thread of the
   * highest priority ready node is already scheduled.
   */
  MakeSticky( ctx );
  CallWithinISR( RaiseWorkerPriorityWithIdleRunner, ctx );
  CleanSticky( ctx );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerScheduled( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is blocked during a set priority operation.
 */
static void ScoreSchedSmpValSmp_Action_12( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerBlocked( ctx );

  /*
   * Set the priority of the runner thread while the owner thread of the
   * highest priority ready node is blocked.
   */
  SetSelfPriority( PRIO_HIGH );
  T_scheduler_set_event_handler( UpdatePrioritySuspendA, ctx );
  SetSelfPriority( PRIO_NORMAL );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerBlocked( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is already scheduled during a yield operation.
 */
static void ScoreSchedSmpValSmp_Action_13( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerScheduled( ctx );

  /*
   * Yield while the owner thread of the highest priority ready node is already
   * scheduled.
   */
  T_scheduler_set_event_handler( YieldStopBusyC, ctx );
  Yield();

  /*
   * Clean up all used resources.
   */
  CleanupOwnerScheduled( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is already scheduled during a yield operation
 *   while a sticky node is involved.
 */
static void ScoreSchedSmpValSmp_Action_14( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerScheduled( ctx );

  /*
   * Yield while the owner thread of the highest priority ready node is already
   * scheduled.
   */
  MakeSticky( ctx );
  T_scheduler_set_event_handler( YieldStopBusyC, ctx );
  Yield();
  CleanSticky( ctx );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerScheduled( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is blocked during a yield operation.
 */
static void ScoreSchedSmpValSmp_Action_15( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerBlocked( ctx );

  /*
   * Yield while the owner thread of the highest priority ready node is
   * blocked.
   */
  T_scheduler_set_event_handler( YieldSuspendA, ctx );
  Yield();

  /*
   * Clean up all used resources.
   */
  CleanupOwnerBlocked( ctx );
}

/**
 * @brief Construct a system state in which a scheduler tries to schedule a
 *   node those owner thread is blocked during a yield operation while a sticky
 *   node is involved.
 */
static void ScoreSchedSmpValSmp_Action_16( ScoreSchedSmpValSmp_Context *ctx )
{
  PrepareOwnerBlocked( ctx );

  /*
   * Yield while the owner thread of the highest priority ready node is
   * blocked.
   */
  MakeSticky( ctx );
  T_scheduler_set_event_handler( YieldSuspendA, ctx );
  Yield();
  CleanSticky( ctx );

  /*
   * Clean up all used resources.
   */
  CleanupOwnerBlocked( ctx );
}

/**
 * @brief Create three worker threads and a mutex.  Use the mutex and the
 *   worker to check that a not scheduled thread does not get removed from the
 *   set of ready threads of a scheduler when a help request is reconsidered.
 */
static void ScoreSchedSmpValSmp_Action_17( ScoreSchedSmpValSmp_Context *ctx )
{
  Thread_Control *worker_b;

  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_B_ID, PRIO_NORMAL );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_B_ID, PRIO_HIGH );
  SendAndSync( ctx, WORKER_B, EVENT_OBTAIN );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_OBTAIN );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_LOW );
  MakeBusy( ctx, WORKER_B );
  WaitForBusy( ctx, WORKER_B );
  MakeBusy( ctx, WORKER_C );
  WaitForBusy( ctx, WORKER_C );

  /*
   * Prevent that worker B can perform a post-switch cleanup.
   */
  worker_b = GetThread( ctx->worker_id[ WORKER_B ] );
  _Thread_State_acquire( worker_b, &ctx->lock_context );
  _ISR_lock_ISR_enable( &ctx->lock_context );

  /*
   * Give worker C a lower priority than worker B.  Worker B will try to finish
   * the thread dispatch by doing a post-switch cleanup.  The post-switch
   * cleanup cannot progress since the runner owns the thread state lock.  Wait
   * until the other processor waits on the thread state lock of worker B.
   */
  SetPriority( ctx->worker_id[ WORKER_C ], PRIO_LOW );
  TicketLockWaitForOthers( &worker_b->Join_queue.Queue.Lock, 1 );

  /*
   * Give worker C a higher priority than worker B.  Let worker B do its
   * post-switch cleanup which will carry out the reconsider help requests for
   * a not scheduled thread.
   */
  ctx->counter = 0;
  T_scheduler_set_event_handler( ReleaseThreadLockB, ctx );
  SetPriority( ctx->worker_id[ WORKER_C ], PRIO_HIGH );
  T_scheduler_set_event_handler( NULL, NULL );
  T_eq_u32( ctx->counter, 4 );

  /*
   * Clean up all used resources.
   */
  StopBusy( ctx, WORKER_B );
  StopBusy( ctx, WORKER_C );
  SendAndSync( ctx, WORKER_B, EVENT_RELEASE );
  SetPriority( ctx->worker_id[ WORKER_A ], PRIO_HIGH );
  SendEvents( ctx->worker_id[ WORKER_A ], EVENT_RELEASE );
  SetScheduler( ctx->worker_id[ WORKER_B ], SCHEDULER_A_ID, PRIO_HIGH );
  SetScheduler( ctx->worker_id[ WORKER_C ], SCHEDULER_A_ID, PRIO_HIGH );
}

/**
 * @fn void T_case_body_ScoreSchedSmpValSmp( void )
 */
T_TEST_CASE_FIXTURE( ScoreSchedSmpValSmp, &ScoreSchedSmpValSmp_Fixture )
{
  ScoreSchedSmpValSmp_Context *ctx;

  ctx = T_fixture_context();

  ScoreSchedSmpValSmp_Action_0( ctx );
  ScoreSchedSmpValSmp_Action_1( ctx );
  ScoreSchedSmpValSmp_Action_2( ctx );
  ScoreSchedSmpValSmp_Action_3( ctx );
  ScoreSchedSmpValSmp_Action_4( ctx );
  ScoreSchedSmpValSmp_Action_5( ctx );
  ScoreSchedSmpValSmp_Action_6( ctx );
  ScoreSchedSmpValSmp_Action_7( ctx );
  ScoreSchedSmpValSmp_Action_8( ctx );
  ScoreSchedSmpValSmp_Action_9( ctx );
  ScoreSchedSmpValSmp_Action_10( ctx );
  ScoreSchedSmpValSmp_Action_11( ctx );
  ScoreSchedSmpValSmp_Action_12( ctx );
  ScoreSchedSmpValSmp_Action_13( ctx );
  ScoreSchedSmpValSmp_Action_14( ctx );
  ScoreSchedSmpValSmp_Action_15( ctx );
  ScoreSchedSmpValSmp_Action_16( ctx );
  ScoreSchedSmpValSmp_Action_17( ctx );
}

/** @} */
