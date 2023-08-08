/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskValTask
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
#include <string.h>
#include <rtems/score/apimutex.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskValTask spec:/rtems/task/val/task
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @brief This test case collection provides validation test cases for
 *   requirements of the @ref RTEMSAPIClassicTasks.
 *
 * This test case performs the following actions:
 *
 * - Call rtems_task_self() check the returned value.
 *
 *   - Check that the returned value is equal to the object identifier of the
 *     calling task.
 *
 * - Call rtems_task_iterate() with a visitor which always returns false.
 *
 *   - Check that the all counter is equal to the count of tasks.  Check that
 *     the calling task was visited exacly once.  Firstly, this shows that
 *     rtems_task_iterate() used the parameters specified by ``visitor`` and
 *     ``arg``. Secondly, this shows that the iteration was done over all
 *     tasks.
 *
 *   - Check that the object alloctor mutex was not owned before and after the
 *     call.  Check that the object alloctor mutex was owned during the
 *     iteration.
 *
 * - Call rtems_task_iterate() with a visitor which returns true.
 *
 *   - Check that the all counter is equal to one.  This shows that the
 *     iteration stops when the visitor returns true.
 *
 * - Assert that RTEMS_TASK_STORAGE_ALIGNMENT is a constant expression which
 *   evaluates to the expected value.
 *
 * - Assert that RTEMS_NO_PRIORITY is a constant expression which evaluates to
 *   the expected value.
 *
 * - Assert that RTEMS_MINIMUM_STACK_SIZE is a constant expression which
 *   evaluates to the expected value.
 *
 * - Assert that RTEMS_CONFIGURED_MINIMUM_STACK_SIZE is a constant expression
 *   which evaluates to the expected value.
 *
 * - Assert that RTEMS_MINIMUM_PRIORITY is a constant expression which
 *   evaluates to the expected value.
 *
 * - Validate RTEMS_SELF using a sample directive call.
 *
 *   - Check that rtems_task_is_suspended() returns the expected status if
 *     called with a task identifier parameter of RTEMS_SELF.
 *
 * - Validate the home scheduler of tasks created by rtems_task_create() and
 *   constructed by rtems_task_construct().
 *
 *   - Create a task.  Check that the home scheduler of the created task is
 *     scheduler A.
 *
 *   - Construct a task.  Check that the home scheduler of the constructed task
 *     is scheduler A.
 *
 * @{
 */

typedef struct {
  bool     owner_before;
  bool     owner_in_visitor;
  bool     owner_after;
  uint32_t counter_all;
  uint32_t counter_self;
  bool     done;
} TaskIterateContext;

static bool TaskVisitor( rtems_tcb *tcb, void *arg )
{
  TaskIterateContext *ctx;

  ctx = arg;
  ++ctx->counter_all;

  if ( rtems_task_self() == tcb->Object.id ) {
    ++ctx->counter_self;
  }

  ctx->owner_in_visitor = _RTEMS_Allocator_is_owner();

  return ctx->done;
}

/**
 * @brief Call rtems_task_self() check the returned value.
 */
static void RtemsTaskValTask_Action_0( void )
{
  rtems_id id;

  id = rtems_task_self();

  /*
   * Check that the returned value is equal to the object identifier of the
   * calling task.
   */
  T_step_eq_u32( 0, id, 0x0a010001 );
}

/**
 * @brief Call rtems_task_iterate() with a visitor which always returns false.
 */
static void RtemsTaskValTask_Action_1( void )
{
  TaskIterateContext ctx;
  uint32_t           task_count;

  task_count = rtems_scheduler_get_processor_maximum();

  if ( task_count > 4 ) {
    task_count = 4;
  }

  ++task_count;

  memset( &ctx, 0, sizeof( ctx ) );
  ctx.owner_before = _RTEMS_Allocator_is_owner();
  rtems_task_iterate( TaskVisitor, &ctx );
  ctx.owner_after = _RTEMS_Allocator_is_owner();

  /*
   * Check that the all counter is equal to the count of tasks.  Check that the
   * calling task was visited exacly once.  Firstly, this shows that
   * rtems_task_iterate() used the parameters specified by ``visitor`` and
   * ``arg``. Secondly, this shows that the iteration was done over all tasks.
   */
  T_step_eq_u32( 1, ctx.counter_all, task_count );
  T_step_eq_u32( 2, ctx.counter_self, 1 );

  /*
   * Check that the object alloctor mutex was not owned before and after the
   * call.  Check that the object alloctor mutex was owned during the
   * iteration.
   */
  T_step_false( 3, ctx.owner_before );
  T_step_true( 4, ctx.owner_in_visitor );
  T_step_false( 5, ctx.owner_after );
}

/**
 * @brief Call rtems_task_iterate() with a visitor which returns true.
 */
static void RtemsTaskValTask_Action_2( void )
{
  TaskIterateContext ctx;

  memset( &ctx, 0, sizeof( ctx ) );
  ctx.done = true;
  rtems_task_iterate( TaskVisitor, &ctx );

  /*
   * Check that the all counter is equal to one.  This shows that the iteration
   * stops when the visitor returns true.
   */
  T_step_eq_u32( 6, ctx.counter_all, 1 );
}

/**
 * @brief Assert that RTEMS_TASK_STORAGE_ALIGNMENT is a constant expression
 *   which evaluates to the expected value.
 */
static void RtemsTaskValTask_Action_3( void )
{
  RTEMS_STATIC_ASSERT(
    RTEMS_TASK_STORAGE_ALIGNMENT == CPU_STACK_ALIGNMENT,
    STORAGE_ALIGNMENT
  );
}

/**
 * @brief Assert that RTEMS_NO_PRIORITY is a constant expression which
 *   evaluates to the expected value.
 */
static void RtemsTaskValTask_Action_4( void )
{
  RTEMS_STATIC_ASSERT(
    RTEMS_NO_PRIORITY == RTEMS_CURRENT_PRIORITY,
    NO_PRIORITY
  );
}

/**
 * @brief Assert that RTEMS_MINIMUM_STACK_SIZE is a constant expression which
 *   evaluates to the expected value.
 */
static void RtemsTaskValTask_Action_5( void )
{
  RTEMS_STATIC_ASSERT(
    RTEMS_MINIMUM_STACK_SIZE == STACK_MINIMUM_SIZE,
    MINIMUM_STACK_SIZE
  );
}

/**
 * @brief Assert that RTEMS_CONFIGURED_MINIMUM_STACK_SIZE is a constant
 *   expression which evaluates to the expected value.
 */
static void RtemsTaskValTask_Action_6( void )
{
  RTEMS_STATIC_ASSERT(
    RTEMS_CONFIGURED_MINIMUM_STACK_SIZE == 0,
    CONFIGURED_MINIMUM_STACK_SIZE
  );
}

/**
 * @brief Assert that RTEMS_MINIMUM_PRIORITY is a constant expression which
 *   evaluates to the expected value.
 */
static void RtemsTaskValTask_Action_7( void )
{
  RTEMS_STATIC_ASSERT( RTEMS_MINIMUM_PRIORITY == 1, MINIMUM_PRIORITY );
}

/**
 * @brief Validate RTEMS_SELF using a sample directive call.
 */
static void RtemsTaskValTask_Action_8( void )
{
  rtems_status_code sc;

  /*
   * Check that rtems_task_is_suspended() returns the expected status if called
   * with a task identifier parameter of RTEMS_SELF.
   */
  sc = rtems_task_is_suspended( RTEMS_SELF );
  T_step_rsc_success( 7, sc );
}

/**
 * @brief Validate the home scheduler of tasks created by rtems_task_create()
 *   and constructed by rtems_task_construct().
 */
static void RtemsTaskValTask_Action_9( void )
{
  rtems_status_code sc;
  rtems_id          id;

  /*
   * Create a task.  Check that the home scheduler of the created task is
   * scheduler A.
   */
  sc = rtems_task_create(
    OBJECT_NAME,
    1,
    TEST_MINIMUM_STACK_SIZE,
    RTEMS_DEFAULT_MODES,
    RTEMS_DEFAULT_ATTRIBUTES,
    &id
  );
  T_step_rsc_success( 8, sc );

  T_step_eq_u32( 9, GetScheduler( id ), SCHEDULER_A_ID );
  DeleteTask( id );

  /*
   * Construct a task.  Check that the home scheduler of the constructed task
   * is scheduler A.
   */
  sc = rtems_task_construct( &DefaultTaskConfig, &id );
  T_step_rsc_success( 10, sc );

  T_step_eq_u32( 11, GetScheduler( id ), SCHEDULER_A_ID );
  DeleteTask( id );
}

/**
 * @fn void T_case_body_RtemsTaskValTask( void )
 */
T_TEST_CASE( RtemsTaskValTask )
{
  T_plan( 12 );

  RtemsTaskValTask_Action_0();
  RtemsTaskValTask_Action_1();
  RtemsTaskValTask_Action_2();
  RtemsTaskValTask_Action_3();
  RtemsTaskValTask_Action_4();
  RtemsTaskValTask_Action_5();
  RtemsTaskValTask_Action_6();
  RtemsTaskValTask_Action_7();
  RtemsTaskValTask_Action_8();
  RtemsTaskValTask_Action_9();
}

/** @} */
