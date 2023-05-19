/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibValSysLock
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

#include <errno.h>
#include <string.h>
#include <sys/lock.h>

#include "tr-mtx-seize-try.h"
#include "tr-mtx-seize-wait.h"
#include "tr-mtx-surrender.h"
#include "tr-tq-timeout-priority-inherit.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup NewlibValSysLock spec:/newlib/val/sys-lock
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the <sys/lock.h> mutex directives.
 *
 * This test case performs the following actions:
 *
 * - Create a mutex and validate the mutex directives.
 *
 *   - Validate the _Mutex_Try_acquire() directive.
 *
 *   - Validate the _Mutex_Acquire_timed() directive for valid timeout
 *     parameters.
 *
 *   - Validate the _Mutex_Acquire_timed() directive for an invalid timeout
 *     parameter.
 *
 *   - Validate the _Mutex_Acquire_timed() timeout behaviour.
 *
 *   - Validate the _Mutex_Acquire() directive.
 *
 *   - Validate the _Mutex_Try_acquire() directive.
 *
 *   - Destroy the mutex.
 *
 * - Create a recursive mutex and validate the mutex directives.
 *
 *   - Validate the _Mutex_recursive_Try_acquire() directive.
 *
 *   - Validate the _Mutex_recursive_Acquire_timed() directive for valid
 *     timeout parameters.
 *
 *   - Validate the _Mutex_recursive_Acquire_timed() directive for an invalid
 *     timeout parameter.
 *
 *   - Validate the _Mutex_recursive_Acquire_timed() timeout behaviour.
 *
 *   - Validate the _Mutex_recursive_Acquire() directive.
 *
 *   - Validate the _Mutex_recursive_Try_acquire() directive.
 *
 *   - Destroy the mutex.
 *
 * @{
 */

/**
 * @brief Test context for spec:/newlib/val/sys-lock test case.
 */
typedef struct {
  /**
   * @brief This member contains the thread queue test context.
   */
  TQMtxContext tq_mtx_ctx;
} NewlibValSysLock_Context;

static NewlibValSysLock_Context
  NewlibValSysLock_Instance;

static Status_Control Enqueue( TQContext *ctx, TQWait wait )
{
  const struct timespec abstime = {
    .tv_sec = INT64_MAX,
    .tv_nsec = 0
  };
  int                   eno;

  switch ( wait ) {
    case TQ_NO_WAIT:
      eno = _Mutex_Try_acquire( ctx->thread_queue_object );
      break;
    case TQ_WAIT_FOREVER:
      _Mutex_Acquire( ctx->thread_queue_object );
      eno = 0;
      break;
    case TQ_WAIT_TIMED:
      eno = _Mutex_Acquire_timed( ctx->thread_queue_object, &abstime );
      break;
    default:
      T_unreachable();
      break;
  }

  return STATUS_BUILD( 0, eno );
}

static Status_Control Surrender( TQContext *ctx )
{
  _Mutex_Release( ctx->thread_queue_object );

  return STATUS_SUCCESSFUL;
}

static rtems_tcb *GetOwner( TQContext *ctx )
{
  const struct _Mutex_Control *mutex;

  mutex = ctx->thread_queue_object;

  return mutex->_Queue._owner;
}

static Status_Control RecursiveEnqueue( TQContext *ctx, TQWait wait )
{
  const struct timespec abstime = {
    .tv_sec = INT64_MAX,
    .tv_nsec = 0
  };
  int                   eno;

  switch ( wait ) {
    case TQ_NO_WAIT:
      eno = _Mutex_recursive_Try_acquire( ctx->thread_queue_object );
      break;
    case TQ_WAIT_FOREVER:
      _Mutex_recursive_Acquire( ctx->thread_queue_object );
      eno = 0;
      break;
    case TQ_WAIT_TIMED:
      eno = _Mutex_recursive_Acquire_timed(
        ctx->thread_queue_object,
        &abstime
      );
      break;
    default:
      T_unreachable();
      break;
  }

  return STATUS_BUILD( 0, eno );
}

static Status_Control RecursiveSurrender( TQContext *ctx )
{
  _Mutex_recursive_Release( ctx->thread_queue_object );

  return STATUS_SUCCESSFUL;
}

static rtems_tcb *RecursiveGetOwner( TQContext *ctx )
{
  const struct _Mutex_recursive_Control *mutex;

  mutex = ctx->thread_queue_object;

  return mutex->_Mutex._Queue._owner;
}

static void NewlibValSysLock_Setup( NewlibValSysLock_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->tq_mtx_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
  ctx->tq_mtx_ctx.base.discipline = TQ_PRIORITY;
  ctx->tq_mtx_ctx.base.deadlock = TQ_DEADLOCK_FATAL;
  ctx->tq_mtx_ctx.base.convert_status = TQConvertStatusPOSIX;
  ctx->tq_mtx_ctx.protocol = TQ_MTX_PRIORITY_INHERIT;
  ctx->tq_mtx_ctx.owner_check = TQ_MTX_NO_OWNER_CHECK;
  ctx->tq_mtx_ctx.priority_ceiling = PRIO_INVALID;
  TQInitialize( &ctx->tq_mtx_ctx.base );
}

static void NewlibValSysLock_Setup_Wrap( void *arg )
{
  NewlibValSysLock_Context *ctx;

  ctx = arg;
  NewlibValSysLock_Setup( ctx );
}

static void NewlibValSysLock_Teardown( NewlibValSysLock_Context *ctx )
{
  TQDestroy( &ctx->tq_mtx_ctx.base );
  RestoreRunnerPriority();
}

static void NewlibValSysLock_Teardown_Wrap( void *arg )
{
  NewlibValSysLock_Context *ctx;

  ctx = arg;
  NewlibValSysLock_Teardown( ctx );
}

static T_fixture NewlibValSysLock_Fixture = {
  .setup = NewlibValSysLock_Setup_Wrap,
  .stop = NULL,
  .teardown = NewlibValSysLock_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &NewlibValSysLock_Instance
};

/**
 * @brief Create a mutex and validate the mutex directives.
 */
static void NewlibValSysLock_Action_0( NewlibValSysLock_Context *ctx )
{
  const struct timespec invalid_abstime = {
    .tv_sec = -1,
    .tv_nsec = -1
  };
  int                   eno;
  struct _Mutex_Control mutex;

  _Mutex_Initialize( &mutex );

  ctx->tq_mtx_ctx.base.thread_queue_object = &mutex;
  ctx->tq_mtx_ctx.base.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_mtx_ctx.base.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_mtx_ctx.base.enqueue = Enqueue;
  ctx->tq_mtx_ctx.base.surrender = Surrender;
  ctx->tq_mtx_ctx.base.get_owner = GetOwner;

  /*
   * Validate the _Mutex_Try_acquire() directive.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_NO_WAIT;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_UNAVAILABLE;
  ScoreMtxReqSeizeTry_Run( &ctx->tq_mtx_ctx );

  /*
   * Validate the _Mutex_Acquire_timed() directive for valid timeout
   * parameters.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_WAIT_TIMED;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_DEADLOCK;
  ScoreMtxReqSeizeWait_Run( &ctx->tq_mtx_ctx );

  /*
   * Validate the _Mutex_Acquire_timed() directive for an invalid timeout
   * parameter.
   */
  TQSetScheduler(
    &ctx->tq_mtx_ctx.base,
    TQ_HELPER_A,
    SCHEDULER_A_ID,
    PRIO_HIGH
  );
  TQSend( &ctx->tq_mtx_ctx.base, TQ_HELPER_A, TQ_EVENT_ENQUEUE );
  eno = _Mutex_Acquire_timed( &mutex, &invalid_abstime );
  T_eq_int( eno, EINVAL );
  TQSend( &ctx->tq_mtx_ctx.base, TQ_HELPER_A, TQ_EVENT_SURRENDER );

  /*
   * Validate the _Mutex_Acquire_timed() timeout behaviour.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_WAIT_FOREVER;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_DEADLOCK;
  ScoreTqReqTimeoutPriorityInherit_Run(
    &ctx->tq_mtx_ctx.base
  );

  /*
   * Validate the _Mutex_Acquire() directive.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_WAIT_FOREVER;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_DEADLOCK;
  ScoreMtxReqSeizeWait_Run( &ctx->tq_mtx_ctx );

  /*
   * Validate the _Mutex_Try_acquire() directive.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_WAIT_FOREVER;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_DEADLOCK;
  ScoreMtxReqSurrender_Run( &ctx->tq_mtx_ctx );

  /*
   * Destroy the mutex.
   */
  _Mutex_Destroy( &mutex );
}

/**
 * @brief Create a recursive mutex and validate the mutex directives.
 */
static void NewlibValSysLock_Action_1( NewlibValSysLock_Context *ctx )
{
  const struct timespec invalid_abstime = {
    .tv_sec = -1,
    .tv_nsec = -1
  };
  int                             eno;
  struct _Mutex_recursive_Control mutex;

  _Mutex_recursive_Initialize( &mutex );

  ctx->tq_mtx_ctx.base.thread_queue_object = &mutex;
  ctx->tq_mtx_ctx.base.enqueue_prepare = TQEnqueuePrepareDefault;
  ctx->tq_mtx_ctx.base.enqueue_done = TQEnqueueDoneDefault;
  ctx->tq_mtx_ctx.base.enqueue = RecursiveEnqueue;
  ctx->tq_mtx_ctx.base.surrender = RecursiveSurrender;
  ctx->tq_mtx_ctx.base.get_owner = RecursiveGetOwner;

  /*
   * Validate the _Mutex_recursive_Try_acquire() directive.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_NO_WAIT;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
  ScoreMtxReqSeizeTry_Run( &ctx->tq_mtx_ctx );

  /*
   * Validate the _Mutex_recursive_Acquire_timed() directive for valid timeout
   * parameters.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_WAIT_TIMED;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
  ScoreMtxReqSeizeWait_Run( &ctx->tq_mtx_ctx );

  /*
   * Validate the _Mutex_recursive_Acquire_timed() directive for an invalid
   * timeout parameter.
   */
  TQSetScheduler(
    &ctx->tq_mtx_ctx.base,
    TQ_HELPER_A,
    SCHEDULER_A_ID,
    PRIO_HIGH
  );
  TQSend( &ctx->tq_mtx_ctx.base, TQ_HELPER_A, TQ_EVENT_ENQUEUE );
  eno = _Mutex_recursive_Acquire_timed( &mutex, &invalid_abstime );
  T_eq_int( eno, EINVAL );
  TQSend( &ctx->tq_mtx_ctx.base, TQ_HELPER_A, TQ_EVENT_SURRENDER );

  /*
   * Validate the _Mutex_recursive_Acquire_timed() timeout behaviour.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_WAIT_FOREVER;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
  ScoreTqReqTimeoutPriorityInherit_Run(
    &ctx->tq_mtx_ctx.base
  );

  /*
   * Validate the _Mutex_recursive_Acquire() directive.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_WAIT_FOREVER;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
  ScoreMtxReqSeizeWait_Run( &ctx->tq_mtx_ctx );

  /*
   * Validate the _Mutex_recursive_Try_acquire() directive.
   */
  ctx->tq_mtx_ctx.base.wait = TQ_WAIT_FOREVER;
  ctx->tq_mtx_ctx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
  ScoreMtxReqSurrender_Run( &ctx->tq_mtx_ctx );

  /*
   * Destroy the mutex.
   */
  _Mutex_recursive_Destroy( &mutex );
}

/**
 * @fn void T_case_body_NewlibValSysLock( void )
 */
T_TEST_CASE_FIXTURE( NewlibValSysLock, &NewlibValSysLock_Fixture )
{
  NewlibValSysLock_Context *ctx;

  ctx = T_fixture_context();

  NewlibValSysLock_Action_0( ctx );
  NewlibValSysLock_Action_1( ctx );
}

/** @} */
