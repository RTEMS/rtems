/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreTqValSmp
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

#include <rtems/score/smpbarrier.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/threadqimpl.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreTqValSmp spec:/score/tq/val/smp
 *
 * @ingroup TestsuitesValidationSmpOnly0
 *
 * @brief Tests SMP-specific thread queue behaviour.
 *
 * This test case performs the following actions:
 *
 * - Create two or three worker threads and a mutex.  Use the mutex and the
 *   worker to do a thread priority change in parallel with a thread queue
 *   extraction.
 *
 *   - Create a mutex and let the runner obtain it.
 *
 *   - Create and start worker A on a second processor. mutex.  Let it wait on
 *     the barrier.
 *
 *   - If there are more than two processors, then create and start also worker
 *     C.  Let it wait on the barrier.
 *
 *   - Create and start worker B.  Let it try to obtain the mutex which is
 *     owned by the runner.  Delete worker B to extract it from the thread
 *     queue. Wrap the thread queue extract operation to do a parallel thread
 *     priority change carried out by worker A (and maybe C).
 *
 *   - Clean up all used resources.
 *
 * - Build a cyclic dependency graph using several worker threads and mutexes.
 *   Use the mutexes and the worker to construct a thread queue deadlock which
 *   is detected on one processor while it uses thread queue links inserted by
 *   another processor.  The runner thread controls the test scenario via the
 *   two thread queue locks.  This is an important test scenario which shows
 *   why the thread queue implementation is a bit more complicated in SMP
 *   configurations.
 *
 *   - Let worker D wait for mutex A.  Let worker C wait for mutex D.  Let
 *     worker B wait for mutex C.
 *
 *   - Let worker A attempt to obtain mutex B.  Let worker A wait on the lock
 *     of mutex C.  Worker A will insert two thread queue links.
 *
 *   - Let worker E try to obtain mutex D.  Worker E will add a thread queue
 *     link which is later used by worker A to detect the deadlock.
 *
 *   - Let worker A continue the obtain sequence.  It will detect a deadlock.
 *
 *   - Clean up all used resources.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/tq/val/smp test case.
 */
typedef struct {
  /**
   * @brief This member contains the runner identifier.
   */
  rtems_id runner_id;

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
   * @brief This member contains the worker D identifier.
   */
  rtems_id worker_d_id;

  /**
   * @brief This member contains the worker E identifier.
   */
  rtems_id worker_e_id;

  /**
   * @brief This member contains the mutex A identifier.
   */
  rtems_id mutex_a_id;

  /**
   * @brief This member contains the mutex B identifier.
   */
  rtems_id mutex_b_id;

  /**
   * @brief This member contains the mutex C identifier.
   */
  rtems_id mutex_c_id;

  /**
   * @brief This member contains the mutex D identifier.
   */
  rtems_id mutex_d_id;

  /**
   * @brief This member contains the count of processors used by the test.
   */
  uint32_t used_cpus;

  /**
   * @brief This member contains the thread queue of the mutex.
   */
  Thread_queue_Queue *thread_queue;

  /**
   * @brief This member contains the context to wrap the thread queue extract.
   */
  WrapThreadQueueContext wrap;

  /**
   * @brief This member contains the barrier to synchronize the runner and the
   *   workers.
   */
  SMP_barrier_Control barrier;

  /**
   * @brief This member contains the barrier state for the runner processor.
   */
  SMP_barrier_State barrier_state;
} ScoreTqValSmp_Context;

static ScoreTqValSmp_Context
  ScoreTqValSmp_Instance;

typedef ScoreTqValSmp_Context Context;

static void Extract( void *arg )
{
  Context *ctx;

  ctx = arg;

  /* PC0 */
  _SMP_barrier_Wait( &ctx->barrier, &ctx->barrier_state, ctx->used_cpus );

  /*
   * Ensure that worker A (and maybe C) acquired the thread wait lock of
   * worker B.
   */
  TicketLockWaitForOthers( &ctx->thread_queue->Lock, ctx->used_cpus - 1 );

  /*
   * Continue with the thread queue extraction.  The thread wait lock of
   * worker B will be changed back to the default thread wait lock.  This
   * will cause worker A (and maybe C) to release the thread queue lock and
   * acquire the default thread wait lock of worker B instead to carry out
   * the priority change.
   *
   * See also _Thread_Wait_acquire_critical().
   */
}

static void PriorityChangeWorker( rtems_task_argument arg )
{
  Context          *ctx;
  SMP_barrier_State state;

  ctx = (Context *) arg;
  _SMP_barrier_State_initialize( &state );

  /* PC0 */
  _SMP_barrier_Wait( &ctx->barrier, &state, ctx->used_cpus );

  SetPriority( ctx->worker_b_id, PRIO_VERY_HIGH );

  /* PC1 */
  _SMP_barrier_Wait( &ctx->barrier, &state, ctx->used_cpus );

  (void) ReceiveAnyEvents();
}

static void MutexObtainWorker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  ObtainMutex( ctx->mutex_a_id );
}

static void DeadlockWorkerA( rtems_task_argument arg )
{
  Context          *ctx;
  SMP_barrier_State state;

  ctx = (Context *) arg;
  _SMP_barrier_State_initialize( &state );

  ObtainMutex( ctx->mutex_a_id );

  /* D0 */
  _SMP_barrier_Wait( &ctx->barrier, &state, 2 );

  /* D1 */
  _SMP_barrier_Wait( &ctx->barrier, &state, 2 );

  ObtainMutexDeadlock( ctx->mutex_b_id );

  ReleaseMutex( ctx->mutex_a_id );
  SendEvents( ctx->runner_id, RTEMS_EVENT_0 );
  (void) ReceiveAnyEvents();
}

static void DeadlockWorkerB( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  ObtainMutex( ctx->mutex_b_id );
  SendEvents( ctx->runner_id, RTEMS_EVENT_5 );
  ObtainMutex( ctx->mutex_c_id );
  ReleaseMutex( ctx->mutex_c_id );
  ReleaseMutex( ctx->mutex_b_id );
  SendEvents( ctx->runner_id, RTEMS_EVENT_1 );
  (void) ReceiveAnyEvents();
}

static void DeadlockWorkerC( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  ObtainMutex( ctx->mutex_c_id );
  ObtainMutex( ctx->mutex_d_id );
  ReleaseMutex( ctx->mutex_d_id );
  ReleaseMutex( ctx->mutex_c_id );
  SendEvents( ctx->runner_id, RTEMS_EVENT_2 );
  (void) ReceiveAnyEvents();
}

static void DeadlockWorkerD( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  ObtainMutex( ctx->mutex_d_id );
  ObtainMutex( ctx->mutex_a_id );
  ReleaseMutex( ctx->mutex_a_id );
  ReleaseMutex( ctx->mutex_d_id );
  SendEvents( ctx->runner_id, RTEMS_EVENT_3 );
  (void) ReceiveAnyEvents();
}

static void DeadlockWorkerE( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  ObtainMutex( ctx->mutex_d_id );
  ReleaseMutex( ctx->mutex_d_id );
  SendEvents( ctx->runner_id, RTEMS_EVENT_4 );
  (void) ReceiveAnyEvents();
}

static void ScoreTqValSmp_Setup( ScoreTqValSmp_Context *ctx )
{
  SetSelfPriority( PRIO_NORMAL );
}

static void ScoreTqValSmp_Setup_Wrap( void *arg )
{
  ScoreTqValSmp_Context *ctx;

  ctx = arg;
  ScoreTqValSmp_Setup( ctx );
}

static void ScoreTqValSmp_Teardown( ScoreTqValSmp_Context *ctx )
{
  RestoreRunnerPriority();
}

static void ScoreTqValSmp_Teardown_Wrap( void *arg )
{
  ScoreTqValSmp_Context *ctx;

  ctx = arg;
  ScoreTqValSmp_Teardown( ctx );
}

static T_fixture ScoreTqValSmp_Fixture = {
  .setup = ScoreTqValSmp_Setup_Wrap,
  .stop = NULL,
  .teardown = ScoreTqValSmp_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &ScoreTqValSmp_Instance
};

/**
 * @brief Create two or three worker threads and a mutex.  Use the mutex and
 *   the worker to do a thread priority change in parallel with a thread queue
 *   extraction.
 */
static void ScoreTqValSmp_Action_0( ScoreTqValSmp_Context *ctx )
{
  _SMP_barrier_Control_initialize( &ctx->barrier );
  _SMP_barrier_State_initialize( &ctx->barrier_state );
  WrapThreadQueueInitialize( &ctx->wrap, Extract, ctx );

  if ( rtems_scheduler_get_processor_maximum() > 2 ) {
    ctx->used_cpus = 3;
  } else {
    ctx->used_cpus = 2;
  }

  /*
   * Create a mutex and let the runner obtain it.
   */
  ctx->mutex_a_id = CreateMutexNoProtocol();
  ctx->thread_queue = GetMutexThreadQueue( ctx->mutex_a_id );
  ObtainMutex( ctx->mutex_a_id );

  /*
   * Create and start worker A on a second processor. mutex.  Let it wait on
   * the barrier.
   */
  ctx->worker_a_id = CreateTask( "WRKA", PRIO_NORMAL );
  SetScheduler( ctx->worker_a_id, SCHEDULER_B_ID, PRIO_NORMAL );
  StartTask( ctx->worker_a_id, PriorityChangeWorker, ctx );

  /*
   * If there are more than two processors, then create and start also worker
   * C.  Let it wait on the barrier.
   */
  if ( ctx->used_cpus > 2 ) {
    ctx->worker_c_id = CreateTask( "WRKC", PRIO_NORMAL );
    SetScheduler( ctx->worker_c_id, SCHEDULER_C_ID, PRIO_NORMAL );
    StartTask( ctx->worker_c_id, PriorityChangeWorker, ctx );
  }

  /*
   * Create and start worker B.  Let it try to obtain the mutex which is owned
   * by the runner.  Delete worker B to extract it from the thread queue. Wrap
   * the thread queue extract operation to do a parallel thread priority change
   * carried out by worker A (and maybe C).
   */
  ctx->worker_b_id = CreateTask( "WRKB", PRIO_HIGH );
  StartTask( ctx->worker_b_id, MutexObtainWorker, ctx );
  WrapThreadQueueExtractDirect( &ctx->wrap, GetThread( ctx->worker_b_id ) );
  DeleteTask( ctx->worker_b_id );

  /*
   * Clean up all used resources.
   */
  /* PC1 */
  _SMP_barrier_Wait( &ctx->barrier, &ctx->barrier_state, ctx->used_cpus );

  WaitForExecutionStop( ctx->worker_a_id );
  DeleteTask( ctx->worker_a_id );

  if ( ctx->used_cpus > 2 ) {
    WaitForExecutionStop( ctx->worker_c_id );
    DeleteTask( ctx->worker_c_id );
  }

  ReleaseMutex( ctx->mutex_a_id );
  DeleteMutex( ctx->mutex_a_id );
  WrapThreadQueueDestroy( &ctx->wrap );
}

/**
 * @brief Build a cyclic dependency graph using several worker threads and
 *   mutexes. Use the mutexes and the worker to construct a thread queue
 *   deadlock which is detected on one processor while it uses thread queue
 *   links inserted by another processor.  The runner thread controls the test
 *   scenario via the two thread queue locks.  This is an important test
 *   scenario which shows why the thread queue implementation is a bit more
 *   complicated in SMP configurations.
 */
static void ScoreTqValSmp_Action_1( ScoreTqValSmp_Context *ctx )
{
  Thread_queue_Queue *queue_b;
  Thread_queue_Queue *queue_c;
  ISR_lock_Context    lock_context;
  SMP_barrier_State   state;

  if ( rtems_scheduler_get_processor_maximum() <= 2 ) {
    /*
     * We can only run this validation test on systems with three or more
     * processors.  The sequence under test can happen on systems with only two
     * processors, however, we need a third processor to control the other two
     * processors via ISR locks to get a deterministic test scenario.
     */
    return;
  }

  ctx->runner_id = rtems_task_self();

  _SMP_barrier_Control_initialize( &ctx->barrier );
  _SMP_barrier_State_initialize( &state );

  ctx->mutex_a_id = CreateMutexNoProtocol();
  ctx->mutex_b_id = CreateMutexNoProtocol();
  ctx->mutex_c_id = CreateMutexNoProtocol();
  ctx->mutex_d_id = CreateMutexNoProtocol();

  queue_b = GetMutexThreadQueue( ctx->mutex_b_id );
  queue_c = GetMutexThreadQueue( ctx->mutex_c_id );

  ctx->worker_a_id = CreateTask( "WRKA", PRIO_NORMAL );
  ctx->worker_b_id = CreateTask( "WRKB", PRIO_NORMAL );
  ctx->worker_c_id = CreateTask( "WRKC", PRIO_NORMAL );
  ctx->worker_d_id = CreateTask( "WRKD", PRIO_NORMAL );
  ctx->worker_e_id = CreateTask( "WRKE", PRIO_NORMAL );

  SetScheduler( ctx->worker_a_id, SCHEDULER_B_ID, PRIO_NORMAL );
  SetScheduler( ctx->worker_b_id, SCHEDULER_B_ID, PRIO_HIGH );
  SetScheduler( ctx->worker_c_id, SCHEDULER_B_ID, PRIO_HIGH );
  SetScheduler( ctx->worker_d_id, SCHEDULER_B_ID, PRIO_HIGH );
  SetScheduler( ctx->worker_e_id, SCHEDULER_C_ID, PRIO_NORMAL );

  /*
   * Let worker D wait for mutex A.  Let worker C wait for mutex D.  Let worker
   * B wait for mutex C.
   */
  StartTask( ctx->worker_a_id, DeadlockWorkerA, ctx );

  /* D0 */
  _SMP_barrier_Wait( &ctx->barrier, &state, 2 );

  StartTask( ctx->worker_d_id, DeadlockWorkerD, ctx );
  StartTask( ctx->worker_c_id, DeadlockWorkerC, ctx );
  StartTask( ctx->worker_b_id, DeadlockWorkerB, ctx );
  ReceiveAllEvents( RTEMS_EVENT_5 );
  WaitForExecutionStop( ctx->worker_b_id );

  /*
   * Let worker A attempt to obtain mutex B.  Let worker A wait on the lock of
   * mutex C.  Worker A will insert two thread queue links.
   */
  _ISR_lock_ISR_disable( &lock_context );
  _Thread_queue_Queue_acquire_critical(
    queue_c,
    &_Thread_Executing->Potpourri_stats,
    &lock_context
  );
  _ISR_lock_ISR_enable( &lock_context );

  /* D1 */
  _SMP_barrier_Wait( &ctx->barrier, &state, 2 );

  TicketLockWaitForOthers( &queue_c->Lock, 1 );

  /*
   * Let worker E try to obtain mutex D.  Worker E will add a thread queue link
   * which is later used by worker A to detect the deadlock.
   */
  StartTask( ctx->worker_e_id, DeadlockWorkerE, ctx );
  TicketLockWaitForOthers( &queue_b->Lock, 1 );

  /*
   * Let worker A continue the obtain sequence.  It will detect a deadlock.
   */
  _ISR_lock_ISR_disable( &lock_context );
  _Thread_queue_Queue_release( queue_c, &lock_context );

  /*
   * Clean up all used resources.
   */
  ReceiveAllEvents(
    RTEMS_EVENT_0 | RTEMS_EVENT_1 | RTEMS_EVENT_2 | RTEMS_EVENT_3 |
    RTEMS_EVENT_4
  );
  WaitForExecutionStop( ctx->worker_a_id );
  WaitForExecutionStop( ctx->worker_b_id );
  WaitForExecutionStop( ctx->worker_c_id );
  WaitForExecutionStop( ctx->worker_d_id );
  WaitForExecutionStop( ctx->worker_e_id );
  DeleteTask( ctx->worker_a_id );
  DeleteTask( ctx->worker_b_id );
  DeleteTask( ctx->worker_c_id );
  DeleteTask( ctx->worker_d_id );
  DeleteTask( ctx->worker_e_id );
  DeleteMutex( ctx->mutex_a_id );
  DeleteMutex( ctx->mutex_b_id );
  DeleteMutex( ctx->mutex_c_id );
  DeleteMutex( ctx->mutex_d_id );
}

/**
 * @fn void T_case_body_ScoreTqValSmp( void )
 */
T_TEST_CASE_FIXTURE( ScoreTqValSmp, &ScoreTqValSmp_Fixture )
{
  ScoreTqValSmp_Context *ctx;

  ctx = T_fixture_context();

  ScoreTqValSmp_Action_0( ctx );
  ScoreTqValSmp_Action_1( ctx );
}

/** @} */
