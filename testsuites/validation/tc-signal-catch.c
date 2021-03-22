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

#include "tc-support.h"

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
  RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
  RtemsSignalReqCatch_Post_Status_NotImplIntLvlSMP,
  RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
  RtemsSignalReqCatch_Post_Status_NA
} RtemsSignalReqCatch_Post_Status;

typedef enum {
  RtemsSignalReqCatch_Post_ASRInfo_NopIntLvl,
  RtemsSignalReqCatch_Post_ASRInfo_NopIntLvlSMP,
  RtemsSignalReqCatch_Post_ASRInfo_NopNoPreempt,
  RtemsSignalReqCatch_Post_ASRInfo_New,
  RtemsSignalReqCatch_Post_ASRInfo_Inactive,
  RtemsSignalReqCatch_Post_ASRInfo_NA
} RtemsSignalReqCatch_Post_ASRInfo;

/**
 * @brief Test context for spec:/rtems/signal/req/catch test case.
 */
typedef struct {
  /**
   * @brief This member contains the object identifier of the runner task.
   */
  rtems_id runner_id;

  /**
   * @brief This member contains the object identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief null If this member is non-zero, then rtems_signal_catch() is
   *   called with pending signals, otherwise it is called with no pending
   *   signals.
   */
  uint32_t pending_signals;

  /**
   * @brief This member provides a barrier to synchronize the runner and worker
   *   tasks.
   */
  SMP_barrier_Control barrier;

  /**
   * @brief This member is used for barrier operations done by the runner task.
   */
  SMP_barrier_State runner_barrier_state;

  /**
   * @brief When the default handler is called, this member is incremented.
   */
  uint32_t default_handler_calls;

  /**
   * @brief When the handler is called, this member is incremented.
   */
  uint32_t handler_calls;

  /**
   * @brief This member contains the mode observed in the last handler call.
   */
  rtems_mode handler_mode;

  /**
   * @brief This member specifies the normal task mode for the action.
   */
  rtems_mode normal_mode;

  /**
   * @brief This member specifies the handler for the action.
   */
  rtems_asr_entry handler;

  /**
   * @brief This member specifies the task mode for the action.
   */
  rtems_mode mode;

  /**
   * @brief This member contains the return status of the rtems_signal_catch()
   *   call of the action.
   */
  rtems_status_code catch_status;

  /**
   * @brief This member contains the return status of the rtems_signal_send()
   *   call of the action.
   */
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

static void CheckNoASRChange( Context *ctx )
{
  T_rsc_success( ctx->send_status );
  T_eq_u32( ctx->default_handler_calls, 1 + ctx->pending_signals );
  T_eq_u32( ctx->handler_calls, 0 );
  T_eq_u32( ctx->handler_mode, 0xffffffff );
}

static void CheckNewASRSettings( Context *ctx )
{
  T_rsc_success( ctx->send_status );
  T_eq_u32( ctx->default_handler_calls, 0 );
  T_eq_u32( ctx->handler_calls, 1 + ctx->pending_signals );
  T_eq_u32( ctx->handler_mode, ctx->mode );
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
       * Where the system has more than one processor, while the calling task
       * has pending signals.
       *
       * Where the system has exactly one processor, while the calling task has
       * no pending signals.
       */
      if ( rtems_scheduler_get_processor_maximum() > 1 ) {
        ctx->pending_signals = 1;
      } else {
        ctx->pending_signals = 0;
      }
      break;
    }

    case RtemsSignalReqCatch_Pre_Pending_No: {
      /*
       * While the calling task has no pending signals.
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
       * While the ``asr_handler`` parameter is NULL.
       */
      ctx->handler = NULL;
      break;
    }

    case RtemsSignalReqCatch_Pre_Handler_Valid: {
      /*
       * While the ``asr_handler`` parameter is a valid ASR handler.
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
       * While the ``mode_set`` parameter specifies that preemption is enabled.
       */
      if ( rtems_configuration_get_maximum_processors() == 1 ) {
        ctx->normal_mode |= RTEMS_NO_PREEMPT;
      }
      break;
    }

    case RtemsSignalReqCatch_Pre_Preempt_No: {
      /*
       * While the ``mode_set`` parameter specifies that preemption is
       * disabled.
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
       * While the ``mode_set`` parameter specifies that timeslicing is
       * enabled.
       */
      ctx->mode |= RTEMS_TIMESLICE;
      break;
    }

    case RtemsSignalReqCatch_Pre_Timeslice_No: {
      /*
       * While the ``mode_set`` parameter specifies that timeslicing is
       * disabled.
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
       * While the ``mode_set`` parameter specifies that ASR processing is
       * enabled.
       */
      /*
       * We cannot disable ASR processing at normal task level for this state.
       */
      break;
    }

    case RtemsSignalReqCatch_Pre_ASR_No: {
      /*
       * While the ``mode_set`` parameter specifies that ASR processing is
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
       * While the ``mode_set`` parameter specifies an interrupt level of zero.
       */
      #if CPU_ENABLE_ROBUST_THREAD_DISPATCH == FALSE && !defined(RTEMS_SMP)
      ctx->normal_mode |= RTEMS_INTERRUPT_LEVEL( 1 );
      #endif
      break;
    }

    case RtemsSignalReqCatch_Pre_IntLvl_Positive: {
      /*
       * While the ``mode_set`` parameter specifies an interrupt level greater
       * than or equal to one and less than or equal to
       * CPU_MODES_INTERRUPT_MASK.
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

    case RtemsSignalReqCatch_Post_Status_NotImplIntLvl: {
      /*
       * The return status of rtems_signal_catch() shall be
       * RTEMS_NOT_IMPLEMENTED.
       */
      T_rsc( ctx->catch_status, RTEMS_NOT_IMPLEMENTED );
      break;
    }

    case RtemsSignalReqCatch_Post_Status_NotImplIntLvlSMP: {
      /*
       * Where the system needs inter-processor interrupts, the return status
       * of rtems_signal_catch() shall be RTEMS_NOT_IMPLEMENTED.
       *
       * Where the system does not need inter-processor interrupts, the return
       * status of rtems_signal_catch() shall be RTEMS_SUCCESSFUL.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_rsc( ctx->catch_status, RTEMS_NOT_IMPLEMENTED );
      } else {
        T_rsc_success( ctx->catch_status );
      }
      break;
    }

    case RtemsSignalReqCatch_Post_Status_NotImplNoPreempt: {
      /*
       * Where the scheduler does not support the no-preempt mode, the return
       * status of rtems_signal_catch() shall be RTEMS_NOT_IMPLEMENTED.
       *
       * Where the scheduler does support the no-preempt mode, the return
       * status of rtems_signal_catch() shall be RTEMS_SUCCESSFUL.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_rsc( ctx->catch_status, RTEMS_NOT_IMPLEMENTED );
      } else {
        T_rsc_success( ctx->catch_status );
      }
      break;
    }

    case RtemsSignalReqCatch_Post_Status_NA:
      break;
  }
}

static void RtemsSignalReqCatch_Post_ASRInfo_Check(
  RtemsSignalReqCatch_Context     *ctx,
  RtemsSignalReqCatch_Post_ASRInfo state
)
{
  switch ( state ) {
    case RtemsSignalReqCatch_Post_ASRInfo_NopIntLvl: {
      /*
       * The ASR information of the caller of rtems_signal_catch() shall not be
       * changed by the rtems_signal_catch() call.
       */
      CheckNoASRChange( ctx );
      break;
    }

    case RtemsSignalReqCatch_Post_ASRInfo_NopIntLvlSMP: {
      /*
       * Where the system needs inter-processor interrupts, the ASR information
       * of the caller of rtems_signal_catch() shall not be changed by the
       * rtems_signal_catch() call.
       *
       * Where the system does not need inter-processor interrupts, the ASR
       * processing for the caller of rtems_signal_catch() shall be done using
       * the handler specified by ``asr_handler`` in the mode specified by
       * ``mode_set``.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        CheckNoASRChange( ctx );
      } else {
        CheckNewASRSettings( ctx );
      }
      break;
    }

    case RtemsSignalReqCatch_Post_ASRInfo_NopNoPreempt: {
      /*
       * Where the scheduler does not support the no-preempt mode, the ASR
       * information of the caller of rtems_signal_catch() shall not be changed
       * by the rtems_signal_catch() call.
       *
       * Where the scheduler does support the no-preempt mode, the ASR
       * processing for the caller of rtems_signal_catch() shall be done using
       * the handler specified by ``asr_handler`` in the mode specified by
       * ``mode_set``.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        CheckNoASRChange( ctx );
      } else {
        CheckNewASRSettings( ctx );
      }
      break;
    }

    case RtemsSignalReqCatch_Post_ASRInfo_New: {
      /*
       * The ASR processing for the caller of rtems_signal_catch() shall be
       * done using the handler specified by ``asr_handler`` in the mode
       * specified by ``mode_set``.
       */
      CheckNewASRSettings( ctx );
      break;
    }

    case RtemsSignalReqCatch_Post_ASRInfo_Inactive: {
      /*
       * The ASR processing for the caller of rtems_signal_catch() shall be
       * deactivated.
       *
       * The pending signals of the caller of rtems_signal_catch() shall be
       * cleared.
       */
      T_rsc( ctx->send_status, RTEMS_NOT_DEFINED );
      T_eq_u32( ctx->default_handler_calls, 0 );
      T_eq_u32( ctx->handler_calls, 0 );
      T_eq_u32( ctx->handler_mode, 0xffffffff );
      break;
    }

    case RtemsSignalReqCatch_Post_ASRInfo_NA:
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

    ctx->worker_id = CreateTask( "WORK", 1 );

    sc = rtems_scheduler_ident_by_processor( 1, &scheduler_id );
    T_assert_rsc_success( sc );

    sc = rtems_task_set_scheduler( ctx->worker_id, scheduler_id, 1 );
    T_assert_rsc_success( sc );

    StartTask( ctx->worker_id, Worker, ctx );
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
  DeleteTask( ctx->worker_id );
  RestoreRunnerASR();
}

static void RtemsSignalReqCatch_Teardown_Wrap( void *arg )
{
  RtemsSignalReqCatch_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsSignalReqCatch_Teardown( ctx );
}

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

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Pending_NA : 1;
  uint16_t Pre_Handler_NA : 1;
  uint16_t Pre_Preempt_NA : 1;
  uint16_t Pre_Timeslice_NA : 1;
  uint16_t Pre_ASR_NA : 1;
  uint16_t Pre_IntLvl_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_ASRInfo : 3;
} RtemsSignalReqCatch_Entry;

static const RtemsSignalReqCatch_Entry
RtemsSignalReqCatch_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_ASRInfo_Inactive },
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 0, 0, 0, 0, 0, 0, 0, RtemsSignalReqCatch_Post_Status_NotImplIntLvl,
    RtemsSignalReqCatch_Post_ASRInfo_NopIntLvl },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, RtemsSignalReqCatch_Post_Status_NotImplIntLvlSMP,
    RtemsSignalReqCatch_Post_ASRInfo_NopIntLvlSMP },
#else
  { 0, 0, 0, 0, 0, 0, 0, RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_ASRInfo_New },
#endif
  { 0, 0, 0, 0, 0, 0, 0, RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_ASRInfo_New },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, RtemsSignalReqCatch_Post_Status_NotImplNoPreempt,
    RtemsSignalReqCatch_Post_ASRInfo_NopNoPreempt }
#else
  { 0, 0, 0, 0, 0, 0, 0, RtemsSignalReqCatch_Post_Status_Ok,
    RtemsSignalReqCatch_Post_ASRInfo_New }
#endif
};

static const uint8_t
RtemsSignalReqCatch_Map[] = {
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1, 2, 1, 2, 1, 3, 1,
  3, 1, 3, 1, 3, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 1,
  2, 1, 2, 1, 3, 1, 3, 1, 3, 1, 3, 1
};

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

static inline RtemsSignalReqCatch_Entry RtemsSignalReqCatch_GetEntry(
  size_t index
)
{
  return RtemsSignalReqCatch_Entries[
    RtemsSignalReqCatch_Map[ index ]
  ];
}

/**
 * @fn void T_case_body_RtemsSignalReqCatch( void )
 */
T_TEST_CASE_FIXTURE( RtemsSignalReqCatch, &RtemsSignalReqCatch_Fixture )
{
  RtemsSignalReqCatch_Context *ctx;
  RtemsSignalReqCatch_Entry entry;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsSignalReqCatch_Pre_Pending_Yes;
    ctx->pcs[ 0 ] < RtemsSignalReqCatch_Pre_Pending_NA;
    ++ctx->pcs[ 0 ]
  ) {
    entry = RtemsSignalReqCatch_GetEntry( index );

    if ( entry.Pre_Pending_NA ) {
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
      entry = RtemsSignalReqCatch_GetEntry( index );

      if ( entry.Pre_Handler_NA ) {
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
        entry = RtemsSignalReqCatch_GetEntry( index );

        if ( entry.Pre_Preempt_NA ) {
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
          entry = RtemsSignalReqCatch_GetEntry( index );

          if ( entry.Pre_Timeslice_NA ) {
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
            entry = RtemsSignalReqCatch_GetEntry( index );

            if ( entry.Pre_ASR_NA ) {
              ctx->pcs[ 4 ] = RtemsSignalReqCatch_Pre_ASR_NA;
              index += ( RtemsSignalReqCatch_Pre_ASR_NA - 1 )
                * RtemsSignalReqCatch_Pre_IntLvl_NA;
            }

            for (
              ctx->pcs[ 5 ] = RtemsSignalReqCatch_Pre_IntLvl_Zero;
              ctx->pcs[ 5 ] < RtemsSignalReqCatch_Pre_IntLvl_NA;
              ++ctx->pcs[ 5 ]
            ) {
              entry = RtemsSignalReqCatch_GetEntry( index );

              if ( entry.Pre_IntLvl_NA ) {
                ctx->pcs[ 5 ] = RtemsSignalReqCatch_Pre_IntLvl_NA;
                index += ( RtemsSignalReqCatch_Pre_IntLvl_NA - 1 );
              }

              if ( entry.Skip ) {
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
              RtemsSignalReqCatch_Post_Status_Check( ctx, entry.Post_Status );
              RtemsSignalReqCatch_Post_ASRInfo_Check(
                ctx,
                entry.Post_ASRInfo
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
