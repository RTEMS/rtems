/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup NewlibValSysLockCondition
 */

/*
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#include <errno.h>
#include <string.h>
#include <sys/lock.h>
#include <rtems/score/condimpl.h>

#include "tr-cond-wait.h"
#include "tr-tq-enqueue-fifo.h"
#include "tr-tq-flush-fifo.h"
#include "tx-thread-queue.h"

#include <rtems/test.h>

/**
 * @defgroup NewlibValSysLockCondition spec:/newlib/val/sys-lock-condition
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief Tests the `<sys/lock.h>` condition variable directives.
 *
 * This test case performs the following actions:
 *
 * - Create a condition variable with a mutex and validate the condition
 *   variable directives.
 *
 *   - Validate the _Condition_Wait() directive.
 *
 *   - Validate the _Condition_Wait_timed() directive for valid timeout
 *     parameters.
 *
 *   - Validate the _Condition_Signal() and the _Condition_Broadcast()
 *     directives.
 *
 *   - Validate the _Condition_Wait_timed() directive for an expired timeout
 *     parameter.
 *
 *   - Validate the _Condition_Wait_timed() directive for an invalid timeout
 *     parameter.
 *
 *   - Destroy the condition variable test context.
 *
 * - Create a condition variable with a recursive mutex and validate the wait
 *   directives.
 *
 *   - Validate the _Condition_Wait_recursive() directive.
 *
 *   - Validate the _Condition_Wait_recursive_timed() directive for valid
 *     timeout parameters.
 *
 *   - Validate the _Condition_Wait_recursive_timed() directive for an expired
 *     timeout parameter.
 *
 *   - Validate the _Condition_Wait_recursive_timed() directive for an invalid
 *     timeout parameter.
 *
 *   - Destroy the condition variable test context.
 *
 * - Create a condition variable and validate the enqueue and the flush
 *   directives.
 *
 *   - Validate the enqueue directive of the condition variable.
 *
 *   - Validate the flush directive of the condition variable.
 *
 *   - Destroy the condition variable test context.
 *
 * @{
 */

/**
 * @brief Test context for spec:/newlib/val/sys-lock-condition test case.
 */
typedef struct {
  /**
   * @brief This member contains the condition variable test context.
   */
  TQCondContext tq_cond_ctx;
} NewlibValSysLockCondition_Context;

static NewlibValSysLockCondition_Context NewlibValSysLockCondition_Instance;

static TQCondContext *ToCondContext( TQContext *base )
{
  return RTEMS_CONTAINER_OF( base, TQCondContext, base );
}

static rtems_tcb *CondGetOwner( TQContext *base )
{
  const struct _Mutex_Control *mutex;

  mutex = ToCondContext( base )->mtx.base.thread_queue_object;

  return mutex->_Queue._owner;
}

static rtems_tcb *CondRecursiveGetOwner( TQContext *base )
{
  const struct _Mutex_recursive_Control *mutex;

  mutex = ToCondContext( base )->mtx.base.thread_queue_object;

  return mutex->_Mutex._Queue._owner;
}

static const struct timespec never = { .tv_sec = INT64_MAX, .tv_nsec = 0 };

static int CondWait( TQContext *base, TQWait wait )
{
  TQCondContext *ctx;
  int            eno;

  ctx = ToCondContext( base );
  eno = 0;

  switch ( wait ) {
    case TQ_WAIT_FOREVER:
      _Condition_Wait(
        base->thread_queue_object,
        ctx->mtx.base.thread_queue_object
      );
      break;
    case TQ_WAIT_TIMED:
      eno = _Condition_Wait_timed(
        base->thread_queue_object,
        ctx->mtx.base.thread_queue_object,
        &never
      );
      break;
    default:
      T_unreachable();
      break;
  }

  return eno;
}

static int CondRecursiveWait( TQContext *base, TQWait wait )
{
  TQCondContext *ctx;
  int            eno;

  ctx = ToCondContext( base );
  eno = 0;

  switch ( wait ) {
    case TQ_WAIT_FOREVER:
      _Condition_Wait_recursive(
        base->thread_queue_object,
        ctx->mtx.base.thread_queue_object
      );
      break;
    case TQ_WAIT_TIMED:
      eno = _Condition_Wait_recursive_timed(
        base->thread_queue_object,
        ctx->mtx.base.thread_queue_object,
        &never
      );
      break;
    default:
      T_unreachable();
      break;
  }

  return eno;
}

static Status_Control CondEnqueue( TQContext *base, TQWait wait )
{
  TQCondContext *ctx;
  int            eno;

  ctx = ToCondContext( base );
  _Mutex_Acquire( ctx->mtx.base.thread_queue_object );
  eno = CondWait( base, wait );
  ctx->owner_after = CondGetOwner( base );
  ctx->nest_after = 1;
  _Mutex_Release( ctx->mtx.base.thread_queue_object );

  return STATUS_BUILD( 0, eno );
}

static Status_Control CondRecursiveEnqueue( TQContext *base, TQWait wait )
{
  struct _Mutex_recursive_Control *mutex;
  TQCondContext                   *ctx;
  unsigned int                     i;
  int                              eno;

  ctx = ToCondContext( base );
  mutex = ctx->mtx.base.thread_queue_object;

  for ( i = 0; i < ctx->nest; ++i ) {
    _Mutex_recursive_Acquire( mutex );
  }

  eno = CondRecursiveWait( base, wait );
  ctx->owner_after = CondRecursiveGetOwner( base );
  ctx->nest_after = mutex->_nest_level + 1;

  for ( i = 0; i < ctx->nest; ++i ) {
    _Mutex_recursive_Release( mutex );
  }

  return STATUS_BUILD( 0, eno );
}

static Status_Control CondSurrender( TQContext *base )
{
  _Condition_Signal( base->thread_queue_object );

  return STATUS_SUCCESSFUL;
}

static void CondEnqueueDone( TQContext *base )
{
  _Condition_Broadcast( base->thread_queue_object );
}

static uint32_t CondFlush( TQContext *base, uint32_t thread_count, bool all )
{
  if ( all ) {
    _Condition_Broadcast( base->thread_queue_object );

    return thread_count;
  }

  if ( thread_count == 0 ) {
    return 0;
  }

  _Condition_Signal( base->thread_queue_object );

  return 1;
}

static Status_Control CondEnqueueDirect( TQContext *base, TQWait wait )
{
  Thread_queue_Context queue_context;

  T_eq_int( wait, TQ_WAIT_FOREVER );
  _Thread_queue_Context_initialize( &queue_context );
  _Condition_Acquire( base->thread_queue_object, &queue_context );
  _Condition_Enqueue( base->thread_queue_object, &queue_context );

  return STATUS_SUCCESSFUL;
}

static uint32_t CondFlushDirect(
  TQContext *base,
  uint32_t   thread_count,
  bool       all
)
{
  Thread_queue_Context queue_context;
  size_t               flushed;

  (void) thread_count;
  T_true( all );
  _Thread_queue_Context_initialize( &queue_context );
  _Condition_Acquire( base->thread_queue_object, &queue_context );
  flushed = _Condition_Flush( base->thread_queue_object, &queue_context );

  return (uint32_t) flushed;
}

static void NewlibValSysLockCondition_Setup(
  NewlibValSysLockCondition_Context *ctx
)
{
  memset( ctx, 0, sizeof( *ctx ) );
}

static void NewlibValSysLockCondition_Setup_Wrap( void *arg )
{
  NewlibValSysLockCondition_Context *ctx;

  ctx = arg;
  NewlibValSysLockCondition_Setup( ctx );
}

static void NewlibValSysLockCondition_Teardown( void )
{
  RestoreRunnerPriority();
}

static void NewlibValSysLockCondition_Teardown_Wrap( void *arg )
{
  (void) arg;
  NewlibValSysLockCondition_Teardown();
}

static T_fixture NewlibValSysLockCondition_Fixture = {
  .setup = NewlibValSysLockCondition_Setup_Wrap,
  .stop = NULL,
  .teardown = NewlibValSysLockCondition_Teardown_Wrap,
  .scope = NULL,
  .initial_context = &NewlibValSysLockCondition_Instance
};

/**
 * @brief Create a condition variable with a mutex and validate the condition
 *   variable directives.
 */
static void NewlibValSysLockCondition_Action_0(
  NewlibValSysLockCondition_Context *ctx
)
{
  const struct timespec     invalid_abstime = { .tv_sec = -1, .tv_nsec = -1 };
  const struct timespec     expired_abstime = { .tv_sec = 0, .tv_nsec = 0 };
  int                       eno;
  struct _Condition_Control condition;
  struct _Mutex_Control     mutex;

  memset( &ctx->tq_cond_ctx, 0, sizeof( ctx->tq_cond_ctx ) );
  _Condition_Initialize( &condition );
  _Mutex_Initialize( &mutex );

  ctx->tq_cond_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
  ctx->tq_cond_ctx.base.discipline = TQ_FIFO;
  ctx->tq_cond_ctx.base.deadlock = TQ_DEADLOCK_FATAL;
  ctx->tq_cond_ctx.base.convert_status = TQConvertStatusPOSIX;
  ctx->tq_cond_ctx.base.thread_queue_object = &condition;
  ctx->tq_cond_ctx.base.enqueue_prepare = TQDoNothing;
  ctx->tq_cond_ctx.base.enqueue_done = CondEnqueueDone;
  ctx->tq_cond_ctx.base.surrender = CondSurrender;
  ctx->tq_cond_ctx.base.flush = CondFlush;
  ctx->tq_cond_ctx.mtx.protocol = TQ_MTX_PRIORITY_INHERIT;
  ctx->tq_cond_ctx.mtx.owner_check = TQ_MTX_NO_OWNER_CHECK;
  ctx->tq_cond_ctx.mtx.priority_ceiling = PRIO_INVALID;
  ctx->tq_cond_ctx.base.enqueue = CondEnqueue;
  ctx->tq_cond_ctx.base.get_owner = CondGetOwner;
  ctx->tq_cond_ctx.mtx.recursive = TQ_MTX_RECURSIVE_DEADLOCK;
  ctx->tq_cond_ctx.mtx.base.thread_queue_object = &mutex;
  TQInitialize( &ctx->tq_cond_ctx.base );

  /*
   * Validate the _Condition_Wait() directive.
   */
  ctx->tq_cond_ctx.base.wait = TQ_WAIT_FOREVER;
  ScoreCondReqWait_Run( &ctx->tq_cond_ctx );

  /*
   * Validate the _Condition_Wait_timed() directive for valid timeout
   * parameters.
   */
  ctx->tq_cond_ctx.base.wait = TQ_WAIT_TIMED;
  ScoreCondReqWait_Run( &ctx->tq_cond_ctx );

  /*
   * Validate the _Condition_Signal() and the _Condition_Broadcast()
   * directives.
   */
  ctx->tq_cond_ctx.base.wait = TQ_WAIT_FOREVER;
  ScoreTqReqFlushFifo_Run( &ctx->tq_cond_ctx.base, true );

  /*
   * Validate the _Condition_Wait_timed() directive for an expired timeout
   * parameter.
   */
  _Mutex_Acquire( &mutex );
  eno = _Condition_Wait_timed( &condition, &mutex, &expired_abstime );
  T_eq_int( eno, ETIMEDOUT );
  _Mutex_Release( &mutex );

  /*
   * Validate the _Condition_Wait_timed() directive for an invalid timeout
   * parameter.
   */
  _Mutex_Acquire( &mutex );
  eno = _Condition_Wait_timed( &condition, &mutex, &invalid_abstime );
  T_eq_int( eno, EINVAL );
  _Mutex_Release( &mutex );

  /*
   * Destroy the condition variable test context.
   */
  TQDestroy( &ctx->tq_cond_ctx.base );
}

/**
 * @brief Create a condition variable with a recursive mutex and validate the
 *   wait directives.
 */
static void NewlibValSysLockCondition_Action_1(
  NewlibValSysLockCondition_Context *ctx
)
{
  const struct timespec     invalid_abstime = { .tv_sec = -1, .tv_nsec = -1 };
  const struct timespec     expired_abstime = { .tv_sec = 0, .tv_nsec = 0 };
  int                       eno;
  struct _Condition_Control condition;
  struct _Mutex_recursive_Control mutex;

  memset( &ctx->tq_cond_ctx, 0, sizeof( ctx->tq_cond_ctx ) );
  _Condition_Initialize( &condition );
  _Mutex_recursive_Initialize( &mutex );

  ctx->tq_cond_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
  ctx->tq_cond_ctx.base.discipline = TQ_FIFO;
  ctx->tq_cond_ctx.base.deadlock = TQ_DEADLOCK_FATAL;
  ctx->tq_cond_ctx.base.convert_status = TQConvertStatusPOSIX;
  ctx->tq_cond_ctx.base.thread_queue_object = &condition;
  ctx->tq_cond_ctx.base.enqueue_prepare = TQDoNothing;
  ctx->tq_cond_ctx.base.enqueue_done = CondEnqueueDone;
  ctx->tq_cond_ctx.base.surrender = CondSurrender;
  ctx->tq_cond_ctx.base.flush = CondFlush;
  ctx->tq_cond_ctx.mtx.protocol = TQ_MTX_PRIORITY_INHERIT;
  ctx->tq_cond_ctx.mtx.owner_check = TQ_MTX_NO_OWNER_CHECK;
  ctx->tq_cond_ctx.mtx.priority_ceiling = PRIO_INVALID;
  ctx->tq_cond_ctx.base.enqueue = CondRecursiveEnqueue;
  ctx->tq_cond_ctx.base.get_owner = CondRecursiveGetOwner;
  ctx->tq_cond_ctx.mtx.recursive = TQ_MTX_RECURSIVE_ALLOWED;
  ctx->tq_cond_ctx.mtx.base.thread_queue_object = &mutex;
  TQInitialize( &ctx->tq_cond_ctx.base );

  /*
   * Validate the _Condition_Wait_recursive() directive.
   */
  ctx->tq_cond_ctx.base.wait = TQ_WAIT_FOREVER;
  ScoreCondReqWait_Run( &ctx->tq_cond_ctx );

  /*
   * Validate the _Condition_Wait_recursive_timed() directive for valid timeout
   * parameters.
   */
  ctx->tq_cond_ctx.base.wait = TQ_WAIT_TIMED;
  ScoreCondReqWait_Run( &ctx->tq_cond_ctx );

  /*
   * Validate the _Condition_Wait_recursive_timed() directive for an expired
   * timeout parameter.
   */
  _Mutex_recursive_Acquire( &mutex );
  eno = _Condition_Wait_recursive_timed(
    &condition,
    &mutex,
    &expired_abstime
  );
  T_eq_int( eno, ETIMEDOUT );
  _Mutex_recursive_Release( &mutex );

  /*
   * Validate the _Condition_Wait_recursive_timed() directive for an invalid
   * timeout parameter.
   */
  _Mutex_recursive_Acquire( &mutex );
  eno = _Condition_Wait_recursive_timed(
    &condition,
    &mutex,
    &invalid_abstime
  );
  T_eq_int( eno, EINVAL );
  _Mutex_recursive_Release( &mutex );

  /*
   * Destroy the condition variable test context.
   */
  TQDestroy( &ctx->tq_cond_ctx.base );
}

/**
 * @brief Create a condition variable and validate the enqueue and the flush
 *   directives.
 */
static void NewlibValSysLockCondition_Action_2(
  NewlibValSysLockCondition_Context *ctx
)
{
  struct _Condition_Control condition;

  memset( &ctx->tq_cond_ctx, 0, sizeof( ctx->tq_cond_ctx ) );
  _Condition_Initialize( &condition );

  ctx->tq_cond_ctx.base.enqueue_variant = TQ_ENQUEUE_BLOCKS;
  ctx->tq_cond_ctx.base.discipline = TQ_FIFO;
  ctx->tq_cond_ctx.base.deadlock = TQ_DEADLOCK_FATAL;
  ctx->tq_cond_ctx.base.convert_status = TQConvertStatusPOSIX;
  ctx->tq_cond_ctx.base.thread_queue_object = &condition;
  ctx->tq_cond_ctx.base.enqueue_prepare = TQDoNothing;
  ctx->tq_cond_ctx.base.enqueue_done = CondEnqueueDone;
  ctx->tq_cond_ctx.base.surrender = CondSurrender;
  ctx->tq_cond_ctx.base.flush = CondFlushDirect;
  ctx->tq_cond_ctx.base.enqueue = CondEnqueueDirect;
  ctx->tq_cond_ctx.base.get_owner = NULL;
  ctx->tq_cond_ctx.base.wait = TQ_WAIT_FOREVER;
  TQInitialize( &ctx->tq_cond_ctx.base );

  /*
   * Validate the enqueue directive of the condition variable.
   */
  ScoreTqReqEnqueueFifo_Run( &ctx->tq_cond_ctx.base );

  /*
   * Validate the flush directive of the condition variable.
   */
  ScoreTqReqFlushFifo_Run( &ctx->tq_cond_ctx.base, false );

  /*
   * Destroy the condition variable test context.
   */
  TQDestroy( &ctx->tq_cond_ctx.base );
}

/**
 * @fn void T_case_body_NewlibValSysLockCondition( void )
 */
T_TEST_CASE_FIXTURE(
  NewlibValSysLockCondition,
  &NewlibValSysLockCondition_Fixture
)
{
  NewlibValSysLockCondition_Context *ctx;

  ctx = T_fixture_context();

  NewlibValSysLockCondition_Action_0( ctx );
  NewlibValSysLockCondition_Action_1( ctx );
  NewlibValSysLockCondition_Action_2( ctx );
}

/** @} */
