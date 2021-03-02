/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsSignalReqCatch
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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
#include <rtems/score/smpbarrier.h>

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseRtemsSignalReqCatch spec:/rtems/signal/req/catch
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 * @ingroup RTEMSTestSuiteTestsuitesValidation1
 *
 * @{
 */

typedef enum {
  RtemsSignalReqCatch_Pre_Pending_Yes,
  RtemsSignalReqCatch_Pre_Pending_No,
  RtemsSignalReqCatch_Pre_Pending_NA
} RtemsSignalReqCatch_Pre_Pending;

typedef enum {
  RtemsSignalReqCatch_Pre_Handler_Invalid,
  RtemsSignalReqCatch_Pre_Handler_Valid,
  RtemsSignalReqCatch_Pre_Handler_NA
} RtemsSignalReqCatch_Pre_Handler;

typedef enum {
  RtemsSignalReqCatch_Pre_Preempt_Yes,
  RtemsSignalReqCatch_Pre_Preempt_No,
  RtemsSignalReqCatch_Pre_Preempt_NA
} RtemsSignalReqCatch_Pre_Preempt;

typedef enum {
  RtemsSignalReqCatch_Pre_Timeslice_Yes,
  RtemsSignalReqCatch_Pre_Timeslice_No,
  RtemsSignalReqCatch_Pre_Timeslice_NA
} RtemsSignalReqCatch_Pre_Timeslice;

typedef enum {
  RtemsSignalReqCatch_Pre_ASR_Yes,
  RtemsSignalReqCatch_Pre_ASR_No,
  RtemsSignalReqCatch_Pre_ASR_NA
} RtemsSignalReqCatch_Pre_ASR;

typedef enum {
  RtemsSignalReqCatch_Pre_IntLvl_Zero,
  RtemsSignalReqCatch_Pre_IntLvl_Positive,
  RtemsSignalReqCatch_Pre_IntLvl_NA
} RtemsSignalReqCatch_Pre_IntLvl;

typedef enum {
  RtemsSignalReqCatch_Post_Status_Ok,
  RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
  RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
  RtemsSignalReqCatch_Post_Status_NA
} RtemsSignalReqCatch_Post_Status;

typedef enum {
  RtemsSignalReqCatch_Post_Send_New,
  RtemsSignalReqCatch_Post_Send_NotDef,
  RtemsSignalReqCatch_Post_Send_NA
} RtemsSignalReqCatch_Post_Send;

typedef enum {
  RtemsSignalReqCatch_Post_Preempt_Yes,
  RtemsSignalReqCatch_Post_Preempt_No,
  RtemsSignalReqCatch_Post_Preempt_NA
} RtemsSignalReqCatch_Post_Preempt;

typedef enum {
  RtemsSignalReqCatch_Post_Timeslice_Yes,
  RtemsSignalReqCatch_Post_Timeslice_No,
  RtemsSignalReqCatch_Post_Timeslice_NA
} RtemsSignalReqCatch_Post_Timeslice;

typedef enum {
  RtemsSignalReqCatch_Post_ASR_Yes,
  RtemsSignalReqCatch_Post_ASR_No,
  RtemsSignalReqCatch_Post_ASR_NA
} RtemsSignalReqCatch_Post_ASR;

typedef enum {
  RtemsSignalReqCatch_Post_IntLvl_Zero,
  RtemsSignalReqCatch_Post_IntLvl_Positive,
  RtemsSignalReqCatch_Post_IntLvl_NA
} RtemsSignalReqCatch_Post_IntLvl;

/**
 * @brief Test context for spec:/rtems/signal/req/catch test case.
 */
typedef struct {
  rtems_id runner_id;

  rtems_id worker_id;

  uint32_t pending_signals;

  SMP_barrier_Control barrier;

  SMP_barrier_State runner_barrier_state;

  uint32_t default_handler_calls;

  uint32_t handler_calls;

  rtems_mode handler_mode;

  rtems_mode normal_mode;

  rtems_asr_entry handler;

  rtems_mode mode;

  rtems_status_code catch_status;

  rtems_status_code send_status;

  /**
   * @brief This member defines the pre-condition states for the next action.
   */
  size_t pcs[ 6 ];

  /**
   * @brief This member indicates if the test action loop is currently
   *   executed.
   */
  bool in_action_loop;
} RtemsSignalReqCatch_Context;

static RtemsSignalReqCatch_Context
  RtemsSignalReqCatch_Instance;

static const char * const RtemsSignalReqCatch_PreDesc_Pending[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSignalReqCatch_PreDesc_Handler[] = {
  "Invalid",
  "Valid",
  "NA"
};

static const char * const RtemsSignalReqCatch_PreDesc_Preempt[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSignalReqCatch_PreDesc_Timeslice[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSignalReqCatch_PreDesc_ASR[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSignalReqCatch_PreDesc_IntLvl[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const * const RtemsSignalReqCatch_PreDesc[] = {
  RtemsSignalReqCatch_PreDesc_Pending,
  RtemsSignalReqCatch_PreDesc_Handler,
  RtemsSignalReqCatch_PreDesc_Preempt,
  RtemsSignalReqCatch_PreDesc_Timeslice,
  RtemsSignalReqCatch_PreDesc_ASR,
  RtemsSignalReqCatch_PreDesc_IntLvl,
  NULL
};

typedef RtemsSignalReqCatch_Context Context;

static void DefaultHandler( rtems_signal_set signal_set )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->default_handler_calls;

  if ( ctx->pending_signals != 0 && ctx->default_handler_calls == 1 ) {
    T_eq_u32( signal_set, 0x600df00d );
  } else {
    T_eq_u32( signal_set, 0xdeadbeef );
  }
}

static void SignalHandler( rtems_signal_set signal_set )
{
  Context          *ctx;
  rtems_status_code sc;

  ctx = T_fixture_context();
  ++ctx->handler_calls;

  sc = rtems_task_mode(
    RTEMS_DEFAULT_MODES,
    RTEMS_CURRENT_MODE,
    &ctx->handler_mode
  );
  T_rsc_success( sc );

  if ( ctx->pending_signals != 0 && ctx->handler_calls == 1 ) {
    T_eq_u32( signal_set, 0x600df00d );
  } else {
    T_eq_u32( signal_set, 0xdeadbeef );
  }
}

static void CheckHandlerMode( Context *ctx, rtems_mode mask, rtems_mode mode )
{
  if ( ctx->catch_status == RTEMS_SUCCESSFUL && ctx->handler != NULL ) {
    T_ne_u32( ctx->handler_mode, 0xffffffff );
    T_eq_u32( ctx->handler_mode & mask, mode );
  }
}

static void Worker( rtems_task_argument arg )
{
  Context          *ctx;
  SMP_barrier_State barrier_state;

  ctx = (Context *) arg;
  _SMP_barrier_State_initialize( &barrier_state );

  while ( true ) {
    rtems_status_code sc;

    _SMP_barrier_Wait( &ctx->barrier, &barrier_state, 2 );

    sc = rtems_signal_send( ctx->runner_id, 0x600df00d );
    T_rsc_success( sc );

    _SMP_barrier_Wait( &ctx->barrier, &barrier_state, 2 );
  }
}

static void RtemsSignalReqCatch_Pre_Pending_Prepare(
  RtemsSignalReqCatch_Context    *ctx,
  RtemsSignalReqCatch_Pre_Pending state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Pre_Pending_Yes: {
      /*
       * Where the system has more than one processor, when
       * rtems_signal_catch() is called, the calling task shall have pending
       * signals.
       */
      ctx->pending_signals = ( rtems_scheduler_get_processor_maximum() > 1 ) ? 1 : 0;
      break;
    }

    case RtemsSignalReqCatch_Pre_Pending_No: {
      /*
       * When rtems_signal_catch() is called, the calling task shall have no
       * pending signals.
       */
      ctx->pending_signals = 0;
      break;
    }

    case RtemsSignalReqCatch_Pre_Pending_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Pre_Handler_Prepare(
  RtemsSignalReqCatch_Context    *ctx,
  RtemsSignalReqCatch_Pre_Handler state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Pre_Handler_Invalid: {
      /*
       * The ``asr_handler`` parameter shall be NULL.
       */
      ctx->handler = NULL;
      break;
    }

    case RtemsSignalReqCatch_Pre_Handler_Valid: {
      /*
       * The ``asr_handler`` parameter shall be a valid ASR handler.
       */
      ctx->handler = SignalHandler;
      break;
    }

    case RtemsSignalReqCatch_Pre_Handler_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Pre_Preempt_Prepare(
  RtemsSignalReqCatch_Context    *ctx,
  RtemsSignalReqCatch_Pre_Preempt state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Pre_Preempt_Yes: {
      /*
       * The ``mode_set`` parameter shall specify that preemption is enabled.
       */
      #if defined(RTEMS_SMP)
      if ( rtems_configuration_get_maximum_processors() == 1 ) {
        ctx->normal_mode |= RTEMS_NO_PREEMPT;
      }
      #else
      ctx->normal_mode |= RTEMS_NO_PREEMPT;
      #endif
      break;
    }

    case RtemsSignalReqCatch_Pre_Preempt_No: {
      /*
       * The ``mode_set`` parameter shall specify that preemption is disabled.
       */
      ctx->mode |= RTEMS_NO_PREEMPT;
      break;
    }

    case RtemsSignalReqCatch_Pre_Preempt_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Pre_Timeslice_Prepare(
  RtemsSignalReqCatch_Context      *ctx,
  RtemsSignalReqCatch_Pre_Timeslice state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Pre_Timeslice_Yes: {
      /*
       * The ``mode_set`` parameter shall specify that timeslicing is enabled.
       */
      ctx->mode |= RTEMS_TIMESLICE;
      break;
    }

    case RtemsSignalReqCatch_Pre_Timeslice_No: {
      /*
       * The ``mode_set`` parameter shall specify that timeslicing is disabled.
       */
      ctx->normal_mode |= RTEMS_TIMESLICE;
      break;
    }

    case RtemsSignalReqCatch_Pre_Timeslice_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Pre_ASR_Prepare(
  RtemsSignalReqCatch_Context *ctx,
  RtemsSignalReqCatch_Pre_ASR  state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Pre_ASR_Yes: {
      /*
       * The ``mode_set`` parameter shall specify that ASR processing is
       * enabled.
       */
      /* We cannot disable ASR processing at normal task level for this test */
      break;
    }

    case RtemsSignalReqCatch_Pre_ASR_No: {
      /*
       * The ``mode_set`` parameter shall specify that ASR processing is
       * disabled.
       */
      ctx->mode |= RTEMS_NO_ASR;
      break;
    }

    case RtemsSignalReqCatch_Pre_ASR_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Pre_IntLvl_Prepare(
  RtemsSignalReqCatch_Context   *ctx,
  RtemsSignalReqCatch_Pre_IntLvl state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Pre_IntLvl_Zero: {
      /*
       * The ``mode_set`` parameter shall specify an interrupt level of zero.
       */
      #if !defined(RTEMS_SMP) && CPU_ENABLE_ROBUST_THREAD_DISPATCH == FALSE
      ctx->normal_mode |= RTEMS_INTERRUPT_LEVEL( 1 );
      #endif
      break;
    }

    case RtemsSignalReqCatch_Pre_IntLvl_Positive: {
      /*
       * The ``mode_set`` parameter shall specify a positive interrupt level.
       */
      ctx->mode |= RTEMS_INTERRUPT_LEVEL( 1 );
      break;
    }

    case RtemsSignalReqCatch_Pre_IntLvl_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Post_Status_Check(
  RtemsSignalReqCatch_Context    *ctx,
  RtemsSignalReqCatch_Post_Status state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Post_Status_Ok: {
      /*
       * The return status of rtems_signal_catch() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->catch_status );
      break;
    }

    case RtemsSignalReqCatch_Post_Status_NotImplNoPreempt: {
      /*
       * Where the system is configured with SMP support, if the scheduler does
       * not support the no-preempt mode, then the return status of
       * rtems_signal_catch() shall be RTEMS_NOT_IMPLEMENTED, otherwise the
       * return status shall be RTEMS_SUCCESSFUL.
       */
      #if defined(RTEMS_SMP)
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_rsc( ctx->catch_status, RTEMS_NOT_IMPLEMENTED );
      } else {
        T_rsc_success( ctx->catch_status );
      }
      #else
      T_rsc_success( ctx->catch_status );
      #endif
      break;
    }

    case RtemsSignalReqCatch_Post_Status_NotImplIntLvl: {
      /*
       * Where the system is configured with SMP support and the configured
       * processor maximum is greater than one, or the CPU port enabled robust
       * thread dispatching, the return status of rtems_signal_catch() shall be
       * RTEMS_NOT_IMPLEMENTED, otherwise the return status shall be
       * RTEMS_SUCCESSFUL.
       */
      #if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
      T_rsc( ctx->catch_status, RTEMS_NOT_IMPLEMENTED );
      #elif defined(RTEMS_SMP)
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_rsc( ctx->catch_status, RTEMS_NOT_IMPLEMENTED );
      } else {
        T_rsc_success( ctx->catch_status );
      }
      #else
      T_rsc_success( ctx->catch_status );
      #endif
      break;
    }

    case RtemsSignalReqCatch_Post_Status_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Post_Send_Check(
  RtemsSignalReqCatch_Context  *ctx,
  RtemsSignalReqCatch_Post_Send state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Post_Send_New: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call was successful, the ASR processing shall be done with the
       * specified handler, otherwise the ASR information of the caller shall
       * be unchanged.
       */
      T_rsc_success( ctx->send_status );

      if ( ctx->catch_status == RTEMS_SUCCESSFUL ) {
        T_eq_u32( ctx->default_handler_calls, 0 );
        T_eq_u32( ctx->handler_calls, 1 + ctx->pending_signals );
        T_ne_u32( ctx->handler_mode, 0xffffffff );
      } else {
        T_eq_u32( ctx->default_handler_calls, 1 + ctx->pending_signals );
        T_eq_u32( ctx->handler_calls, 0 );
        T_eq_u32( ctx->handler_mode, 0xffffffff );
      }
      break;
    }

    case RtemsSignalReqCatch_Post_Send_NotDef: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call was successful, the ASR processing shall be deactivated and
       * all pending signals shall be cleared, otherwise the ASR information of
       * the caller shall be unchanged.
       */
      if ( ctx->catch_status == RTEMS_SUCCESSFUL ) {
        T_rsc( ctx->send_status, RTEMS_NOT_DEFINED );
        T_eq_u32( ctx->default_handler_calls, 0 );
        T_eq_u32( ctx->handler_calls, 0 );
        T_eq_u32( ctx->handler_mode, 0xffffffff );
      } else {
        T_rsc_success( ctx->send_status );
        T_eq_u32( ctx->default_handler_calls, 1 + ctx->pending_signals );
        T_eq_u32( ctx->handler_calls, 0 );
        T_eq_u32( ctx->handler_mode, 0xffffffff );
      }
      break;
    }

    case RtemsSignalReqCatch_Post_Send_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Post_Preempt_Check(
  RtemsSignalReqCatch_Context     *ctx,
  RtemsSignalReqCatch_Post_Preempt state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Post_Preempt_Yes: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call with a valid handler was successful, the ASR processing shall
       * be done with preemption enabled.
       */
      CheckHandlerMode( ctx, RTEMS_PREEMPT_MASK, RTEMS_PREEMPT );
      break;
    }

    case RtemsSignalReqCatch_Post_Preempt_No: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call with a valid handler was successful, the ASR processing shall
       * be done with preemption disabled.
       */
      CheckHandlerMode( ctx, RTEMS_PREEMPT_MASK, RTEMS_NO_PREEMPT );
      break;
    }

    case RtemsSignalReqCatch_Post_Preempt_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Post_Timeslice_Check(
  RtemsSignalReqCatch_Context       *ctx,
  RtemsSignalReqCatch_Post_Timeslice state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Post_Timeslice_Yes: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call with a valid handler was successful, the ASR processing shall
       * be done with timeslicing enabled.
       */
      CheckHandlerMode( ctx, RTEMS_TIMESLICE_MASK, RTEMS_TIMESLICE );
      break;
    }

    case RtemsSignalReqCatch_Post_Timeslice_No: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call with a valid handler was successful, the ASR processing shall
       * be done with timeslicing disabled.
       */
      CheckHandlerMode( ctx, RTEMS_TIMESLICE_MASK, RTEMS_NO_TIMESLICE );
      break;
    }

    case RtemsSignalReqCatch_Post_Timeslice_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Post_ASR_Check(
  RtemsSignalReqCatch_Context *ctx,
  RtemsSignalReqCatch_Post_ASR state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Post_ASR_Yes: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call with a valid handler was successful, the ASR processing shall
       * be done with ASR processing enabled.
       */
      CheckHandlerMode( ctx, RTEMS_ASR_MASK, RTEMS_ASR );
      break;
    }

    case RtemsSignalReqCatch_Post_ASR_No: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call with a valid handler was successful, the ASR processing shall
       * be done with ASR processing disabled.
       */
      CheckHandlerMode( ctx, RTEMS_ASR_MASK, RTEMS_NO_ASR );
      break;
    }

    case RtemsSignalReqCatch_Post_ASR_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Post_IntLvl_Check(
  RtemsSignalReqCatch_Context    *ctx,
  RtemsSignalReqCatch_Post_IntLvl state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Post_IntLvl_Zero: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call with a valid handler was successful, the ASR processing shall
       * be done with interrupts enabled.
       */
      CheckHandlerMode( ctx, RTEMS_INTERRUPT_MASK, RTEMS_INTERRUPT_LEVEL( 0 ) );
      break;
    }

    case RtemsSignalReqCatch_Post_IntLvl_Positive: {
      /*
       * When a signal set is sent to the caller of rtems_signal_catch() and
       * the call with a valid handler was successful, the ASR processing shall
       * be done with interrupts disabled according to the specified interrupt
       * level.
       */
      CheckHandlerMode( ctx, RTEMS_INTERRUPT_MASK, RTEMS_INTERRUPT_LEVEL( 1 ) );
      break;
    }

    case RtemsSignalReqCatch_Post_IntLvl_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Setup( RtemsSignalReqCatch_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->runner_id = rtems_task_self();
  _SMP_barrier_Control_initialize( &ctx->barrier );
  _SMP_barrier_State_initialize( &ctx->runner_barrier_state );

  if ( rtems_scheduler_get_processor_maximum() > 1 ) {
    rtems_status_code sc;
    rtems_id          scheduler_id;

    sc = rtems_task_create(
      rtems_build_name( 'W', 'O', 'R', 'K' ),
      1,
      RTEMS_MINIMUM_STACK_SIZE,
      RTEMS_DEFAULT_MODES,
      RTEMS_DEFAULT_ATTRIBUTES,
      &ctx->worker_id
    );
    T_assert_rsc_success( sc );

    sc = rtems_scheduler_ident_by_processor( 1, &scheduler_id );
    T_assert_rsc_success( sc );

    sc = rtems_task_set_scheduler( ctx->worker_id, scheduler_id, 1 );
    T_assert_rsc_success( sc );

    sc = rtems_task_start(
      ctx->worker_id,
      Worker,
      (rtems_task_argument) ctx
    );
    T_assert_rsc_success( sc );
  }
}

static void RtemsSignalReqCatch_Setup_Wrap( void *arg )
{
  RtemsSignalReqCatch_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsSignalReqCatch_Setup( ctx );
}

static void RtemsSignalReqCatch_Teardown( RtemsSignalReqCatch_Context *ctx )
{
  rtems_status_code sc;

  if ( ctx->worker_id != 0 ) {
    sc = rtems_task_delete( ctx->worker_id );
    T_rsc_success( sc );
  }

  sc = rtems_signal_catch( NULL, RTEMS_DEFAULT_MODES );
  T_rsc_success( sc );
}

static void RtemsSignalReqCatch_Teardown_Wrap( void *arg )
{
  RtemsSignalReqCatch_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsSignalReqCatch_Teardown( ctx );
}

static size_t RtemsSignalReqCatch_Scope( void *arg, char *buf, size_t n )
{
  RtemsSignalReqCatch_Context *ctx;

  ctx = arg;

  if ( ctx->in_action_loop ) {
    return T_get_scope( RtemsSignalReqCatch_PreDesc, buf, n, ctx->pcs );
  }

  return 0;
}

static T_fixture RtemsSignalReqCatch_Fixture = {
  .setup = RtemsSignalReqCatch_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsSignalReqCatch_Teardown_Wrap,
  .scope = RtemsSignalReqCatch_Scope,
  .initial_context = &RtemsSignalReqCatch_Instance
};

static const uint8_t RtemsSignalReqCatch_TransitionMap[][ 6 ] = {
  {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_NotDef,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_Yes,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_Yes,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_Yes,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Zero
  }, {
    RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_Send_New,
    RtemsSignalReqCatch_Post_Preempt_No,
    RtemsSignalReqCatch_Post_Timeslice_No,
    RtemsSignalReqCatch_Post_ASR_No,
    RtemsSignalReqCatch_Post_IntLvl_Positive
  }
};

static const struct {
  uint8_t Skip : 1;
  uint8_t Pre_Pending_NA : 1;
  uint8_t Pre_Handler_NA : 1;
  uint8_t Pre_Preempt_NA : 1;
  uint8_t Pre_Timeslice_NA : 1;
  uint8_t Pre_ASR_NA : 1;
  uint8_t Pre_IntLvl_NA : 1;
} RtemsSignalReqCatch_TransitionInfo[] = {
  {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }, {
    0, 0, 0, 0, 0, 0, 0
  }
};

static void RtemsSignalReqCatch_Prepare( RtemsSignalReqCatch_Context *ctx )
{
  rtems_status_code sc;

  ctx->default_handler_calls = 0;
  ctx->handler_calls = 0;
  ctx->handler_mode = 0xffffffff;
  ctx->normal_mode = RTEMS_DEFAULT_MODES;
  ctx->handler = NULL;
  ctx->mode = RTEMS_DEFAULT_MODES;

  sc = rtems_signal_catch( DefaultHandler, RTEMS_NO_ASR );
  T_rsc_success( sc );
}

static void RtemsSignalReqCatch_Action( RtemsSignalReqCatch_Context *ctx )
{
  rtems_status_code sc;
  rtems_mode        mode;

  if ( ctx->pending_signals != 0 ) {
    rtems_interrupt_level level;

    rtems_interrupt_local_disable(level);
    _SMP_barrier_Wait( &ctx->barrier, &ctx->runner_barrier_state, 2 );
    _SMP_barrier_Wait( &ctx->barrier, &ctx->runner_barrier_state, 2 );
    ctx->catch_status = rtems_signal_catch( ctx->handler, ctx->mode );
    rtems_interrupt_local_enable(level);
  } else {
    ctx->catch_status = rtems_signal_catch( ctx->handler, ctx->mode );
  }

  sc = rtems_task_mode( ctx->normal_mode, RTEMS_ALL_MODE_MASKS, &mode );
  T_rsc_success( sc );

  ctx->send_status = rtems_signal_send( RTEMS_SELF, 0xdeadbeef );

  sc = rtems_task_mode( mode, RTEMS_ALL_MODE_MASKS, &mode );
  T_rsc_success( sc );
}

/**
 * @fn void T_case_body_RtemsSignalReqCatch( void )
 */
T_TEST_CASE_FIXTURE( RtemsSignalReqCatch, &RtemsSignalReqCatch_Fixture )
{
  RtemsSignalReqCatch_Context *ctx;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsSignalReqCatch_Pre_Pending_Yes;
    ctx->pcs[ 0 ] < RtemsSignalReqCatch_Pre_Pending_NA;
    ++ctx->pcs[ 0 ]
  ) {
    if ( RtemsSignalReqCatch_TransitionInfo[ index ].Pre_Pending_NA ) {
      ctx->pcs[ 0 ] = RtemsSignalReqCatch_Pre_Pending_NA;
      index += ( RtemsSignalReqCatch_Pre_Pending_NA - 1 )
        * RtemsSignalReqCatch_Pre_Handler_NA
        * RtemsSignalReqCatch_Pre_Preempt_NA
        * RtemsSignalReqCatch_Pre_Timeslice_NA
        * RtemsSignalReqCatch_Pre_ASR_NA
        * RtemsSignalReqCatch_Pre_IntLvl_NA;
    }

    for (
      ctx->pcs[ 1 ] = RtemsSignalReqCatch_Pre_Handler_Invalid;
      ctx->pcs[ 1 ] < RtemsSignalReqCatch_Pre_Handler_NA;
      ++ctx->pcs[ 1 ]
    ) {
      if ( RtemsSignalReqCatch_TransitionInfo[ index ].Pre_Handler_NA ) {
        ctx->pcs[ 1 ] = RtemsSignalReqCatch_Pre_Handler_NA;
        index += ( RtemsSignalReqCatch_Pre_Handler_NA - 1 )
          * RtemsSignalReqCatch_Pre_Preempt_NA
          * RtemsSignalReqCatch_Pre_Timeslice_NA
          * RtemsSignalReqCatch_Pre_ASR_NA
          * RtemsSignalReqCatch_Pre_IntLvl_NA;
      }

      for (
        ctx->pcs[ 2 ] = RtemsSignalReqCatch_Pre_Preempt_Yes;
        ctx->pcs[ 2 ] < RtemsSignalReqCatch_Pre_Preempt_NA;
        ++ctx->pcs[ 2 ]
      ) {
        if ( RtemsSignalReqCatch_TransitionInfo[ index ].Pre_Preempt_NA ) {
          ctx->pcs[ 2 ] = RtemsSignalReqCatch_Pre_Preempt_NA;
          index += ( RtemsSignalReqCatch_Pre_Preempt_NA - 1 )
            * RtemsSignalReqCatch_Pre_Timeslice_NA
            * RtemsSignalReqCatch_Pre_ASR_NA
            * RtemsSignalReqCatch_Pre_IntLvl_NA;
        }

        for (
          ctx->pcs[ 3 ] = RtemsSignalReqCatch_Pre_Timeslice_Yes;
          ctx->pcs[ 3 ] < RtemsSignalReqCatch_Pre_Timeslice_NA;
          ++ctx->pcs[ 3 ]
        ) {
          if ( RtemsSignalReqCatch_TransitionInfo[ index ].Pre_Timeslice_NA ) {
            ctx->pcs[ 3 ] = RtemsSignalReqCatch_Pre_Timeslice_NA;
            index += ( RtemsSignalReqCatch_Pre_Timeslice_NA - 1 )
              * RtemsSignalReqCatch_Pre_ASR_NA
              * RtemsSignalReqCatch_Pre_IntLvl_NA;
          }

          for (
            ctx->pcs[ 4 ] = RtemsSignalReqCatch_Pre_ASR_Yes;
            ctx->pcs[ 4 ] < RtemsSignalReqCatch_Pre_ASR_NA;
            ++ctx->pcs[ 4 ]
          ) {
            if ( RtemsSignalReqCatch_TransitionInfo[ index ].Pre_ASR_NA ) {
              ctx->pcs[ 4 ] = RtemsSignalReqCatch_Pre_ASR_NA;
              index += ( RtemsSignalReqCatch_Pre_ASR_NA - 1 )
                * RtemsSignalReqCatch_Pre_IntLvl_NA;
            }

            for (
              ctx->pcs[ 5 ] = RtemsSignalReqCatch_Pre_IntLvl_Zero;
              ctx->pcs[ 5 ] < RtemsSignalReqCatch_Pre_IntLvl_NA;
              ++ctx->pcs[ 5 ]
            ) {
              if ( RtemsSignalReqCatch_TransitionInfo[ index ].Pre_IntLvl_NA ) {
                ctx->pcs[ 5 ] = RtemsSignalReqCatch_Pre_IntLvl_NA;
                index += ( RtemsSignalReqCatch_Pre_IntLvl_NA - 1 );
              }

              if ( RtemsSignalReqCatch_TransitionInfo[ index ].Skip ) {
                ++index;
                continue;
              }

              RtemsSignalReqCatch_Prepare( ctx );
              RtemsSignalReqCatch_Pre_Pending_Prepare( ctx, ctx->pcs[ 0 ] );
              RtemsSignalReqCatch_Pre_Handler_Prepare( ctx, ctx->pcs[ 1 ] );
              RtemsSignalReqCatch_Pre_Preempt_Prepare( ctx, ctx->pcs[ 2 ] );
              RtemsSignalReqCatch_Pre_Timeslice_Prepare( ctx, ctx->pcs[ 3 ] );
              RtemsSignalReqCatch_Pre_ASR_Prepare( ctx, ctx->pcs[ 4 ] );
              RtemsSignalReqCatch_Pre_IntLvl_Prepare( ctx, ctx->pcs[ 5 ] );
              RtemsSignalReqCatch_Action( ctx );
              RtemsSignalReqCatch_Post_Status_Check(
                ctx,
                RtemsSignalReqCatch_TransitionMap[ index ][ 0 ]
              );
              RtemsSignalReqCatch_Post_Send_Check(
                ctx,
                RtemsSignalReqCatch_TransitionMap[ index ][ 1 ]
              );
              RtemsSignalReqCatch_Post_Preempt_Check(
                ctx,
                RtemsSignalReqCatch_TransitionMap[ index ][ 2 ]
              );
              RtemsSignalReqCatch_Post_Timeslice_Check(
                ctx,
                RtemsSignalReqCatch_TransitionMap[ index ][ 3 ]
              );
              RtemsSignalReqCatch_Post_ASR_Check(
                ctx,
                RtemsSignalReqCatch_TransitionMap[ index ][ 4 ]
              );
              RtemsSignalReqCatch_Post_IntLvl_Check(
                ctx,
                RtemsSignalReqCatch_TransitionMap[ index ][ 5 ]
              );
              ++index;
            }
          }
        }
      }
    }
  }
}

/** @} */
