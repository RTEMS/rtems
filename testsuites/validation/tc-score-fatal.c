/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreValFatal
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
#include <setjmp.h>
#include <string.h>
#include <rtems/score/atomic.h>
#include <rtems/score/isrlevel.h>
#include <rtems/score/threaddispatch.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreValFatal spec:/score/val/fatal
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @brief Tests some fatal errors.
 *
 * This test case performs the following actions:
 *
 * - Construct a task with a task body which returns.  Check that the right
 *   fatal error occurs.
 *
 * - Construct a task which performs a thread dispatch with maskable interrupts
 *   disabled.  Check that the right fatal error occurs or no fatal error
 *   occurs.
 *
 * - Construct a task which performs a direct thread dispatch with a thread
 *   dispatch level not equal to one.  Check that the right fatal error occurs.
 *
 * - Create a mutex and construct a task which produces a deadlock which
 *   involves the allocator mutex.
 *
 * - Check that rtems_fatal() terminates the system.  Since SetFatalHandler()
 *   requires an initial extension this validates CONFIGURE_INITIAL_EXTENSIONS.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/val/fatal test case.
 */
typedef struct {
  /**
   * @brief This member is a fatal extension invocation counter.
   */
  Atomic_Uint counter;

  /**
   * @brief This member contains the last fatal source.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains the last fatal code.
   */
  rtems_fatal_code code;
} ScoreValFatal_Context;

static ScoreValFatal_Context
  ScoreValFatal_Instance;

typedef ScoreValFatal_Context Context;

static unsigned int GetFatalCounter( const Context *ctx )
{
  return _Atomic_Load_uint( &ctx->counter, ATOMIC_ORDER_RELAXED );
}

static unsigned int ResetFatalInfo( Context *ctx )
{
  ctx->source = RTEMS_FATAL_SOURCE_APPLICATION;
  ctx->code = INTERNAL_ERROR_NO_MPCI;

  return GetFatalCounter( ctx );
}

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  Context           *ctx
)
{
  ctx->source = source;
  ctx->code = code;
  _Atomic_Fetch_add_uint( &ctx->counter, 1, ATOMIC_ORDER_RELAXED );
}

static void FatalTaskExit(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Fatal( source, code, arg );
  rtems_task_exit();
}

static void ExitTask( rtems_task_argument arg )
{
  (void) arg;
}

static void FatalBadThreadDispatchEnvironment(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Fatal( source, code, arg );
  _ISR_Set_level( 0 );
  _Thread_Dispatch_direct_no_return( _Per_CPU_Get() );
}

static void ISRDisabledThreadDispatchTask( rtems_task_argument arg )
{
  rtems_interrupt_level level;

  (void) arg;
  rtems_interrupt_local_disable( level );
  (void) level;
  rtems_task_exit();
}

static void FatalBadThreadDispatchDisableLevel(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Per_CPU_Control *cpu_self;

  Fatal( source, code, arg );
  cpu_self = _Per_CPU_Get();
  _Thread_Dispatch_unnest( cpu_self );
  _Thread_Dispatch_direct_no_return( cpu_self );
}

static void BadLevelThreadDispatchTask( rtems_task_argument arg )
{
  (void) arg;
  _Thread_Dispatch_disable();
  rtems_task_exit();
}

static jmp_buf before_fatal;

static rtems_id deadlock_mutex;

static bool ThreadCreateDeadlock( rtems_tcb *executing, rtems_tcb *created )
{
  (void) executing;
  (void) created;

  ObtainMutex( deadlock_mutex );
  ReleaseMutex( deadlock_mutex );

  return true;
}

static void FatalJumpBack(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  SetFatalHandler( NULL, NULL );
  Fatal( source, code, arg );
  longjmp( before_fatal, 1 );
}

static void ThreadQueueDeadlockTask( rtems_task_argument arg )
{
  rtems_id id;

  (void) arg;
  id = CreateTask( "DORM", PRIO_NORMAL );
  DeleteTask( id );

  rtems_task_exit();
}

static T_fixture ScoreValFatal_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreValFatal_Instance
};

/**
 * @brief Construct a task with a task body which returns.  Check that the
 *   right fatal error occurs.
 */
static void ScoreValFatal_Action_0( ScoreValFatal_Context *ctx )
{
  rtems_id     id;
  unsigned int counter;

  SetFatalHandler( FatalTaskExit, ctx );
  SetSelfPriority( PRIO_NORMAL );
  counter = ResetFatalInfo( ctx );
  id = CreateTask( "EXIT", PRIO_HIGH );
  StartTask( id, ExitTask, NULL );
  T_eq_uint( GetFatalCounter( ctx ), counter + 1 );
  T_eq_int( ctx->source, INTERNAL_ERROR_CORE );
  T_eq_ulong( ctx->code, INTERNAL_ERROR_THREAD_EXITTED );
  RestoreRunnerPriority();
  SetFatalHandler( NULL, NULL );
}

/**
 * @brief Construct a task which performs a thread dispatch with maskable
 *   interrupts disabled.  Check that the right fatal error occurs or no fatal
 *   error occurs.
 */
static void ScoreValFatal_Action_1( ScoreValFatal_Context *ctx )
{
  rtems_id     id;
  unsigned int counter;

  SetFatalHandler( FatalBadThreadDispatchEnvironment, ctx );
  SetSelfPriority( PRIO_NORMAL );
  counter = ResetFatalInfo( ctx );
  id = CreateTask( "BENV", PRIO_HIGH );
  StartTask( id, ISRDisabledThreadDispatchTask, NULL );

  #if CPU_ENABLE_ROBUST_THREAD_DISPATCH == FALSE
  if ( rtems_configuration_get_maximum_processors() > 1 ) {
  #endif
    T_eq_uint( GetFatalCounter( ctx ), counter + 1 );
    T_eq_int( ctx->source, INTERNAL_ERROR_CORE );
    T_eq_ulong( ctx->code, INTERNAL_ERROR_BAD_THREAD_DISPATCH_ENVIRONMENT );
  #if CPU_ENABLE_ROBUST_THREAD_DISPATCH == FALSE
  } else {
    T_eq_uint( GetFatalCounter( ctx ), counter );
  }
  #endif

  RestoreRunnerPriority();
  SetFatalHandler( NULL, NULL );
}

/**
 * @brief Construct a task which performs a direct thread dispatch with a
 *   thread dispatch level not equal to one.  Check that the right fatal error
 *   occurs.
 */
static void ScoreValFatal_Action_2( ScoreValFatal_Context *ctx )
{
  rtems_id     id;
  unsigned int counter;

  SetFatalHandler( FatalBadThreadDispatchDisableLevel, ctx );
  SetSelfPriority( PRIO_NORMAL );
  counter = ResetFatalInfo( ctx );
  id = CreateTask( "BLVL", PRIO_HIGH );
  StartTask( id, BadLevelThreadDispatchTask, NULL );
  T_eq_uint( GetFatalCounter( ctx ), counter + 1 );
  T_eq_int( ctx->source, INTERNAL_ERROR_CORE );
  T_eq_ulong( ctx->code, INTERNAL_ERROR_BAD_THREAD_DISPATCH_DISABLE_LEVEL );
  RestoreRunnerPriority();
  SetFatalHandler( NULL, NULL );
}

/**
 * @brief Create a mutex and construct a task which produces a deadlock which
 *   involves the allocator mutex.
 */
static void ScoreValFatal_Action_3( ScoreValFatal_Context *ctx )
{
  rtems_extensions_table extensions;
  rtems_status_code      sc;
  rtems_id               extension_id;
  rtems_id               task_id;
  unsigned int           counter;

  memset( &extensions, 0, sizeof( extensions ) );
  extensions.thread_create = ThreadCreateDeadlock;

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'X', 'T' ),
    &extensions,
    &extension_id
  );
  T_rsc_success( sc );

  deadlock_mutex = CreateMutex();

  SetFatalHandler( FatalJumpBack, ctx );
  SetSelfPriority( PRIO_NORMAL );
  counter = ResetFatalInfo( ctx );

  ObtainMutex( deadlock_mutex );

  task_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( task_id, ThreadQueueDeadlockTask, NULL );

  if ( setjmp( before_fatal ) == 0 ) {
    (void) CreateTask( "DLCK", PRIO_NORMAL );
  }

  ReleaseMutex( deadlock_mutex );

  T_eq_uint( GetFatalCounter( ctx ), counter + 1 );
  T_eq_int( ctx->source, INTERNAL_ERROR_CORE );
  T_eq_ulong( ctx->code, INTERNAL_ERROR_THREAD_QUEUE_DEADLOCK );

  RestoreRunnerPriority();

  sc = rtems_extension_delete( extension_id );
  T_rsc_success( sc );

  DeleteMutex( deadlock_mutex );
}

/**
 * @brief Check that rtems_fatal() terminates the system.  Since
 *   SetFatalHandler() requires an initial extension this validates
 *   CONFIGURE_INITIAL_EXTENSIONS.
 */
static void ScoreValFatal_Action_4( ScoreValFatal_Context *ctx )
{
  unsigned int counter;

  SetFatalHandler( FatalJumpBack, ctx );
  counter = ResetFatalInfo( ctx );

  if ( setjmp( before_fatal ) == 0 ) {
    rtems_fatal( 123, 4567890 );
  }

  T_eq_uint( GetFatalCounter( ctx ), counter + 1 );
  T_eq_int( ctx->source, 123 );
  T_eq_ulong( ctx->code, 4567890 );
}

/**
 * @fn void T_case_body_ScoreValFatal( void )
 */
T_TEST_CASE_FIXTURE( ScoreValFatal, &ScoreValFatal_Fixture )
{
  ScoreValFatal_Context *ctx;

  ctx = T_fixture_context();

  ScoreValFatal_Action_0( ctx );
  ScoreValFatal_Action_1( ctx );
  ScoreValFatal_Action_2( ctx );
  ScoreValFatal_Action_3( ctx );
  ScoreValFatal_Action_4( ctx );
}

/** @} */
