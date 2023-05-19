/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreThreadValSmp
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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
#include <rtems/score/smpbarrier.h>
#include <rtems/score/threadimpl.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreThreadValSmp spec:/score/thread/val/smp
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests SMP-specific thread behaviour.
 *
 * This test case performs the following actions:
 *
 * - Create three worker threads and a mutex.  Use the mutex and the worker to
 *   move to a helping scheduler.
 *
 *   - Pin the runner thread while it executes on a processor owned by a
 *     helping scheduler.
 *
 *   - Pin and unpin the runner thread.  This is a nested operation.
 *
 *   - Preempt the pinned runner thread.  Worker B and C execute at the same
 *     time on processor 0 and 1 respectively for some point in time.  This
 *     shows that the pinning of the runner thread is maintained.
 *
 *   - Unpin the runner thread.  The runner moves back to its home scheduler.
 *
 *   - Release the mutex.
 *
 *   - Pin the runner thread.  Unpin the runner thread while it is suspended.
 *
 *   - Make sure the worker released the mutex.
 *
 *   - Clean up all used resources.
 *
 * - Create three worker threads and a mutex.  Use the mutex and the worker to
 *   check that a suspended thread does not reconsider help requests.
 *
 *   - Let worker B help worker A through the mutex.  Preempt worker A.  Delay
 *     the thread switch to worker A.
 *
 *   - Suspend worker A and let it wait on its thread state lock.  Check that
 *     worker A did not reconsider help requests.
 *
 *   - Resume worker A.  Check that worker A did reconsider help requests after
 *     the thread dispatch.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/thread/val/smp test case.
 */
typedef struct {
  /**
   * @brief This member contains the worker A identifier.
   */
  rtems_id worker_a_id;

  /**
   * @brief This member contains the worker B identifier.
   */
  rtems_id worker_b_id;

  /**
   * @brief This member contains the worker C identifier.
   */
  rtems_id worker_c_id;

  /**
   * @brief This member contains the mutex identifier.
   */
  rtems_id mutex_id;

  /**
   * @brief If this member is true, then the worker shall busy wait.
   */
  volatile bool busy;

  /**
   * @brief This member contains a counter for EVENT_COUNT.
   */
  volatile uint32_t counter;

  /**
   * @brief This member contains the barrier to synchronize the runner and the
   *   workers.
   */
  SMP_barrier_Control barrier;

  /**
   * @brief This member contains the barrier state for the runner processor.
   */
  SMP_barrier_State barrier_state;
} ScoreThreadValSmp_Context;

static ScoreThreadValSmp_Context
  ScoreThreadValSmp_Instance;

#define EVENT_OBTAIN RTEMS_EVENT_0

#define EVENT_RELEASE RTEMS_EVENT_1

#define EVENT_COUNT_EARLY RTEMS_EVENT_2

#define EVENT_BUSY RTEMS_EVENT_3

#define EVENT_COUNT RTEMS_EVENT_4

#define EVENT_LET_WORKER_C_COUNT RTEMS_EVENT_5

#define EVENT_SET_TASK_SWITCH_EXTENSION RTEMS_EVENT_6

typedef ScoreThreadValSmp_Context Context;

static void TaskSwitchExtension( rtems_tcb *executing, rtems_tcb *heir )
{
  Context        *ctx;
  Thread_Control *thread;

  (void) executing;
  (void) heir;

  ctx = T_fixture_context();
  thread = GetThread( ctx->worker_a_id );

  if ( thread == heir ) {
    SMP_barrier_State state;

    _SMP_barrier_State_initialize( &state );

    /* B0 */
    _SMP_barrier_Wait( &ctx->barrier, &state, 2 );

    /* B1 */
    _SMP_barrier_Wait( &ctx->barrier, &state, 2 );
  }
}

static void WorkerTask( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_OBTAIN ) != 0 ) {
      ObtainMutex( ctx->mutex_id );
    }

    if ( ( events & EVENT_RELEASE ) != 0 ) {
      ReleaseMutex( ctx->mutex_id );
    }

    if ( ( events & EVENT_COUNT_EARLY ) != 0 ) {
      ++ctx->counter;
    }

    if ( ( events & EVENT_BUSY ) != 0 ) {
      while ( ctx->busy ) {
        /* Do nothing */
      }
    }

    if ( ( events & EVENT_COUNT ) != 0 ) {
      ++ctx->counter;
    }

    if ( ( events & EVENT_LET_WORKER_C_COUNT ) != 0 ) {
      uint32_t counter;

      counter = ctx->counter;
      SendEvents( ctx->worker_c_id, EVENT_COUNT );

      while ( ctx->counter == counter ) {
        /* Wait */
      }
    }

    if ( ( events & EVENT_SET_TASK_SWITCH_EXTENSION ) != 0 ) {
      SetTaskSwitchExtension( TaskSwitchExtension );
    }
  }
}

static void SchedulerBlock(
  void                    *arg,
  const T_scheduler_event *event,
  T_scheduler_when         when
)
{
  Context *ctx;

  ctx = arg;

  if (
    when == T_SCHEDULER_BEFORE &&
    event->operation == T_SCHEDULER_BLOCK
  ) {
    Thread_Control *thread;

    T_scheduler_set_event_handler( NULL, NULL );

    /* B1 */
    _SMP_barrier_Wait( &ctx->barrier, &ctx->barrier_state, 2 );

    thread = GetThread( ctx->worker_a_id );
    TicketLockWaitForOthers( &thread->Join_queue.Queue.Lock, 1 );
  }
}

static void Suspend( void *arg )
{
  Thread_Control *thread;

  thread = arg;
  SuspendTask( thread->Object.id );
}

static void Resume( void *arg )
{
  Thread_Control *thread;

  thread = arg;
  ResumeTask( thread->Object.id );
}

static void WaitForCounter( const Context *ctx, uint32_t expected )
{
  while ( ctx->counter != expected ) {
    /* Wait */
  }
}

static void ScoreThreadValSmp_Setup( ScoreThreadValSmp_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void ScoreThreadValSmp_Setup_Wrap( void *arg )
{
  ScoreThreadValSmp_Context *ctx;

  ctx = arg;
  ScoreThreadValSmp_Setup( ctx );
}

static void ScoreThreadValSmp_Teardown( ScoreThreadValSmp_Context *ctx )
{
  RestoreRunnerPriority();
}

static void ScoreThreadValSmp_Teardown_Wrap( void *arg )
{
  ScoreThreadValSmp_Context *ctx;

  ctx = arg;
  ScoreThreadValSmp_Teardown( ctx );
}

static T_fixture ScoreThreadValSmp_Fixture = {
  .setup = ScoreThreadValSmp_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreThreadValSmp_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &ScoreThreadValSmp_Instance
};

/**
 * @brief Create three worker threads and a mutex.  Use the mutex and the
 *   worker to move to a helping scheduler.
 */
static void ScoreThreadValSmp_Action_0( ScoreThreadValSmp_Context *ctx )
{
  Per_CPU_Control*cpu_self;
  Thread_Control *executing;

  executing = _Thread_Get_executing();
  ctx->counter = 0;

  ctx->mutex_id = CreateMutex();

  ctx->worker_a_id = CreateTask( "WRKA", PRIO_NORMAL );
  SetScheduler( ctx->worker_a_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_a_id, WorkerTask, ctx );

  ctx->worker_b_id = CreateTask( "WRKB", PRIO_HIGH );
  StartTask( ctx->worker_b_id, WorkerTask, ctx );

  ctx->worker_c_id = CreateTask( "WRKC", PRIO_LOW );
  StartTask( ctx->worker_c_id, WorkerTask, ctx );

  ObtainMutex( ctx->mutex_id );
  SendEvents( ctx->worker_a_id, EVENT_OBTAIN | EVENT_RELEASE );

  ctx->busy = true;
  SendEvents( ctx->worker_b_id, EVENT_BUSY );

  /*
   * Pin the runner thread while it executes on a processor owned by a helping
   * scheduler.
   */
  T_eq_u32( rtems_scheduler_get_processor(), 1 );
  _Thread_Pin( executing );

  /*
   * Pin and unpin the runner thread.  This is a nested operation.
   */
  T_eq_u32( rtems_scheduler_get_processor(), 1 );
  _Thread_Pin( executing );
  _Thread_Unpin( executing, _Per_CPU_Get_snapshot() );

  /*
   * Preempt the pinned runner thread.  Worker B and C execute at the same time
   * on processor 0 and 1 respectively for some point in time.  This shows that
   * the pinning of the runner thread is maintained.
   */
  ctx->busy = false;
  SetScheduler( ctx->worker_b_id, SCHEDULER_B_ID, PRIO_HIGH );
  SendEvents( ctx->worker_b_id, EVENT_LET_WORKER_C_COUNT );

  T_eq_u32( rtems_scheduler_get_processor(), 1 );
  T_eq_u32( ctx->counter, 1 );

  /*
   * Unpin the runner thread.  The runner moves back to its home scheduler.
   */
  cpu_self = _Thread_Dispatch_disable();
  _Thread_Unpin( executing, cpu_self );
  _Thread_Dispatch_direct( cpu_self );

  T_eq_u32( rtems_scheduler_get_processor(), 0 );

  /*
   * Release the mutex.
   */
  ReleaseMutex( ctx->mutex_id);
  T_eq_u32( rtems_scheduler_get_processor(), 0 );

  /*
   * Pin the runner thread.  Unpin the runner thread while it is suspended.
   */
  _Thread_Pin( executing );

  /* We have to preempt the runner to end up in _Thread_Do_unpin() */
  SetPriority( ctx->worker_c_id, PRIO_HIGH );
  SendEvents( ctx->worker_c_id, EVENT_COUNT );
  T_eq_u32( ctx->counter, 2 );

  cpu_self = _Thread_Dispatch_disable();
  CallWithinISR( Suspend, executing );
  _Thread_Unpin( executing, cpu_self );
  CallWithinISR( Resume, executing );
  _Thread_Dispatch_direct( cpu_self );

  /*
   * Make sure the worker released the mutex.
   */
  SetSelfScheduler( SCHEDULER_B_ID, PRIO_LOW );
  SetSelfScheduler( SCHEDULER_A_ID, PRIO_NORMAL );

  /*
   * Clean up all used resources.
   */
  DeleteTask( ctx->worker_a_id );
  DeleteTask( ctx->worker_b_id );
  DeleteTask( ctx->worker_c_id );
  DeleteMutex( ctx->mutex_id );
}

/**
 * @brief Create three worker threads and a mutex.  Use the mutex and the
 *   worker to check that a suspended thread does not reconsider help requests.
 */
static void ScoreThreadValSmp_Action_1( ScoreThreadValSmp_Context *ctx )
{
  T_scheduler_log_10       scheduler_log;
  size_t                   index;
  const T_scheduler_event *event;

  _SMP_barrier_Control_initialize( &ctx->barrier );
  _SMP_barrier_State_initialize( &ctx->barrier_state );

  ctx->counter = 0;
  ctx->mutex_id = CreateMutex();

  ctx->worker_a_id = CreateTask( "WRKA", PRIO_NORMAL );
  SetScheduler( ctx->worker_a_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_a_id, WorkerTask, ctx );

  ctx->worker_b_id = CreateTask( "WRKB", PRIO_HIGH );
  StartTask( ctx->worker_b_id, WorkerTask, ctx );

  ctx->worker_c_id = CreateTask( "WRKC", PRIO_NORMAL );
  SetScheduler( ctx->worker_c_id, SCHEDULER_B_ID, PRIO_HIGH );
  StartTask( ctx->worker_c_id, WorkerTask, ctx );

  /*
   * Let worker B help worker A through the mutex.  Preempt worker A.  Delay
   * the thread switch to worker A.
   */
  ctx->busy = true;
  SendEvents(
    ctx->worker_a_id,
    EVENT_OBTAIN | EVENT_COUNT_EARLY | EVENT_BUSY | EVENT_COUNT
  );
  WaitForCounter( ctx, 1 );

  SendEvents( ctx->worker_b_id, EVENT_OBTAIN );
  SetPriority( ctx->worker_b_id, PRIO_LOW );
  SendEvents( ctx->worker_c_id, EVENT_SET_TASK_SWITCH_EXTENSION );

  /* B0 */
  _SMP_barrier_Wait( &ctx->barrier, &ctx->barrier_state, 2 );

  /*
   * Suspend worker A and let it wait on its thread state lock.  Check that
   * worker A did not reconsider help requests.
   */
  T_scheduler_record_10( &scheduler_log );
  T_scheduler_set_event_handler( SchedulerBlock, ctx );
  SuspendTask( ctx->worker_a_id );
  WaitForExecutionStop( ctx->worker_a_id );
  T_scheduler_record( NULL );
  T_eq_sz( scheduler_log.header.recorded, 2 );
  index = 0;
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_int( event->operation, T_SCHEDULER_BLOCK );
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_int( event->operation, T_SCHEDULER_WITHDRAW_NODE );
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_ptr( event, &T_scheduler_event_null );
  SetTaskSwitchExtension( NULL );

  /*
   * Resume worker A.  Check that worker A did reconsider help requests after
   * the thread dispatch.
   */
  T_scheduler_record_10( &scheduler_log );
  ResumeTask( ctx->worker_a_id );
  ctx->busy = false;
  WaitForCounter( ctx, 2 );
  WaitForExecutionStop( ctx->worker_a_id );
  T_scheduler_record( NULL );
  T_eq_sz( scheduler_log.header.recorded, 5 );
  index = 0;
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_int( event->operation, T_SCHEDULER_UNBLOCK );
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_int( event->operation, T_SCHEDULER_RECONSIDER_HELP_REQUEST );
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_int( event->operation, T_SCHEDULER_RECONSIDER_HELP_REQUEST );
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_int( event->operation, T_SCHEDULER_BLOCK );
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_int( event->operation, T_SCHEDULER_WITHDRAW_NODE );
  event = T_scheduler_next_any( &scheduler_log.header, &index );
  T_eq_ptr( event, &T_scheduler_event_null );

  /*
   * Clean up all used resources.
   */
  SendEvents( ctx->worker_a_id, EVENT_RELEASE | EVENT_COUNT );
  WaitForCounter( ctx, 3 );

  SetPriority( ctx->worker_b_id, PRIO_HIGH );
  SendEvents( ctx->worker_b_id, EVENT_RELEASE );

  DeleteTask( ctx->worker_a_id );
  DeleteTask( ctx->worker_b_id );
  DeleteTask( ctx->worker_c_id );
  DeleteMutex( ctx->mutex_id );
}

/**
 * @fn void T_case_body_ScoreThreadValSmp( void )
 */
T_TEST_CASE_FIXTURE( ScoreThreadValSmp, &ScoreThreadValSmp_Fixture )
{
  ScoreThreadValSmp_Context *ctx;

  ctx = T_fixture_context();

  ScoreThreadValSmp_Action_0( ctx );
  ScoreThreadValSmp_Action_1( ctx );
}

/** @} */
