/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup ScoreSmpValFatal
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

#include <setjmp.h>
#include <rtems/sysinit.h>
#include <rtems/score/atomic.h>
#include <rtems/score/percpu.h>
#include <rtems/score/smpimpl.h>

#include "tr-fatal-smp.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup ScoreSmpValFatal spec:/score/smp/val/fatal
 *
 * @ingroup TestsuitesFatalSmp
 *
 * @brief Tests four fatal errors.
 *
 * This test case performs the following actions:
 *
 * - The test action is carried out by TriggerTestCase().
 *
 *   - Check that the expected fatal source is present.
 *
 *   - Check that the expected fatal code is present.
 *
 *   - Check that the processor state is shutdown.
 *
 *   - Check that a second shutdown request does not end in a recursive
 *     shutdown response.
 *
 * - Issue a job on a processor in the shutdown state.  Check that the right
 *   fatal error occurs if we try to wait for this job to complete.
 *
 * - Start multitasking on an invalid processor.  Check that the right fatal
 *   error occurs.
 *
 * - Start multitasking on an unassigned processor.  Check that the right fatal
 *   error occurs.
 *
 * @{
 */

/**
 * @brief Test context for spec:/score/smp/val/fatal test case.
 */
typedef struct {
  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreSmpValFatal_Run() parameter.
   */
  rtems_fatal_source source;

  /**
   * @brief This member contains a copy of the corresponding
   *   ScoreSmpValFatal_Run() parameter.
   */
  rtems_fatal_code code;
} ScoreSmpValFatal_Context;

static ScoreSmpValFatal_Context
  ScoreSmpValFatal_Instance;

static void TriggerTestCase( void )
{
  _SMP_Request_shutdown();
  (void) _CPU_Thread_Idle_body( 0 );
}

RTEMS_SYSINIT_ITEM(
  TriggerTestCase,
  RTEMS_SYSINIT_DEVICE_DRIVERS,
  RTEMS_SYSINIT_ORDER_MIDDLE
);

static jmp_buf fatal_before;

static Atomic_Uint fatal_counter;

static rtems_fatal_source fatal_source;

static rtems_fatal_code fatal_code;

static void FatalRecordAndJump(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  (void) arg;

  fatal_source = source;
  fatal_code = code;
  _Atomic_Fetch_add_uint( &fatal_counter, 1, ATOMIC_ORDER_RELAXED );
  longjmp( fatal_before, 1 );
}

static void DoNothing( void *arg )
{
  (void) arg;
}

static const Per_CPU_Job_context job_context = {
  .handler = DoNothing
};

Per_CPU_Job job = {
  .context = &job_context
};

static T_fixture ScoreSmpValFatal_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &ScoreSmpValFatal_Instance
};

/**
 * @brief The test action is carried out by TriggerTestCase().
 */
static void ScoreSmpValFatal_Action_0( ScoreSmpValFatal_Context *ctx )
{
  /* Nothing to do */

  /*
   * Check that the expected fatal source is present.
   */
  T_step_eq_int( 0, ctx->source, RTEMS_FATAL_SOURCE_SMP );

  /*
   * Check that the expected fatal code is present.
   */
  T_step_eq_ulong( 1, ctx->code, SMP_FATAL_SHUTDOWN_RESPONSE );

  /*
   * Check that the processor state is shutdown.
   */
  T_step_eq_int(
    2,
    _Per_CPU_Get_state( _Per_CPU_Get() ),
    PER_CPU_STATE_SHUTDOWN
  );

  /*
   * Check that a second shutdown request does not end in a recursive shutdown
   * response.
   */
  _SMP_Process_message( _Per_CPU_Get(), SMP_MESSAGE_SHUTDOWN );
}

/**
 * @brief Issue a job on a processor in the shutdown state.  Check that the
 *   right fatal error occurs if we try to wait for this job to complete.
 */
static void ScoreSmpValFatal_Action_1( ScoreSmpValFatal_Context *ctx )
{
  Per_CPU_Control *cpu;

  SetFatalHandler( FatalRecordAndJump, ctx );
  cpu = _Per_CPU_Get_by_index( 0 );
  _Per_CPU_Submit_job( cpu, &job );

  if ( setjmp( fatal_before ) == 0 ) {
    _Per_CPU_Wait_for_job( cpu, &job );
  }

  T_step_eq_uint(
    3,
    _Atomic_Load_uint( &fatal_counter, ATOMIC_ORDER_RELAXED ),
    1
  );
  T_step_eq_int( 4, fatal_source, RTEMS_FATAL_SOURCE_SMP );
  T_step_eq_ulong(
    5,
    fatal_code,
    SMP_FATAL_WRONG_CPU_STATE_TO_PERFORM_JOBS
  );
  SetFatalHandler( NULL, NULL );
}

/**
 * @brief Start multitasking on an invalid processor.  Check that the right
 *   fatal error occurs.
 */
static void ScoreSmpValFatal_Action_2( ScoreSmpValFatal_Context *ctx )
{
  Per_CPU_Control *cpu;

  SetFatalHandler( FatalRecordAndJump, ctx );

  /*
   * This element is outside the array.  This is not an issue since
   * _SMP_Start_multitasking_on_secondary_processor() does not access the
   * structure.
   */
  cpu = _Per_CPU_Get_by_index( 3 );

  if ( setjmp( fatal_before ) == 0 ) {
    _SMP_Start_multitasking_on_secondary_processor( cpu );
  }

  T_step_eq_uint(
    6,
    _Atomic_Load_uint( &fatal_counter, ATOMIC_ORDER_RELAXED ),
    2
  );
  T_step_eq_int( 7, fatal_source, RTEMS_FATAL_SOURCE_SMP );
  T_step_eq_ulong(
    8,
    fatal_code,
    SMP_FATAL_MULTITASKING_START_ON_INVALID_PROCESSOR
  );
  SetFatalHandler( NULL, NULL );
}

/**
 * @brief Start multitasking on an unassigned processor.  Check that the right
 *   fatal error occurs.
 */
static void ScoreSmpValFatal_Action_3( ScoreSmpValFatal_Context *ctx )
{
  Per_CPU_Control *cpu;

  SetFatalHandler( FatalRecordAndJump, ctx );
  cpu = _Per_CPU_Get_by_index( 2 );

  if ( setjmp( fatal_before ) == 0 ) {
    _SMP_Start_multitasking_on_secondary_processor( cpu );
  }

  T_step_eq_uint(
    9,
    _Atomic_Load_uint( &fatal_counter, ATOMIC_ORDER_RELAXED ),
    3
  );
  T_step_eq_int( 10, fatal_source, RTEMS_FATAL_SOURCE_SMP );
  T_step_eq_ulong(
    11,
    fatal_code,
    SMP_FATAL_MULTITASKING_START_ON_UNASSIGNED_PROCESSOR
  );
  SetFatalHandler( NULL, NULL );
}

void ScoreSmpValFatal_Run( rtems_fatal_source source, rtems_fatal_code code )
{
  ScoreSmpValFatal_Context *ctx;

  ctx = &ScoreSmpValFatal_Instance;
  ctx->source = source;
  ctx->code = code;

  ctx = T_case_begin( "ScoreSmpValFatal", &ScoreSmpValFatal_Fixture );

  T_plan( 12 );

  ScoreSmpValFatal_Action_0( ctx );
  ScoreSmpValFatal_Action_1( ctx );
  ScoreSmpValFatal_Action_2( ctx );
  ScoreSmpValFatal_Action_3( ctx );

  T_case_end();
}

/** @} */
