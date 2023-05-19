/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRatemonReqGetStatus
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
#include <rtems/cpuuse.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsRatemonReqGetStatus spec:/rtems/ratemon/req/get-status
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsRatemonReqGetStatus_Pre_StatusAddr_Valid,
  RtemsRatemonReqGetStatus_Pre_StatusAddr_Null,
  RtemsRatemonReqGetStatus_Pre_StatusAddr_NA
} RtemsRatemonReqGetStatus_Pre_StatusAddr;

typedef enum {
  RtemsRatemonReqGetStatus_Pre_Id_Valid,
  RtemsRatemonReqGetStatus_Pre_Id_Invalid,
  RtemsRatemonReqGetStatus_Pre_Id_NA
} RtemsRatemonReqGetStatus_Pre_Id;

typedef enum {
  RtemsRatemonReqGetStatus_Pre_State_Inactive,
  RtemsRatemonReqGetStatus_Pre_State_Active,
  RtemsRatemonReqGetStatus_Pre_State_Expired,
  RtemsRatemonReqGetStatus_Pre_State_NA
} RtemsRatemonReqGetStatus_Pre_State;

typedef enum {
  RtemsRatemonReqGetStatus_Pre_Elapsed_Time,
  RtemsRatemonReqGetStatus_Pre_Elapsed_NA
} RtemsRatemonReqGetStatus_Pre_Elapsed;

typedef enum {
  RtemsRatemonReqGetStatus_Pre_Consumed_CpuTime,
  RtemsRatemonReqGetStatus_Pre_Consumed_NA
} RtemsRatemonReqGetStatus_Pre_Consumed;

typedef enum {
  RtemsRatemonReqGetStatus_Pre_Postponed_Zero,
  RtemsRatemonReqGetStatus_Pre_Postponed_One,
  RtemsRatemonReqGetStatus_Pre_Postponed_Several,
  RtemsRatemonReqGetStatus_Pre_Postponed_NA
} RtemsRatemonReqGetStatus_Pre_Postponed;

typedef enum {
  RtemsRatemonReqGetStatus_Post_Status_Ok,
  RtemsRatemonReqGetStatus_Post_Status_InvAddr,
  RtemsRatemonReqGetStatus_Post_Status_InvId,
  RtemsRatemonReqGetStatus_Post_Status_NA
} RtemsRatemonReqGetStatus_Post_Status;

typedef enum {
  RtemsRatemonReqGetStatus_Post_Owner_OwnerTask,
  RtemsRatemonReqGetStatus_Post_Owner_Nop,
  RtemsRatemonReqGetStatus_Post_Owner_NA
} RtemsRatemonReqGetStatus_Post_Owner;

typedef enum {
  RtemsRatemonReqGetStatus_Post_State_Inactive,
  RtemsRatemonReqGetStatus_Post_State_Active,
  RtemsRatemonReqGetStatus_Post_State_Expired,
  RtemsRatemonReqGetStatus_Post_State_Nop,
  RtemsRatemonReqGetStatus_Post_State_NA
} RtemsRatemonReqGetStatus_Post_State;

typedef enum {
  RtemsRatemonReqGetStatus_Post_Elapsed_Time,
  RtemsRatemonReqGetStatus_Post_Elapsed_Zero,
  RtemsRatemonReqGetStatus_Post_Elapsed_Nop,
  RtemsRatemonReqGetStatus_Post_Elapsed_NA
} RtemsRatemonReqGetStatus_Post_Elapsed;

typedef enum {
  RtemsRatemonReqGetStatus_Post_Consumed_CpuTime,
  RtemsRatemonReqGetStatus_Post_Consumed_Zero,
  RtemsRatemonReqGetStatus_Post_Consumed_Nop,
  RtemsRatemonReqGetStatus_Post_Consumed_NA
} RtemsRatemonReqGetStatus_Post_Consumed;

typedef enum {
  RtemsRatemonReqGetStatus_Post_Postponed_Zero,
  RtemsRatemonReqGetStatus_Post_Postponed_One,
  RtemsRatemonReqGetStatus_Post_Postponed_Several,
  RtemsRatemonReqGetStatus_Post_Postponed_Nop,
  RtemsRatemonReqGetStatus_Post_Postponed_NA
} RtemsRatemonReqGetStatus_Post_Postponed;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_StatusAddr_NA : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_State_NA : 1;
  uint32_t Pre_Elapsed_NA : 1;
  uint32_t Pre_Consumed_NA : 1;
  uint32_t Pre_Postponed_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Owner : 2;
  uint32_t Post_State : 3;
  uint32_t Post_Elapsed : 2;
  uint32_t Post_Consumed : 2;
  uint32_t Post_Postponed : 3;
} RtemsRatemonReqGetStatus_Entry;

/**
 * @brief Test context for spec:/rtems/ratemon/req/get-status test case.
 */
typedef struct {
  /**
   * @brief This member contains a valid identifier of a period.
   */
  rtems_id period_id;

  /**
   * @brief This member contains the previous timecounter handler to restore.
   */
  GetTimecountHandler previous_timecounter_handler;

  /**
   * @brief This member is used to receive the
   *   rtems_rate_monotonic_period_status from the action.
   */
  rtems_rate_monotonic_period_status period_status;

  /**
   * @brief This member specifies the ``id`` parameter for the action.
   */
  rtems_id id_param;

  /**
   * @brief This member specifies the ``status`` parameter for the action.
   */
  rtems_rate_monotonic_period_status *status_param;

  /**
   * @brief This member contains the returned status code of the action.
   */
  rtems_status_code status;

  /**
   * @brief This member contains the task identifier of the owner task.
   */
  rtems_id task_id;

  /**
   * @brief This member contains the state before the action.
   */
  rtems_rate_monotonic_period_states previous_state;

  /**
   * @brief If the rtems_cpu_usage_reset() directive should be called before
   *   rtems_rate_monotonic_get_status(), this member contains a pointer to it.
   */
  void (*do_reset)( void );

  /**
   * @brief This member contains the CLOCK_MONOTONIC time elapsed.
   */
  struct timespec elapsed;

  /**
   * @brief This member contains the CPU time consumed by the owner task.
   */
  struct timespec consumed;

  /**
   * @brief This member contains the number of postponed jobs.
   */
  uint32_t postponed_jobs_count;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    RtemsRatemonReqGetStatus_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsRatemonReqGetStatus_Context;

static RtemsRatemonReqGetStatus_Context
  RtemsRatemonReqGetStatus_Instance;

static const char * const RtemsRatemonReqGetStatus_PreDesc_StatusAddr[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsRatemonReqGetStatus_PreDesc_Id[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsRatemonReqGetStatus_PreDesc_State[] = {
  "Inactive",
  "Active",
  "Expired",
  "NA"
};

static const char * const RtemsRatemonReqGetStatus_PreDesc_Elapsed[] = {
  "Time",
  "NA"
};

static const char * const RtemsRatemonReqGetStatus_PreDesc_Consumed[] = {
  "CpuTime",
  "NA"
};

static const char * const RtemsRatemonReqGetStatus_PreDesc_Postponed[] = {
  "Zero",
  "One",
  "Several",
  "NA"
};

static const char * const * const RtemsRatemonReqGetStatus_PreDesc[] = {
  RtemsRatemonReqGetStatus_PreDesc_StatusAddr,
  RtemsRatemonReqGetStatus_PreDesc_Id,
  RtemsRatemonReqGetStatus_PreDesc_State,
  RtemsRatemonReqGetStatus_PreDesc_Elapsed,
  RtemsRatemonReqGetStatus_PreDesc_Consumed,
  RtemsRatemonReqGetStatus_PreDesc_Postponed,
  NULL
};

static const rtems_id initial_owner = 0xFFFFFFFF;
static const rtems_rate_monotonic_period_states initial_state =
  (rtems_rate_monotonic_period_states) 0xFFFFFFFF;
static const struct timespec initial_period = { 0xFFFFFFFF, 0xFFFFFFFF };
static const uint32_t initial_postponed_jobs_count = 0xFFFFFFFF;
static const rtems_interval period_length = 5;
static const uint32_t elapsed_cpu_ticks = 3;

static uint32_t FreezeTime( void )
{
  return GetTimecountCounter() - 1;
}

static void TickTheClock(
  RtemsRatemonReqGetStatus_Context *ctx,
  uint32_t ticks
)
{
  uint32_t i;
  for ( i = 0; i < ticks; ++i ) {
    TimecounterTick();
  }
  ctx->elapsed.tv_nsec +=
    rtems_configuration_get_nanoseconds_per_tick() * ticks;
}

static void CreatePostponedJobs(
  RtemsRatemonReqGetStatus_Context *ctx,
  uint32_t jobs_count
)
{
  rtems_status_code status;
  ctx->postponed_jobs_count = jobs_count;
  if ( ctx->previous_state == RATE_MONOTONIC_ACTIVE ) {
    jobs_count++;
    TickTheClock( ctx, jobs_count * period_length );
    status = rtems_rate_monotonic_period( ctx->period_id, period_length );
    T_rsc( status, RTEMS_TIMEOUT );
  } else {
    /* ctx->previous_state == RATE_MONOTONIC_INACTIVE || _EXPIRED */
    TickTheClock( ctx, jobs_count * period_length );
  }
  ctx->consumed.tv_nsec +=
    rtems_configuration_get_nanoseconds_per_tick() *
    jobs_count * period_length;
}

static void RtemsRatemonReqGetStatus_Pre_StatusAddr_Prepare(
  RtemsRatemonReqGetStatus_Context       *ctx,
  RtemsRatemonReqGetStatus_Pre_StatusAddr state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Pre_StatusAddr_Valid: {
      /*
       * While the ``status`` parameter references an object of type
       * rtems_rate_monotonic_period_status.
       */
      ctx->status_param = &ctx->period_status;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_StatusAddr_Null: {
      /*
       * While the ``status`` parameter is NULL.
       */
      ctx->status_param = NULL;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_StatusAddr_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Pre_Id_Prepare(
  RtemsRatemonReqGetStatus_Context *ctx,
  RtemsRatemonReqGetStatus_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter is valid.
       */
      ctx->id_param = ctx->period_id;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id_param = RTEMS_ID_NONE;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_Id_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Pre_State_Prepare(
  RtemsRatemonReqGetStatus_Context  *ctx,
  RtemsRatemonReqGetStatus_Pre_State state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Pre_State_Inactive: {
      /*
       * While the ``id`` parameter references an period object in inactive
       * state.
       */
      /* Nothing to do here as the period is newly created. */
      ctx->previous_state = RATE_MONOTONIC_INACTIVE;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_State_Active: {
      /*
       * While the ``id`` parameter references an period object in active
       * state.
       */
      rtems_status_code status;
      status = rtems_rate_monotonic_period( ctx->period_id, period_length );
      T_rsc_success( status );
      ctx->previous_state = RATE_MONOTONIC_ACTIVE;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_State_Expired: {
      /*
       * While the ``id`` parameter references an period object in expired
       * state.
       */
      rtems_status_code status;
      status = rtems_rate_monotonic_period( ctx->period_id, period_length );
      T_rsc_success( status );
      ctx->previous_state = RATE_MONOTONIC_EXPIRED;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_State_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Pre_Elapsed_Prepare(
  RtemsRatemonReqGetStatus_Context    *ctx,
  RtemsRatemonReqGetStatus_Pre_Elapsed state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Pre_Elapsed_Time: {
      /*
       * While a certain time of the CLOCK_MONOTONIC has elapsed.
       */
      /* Time elapsed while having a CPU is added below in "CpuTime". */
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_Elapsed_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Pre_Consumed_Prepare(
  RtemsRatemonReqGetStatus_Context     *ctx,
  RtemsRatemonReqGetStatus_Pre_Consumed state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Pre_Consumed_CpuTime: {
      /*
       * While the owner task has consumed a certain amount of CPU time.
       */
      TickTheClock( ctx, elapsed_cpu_ticks );
      ctx->consumed.tv_nsec +=
        rtems_configuration_get_nanoseconds_per_tick() * elapsed_cpu_ticks;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_Consumed_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Pre_Postponed_Prepare(
  RtemsRatemonReqGetStatus_Context      *ctx,
  RtemsRatemonReqGetStatus_Pre_Postponed state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Pre_Postponed_Zero: {
      /*
       * While the period is not in expired state.
       */
      ctx->postponed_jobs_count = 0;
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_Postponed_One: {
      /*
       * While there is one postponed job.
       */
      CreatePostponedJobs( ctx, 1 );
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_Postponed_Several: {
      /*
       * While there are two or more postponed job.
       */
      CreatePostponedJobs( ctx, 5 );
      break;
    }

    case RtemsRatemonReqGetStatus_Pre_Postponed_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Post_Status_Check(
  RtemsRatemonReqGetStatus_Context    *ctx,
  RtemsRatemonReqGetStatus_Post_Status state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Post_Status_Ok: {
      /*
       * The return status of rtems_rate_monotonic_get_status() shall be
       * RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Status_InvAddr: {
      /*
       * The return status of rtems_rate_monotonic_get_status() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Status_InvId: {
      /*
       * The return status of rtems_rate_monotonic_get_status() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Status_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Post_Owner_Check(
  RtemsRatemonReqGetStatus_Context   *ctx,
  RtemsRatemonReqGetStatus_Post_Owner state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Post_Owner_OwnerTask: {
      /*
       * The value of the member owner of the object referenced by the
       * ``status`` parameter shall be set to the object identifier of the
       * owner task of the period after the return of the
       * rtems_rate_monotonic_get_status() call.
       */
      T_eq_u32( ctx->period_status.owner, ctx->task_id );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Owner_Nop: {
      /*
       * Objects referenced by the ``status`` parameter in past call to
       * rtems_rate_monotonic_get_status() shall not be accessed by the
       * rtems_rate_monotonic_get_status() call (see also Nop).
       */
      T_eq_u32( ctx->period_status.owner, initial_owner );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Owner_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Post_State_Check(
  RtemsRatemonReqGetStatus_Context   *ctx,
  RtemsRatemonReqGetStatus_Post_State state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Post_State_Inactive: {
      /*
       * The value of the member state of the object referenced by the
       * ``status`` parameter shall be set to RATE_MONOTONIC_INACTIVE after the
       * return of the rtems_rate_monotonic_get_status() call. (See also
       * inactive)
       */
      T_eq_int( ctx->period_status.state, RATE_MONOTONIC_INACTIVE );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_State_Active: {
      /*
       * The value of the member state of the object referenced by the
       * ``status`` parameter shall be set to RATE_MONOTONIC_ACTIVE after the
       * return of the rtems_rate_monotonic_get_status() call. (See also
       * active)
       */
      T_eq_int( ctx->period_status.state, RATE_MONOTONIC_ACTIVE );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_State_Expired: {
      /*
       * The value of the member state of the object referenced by the
       * ``status`` parameter shall be set to RATE_MONOTONIC_EXPIRED after the
       * return of the rtems_rate_monotonic_get_status() call. (See also
       * expired)
       */
      T_eq_int( ctx->period_status.state, RATE_MONOTONIC_EXPIRED );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_State_Nop: {
      /*
       * Objects referenced by the ``status`` parameter in past calls to
       * rtems_rate_monotonic_get_status() shall not be accessed by the
       * rtems_rate_monotonic_get_status() call (see also Nop).
       */
      T_eq_u32( ctx->period_status.state, initial_state );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_State_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Post_Elapsed_Check(
  RtemsRatemonReqGetStatus_Context     *ctx,
  RtemsRatemonReqGetStatus_Post_Elapsed state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Post_Elapsed_Time: {
      /*
       * The value of the member since_last_period of the object referenced by
       * the ``status`` parameter shall be set to the time elapsed.
       */
      T_log( T_VERBOSE, "Elapsed: %lld.%ld (expected: %lld.%ld)",
        ctx->period_status.since_last_period.tv_sec,
        ctx->period_status.since_last_period.tv_nsec,
        ctx->elapsed.tv_sec,
        ctx->elapsed.tv_nsec
      );
      T_eq_u64(
        ctx->period_status.since_last_period.tv_sec,
        ctx->elapsed.tv_sec
      );
      /* period_status integer arithmetic is plagued by a rounding error. */
      T_le_long(
        ctx->period_status.since_last_period.tv_nsec,
        ctx->elapsed.tv_nsec + 1
      );
      T_ge_long(
        ctx->period_status.since_last_period.tv_nsec,
        ctx->elapsed.tv_nsec - 1
      );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Elapsed_Zero: {
      /*
       * The value of the member since_last_period of the object referenced by
       * the ``status`` parameter shall be set to 0.
       */
      T_eq_u64( ctx->period_status.since_last_period.tv_sec,   0 );
      T_eq_long( ctx->period_status.since_last_period.tv_nsec, 0 );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Elapsed_Nop: {
      /*
       * Objects referenced by the ``status`` parameter in past calls to
       * rtems_rate_monotonic_get_status() shall not be accessed by the
       * rtems_rate_monotonic_get_status() call (see also Nop).
       */
      T_eq_u64(
        ctx->period_status.since_last_period.tv_sec,
        initial_period.tv_sec
      );
      T_eq_long(
        ctx->period_status.since_last_period.tv_nsec,
        initial_period.tv_nsec
      );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Elapsed_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Post_Consumed_Check(
  RtemsRatemonReqGetStatus_Context      *ctx,
  RtemsRatemonReqGetStatus_Post_Consumed state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Post_Consumed_CpuTime: {
      /*
       * The value of the member executed_since_last_period of the object
       * referenced by the ``status`` parameter shall be set to the CPU time
       * consumed by the owner task.
       */
      T_log( T_VERBOSE, "CPU elapsed: %lld.%ld (expected: %lld.%ld)",
        ctx->period_status.executed_since_last_period.tv_sec,
        ctx->period_status.executed_since_last_period.tv_nsec,
        ctx->consumed.tv_sec,
        ctx->consumed.tv_nsec
      );
      T_eq_u64(
        ctx->period_status.executed_since_last_period.tv_sec,
        ctx->consumed.tv_sec
      );
      /* period_status integer arithmetic is plagued by a rounding error. */
      T_le_long(
        ctx->period_status.executed_since_last_period.tv_nsec,
        ctx->consumed.tv_nsec + 1
      );
      T_ge_long(
        ctx->period_status.executed_since_last_period.tv_nsec,
        ctx->consumed.tv_nsec - 1
      );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Consumed_Zero: {
      /*
       * The value of the member since_last_period of the object referenced by
       * the ``status`` parameter shall be set to 0.
       */
      T_eq_u64( ctx->period_status.executed_since_last_period.tv_sec,   0 );
      T_eq_long( ctx->period_status.executed_since_last_period.tv_nsec, 0 );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Consumed_Nop: {
      /*
       * Objects referenced by the ``status`` parameter in past calls to
       * rtems_rate_monotonic_get_status() shall not be accessed by the
       * rtems_rate_monotonic_get_status() call (see also Nop).
       */
      T_eq_u64(
        ctx->period_status.executed_since_last_period.tv_sec,
        initial_period.tv_sec
      );
      T_eq_long(
        ctx->period_status.executed_since_last_period.tv_nsec,
        initial_period.tv_nsec
      );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Consumed_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Post_Postponed_Check(
  RtemsRatemonReqGetStatus_Context       *ctx,
  RtemsRatemonReqGetStatus_Post_Postponed state
)
{
  switch ( state ) {
    case RtemsRatemonReqGetStatus_Post_Postponed_Zero: {
      /*
       * The value of the member postponed_jobs_count of the object referenced
       * by the ``status`` parameter shall be set to 0 after the return of the
       * rtems_rate_monotonic_get_status() call.
       */
      T_eq_u32( ctx->period_status.postponed_jobs_count, 0 );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Postponed_One: {
      /*
       * The value of the member postponed_jobs_count of the object referenced
       * by the ``status`` parameter shall be set to the number of postponed
       * jobs (here 1) after the return of the
       * rtems_rate_monotonic_get_status() call.
       */
      T_eq_u32( ctx->period_status.postponed_jobs_count, 1 );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Postponed_Several: {
      /*
       * The value of the member postponed_jobs_count of the object referenced
       * by the ``status`` parameter shall be set to the number of postponed
       * jobs after the return of the rtems_rate_monotonic_get_status() call.
       */
      T_eq_u32(
        ctx->period_status.postponed_jobs_count,
        ctx->postponed_jobs_count
      );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Postponed_Nop: {
      /*
       * Objects referenced by the ``status`` parameter in past calls to
       * rtems_rate_monotonic_get_status() shall not be accessed by the
       * rtems_rate_monotonic_get_status() call (see also Nop).
       */
      T_eq_u32( ctx->period_status.postponed_jobs_count,
        initial_postponed_jobs_count );
      break;
    }

    case RtemsRatemonReqGetStatus_Post_Postponed_NA:
      break;
  }
}

static void RtemsRatemonReqGetStatus_Setup(
  RtemsRatemonReqGetStatus_Context *ctx
)
{
  ctx->previous_timecounter_handler = SetGetTimecountHandler( FreezeTime );
  ctx->task_id = rtems_task_self();
}

static void RtemsRatemonReqGetStatus_Setup_Wrap( void *arg )
{
  RtemsRatemonReqGetStatus_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqGetStatus_Setup( ctx );
}

static void RtemsRatemonReqGetStatus_Teardown(
  RtemsRatemonReqGetStatus_Context *ctx
)
{
  SetGetTimecountHandler( ctx->previous_timecounter_handler );
}

static void RtemsRatemonReqGetStatus_Teardown_Wrap( void *arg )
{
  RtemsRatemonReqGetStatus_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqGetStatus_Teardown( ctx );
}

static void RtemsRatemonReqGetStatus_Prepare(
  RtemsRatemonReqGetStatus_Context *ctx
)
{
  rtems_status_code status;
  status =  rtems_rate_monotonic_create(
    rtems_build_name( 'R', 'M', 'O', 'N' ),
    &ctx->period_id
  );
  T_rsc_success( status );

  ctx->period_status = (rtems_rate_monotonic_period_status) {
    .owner = initial_owner,
    .state = initial_state,
    .since_last_period = initial_period,
    .executed_since_last_period = initial_period,
    .postponed_jobs_count = initial_postponed_jobs_count
  };

  ctx->elapsed.tv_sec  = 0;
  ctx->elapsed.tv_nsec = 0;
  ctx->consumed.tv_sec  = 0;
  ctx->consumed.tv_nsec = 0;
  ctx->postponed_jobs_count = 0;
  TimecounterTick();
}

static void RtemsRatemonReqGetStatus_Action(
  RtemsRatemonReqGetStatus_Context *ctx
)
{
  if ( ctx->do_reset != NULL ) {
      ctx->do_reset();
  }
  ctx->status = rtems_rate_monotonic_get_status(
    ctx->id_param,
    ctx->status_param
  );
}

static void RtemsRatemonReqGetStatus_Cleanup(
  RtemsRatemonReqGetStatus_Context *ctx
)
{
  T_rsc_success( rtems_rate_monotonic_delete( ctx->period_id ) );
}

static const RtemsRatemonReqGetStatus_Entry
RtemsRatemonReqGetStatus_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_InvAddr,
    RtemsRatemonReqGetStatus_Post_Owner_Nop,
    RtemsRatemonReqGetStatus_Post_State_Nop,
    RtemsRatemonReqGetStatus_Post_Elapsed_Nop,
    RtemsRatemonReqGetStatus_Post_Consumed_Nop,
    RtemsRatemonReqGetStatus_Post_Postponed_Nop },
  { 1, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_NA,
    RtemsRatemonReqGetStatus_Post_Owner_NA,
    RtemsRatemonReqGetStatus_Post_State_NA,
    RtemsRatemonReqGetStatus_Post_Elapsed_NA,
    RtemsRatemonReqGetStatus_Post_Consumed_NA,
    RtemsRatemonReqGetStatus_Post_Postponed_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_InvId,
    RtemsRatemonReqGetStatus_Post_Owner_Nop,
    RtemsRatemonReqGetStatus_Post_State_Nop,
    RtemsRatemonReqGetStatus_Post_Elapsed_Nop,
    RtemsRatemonReqGetStatus_Post_Consumed_Nop,
    RtemsRatemonReqGetStatus_Post_Postponed_Nop },
  { 1, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_NA,
    RtemsRatemonReqGetStatus_Post_Owner_NA,
    RtemsRatemonReqGetStatus_Post_State_NA,
    RtemsRatemonReqGetStatus_Post_Elapsed_NA,
    RtemsRatemonReqGetStatus_Post_Consumed_NA,
    RtemsRatemonReqGetStatus_Post_Postponed_NA },
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqGetStatus_Post_Status_InvAddr,
    RtemsRatemonReqGetStatus_Post_Owner_Nop,
    RtemsRatemonReqGetStatus_Post_State_Nop,
    RtemsRatemonReqGetStatus_Post_Elapsed_Nop,
    RtemsRatemonReqGetStatus_Post_Consumed_Nop,
    RtemsRatemonReqGetStatus_Post_Postponed_Nop },
  { 0, 0, 0, 0, 1, 1, 1, RtemsRatemonReqGetStatus_Post_Status_Ok,
    RtemsRatemonReqGetStatus_Post_Owner_OwnerTask,
    RtemsRatemonReqGetStatus_Post_State_Inactive,
    RtemsRatemonReqGetStatus_Post_Elapsed_Zero,
    RtemsRatemonReqGetStatus_Post_Consumed_Zero,
    RtemsRatemonReqGetStatus_Post_Postponed_NA },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_Ok,
    RtemsRatemonReqGetStatus_Post_Owner_OwnerTask,
    RtemsRatemonReqGetStatus_Post_State_Active,
    RtemsRatemonReqGetStatus_Post_Elapsed_Time,
    RtemsRatemonReqGetStatus_Post_Consumed_CpuTime,
    RtemsRatemonReqGetStatus_Post_Postponed_Zero },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_Ok,
    RtemsRatemonReqGetStatus_Post_Owner_OwnerTask,
    RtemsRatemonReqGetStatus_Post_State_Active,
    RtemsRatemonReqGetStatus_Post_Elapsed_Time,
    RtemsRatemonReqGetStatus_Post_Consumed_CpuTime,
    RtemsRatemonReqGetStatus_Post_Postponed_One },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_Ok,
    RtemsRatemonReqGetStatus_Post_Owner_OwnerTask,
    RtemsRatemonReqGetStatus_Post_State_Active,
    RtemsRatemonReqGetStatus_Post_Elapsed_Time,
    RtemsRatemonReqGetStatus_Post_Consumed_CpuTime,
    RtemsRatemonReqGetStatus_Post_Postponed_Several },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_Ok,
    RtemsRatemonReqGetStatus_Post_Owner_OwnerTask,
    RtemsRatemonReqGetStatus_Post_State_Expired,
    RtemsRatemonReqGetStatus_Post_Elapsed_Time,
    RtemsRatemonReqGetStatus_Post_Consumed_CpuTime,
    RtemsRatemonReqGetStatus_Post_Postponed_One },
  { 0, 0, 0, 0, 0, 0, 0, RtemsRatemonReqGetStatus_Post_Status_Ok,
    RtemsRatemonReqGetStatus_Post_Owner_OwnerTask,
    RtemsRatemonReqGetStatus_Post_State_Expired,
    RtemsRatemonReqGetStatus_Post_Elapsed_Time,
    RtemsRatemonReqGetStatus_Post_Consumed_CpuTime,
    RtemsRatemonReqGetStatus_Post_Postponed_Several },
  { 0, 0, 0, 0, 0, 0, 1, RtemsRatemonReqGetStatus_Post_Status_InvId,
    RtemsRatemonReqGetStatus_Post_Owner_Nop,
    RtemsRatemonReqGetStatus_Post_State_Nop,
    RtemsRatemonReqGetStatus_Post_Elapsed_Nop,
    RtemsRatemonReqGetStatus_Post_Consumed_Nop,
    RtemsRatemonReqGetStatus_Post_Postponed_Nop }
};

static const uint8_t
RtemsRatemonReqGetStatus_Map[] = {
  5, 1, 1, 6, 7, 8, 3, 9, 10, 11, 1, 1, 2, 2, 2, 3, 2, 2, 4, 1, 1, 0, 0, 0, 3,
  0, 0, 4, 1, 1, 0, 0, 0, 3, 0, 0
};

static size_t RtemsRatemonReqGetStatus_Scope( void *arg, char *buf, size_t n )
{
  RtemsRatemonReqGetStatus_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsRatemonReqGetStatus_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsRatemonReqGetStatus_Fixture = {
  .setup = RtemsRatemonReqGetStatus_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsRatemonReqGetStatus_Teardown_Wrap,
  .scope = RtemsRatemonReqGetStatus_Scope,
  .initial_context = &RtemsRatemonReqGetStatus_Instance
};

static inline RtemsRatemonReqGetStatus_Entry RtemsRatemonReqGetStatus_PopEntry(
  RtemsRatemonReqGetStatus_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsRatemonReqGetStatus_Entries[
    RtemsRatemonReqGetStatus_Map[ index ]
  ];
}

static void RtemsRatemonReqGetStatus_SetPreConditionStates(
  RtemsRatemonReqGetStatus_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Elapsed_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsRatemonReqGetStatus_Pre_Elapsed_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Consumed_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsRatemonReqGetStatus_Pre_Consumed_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  if ( ctx->Map.entry.Pre_Postponed_NA ) {
    ctx->Map.pcs[ 5 ] = RtemsRatemonReqGetStatus_Pre_Postponed_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }
}

static void RtemsRatemonReqGetStatus_TestVariant(
  RtemsRatemonReqGetStatus_Context *ctx
)
{
  RtemsRatemonReqGetStatus_Pre_StatusAddr_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsRatemonReqGetStatus_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsRatemonReqGetStatus_Pre_State_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsRatemonReqGetStatus_Pre_Elapsed_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsRatemonReqGetStatus_Pre_Consumed_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsRatemonReqGetStatus_Pre_Postponed_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsRatemonReqGetStatus_Action( ctx );
  RtemsRatemonReqGetStatus_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsRatemonReqGetStatus_Post_Owner_Check( ctx, ctx->Map.entry.Post_Owner );
  RtemsRatemonReqGetStatus_Post_State_Check( ctx, ctx->Map.entry.Post_State );
  RtemsRatemonReqGetStatus_Post_Elapsed_Check(
    ctx,
    ctx->Map.entry.Post_Elapsed
  );
  RtemsRatemonReqGetStatus_Post_Consumed_Check(
    ctx,
    ctx->Map.entry.Post_Consumed
  );
  RtemsRatemonReqGetStatus_Post_Postponed_Check(
    ctx,
    ctx->Map.entry.Post_Postponed
  );
}

/**
 * @fn void T_case_body_RtemsRatemonReqGetStatus( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsRatemonReqGetStatus,
  &RtemsRatemonReqGetStatus_Fixture
)
{
  RtemsRatemonReqGetStatus_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsRatemonReqGetStatus_Pre_StatusAddr_Valid;
    ctx->Map.pci[ 0 ] < RtemsRatemonReqGetStatus_Pre_StatusAddr_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsRatemonReqGetStatus_Pre_Id_Valid;
      ctx->Map.pci[ 1 ] < RtemsRatemonReqGetStatus_Pre_Id_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsRatemonReqGetStatus_Pre_State_Inactive;
        ctx->Map.pci[ 2 ] < RtemsRatemonReqGetStatus_Pre_State_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsRatemonReqGetStatus_Pre_Elapsed_Time;
          ctx->Map.pci[ 3 ] < RtemsRatemonReqGetStatus_Pre_Elapsed_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsRatemonReqGetStatus_Pre_Consumed_CpuTime;
            ctx->Map.pci[ 4 ] < RtemsRatemonReqGetStatus_Pre_Consumed_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsRatemonReqGetStatus_Pre_Postponed_Zero;
              ctx->Map.pci[ 5 ] < RtemsRatemonReqGetStatus_Pre_Postponed_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = RtemsRatemonReqGetStatus_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              RtemsRatemonReqGetStatus_SetPreConditionStates( ctx );
              RtemsRatemonReqGetStatus_Prepare( ctx );
              RtemsRatemonReqGetStatus_TestVariant( ctx );
              RtemsRatemonReqGetStatus_Cleanup( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
