/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqMode
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
#include <string.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqMode spec:/rtems/task/req/mode
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqMode_Pre_PrevMode_Valid,
  RtemsTaskReqMode_Pre_PrevMode_Null,
  RtemsTaskReqMode_Pre_PrevMode_NA
} RtemsTaskReqMode_Pre_PrevMode;

typedef enum {
  RtemsTaskReqMode_Pre_PreemptCur_Yes,
  RtemsTaskReqMode_Pre_PreemptCur_No,
  RtemsTaskReqMode_Pre_PreemptCur_NA
} RtemsTaskReqMode_Pre_PreemptCur;

typedef enum {
  RtemsTaskReqMode_Pre_TimesliceCur_Yes,
  RtemsTaskReqMode_Pre_TimesliceCur_No,
  RtemsTaskReqMode_Pre_TimesliceCur_NA
} RtemsTaskReqMode_Pre_TimesliceCur;

typedef enum {
  RtemsTaskReqMode_Pre_ASRCur_Yes,
  RtemsTaskReqMode_Pre_ASRCur_No,
  RtemsTaskReqMode_Pre_ASRCur_NA
} RtemsTaskReqMode_Pre_ASRCur;

typedef enum {
  RtemsTaskReqMode_Pre_IntLvlCur_Zero,
  RtemsTaskReqMode_Pre_IntLvlCur_Positive,
  RtemsTaskReqMode_Pre_IntLvlCur_NA
} RtemsTaskReqMode_Pre_IntLvlCur;

typedef enum {
  RtemsTaskReqMode_Pre_Preempt_Yes,
  RtemsTaskReqMode_Pre_Preempt_No,
  RtemsTaskReqMode_Pre_Preempt_NA
} RtemsTaskReqMode_Pre_Preempt;

typedef enum {
  RtemsTaskReqMode_Pre_Timeslice_Yes,
  RtemsTaskReqMode_Pre_Timeslice_No,
  RtemsTaskReqMode_Pre_Timeslice_NA
} RtemsTaskReqMode_Pre_Timeslice;

typedef enum {
  RtemsTaskReqMode_Pre_ASR_Yes,
  RtemsTaskReqMode_Pre_ASR_No,
  RtemsTaskReqMode_Pre_ASR_NA
} RtemsTaskReqMode_Pre_ASR;

typedef enum {
  RtemsTaskReqMode_Pre_IntLvl_Zero,
  RtemsTaskReqMode_Pre_IntLvl_Positive,
  RtemsTaskReqMode_Pre_IntLvl_NA
} RtemsTaskReqMode_Pre_IntLvl;

typedef enum {
  RtemsTaskReqMode_Pre_PreemptMsk_Yes,
  RtemsTaskReqMode_Pre_PreemptMsk_No,
  RtemsTaskReqMode_Pre_PreemptMsk_NA
} RtemsTaskReqMode_Pre_PreemptMsk;

typedef enum {
  RtemsTaskReqMode_Pre_TimesliceMsk_Yes,
  RtemsTaskReqMode_Pre_TimesliceMsk_No,
  RtemsTaskReqMode_Pre_TimesliceMsk_NA
} RtemsTaskReqMode_Pre_TimesliceMsk;

typedef enum {
  RtemsTaskReqMode_Pre_ASRMsk_Yes,
  RtemsTaskReqMode_Pre_ASRMsk_No,
  RtemsTaskReqMode_Pre_ASRMsk_NA
} RtemsTaskReqMode_Pre_ASRMsk;

typedef enum {
  RtemsTaskReqMode_Pre_IntLvlMsk_Yes,
  RtemsTaskReqMode_Pre_IntLvlMsk_No,
  RtemsTaskReqMode_Pre_IntLvlMsk_NA
} RtemsTaskReqMode_Pre_IntLvlMsk;

typedef enum {
  RtemsTaskReqMode_Post_Status_Ok,
  RtemsTaskReqMode_Post_Status_InvAddr,
  RtemsTaskReqMode_Post_Status_NotImplIntLvl,
  RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
  RtemsTaskReqMode_Post_Status_NotImplNoPreempt,
  RtemsTaskReqMode_Post_Status_NA
} RtemsTaskReqMode_Post_Status;

typedef enum {
  RtemsTaskReqMode_Post_Preempt_Yes,
  RtemsTaskReqMode_Post_Preempt_No,
  RtemsTaskReqMode_Post_Preempt_Maybe,
  RtemsTaskReqMode_Post_Preempt_NA
} RtemsTaskReqMode_Post_Preempt;

typedef enum {
  RtemsTaskReqMode_Post_ASR_Yes,
  RtemsTaskReqMode_Post_ASR_No,
  RtemsTaskReqMode_Post_ASR_Maybe,
  RtemsTaskReqMode_Post_ASR_NA
} RtemsTaskReqMode_Post_ASR;

typedef enum {
  RtemsTaskReqMode_Post_PMVar_Set,
  RtemsTaskReqMode_Post_PMVar_Nop,
  RtemsTaskReqMode_Post_PMVar_Maybe,
  RtemsTaskReqMode_Post_PMVar_NA
} RtemsTaskReqMode_Post_PMVar;

typedef enum {
  RtemsTaskReqMode_Post_Mode_Set,
  RtemsTaskReqMode_Post_Mode_Nop,
  RtemsTaskReqMode_Post_Mode_Maybe,
  RtemsTaskReqMode_Post_Mode_NA
} RtemsTaskReqMode_Post_Mode;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_PrevMode_NA : 1;
  uint32_t Pre_PreemptCur_NA : 1;
  uint32_t Pre_TimesliceCur_NA : 1;
  uint32_t Pre_ASRCur_NA : 1;
  uint32_t Pre_IntLvlCur_NA : 1;
  uint32_t Pre_Preempt_NA : 1;
  uint32_t Pre_Timeslice_NA : 1;
  uint32_t Pre_ASR_NA : 1;
  uint32_t Pre_IntLvl_NA : 1;
  uint32_t Pre_PreemptMsk_NA : 1;
  uint32_t Pre_TimesliceMsk_NA : 1;
  uint32_t Pre_ASRMsk_NA : 1;
  uint32_t Pre_IntLvlMsk_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_Preempt : 2;
  uint32_t Post_ASR : 2;
  uint32_t Post_PMVar : 2;
  uint32_t Post_Mode : 2;
} RtemsTaskReqMode_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/mode test case.
 */
typedef struct {
  /**
   * @brief This member contains the object identifier of the worker task.
   */
  rtems_id worker_id;

  /**
   * @brief null If this member is contains the initial mode of the runner.
   */
  rtems_mode runner_mode;

  /**
   * @brief This member provides a value for the previous mode set.
   */
  rtems_mode previous_mode_set_value;

  /**
   * @brief This member specifies the task mode in which rtems_task_mode() is
   *   called.
   */
  rtems_mode current_mode;

  /**
   * @brief This member counts worker activity.
   */
  uint32_t worker_counter;

  /**
   * @brief This member contains worker counter before the rtems_task_mode()
   *   call.
   */
  uint32_t worker_counter_before;

  /**
   * @brief This member contains worker counter after the rtems_task_mode()
   *   call.
   */
  uint32_t worker_counter_after;

  /**
   * @brief This member counts signal handler activity.
   */
  uint32_t signal_counter;

  /**
   * @brief This member contains signal counter before the rtems_task_mode()
   *   call.
   */
  uint32_t signal_counter_before;

  /**
   * @brief This member contains signal counter after the rtems_task_mode()
   *   call.
   */
  uint32_t signal_counter_after;

  /**
   * @brief This member specifies the ``mode_set`` parameter for
   *   rtems_task_mode().
   */
  rtems_mode mode_set;

  /**
   * @brief This member specifies the mode mask ``mask`` parameter for
   *   rtems_task_mode() for the action.
   */
  rtems_mode mode_mask;

  /**
   * @brief This member specifies the previous mode set ``previous_mode_set``
   *   parameter for rtems_task_mode().
   */
  rtems_mode *previous_mode_set;

  /**
   * @brief This member contains the return status of the rtems_task_mode()
   *   call.
   */
  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 13 ];

    /**
     * @brief If this member is true, then the test action loop is executed.
     */
    bool in_action_loop;

    /**
     * @brief This member contains the next transition map index.
     */
    size_t index;

    /**
     * @brief This member contains the current transition map entry.
     */
    RtemsTaskReqMode_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqMode_Context;

static RtemsTaskReqMode_Context
  RtemsTaskReqMode_Instance;

static const char * const RtemsTaskReqMode_PreDesc_PrevMode[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_PreemptCur[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_TimesliceCur[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_ASRCur[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_IntLvlCur[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_Preempt[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_Timeslice[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_ASR[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_IntLvl[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_PreemptMsk[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_TimesliceMsk[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_ASRMsk[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqMode_PreDesc_IntLvlMsk[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsTaskReqMode_PreDesc[] = {
  RtemsTaskReqMode_PreDesc_PrevMode,
  RtemsTaskReqMode_PreDesc_PreemptCur,
  RtemsTaskReqMode_PreDesc_TimesliceCur,
  RtemsTaskReqMode_PreDesc_ASRCur,
  RtemsTaskReqMode_PreDesc_IntLvlCur,
  RtemsTaskReqMode_PreDesc_Preempt,
  RtemsTaskReqMode_PreDesc_Timeslice,
  RtemsTaskReqMode_PreDesc_ASR,
  RtemsTaskReqMode_PreDesc_IntLvl,
  RtemsTaskReqMode_PreDesc_PreemptMsk,
  RtemsTaskReqMode_PreDesc_TimesliceMsk,
  RtemsTaskReqMode_PreDesc_ASRMsk,
  RtemsTaskReqMode_PreDesc_IntLvlMsk,
  NULL
};

#define INVALID_MODE 0xffffffff

#define EVENT_MAKE_READY RTEMS_EVENT_0

#define EVENT_TIMESLICE RTEMS_EVENT_1

typedef RtemsTaskReqMode_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_event_set events;

    events = ReceiveAnyEvents();

    if ( ( events & EVENT_TIMESLICE ) != 0 ) {
      SetSelfPriority( PRIO_NORMAL );
      SetSelfPriority( PRIO_HIGH );
    }

    ++ctx->worker_counter;
  }
}

static void SignalHandler( rtems_signal_set signal_set )
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->signal_counter;
  T_eq_u32( signal_set, 0xdeadbeef );
}

static void ExhaustTimeslice( void )
{
  uint32_t ticks;

  for (
    ticks = 0;
    ticks < rtems_configuration_get_ticks_per_timeslice();
    ++ticks
  ) {
    ClockTick();
  }
}

static void CheckMode(
  Context   *ctx,
  rtems_mode mode,
  rtems_mode mask,
  rtems_mode set
)
{
  rtems_status_code sc;
  uint32_t          counter;

  mode &= ~mask;
  mode |= set & mask;

  counter = ctx->worker_counter;
  SendEvents( ctx->worker_id, EVENT_MAKE_READY );

  if ( ( mode & RTEMS_PREEMPT_MASK ) == RTEMS_PREEMPT ) {
    T_eq_u32( ctx->worker_counter, counter + 1 );
  } else {
    T_eq_u32( ctx->worker_counter, counter );
  }

  counter = ctx->worker_counter;
  SendEvents( ctx->worker_id, EVENT_TIMESLICE );
  ExhaustTimeslice();

  if ( ( mode & RTEMS_PREEMPT_MASK ) == RTEMS_PREEMPT ) {
    if ( ( mode & RTEMS_TIMESLICE_MASK ) == RTEMS_TIMESLICE ) {
      T_eq_u32( ctx->worker_counter, counter + 1 );
    } else {
      T_eq_u32( ctx->worker_counter, counter );
    }
  } else {
    T_eq_u32( ctx->worker_counter, counter );
  }

  counter = ctx->signal_counter;
  sc = rtems_signal_send( RTEMS_SELF, 0xdeadbeef );
  T_rsc_success( sc );

  if ( ( mode & RTEMS_ASR_MASK ) == RTEMS_ASR ) {
    T_eq_u32( ctx->signal_counter, counter + 1 );
  } else {
    T_eq_u32( ctx->signal_counter, counter );
  }

  T_eq_u32( mode & RTEMS_INTERRUPT_MASK, _ISR_Get_level() );
}

static void RtemsTaskReqMode_Pre_PrevMode_Prepare(
  RtemsTaskReqMode_Context     *ctx,
  RtemsTaskReqMode_Pre_PrevMode state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_PrevMode_Valid: {
      /*
       * While the ``previous_mode_set`` parameter references an object of type
       * rtems_mode.
       */
      ctx->previous_mode_set = &ctx->previous_mode_set_value;
      break;
    }

    case RtemsTaskReqMode_Pre_PrevMode_Null: {
      /*
       * While the ``previous_mode_set`` parameter is NULL.
       */
      ctx->previous_mode_set = NULL;
      break;
    }

    case RtemsTaskReqMode_Pre_PrevMode_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_PreemptCur_Prepare(
  RtemsTaskReqMode_Context       *ctx,
  RtemsTaskReqMode_Pre_PreemptCur state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_PreemptCur_Yes: {
      /*
       * While the calling task has preemption enabled.
       */
      ctx->current_mode |= RTEMS_PREEMPT;
      break;
    }

    case RtemsTaskReqMode_Pre_PreemptCur_No: {
      /*
       * Where the scheduler does not support the no-preempt mode, while the
       * calling task has preemption enabled.
       *
       * Where the scheduler does support the no-preempt mode, while the
       * calling task has preemption disabled.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        ctx->current_mode |= RTEMS_PREEMPT;
      } else {
        ctx->current_mode |= RTEMS_NO_PREEMPT;
      }
      break;
    }

    case RtemsTaskReqMode_Pre_PreemptCur_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_TimesliceCur_Prepare(
  RtemsTaskReqMode_Context         *ctx,
  RtemsTaskReqMode_Pre_TimesliceCur state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_TimesliceCur_Yes: {
      /*
       * While the calling task has timeslicing enabled.
       */
      ctx->current_mode |= RTEMS_TIMESLICE;
      break;
    }

    case RtemsTaskReqMode_Pre_TimesliceCur_No: {
      /*
       * While the calling task has timeslicing disabled.
       */
      ctx->current_mode |= RTEMS_NO_TIMESLICE;
      break;
    }

    case RtemsTaskReqMode_Pre_TimesliceCur_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_ASRCur_Prepare(
  RtemsTaskReqMode_Context   *ctx,
  RtemsTaskReqMode_Pre_ASRCur state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_ASRCur_Yes: {
      /*
       * While the calling task has ASR processing enabled.
       */
      ctx->current_mode |= RTEMS_ASR;
      break;
    }

    case RtemsTaskReqMode_Pre_ASRCur_No: {
      /*
       * While the calling task has ASR processing disabled.
       */
      ctx->current_mode |= RTEMS_NO_ASR;
      break;
    }

    case RtemsTaskReqMode_Pre_ASRCur_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_IntLvlCur_Prepare(
  RtemsTaskReqMode_Context      *ctx,
  RtemsTaskReqMode_Pre_IntLvlCur state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_IntLvlCur_Zero: {
      /*
       * While the calling task executes with an interrupt level of zero.
       */
      ctx->current_mode |= RTEMS_INTERRUPT_LEVEL( 0 );
      break;
    }

    case RtemsTaskReqMode_Pre_IntLvlCur_Positive: {
      /*
       * Where the system needs inter-processor interrupts, while the calling
       * task executes with an interrupt level of zero.
       *
       * Where the system does not need inter-processor interrupts, while the
       * calling task executes with an an interrupt level greater than zero and
       * less than or equal to CPU_MODES_INTERRUPT_MASK.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        ctx->current_mode |= RTEMS_INTERRUPT_LEVEL( 0 );
      } else {
        ctx->current_mode |= RTEMS_INTERRUPT_LEVEL( 1 );
      }
      break;
    }

    case RtemsTaskReqMode_Pre_IntLvlCur_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_Preempt_Prepare(
  RtemsTaskReqMode_Context    *ctx,
  RtemsTaskReqMode_Pre_Preempt state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_Preempt_Yes: {
      /*
       * While the ``mode_set`` parameter specifies that preemption is enabled.
       */
      ctx->mode_set |= RTEMS_PREEMPT;
      break;
    }

    case RtemsTaskReqMode_Pre_Preempt_No: {
      /*
       * While the ``mode_set`` parameter specifies that preemption is
       * disabled.
       */
      ctx->mode_set |= RTEMS_NO_PREEMPT;
      break;
    }

    case RtemsTaskReqMode_Pre_Preempt_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_Timeslice_Prepare(
  RtemsTaskReqMode_Context      *ctx,
  RtemsTaskReqMode_Pre_Timeslice state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_Timeslice_Yes: {
      /*
       * While the ``mode_set`` parameter specifies that timeslicing is
       * enabled.
       */
      ctx->mode_set |= RTEMS_TIMESLICE;
      break;
    }

    case RtemsTaskReqMode_Pre_Timeslice_No: {
      /*
       * While the ``mode_set`` parameter specifies that timeslicing is
       * disabled.
       */
      ctx->mode_set |= RTEMS_NO_TIMESLICE;
      break;
    }

    case RtemsTaskReqMode_Pre_Timeslice_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_ASR_Prepare(
  RtemsTaskReqMode_Context *ctx,
  RtemsTaskReqMode_Pre_ASR  state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_ASR_Yes: {
      /*
       * While the ``mode_set`` parameter specifies that ASR processing is
       * enabled.
       */
      ctx->mode_set |= RTEMS_ASR;
      break;
    }

    case RtemsTaskReqMode_Pre_ASR_No: {
      /*
       * While the ``mode_set`` parameter specifies that ASR processing is
       * disabled.
       */
      ctx->mode_set |= RTEMS_NO_ASR;
      break;
    }

    case RtemsTaskReqMode_Pre_ASR_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_IntLvl_Prepare(
  RtemsTaskReqMode_Context   *ctx,
  RtemsTaskReqMode_Pre_IntLvl state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_IntLvl_Zero: {
      /*
       * While the ``mode_set`` parameter specifies an interrupt level of zero.
       */
      ctx->mode_set |= RTEMS_INTERRUPT_LEVEL( 0 );
      break;
    }

    case RtemsTaskReqMode_Pre_IntLvl_Positive: {
      /*
       * While the ``mode_set`` parameter specifies an interrupt level greater
       * than zero and less than or equal to CPU_MODES_INTERRUPT_MASK.
       */
      ctx->mode_set |= RTEMS_INTERRUPT_LEVEL( 1 );
      break;
    }

    case RtemsTaskReqMode_Pre_IntLvl_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_PreemptMsk_Prepare(
  RtemsTaskReqMode_Context       *ctx,
  RtemsTaskReqMode_Pre_PreemptMsk state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_PreemptMsk_Yes: {
      /*
       * While the ``mask`` parameter specifies that the preemption mode shall
       * be set.
       */
      ctx->mode_mask |= RTEMS_PREEMPT_MASK;
      break;
    }

    case RtemsTaskReqMode_Pre_PreemptMsk_No: {
      /*
       * While the ``mask`` parameter specifies that the preemption mode shall
       * not be set.
       */
      /* This is the default mode mask */
      break;
    }

    case RtemsTaskReqMode_Pre_PreemptMsk_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_TimesliceMsk_Prepare(
  RtemsTaskReqMode_Context         *ctx,
  RtemsTaskReqMode_Pre_TimesliceMsk state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_TimesliceMsk_Yes: {
      /*
       * While the ``mask`` parameter specifies that the timeslicing mode shall
       * be set.
       */
      ctx->mode_mask |= RTEMS_TIMESLICE_MASK;
      break;
    }

    case RtemsTaskReqMode_Pre_TimesliceMsk_No: {
      /*
       * While the ``mask`` parameter specifies that the timeslicing mode shall
       * not be set.
       */
      /* This is the default mode mask */
      break;
    }

    case RtemsTaskReqMode_Pre_TimesliceMsk_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_ASRMsk_Prepare(
  RtemsTaskReqMode_Context   *ctx,
  RtemsTaskReqMode_Pre_ASRMsk state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_ASRMsk_Yes: {
      /*
       * While the ``mask`` parameter specifies that the ASR processing mode
       * shall be set.
       */
      ctx->mode_mask |= RTEMS_ASR_MASK;
      break;
    }

    case RtemsTaskReqMode_Pre_ASRMsk_No: {
      /*
       * While the ``mask`` parameter specifies that the ASR processing mode
       * shall not be set.
       */
      /* This is the default mode mask */
      break;
    }

    case RtemsTaskReqMode_Pre_ASRMsk_NA:
      break;
  }
}

static void RtemsTaskReqMode_Pre_IntLvlMsk_Prepare(
  RtemsTaskReqMode_Context      *ctx,
  RtemsTaskReqMode_Pre_IntLvlMsk state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Pre_IntLvlMsk_Yes: {
      /*
       * While the ``mask`` parameter specifies that the interrupt level shall
       * be set.
       */
      ctx->mode_mask |= RTEMS_INTERRUPT_MASK;
      break;
    }

    case RtemsTaskReqMode_Pre_IntLvlMsk_No: {
      /*
       * While the ``mask`` parameter specifies that the interrupt level shall
       * not be set.
       */
      /* This is the default mode mask */
      break;
    }

    case RtemsTaskReqMode_Pre_IntLvlMsk_NA:
      break;
  }
}

static void RtemsTaskReqMode_Post_Status_Check(
  RtemsTaskReqMode_Context    *ctx,
  RtemsTaskReqMode_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Post_Status_Ok: {
      /*
       * The return status of rtems_task_mode() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqMode_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_mode() shall be RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqMode_Post_Status_NotImplIntLvl: {
      /*
       * The return status of rtems_task_mode() shall be RTEMS_NOT_IMPLEMENTED.
       */
      T_rsc( ctx->status, RTEMS_NOT_IMPLEMENTED );
      break;
    }

    case RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP: {
      /*
       * Where the system needs inter-processor interrupts, the return status
       * of rtems_task_mode() shall be RTEMS_NOT_IMPLEMENTED.
       *
       * Where the system does not need inter-processor interrupts, the return
       * status of rtems_task_mode() shall be RTEMS_SUCCESSFUL.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_rsc( ctx->status, RTEMS_NOT_IMPLEMENTED );
      } else {
        T_rsc_success( ctx->status );
      }
      break;
    }

    case RtemsTaskReqMode_Post_Status_NotImplNoPreempt: {
      /*
       * Where the scheduler does not support the no-preempt mode, the return
       * status of rtems_task_mode() shall be RTEMS_NOT_IMPLEMENTED.
       *
       * Where the scheduler does support the no-preempt mode, the return
       * status of rtems_task_mode() shall be RTEMS_SUCCESSFUL.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_rsc( ctx->status, RTEMS_NOT_IMPLEMENTED );
      } else {
        T_rsc_success( ctx->status );
      }
      break;
    }

    case RtemsTaskReqMode_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqMode_Post_Preempt_Check(
  RtemsTaskReqMode_Context     *ctx,
  RtemsTaskReqMode_Post_Preempt state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Post_Preempt_Yes: {
      /*
       * The calling task shall be preempted by a higher priority ready task
       * during the rtems_task_mode() call.
       */
      T_eq_u32( ctx->worker_counter_after, ctx->worker_counter_before + 1 );
      break;
    }

    case RtemsTaskReqMode_Post_Preempt_No: {
      /*
       * The calling task shall not be preempted during the rtems_task_mode()
       * call.
       */
      T_eq_u32( ctx->worker_counter_after, ctx->worker_counter_before );
      break;
    }

    case RtemsTaskReqMode_Post_Preempt_Maybe: {
      /*
       * Where the scheduler does not support the no-preempt mode, the calling
       * task shall not be preempted during the rtems_task_mode() call.
       *
       * Where the scheduler does support the no-preempt mode, the calling task
       * shall be preempted by a higher priority ready task during the
       * rtems_task_mode() call.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_eq_u32( ctx->worker_counter_after, ctx->worker_counter_before );
      } else {
        T_eq_u32( ctx->worker_counter_after, ctx->worker_counter_before + 1 );
      }
      break;
    }

    case RtemsTaskReqMode_Post_Preempt_NA:
      break;
  }
}

static void RtemsTaskReqMode_Post_ASR_Check(
  RtemsTaskReqMode_Context *ctx,
  RtemsTaskReqMode_Post_ASR state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Post_ASR_Yes: {
      /*
       * The calling task shall process pending signals during the
       * rtems_task_mode() call.
       */
      T_eq_u32( ctx->signal_counter_after, ctx->signal_counter_before + 1 );
      break;
    }

    case RtemsTaskReqMode_Post_ASR_No: {
      /*
       * The calling task shall not process signals during the
       * rtems_task_mode() call.
       */
      T_eq_u32( ctx->signal_counter_after, ctx->signal_counter_before );
      break;
    }

    case RtemsTaskReqMode_Post_ASR_Maybe: {
      /*
       * Where the scheduler does not support the no-preempt mode, the calling
       * task shall not process signals during the rtems_task_mode() call.
       *
       * Where the scheduler does support the no-preempt mode, the calling task
       * shall process pending signals during the rtems_task_mode() call.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_eq_u32( ctx->signal_counter_after, ctx->signal_counter_before );
      } else {
        T_eq_u32( ctx->signal_counter_after, ctx->signal_counter_before + 1 );
      }
      break;
    }

    case RtemsTaskReqMode_Post_ASR_NA:
      break;
  }
}

static void RtemsTaskReqMode_Post_PMVar_Check(
  RtemsTaskReqMode_Context   *ctx,
  RtemsTaskReqMode_Post_PMVar state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Post_PMVar_Set: {
      /*
       * The value of the object referenced by the ``previous_mode_set``
       * parameter shall be set to the task modes of the calling task on entry
       * of the call to rtems_task_mode().
       */
      T_eq_ptr( ctx->previous_mode_set, &ctx->previous_mode_set_value );
      T_eq_u32( ctx->previous_mode_set_value, ctx->current_mode );
      break;
    }

    case RtemsTaskReqMode_Post_PMVar_Nop: {
      /*
       * Objects referenced by the ``stack_size`` parameter in past calls to
       * rtems_task_mode() shall not be accessed by the rtems_task_mode() call.
       */
      T_eq_u32( ctx->previous_mode_set_value, INVALID_MODE );
      break;
    }

    case RtemsTaskReqMode_Post_PMVar_Maybe: {
      /*
       * Where the scheduler does not support the no-preempt mode, objects
       * referenced by the ``stack_size`` parameter in past calls to
       * rtems_task_mode() shall not be accessed by the rtems_task_mode() call.
       *
       * Where the scheduler does support the no-preempt mode, the value of the
       * object referenced by the ``previous_mode_set`` parameter shall be set
       * to the task modes of the calling task on entry of the call to
       * rtems_task_mode().
       */
      T_eq_ptr( ctx->previous_mode_set, &ctx->previous_mode_set_value );

      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        T_eq_u32( ctx->previous_mode_set_value, INVALID_MODE );
      } else {
        T_eq_u32( ctx->previous_mode_set_value, ctx->current_mode );
      }
      break;
    }

    case RtemsTaskReqMode_Post_PMVar_NA:
      break;
  }
}

static void RtemsTaskReqMode_Post_Mode_Check(
  RtemsTaskReqMode_Context  *ctx,
  RtemsTaskReqMode_Post_Mode state
)
{
  switch ( state ) {
    case RtemsTaskReqMode_Post_Mode_Set: {
      /*
       * The task modes of the calling task indicated by the ``mask`` parameter
       * shall be set to the corrsponding modes specified by the ``mode_set``
       * parameter.
       */
      CheckMode( ctx, ctx->current_mode, ctx->mode_mask, ctx->mode_set );
      break;
    }

    case RtemsTaskReqMode_Post_Mode_Nop: {
      /*
       * The task modes of the calling task shall not be modified by the
       * rtems_task_mode() call.
       */
      CheckMode( ctx, ctx->current_mode, 0, 0 );
      break;
    }

    case RtemsTaskReqMode_Post_Mode_Maybe: {
      /*
       * Where the scheduler does not support the no-preempt mode, the task
       * modes of the calling task shall not be modified by the
       * rtems_task_mode() call.
       *
       * Where the scheduler does support the no-preempt mode, the task modes
       * of the calling task indicated by the ``mask`` parameter shall be set
       * to the corrsponding modes specified by the ``mode_set`` parameter.
       */
      if ( rtems_configuration_get_maximum_processors() > 1 ) {
        CheckMode( ctx, ctx->current_mode, 0, 0 );
      } else {
        CheckMode( ctx, ctx->current_mode, ctx->mode_mask, ctx->mode_set );
      }
      break;
    }

    case RtemsTaskReqMode_Post_Mode_NA:
      break;
  }
}

static void RtemsTaskReqMode_Setup( RtemsTaskReqMode_Context *ctx )
{
  rtems_status_code sc;

  memset( ctx, 0, sizeof( *ctx ) );

  sc = rtems_task_mode(
    RTEMS_DEFAULT_MODES,
    RTEMS_CURRENT_MODE,
    &ctx->runner_mode
  );
  T_rsc_success( sc );

  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsTaskReqMode_Setup_Wrap( void *arg )
{
  RtemsTaskReqMode_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqMode_Setup( ctx );
}

static void RtemsTaskReqMode_Teardown( RtemsTaskReqMode_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerMode();
  RestoreRunnerPriority();
}

static void RtemsTaskReqMode_Teardown_Wrap( void *arg )
{
  RtemsTaskReqMode_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqMode_Teardown( ctx );
}

static void RtemsTaskReqMode_Prepare( RtemsTaskReqMode_Context *ctx )
{
  ctx->current_mode = RTEMS_DEFAULT_MODES;
  ctx->mode_set = RTEMS_DEFAULT_MODES;
  ctx->mode_mask = RTEMS_CURRENT_MODE;
  ctx->previous_mode_set_value = INVALID_MODE;
}

static void RtemsTaskReqMode_Action( RtemsTaskReqMode_Context *ctx )
{
  rtems_status_code sc;
  rtems_mode        mode;

  sc = rtems_task_mode( ctx->current_mode, RTEMS_ALL_MODE_MASKS, &mode );
  T_rsc_success( sc );

  SendEvents( ctx->worker_id, EVENT_MAKE_READY );

  sc = rtems_signal_catch( SignalHandler, ctx->current_mode | RTEMS_NO_ASR );
  T_rsc_success( sc );

  sc = rtems_signal_send( RTEMS_SELF, 0xdeadbeef );
  T_rsc_success( sc );

  ctx->worker_counter_before = ctx->worker_counter;
  ctx->signal_counter_before = ctx->signal_counter;
  ctx->status = rtems_task_mode(
    ctx->mode_set,
    ctx->mode_mask,
    ctx->previous_mode_set
  );
  ctx->worker_counter_after = ctx->worker_counter;
  ctx->signal_counter_after = ctx->signal_counter;
}

static void RtemsTaskReqMode_Cleanup( RtemsTaskReqMode_Context *ctx )
{
  rtems_status_code sc;
  rtems_mode        mode;

  sc = rtems_task_mode( RTEMS_DEFAULT_MODES, RTEMS_ALL_MODE_MASKS, &mode );
  T_rsc_success( sc );

  sc = rtems_task_wake_after( RTEMS_YIELD_PROCESSOR );
  T_rsc_success( sc );

  sc = rtems_signal_catch( NULL, RTEMS_DEFAULT_MODES );
  T_rsc_success( sc );
}

static const RtemsTaskReqMode_Entry
RtemsTaskReqMode_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_InvAddr, RtemsTaskReqMode_Post_Preempt_No,
    RtemsTaskReqMode_Post_ASR_No, RtemsTaskReqMode_Post_PMVar_Nop,
    RtemsTaskReqMode_Post_Mode_Nop },
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_InvAddr, RtemsTaskReqMode_Post_Preempt_No,
    RtemsTaskReqMode_Post_ASR_No, RtemsTaskReqMode_Post_PMVar_Nop,
    RtemsTaskReqMode_Post_Mode_Nop },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplNoPreempt,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplNoPreempt,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvl,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Nop, RtemsTaskReqMode_Post_Mode_Nop },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Maybe, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Yes, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Maybe, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Yes, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvl,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Nop, RtemsTaskReqMode_Post_Mode_Nop },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplNoPreempt,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplNoPreempt,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Maybe,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvl,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Nop, RtemsTaskReqMode_Post_Mode_Nop },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
    RtemsTaskReqMode_Post_Preempt_Maybe, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Yes, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
    RtemsTaskReqMode_Post_Preempt_Maybe, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Yes, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplNoPreempt,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Maybe,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvl,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Nop, RtemsTaskReqMode_Post_Mode_Nop },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Maybe,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Maybe,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Maybe, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Yes, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Maybe, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Yes, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvl,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Nop, RtemsTaskReqMode_Post_Mode_Nop },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplNoPreempt,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Maybe,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvl,
    RtemsTaskReqMode_Post_Preempt_No, RtemsTaskReqMode_Post_ASR_No,
    RtemsTaskReqMode_Post_PMVar_Nop, RtemsTaskReqMode_Post_Mode_Nop },
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
    RtemsTaskReqMode_Post_Preempt_Maybe, RtemsTaskReqMode_Post_ASR_Maybe,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe },
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Yes, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set },
#endif
#if CPU_ENABLE_ROBUST_THREAD_DISPATCH == TRUE
  { 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_NA,
    RtemsTaskReqMode_Post_Preempt_NA, RtemsTaskReqMode_Post_ASR_NA,
    RtemsTaskReqMode_Post_PMVar_NA, RtemsTaskReqMode_Post_Mode_NA }
#elif defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    RtemsTaskReqMode_Post_Status_NotImplIntLvlSMP,
    RtemsTaskReqMode_Post_Preempt_Maybe, RtemsTaskReqMode_Post_ASR_Maybe,
    RtemsTaskReqMode_Post_PMVar_Maybe, RtemsTaskReqMode_Post_Mode_Maybe }
#else
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, RtemsTaskReqMode_Post_Status_Ok,
    RtemsTaskReqMode_Post_Preempt_Yes, RtemsTaskReqMode_Post_ASR_Yes,
    RtemsTaskReqMode_Post_PMVar_Set, RtemsTaskReqMode_Post_Mode_Set }
#endif
};

static const uint8_t
RtemsTaskReqMode_Map[] = {
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2,
  6, 2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 6, 2,
  6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5,
  5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2,
  6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12,
  5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2,
  2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5,
  5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2,
  6, 2, 6, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 7, 3, 7, 3,
  7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3,
  7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
  7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4,
  4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3,
  3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4,
  4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3,
  10, 10, 2, 2, 10, 10, 2, 2, 10, 10, 2, 2, 10, 10, 2, 2, 17, 10, 6, 2, 17, 10,
  6, 2, 17, 10, 6, 2, 17, 10, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 10, 10, 2, 2, 10, 10,
  2, 2, 10, 10, 2, 2, 10, 10, 2, 2, 17, 10, 6, 2, 17, 10, 6, 2, 17, 10, 6, 2,
  17, 10, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 6, 2, 6,
  2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 16, 16, 5, 5, 16, 16, 5, 5, 10, 10, 2, 2,
  10, 10, 2, 2, 21, 16, 12, 5, 21, 16, 12, 5, 17, 10, 6, 2, 17, 10, 6, 2, 5, 5,
  5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2,
  6, 2, 6, 2, 6, 2, 16, 16, 5, 5, 16, 16, 5, 5, 10, 10, 2, 2, 10, 10, 2, 2, 21,
  16, 12, 5, 21, 16, 12, 5, 17, 10, 6, 2, 17, 10, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5,
  2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2,
  11, 11, 3, 3, 11, 11, 3, 3, 11, 11, 3, 3, 11, 11, 3, 3, 18, 11, 7, 3, 18, 11,
  7, 3, 18, 11, 7, 3, 18, 11, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 11, 11, 3, 3, 11, 11,
  3, 3, 11, 11, 3, 3, 11, 11, 3, 3, 18, 11, 7, 3, 18, 11, 7, 3, 18, 11, 7, 3,
  18, 11, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 7, 3, 7,
  3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 13, 13, 4, 4, 13, 13, 4, 4, 11, 11, 3, 3,
  11, 11, 3, 3, 13, 13, 4, 4, 13, 13, 4, 4, 18, 11, 7, 3, 18, 11, 7, 3, 4, 4,
  4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3,
  7, 3, 7, 3, 13, 13, 4, 4, 13, 13, 4, 4, 11, 11, 3, 3, 11, 11, 3, 3, 13, 13,
  4, 4, 13, 13, 4, 4, 18, 11, 7, 3, 18, 11, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3,
  3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 2, 2, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2,
  6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 6, 2, 6, 2, 6, 2,
  6, 2, 6, 2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2,
  6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  2, 2, 2, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5,
  5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2,
  6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5,
  12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2,
  2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5, 5,
  5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6,
  2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7,
  3, 7, 3, 7, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 7,
  3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 3, 3, 3, 3, 3, 3, 3,
  3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 4,
  4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7,
  3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4,
  4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3,
  3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3,
  3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 10, 10,
  2, 2, 10, 10, 2, 2, 10, 10, 2, 2, 10, 10, 2, 2, 17, 10, 6, 2, 17, 10, 6, 2,
  17, 10, 6, 2, 17, 10, 6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
  6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 6, 2, 10, 10, 2, 2, 10, 10, 2, 2,
  10, 10, 2, 2, 10, 10, 2, 2, 17, 10, 6, 2, 17, 10, 6, 2, 17, 10, 6, 2, 17, 10,
  6, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 6, 2, 6, 2, 6, 2, 6, 2,
  6, 2, 6, 2, 6, 2, 6, 2, 16, 16, 5, 5, 16, 16, 5, 5, 10, 10, 2, 2, 10, 10, 2,
  2, 21, 16, 12, 5, 21, 16, 12, 5, 17, 10, 6, 2, 17, 10, 6, 2, 5, 5, 5, 5, 5,
  5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6,
  2, 6, 2, 16, 16, 5, 5, 16, 16, 5, 5, 10, 10, 2, 2, 10, 10, 2, 2, 21, 16, 12,
  5, 21, 16, 12, 5, 17, 10, 6, 2, 17, 10, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2,
  2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 11, 11,
  3, 3, 11, 11, 3, 3, 11, 11, 3, 3, 11, 11, 3, 3, 18, 11, 7, 3, 18, 11, 7, 3,
  18, 11, 7, 3, 18, 11, 7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 7, 3, 11, 11, 3, 3, 11, 11, 3, 3,
  11, 11, 3, 3, 11, 11, 3, 3, 18, 11, 7, 3, 18, 11, 7, 3, 18, 11, 7, 3, 18, 11,
  7, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 7, 3, 7, 3, 7, 3, 7, 3,
  7, 3, 7, 3, 7, 3, 7, 3, 13, 13, 4, 4, 13, 13, 4, 4, 11, 11, 3, 3, 11, 11, 3,
  3, 13, 13, 4, 4, 13, 13, 4, 4, 18, 11, 7, 3, 18, 11, 7, 3, 4, 4, 4, 4, 4, 4,
  4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3,
  13, 13, 4, 4, 13, 13, 4, 4, 11, 11, 3, 3, 11, 11, 3, 3, 13, 13, 4, 4, 13, 13,
  4, 4, 18, 11, 7, 3, 18, 11, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3,
  3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 8, 8, 8, 8, 8, 8, 8, 8,
  2, 2, 2, 2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2, 6, 2,
  8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8,
  6, 2, 6, 2, 6, 2, 6, 2, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 14,
  8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2, 6, 2, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2,
  2, 2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5,
  5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2,
  6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12,
  5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2,
  2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5,
  5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2,
  6, 2, 6, 2, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15,
  9, 15, 9, 7, 3, 7, 3, 7, 3, 7, 3, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3,
  3, 3, 15, 9, 15, 9, 15, 9, 15, 9, 7, 3, 7, 3, 7, 3, 7, 3, 9, 9, 9, 9, 9, 9,
  9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9, 15, 9, 7, 3, 7, 3, 7, 3,
  7, 3, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9,
  15, 9, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3,
  3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3,
  3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4,
  4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7,
  3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4,
  4, 7, 3, 7, 3, 7, 3, 7, 3, 19, 19, 8, 8, 19, 19, 8, 8, 10, 10, 2, 2, 10, 10,
  2, 2, 22, 19, 14, 8, 22, 19, 14, 8, 17, 10, 6, 2, 17, 10, 6, 2, 8, 8, 8, 8,
  8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2,
  6, 2, 6, 2, 19, 19, 8, 8, 19, 19, 8, 8, 10, 10, 2, 2, 10, 10, 2, 2, 22, 19,
  14, 8, 22, 19, 14, 8, 17, 10, 6, 2, 17, 10, 6, 2, 8, 8, 8, 8, 8, 8, 8, 8, 2,
  2, 2, 2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2, 6, 2, 16,
  16, 5, 5, 16, 16, 5, 5, 10, 10, 2, 2, 10, 10, 2, 2, 21, 16, 12, 5, 21, 16,
  12, 5, 17, 10, 6, 2, 17, 10, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2,
  2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 16, 16, 5, 5, 16,
  16, 5, 5, 10, 10, 2, 2, 10, 10, 2, 2, 21, 16, 12, 5, 21, 16, 12, 5, 17, 10,
  6, 2, 17, 10, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5,
  12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 20, 20, 9, 9, 20, 20, 9, 9, 11,
  11, 3, 3, 11, 11, 3, 3, 23, 20, 15, 9, 23, 20, 15, 9, 18, 11, 7, 3, 18, 11,
  7, 3, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9,
  15, 9, 7, 3, 7, 3, 7, 3, 7, 3, 20, 20, 9, 9, 20, 20, 9, 9, 11, 11, 3, 3, 11,
  11, 3, 3, 23, 20, 15, 9, 23, 20, 15, 9, 18, 11, 7, 3, 18, 11, 7, 3, 9, 9, 9,
  9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9, 15, 9, 7, 3, 7,
  3, 7, 3, 7, 3, 13, 13, 4, 4, 13, 13, 4, 4, 11, 11, 3, 3, 11, 11, 3, 3, 13,
  13, 4, 4, 13, 13, 4, 4, 18, 11, 7, 3, 18, 11, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4,
  3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 13,
  13, 4, 4, 13, 13, 4, 4, 11, 11, 3, 3, 11, 11, 3, 3, 13, 13, 4, 4, 13, 13, 4,
  4, 18, 11, 7, 3, 18, 11, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3,
  3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 8, 8, 8, 8, 8, 8, 8, 8, 2,
  2, 2, 2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2, 6, 2, 8,
  8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6,
  2, 6, 2, 6, 2, 6, 2, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 14, 8,
  14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2, 6, 2, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2,
  2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5,
  5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6,
  2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5,
  12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2,
  2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 5, 5, 5, 5, 5,
  5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6,
  2, 6, 2, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9,
  15, 9, 7, 3, 7, 3, 7, 3, 7, 3, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3,
  3, 15, 9, 15, 9, 15, 9, 15, 9, 7, 3, 7, 3, 7, 3, 7, 3, 9, 9, 9, 9, 9, 9, 9,
  9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9, 15, 9, 7, 3, 7, 3, 7, 3, 7,
  3, 9, 9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9, 15,
  9, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4,
  4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3,
  3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 4, 4, 4, 4, 4,
  4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7,
  3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7,
  3, 7, 3, 7, 3, 7, 3, 19, 19, 8, 8, 19, 19, 8, 8, 10, 10, 2, 2, 10, 10, 2, 2,
  22, 19, 14, 8, 22, 19, 14, 8, 17, 10, 6, 2, 17, 10, 6, 2, 8, 8, 8, 8, 8, 8,
  8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2,
  6, 2, 19, 19, 8, 8, 19, 19, 8, 8, 10, 10, 2, 2, 10, 10, 2, 2, 22, 19, 14, 8,
  22, 19, 14, 8, 17, 10, 6, 2, 17, 10, 6, 2, 8, 8, 8, 8, 8, 8, 8, 8, 2, 2, 2,
  2, 2, 2, 2, 2, 14, 8, 14, 8, 14, 8, 14, 8, 6, 2, 6, 2, 6, 2, 6, 2, 16, 16, 5,
  5, 16, 16, 5, 5, 10, 10, 2, 2, 10, 10, 2, 2, 21, 16, 12, 5, 21, 16, 12, 5,
  17, 10, 6, 2, 17, 10, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2,
  12, 5, 12, 5, 12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 16, 16, 5, 5, 16, 16, 5,
  5, 10, 10, 2, 2, 10, 10, 2, 2, 21, 16, 12, 5, 21, 16, 12, 5, 17, 10, 6, 2,
  17, 10, 6, 2, 5, 5, 5, 5, 5, 5, 5, 5, 2, 2, 2, 2, 2, 2, 2, 2, 12, 5, 12, 5,
  12, 5, 12, 5, 6, 2, 6, 2, 6, 2, 6, 2, 20, 20, 9, 9, 20, 20, 9, 9, 11, 11, 3,
  3, 11, 11, 3, 3, 23, 20, 15, 9, 23, 20, 15, 9, 18, 11, 7, 3, 18, 11, 7, 3, 9,
  9, 9, 9, 9, 9, 9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9, 15, 9, 7,
  3, 7, 3, 7, 3, 7, 3, 20, 20, 9, 9, 20, 20, 9, 9, 11, 11, 3, 3, 11, 11, 3, 3,
  23, 20, 15, 9, 23, 20, 15, 9, 18, 11, 7, 3, 18, 11, 7, 3, 9, 9, 9, 9, 9, 9,
  9, 9, 3, 3, 3, 3, 3, 3, 3, 3, 15, 9, 15, 9, 15, 9, 15, 9, 7, 3, 7, 3, 7, 3,
  7, 3, 13, 13, 4, 4, 13, 13, 4, 4, 11, 11, 3, 3, 11, 11, 3, 3, 13, 13, 4, 4,
  13, 13, 4, 4, 18, 11, 7, 3, 18, 11, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3,
  3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 13, 13, 4, 4, 13,
  13, 4, 4, 11, 11, 3, 3, 11, 11, 3, 3, 13, 13, 4, 4, 13, 13, 4, 4, 18, 11, 7,
  3, 18, 11, 7, 3, 4, 4, 4, 4, 4, 4, 4, 4, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4,
  4, 4, 4, 4, 7, 3, 7, 3, 7, 3, 7, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static size_t RtemsTaskReqMode_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqMode_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqMode_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqMode_Fixture = {
  .setup = RtemsTaskReqMode_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqMode_Teardown_Wrap,
  .scope = RtemsTaskReqMode_Scope,
  .initial_context = &RtemsTaskReqMode_Instance
};

static inline RtemsTaskReqMode_Entry RtemsTaskReqMode_PopEntry(
  RtemsTaskReqMode_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqMode_Entries[
    RtemsTaskReqMode_Map[ index ]
  ];
}

static void RtemsTaskReqMode_TestVariant( RtemsTaskReqMode_Context *ctx )
{
  RtemsTaskReqMode_Pre_PrevMode_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqMode_Pre_PreemptCur_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqMode_Pre_TimesliceCur_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqMode_Pre_ASRCur_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqMode_Pre_IntLvlCur_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTaskReqMode_Pre_Preempt_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsTaskReqMode_Pre_Timeslice_Prepare( ctx, ctx->Map.pcs[ 6 ] );
  RtemsTaskReqMode_Pre_ASR_Prepare( ctx, ctx->Map.pcs[ 7 ] );
  RtemsTaskReqMode_Pre_IntLvl_Prepare( ctx, ctx->Map.pcs[ 8 ] );
  RtemsTaskReqMode_Pre_PreemptMsk_Prepare( ctx, ctx->Map.pcs[ 9 ] );
  RtemsTaskReqMode_Pre_TimesliceMsk_Prepare( ctx, ctx->Map.pcs[ 10 ] );
  RtemsTaskReqMode_Pre_ASRMsk_Prepare( ctx, ctx->Map.pcs[ 11 ] );
  RtemsTaskReqMode_Pre_IntLvlMsk_Prepare( ctx, ctx->Map.pcs[ 12 ] );
  RtemsTaskReqMode_Action( ctx );
  RtemsTaskReqMode_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqMode_Post_Preempt_Check( ctx, ctx->Map.entry.Post_Preempt );
  RtemsTaskReqMode_Post_ASR_Check( ctx, ctx->Map.entry.Post_ASR );
  RtemsTaskReqMode_Post_PMVar_Check( ctx, ctx->Map.entry.Post_PMVar );
  RtemsTaskReqMode_Post_Mode_Check( ctx, ctx->Map.entry.Post_Mode );
}

/**
 * @fn void T_case_body_RtemsTaskReqMode( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqMode, &RtemsTaskReqMode_Fixture )
{
  RtemsTaskReqMode_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTaskReqMode_Pre_PrevMode_Valid;
    ctx->Map.pcs[ 0 ] < RtemsTaskReqMode_Pre_PrevMode_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTaskReqMode_Pre_PreemptCur_Yes;
      ctx->Map.pcs[ 1 ] < RtemsTaskReqMode_Pre_PreemptCur_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTaskReqMode_Pre_TimesliceCur_Yes;
        ctx->Map.pcs[ 2 ] < RtemsTaskReqMode_Pre_TimesliceCur_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsTaskReqMode_Pre_ASRCur_Yes;
          ctx->Map.pcs[ 3 ] < RtemsTaskReqMode_Pre_ASRCur_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsTaskReqMode_Pre_IntLvlCur_Zero;
            ctx->Map.pcs[ 4 ] < RtemsTaskReqMode_Pre_IntLvlCur_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsTaskReqMode_Pre_Preempt_Yes;
              ctx->Map.pcs[ 5 ] < RtemsTaskReqMode_Pre_Preempt_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              for (
                ctx->Map.pcs[ 6 ] = RtemsTaskReqMode_Pre_Timeslice_Yes;
                ctx->Map.pcs[ 6 ] < RtemsTaskReqMode_Pre_Timeslice_NA;
                ++ctx->Map.pcs[ 6 ]
              ) {
                for (
                  ctx->Map.pcs[ 7 ] = RtemsTaskReqMode_Pre_ASR_Yes;
                  ctx->Map.pcs[ 7 ] < RtemsTaskReqMode_Pre_ASR_NA;
                  ++ctx->Map.pcs[ 7 ]
                ) {
                  for (
                    ctx->Map.pcs[ 8 ] = RtemsTaskReqMode_Pre_IntLvl_Zero;
                    ctx->Map.pcs[ 8 ] < RtemsTaskReqMode_Pre_IntLvl_NA;
                    ++ctx->Map.pcs[ 8 ]
                  ) {
                    for (
                      ctx->Map.pcs[ 9 ] = RtemsTaskReqMode_Pre_PreemptMsk_Yes;
                      ctx->Map.pcs[ 9 ] < RtemsTaskReqMode_Pre_PreemptMsk_NA;
                      ++ctx->Map.pcs[ 9 ]
                    ) {
                      for (
                        ctx->Map.pcs[ 10 ] = RtemsTaskReqMode_Pre_TimesliceMsk_Yes;
                        ctx->Map.pcs[ 10 ] < RtemsTaskReqMode_Pre_TimesliceMsk_NA;
                        ++ctx->Map.pcs[ 10 ]
                      ) {
                        for (
                          ctx->Map.pcs[ 11 ] = RtemsTaskReqMode_Pre_ASRMsk_Yes;
                          ctx->Map.pcs[ 11 ] < RtemsTaskReqMode_Pre_ASRMsk_NA;
                          ++ctx->Map.pcs[ 11 ]
                        ) {
                          for (
                            ctx->Map.pcs[ 12 ] = RtemsTaskReqMode_Pre_IntLvlMsk_Yes;
                            ctx->Map.pcs[ 12 ] < RtemsTaskReqMode_Pre_IntLvlMsk_NA;
                            ++ctx->Map.pcs[ 12 ]
                          ) {
                            ctx->Map.entry = RtemsTaskReqMode_PopEntry( ctx );

                            if ( ctx->Map.entry.Skip ) {
                              continue;
                            }

                            RtemsTaskReqMode_Prepare( ctx );
                            RtemsTaskReqMode_TestVariant( ctx );
                            RtemsTaskReqMode_Cleanup( ctx );
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }
}

/** @} */
