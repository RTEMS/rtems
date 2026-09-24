/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CTimeReqClockNanosleep
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <limits.h>
#include <rtems.h>
#include <time.h>
#include <rtems/test-scheduler.h>
#include <rtems/score/timecounter.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CTimeReqClockNanosleep spec:/c/time/req/clock-nanosleep
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CTimeReqClockNanosleep_Pre_ClockId_Monotonic,
  CTimeReqClockNanosleep_Pre_ClockId_Realtime,
  CTimeReqClockNanosleep_Pre_ClockId_Invalid,
  CTimeReqClockNanosleep_Pre_ClockId_NA
} CTimeReqClockNanosleep_Pre_ClockId;

typedef enum {
  CTimeReqClockNanosleep_Pre_Abstime_Yes,
  CTimeReqClockNanosleep_Pre_Abstime_No,
  CTimeReqClockNanosleep_Pre_Abstime_NA
} CTimeReqClockNanosleep_Pre_Abstime;

typedef enum {
  CTimeReqClockNanosleep_Pre_RQTp_Valid,
  CTimeReqClockNanosleep_Pre_RQTp_Null,
  CTimeReqClockNanosleep_Pre_RQTp_NA
} CTimeReqClockNanosleep_Pre_RQTp;

typedef enum {
  CTimeReqClockNanosleep_Pre_RQTpNSec_Valid,
  CTimeReqClockNanosleep_Pre_RQTpNSec_Invalid,
  CTimeReqClockNanosleep_Pre_RQTpNSec_NA
} CTimeReqClockNanosleep_Pre_RQTpNSec;

typedef enum {
  CTimeReqClockNanosleep_Pre_RQTpSec_Negative,
  CTimeReqClockNanosleep_Pre_RQTpSec_FarFuture,
  CTimeReqClockNanosleep_Pre_RQTpSec_Future,
  CTimeReqClockNanosleep_Pre_RQTpSec_PastOrNow,
  CTimeReqClockNanosleep_Pre_RQTpSec_NA
} CTimeReqClockNanosleep_Pre_RQTpSec;

typedef enum {
  CTimeReqClockNanosleep_Pre_RMTp_Valid,
  CTimeReqClockNanosleep_Pre_RMTp_Null,
  CTimeReqClockNanosleep_Pre_RMTp_NA
} CTimeReqClockNanosleep_Pre_RMTp;

typedef enum {
  CTimeReqClockNanosleep_Post_Status_Zero,
  CTimeReqClockNanosleep_Post_Status_ENOTSUP,
  CTimeReqClockNanosleep_Post_Status_EINVAL,
  CTimeReqClockNanosleep_Post_Status_NA
} CTimeReqClockNanosleep_Post_Status;

typedef enum {
  CTimeReqClockNanosleep_Post_Timer_Inactive,
  CTimeReqClockNanosleep_Post_Timer_Monotonic,
  CTimeReqClockNanosleep_Post_Timer_Realtime,
  CTimeReqClockNanosleep_Post_Timer_NA
} CTimeReqClockNanosleep_Post_Timer;

typedef enum {
  CTimeReqClockNanosleep_Post_Expire_Last,
  CTimeReqClockNanosleep_Post_Expire_Absolute,
  CTimeReqClockNanosleep_Post_Expire_Relative,
  CTimeReqClockNanosleep_Post_Expire_NA
} CTimeReqClockNanosleep_Post_Expire;

typedef enum {
  CTimeReqClockNanosleep_Post_Scheduler_Block,
  CTimeReqClockNanosleep_Post_Scheduler_BlockUnblock,
  CTimeReqClockNanosleep_Post_Scheduler_Nop,
  CTimeReqClockNanosleep_Post_Scheduler_NA
} CTimeReqClockNanosleep_Post_Scheduler;

typedef enum {
  CTimeReqClockNanosleep_Post_RMTp_Zero,
  CTimeReqClockNanosleep_Post_RMTp_Nop,
  CTimeReqClockNanosleep_Post_RMTp_NA
} CTimeReqClockNanosleep_Post_RMTp;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_ClockId_NA : 1;
  uint32_t Pre_Abstime_NA : 1;
  uint32_t Pre_RQTp_NA : 1;
  uint32_t Pre_RQTpNSec_NA : 1;
  uint32_t Pre_RQTpSec_NA : 1;
  uint32_t Pre_RMTp_NA : 1;
  uint32_t Post_Status : 2;
  uint32_t Post_Timer : 2;
  uint32_t Post_Expire : 2;
  uint32_t Post_Scheduler : 2;
  uint32_t Post_RMTp : 2;
} CTimeReqClockNanosleep_Entry;

/**
 * @brief Test context for spec:/c/time/req/clock-nanosleep test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_4 scheduler_log;

  /**
   * @brief This member contains the CLOCK_REALTIME value before the
   *   clock_nanosleep() call.
   */
  struct timespec now_realtime;

  /**
   * @brief This member contains the CLOCK_MONOTONIC value before the
   *   clock_nanosleep() call.
   */
  struct timespec now_monotonic;

  /**
   * @brief This member contains the worker task identifier.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the timer information of the worker task.
   */
  TaskTimerInfo timer_info;

  /**
   * @brief This member provides the object referenced by the `rqtp` parameter.
   */
  struct timespec rqtp_obj;

  /**
   * @brief This member provides the object referenced by the `rmtp` parameter.
   */
  struct timespec rmtp_obj;

  /**
   * @brief This member contains the return value of the clock_nanosleep()
   *   call.
   */
  int status;

  /**
   * @brief This member specifies the `clock_id` parameter value.
   */
  clockid_t clock_id;

  /**
   * @brief This member specifies the `flags` parameter value.
   */
  int flags;

  /**
   * @brief This member specifies the `rqtp` parameter value.
   */
  const struct timespec *rqtp;

  /**
   * @brief This member specifies the `rmtp` parameter value.
   */
  struct timespec *rmtp;

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
    CTimeReqClockNanosleep_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CTimeReqClockNanosleep_Context;

static CTimeReqClockNanosleep_Context CTimeReqClockNanosleep_Instance;

static const char *const CTimeReqClockNanosleep_PreDesc_ClockId[] =
  { "Monotonic", "Realtime", "Invalid", "NA" };

static const char *const CTimeReqClockNanosleep_PreDesc_Abstime[] =
  { "Yes", "No", "NA" };

static const char *const CTimeReqClockNanosleep_PreDesc_RQTp[] =
  { "Valid", "Null", "NA" };

static const char *const CTimeReqClockNanosleep_PreDesc_RQTpNSec[] =
  { "Valid", "Invalid", "NA" };

static const char *const CTimeReqClockNanosleep_PreDesc_RQTpSec[] =
  { "Negative", "FarFuture", "Future", "PastOrNow", "NA" };

static const char *const CTimeReqClockNanosleep_PreDesc_RMTp[] =
  { "Valid", "Null", "NA" };

static const char *const *const CTimeReqClockNanosleep_PreDesc[] = {
  CTimeReqClockNanosleep_PreDesc_ClockId,
  CTimeReqClockNanosleep_PreDesc_Abstime,
  CTimeReqClockNanosleep_PreDesc_RQTp,
  CTimeReqClockNanosleep_PreDesc_RQTpNSec,
  CTimeReqClockNanosleep_PreDesc_RQTpSec,
  CTimeReqClockNanosleep_PreDesc_RMTp,
  NULL
};

typedef CTimeReqClockNanosleep_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    T_scheduler_log *log;
    uint32_t         counter;

    SuspendSelf();

    log = T_scheduler_record_4( &ctx->scheduler_log );
    T_null( log );

    counter = GetTimecountCounter();
    _Timecounter_Nanotime( &ctx->now_realtime );
    SetTimecountCounter( counter );

    counter = GetTimecountCounter();
    _Timecounter_Nanouptime( &ctx->now_monotonic );
    SetTimecountCounter( counter );

    ctx->status = clock_nanosleep(
      ctx->clock_id,
      ctx->flags,
      ctx->rqtp,
      ctx->rmtp
    );

    (void) T_scheduler_record( NULL );
  }
}

static void CTimeReqClockNanosleep_Pre_ClockId_Prepare(
  CTimeReqClockNanosleep_Context    *ctx,
  CTimeReqClockNanosleep_Pre_ClockId state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Pre_ClockId_Monotonic: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_MONOTONIC.
       */
      ctx->clock_id = CLOCK_MONOTONIC;
      break;
    }

    case CTimeReqClockNanosleep_Pre_ClockId_Realtime: {
      /*
       * While the `clock_id` parameter is equal to CLOCK_REALTIME.
       */
      ctx->clock_id = CLOCK_REALTIME;
      break;
    }

    case CTimeReqClockNanosleep_Pre_ClockId_Invalid: {
      /*
       * While the `clock_id` parameter is an invalid clock identifier.
       */
      ctx->clock_id = INT_MAX;
      break;
    }

    case CTimeReqClockNanosleep_Pre_ClockId_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Pre_Abstime_Prepare(
  CTimeReqClockNanosleep_Context    *ctx,
  CTimeReqClockNanosleep_Pre_Abstime state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Pre_Abstime_Yes: {
      /*
       * While the `flags` parameter indicates an absolute time.
       */
      ctx->flags |= TIMER_ABSTIME;
      break;
    }

    case CTimeReqClockNanosleep_Pre_Abstime_No: {
      /*
       * While the `flags` parameter does not indicate an absolute time.
       */
      /* This is the default */
      break;
    }

    case CTimeReqClockNanosleep_Pre_Abstime_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Pre_RQTp_Prepare(
  CTimeReqClockNanosleep_Context *ctx,
  CTimeReqClockNanosleep_Pre_RQTp state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Pre_RQTp_Valid: {
      /*
       * While the `rqtp` parameter references an object of type struct
       * timespec.
       */
      ctx->rqtp = &ctx->rqtp_obj;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RQTp_Null: {
      /*
       * While the `rqtp` parameter is equal to NULL.
       */
      ctx->rqtp = NULL;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RQTp_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Pre_RQTpNSec_Prepare(
  CTimeReqClockNanosleep_Context     *ctx,
  CTimeReqClockNanosleep_Pre_RQTpNSec state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Pre_RQTpNSec_Valid: {
      /*
       * While the `tv_nsec` member of the object referenced by the `rqtp`
       * parameter is a valid nanoseconds value.
       */
      ctx->rqtp_obj.tv_nsec = 999999999;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RQTpNSec_Invalid: {
      /*
       * While the `tv_nsec` member of the object referenced by the `rqtp`
       * parameter is an invalid nanoseconds value.
       */
      ctx->rqtp_obj.tv_nsec = -1;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RQTpNSec_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Pre_RQTpSec_Prepare(
  CTimeReqClockNanosleep_Context    *ctx,
  CTimeReqClockNanosleep_Pre_RQTpSec state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Pre_RQTpSec_Negative: {
      /*
       * While the `tv_sec` member of the object referenced by the `rqtp`
       * parameter is negative.
       */
      ctx->rqtp_obj.tv_sec = -238479;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RQTpSec_FarFuture: {
      /*
       * While the `tv_sec` member of the object referenced by the `rqtp`
       * parameter specifies a time point which is past the implementation
       * limit.
       */
      ctx->rqtp_obj.tv_sec = INT64_MAX;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RQTpSec_Future: {
      /*
       * While the `tv_sec` member of the object referenced by the `rqtp`
       * parameter specifies a time point which is after the current time of
       * the clock specified by the `clock_id` parameter and is within the
       * implementation limits.
       */
      ctx->rqtp_obj.tv_sec = 1621322302;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RQTpSec_PastOrNow: {
      /*
       * While the `tv_sec` member of the object referenced by the `rqtp`
       * parameter is non-negative and specifies a time point which is before
       * or at the current time of the clock specified by the `clock_id`
       * parameter.
       */
      ctx->rqtp_obj.tv_sec = 0;

      if ( ctx->rqtp_obj.tv_nsec == 999999999 ) {
        ctx->rqtp_obj.tv_nsec = 0;
      }
      break;
    }

    case CTimeReqClockNanosleep_Pre_RQTpSec_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Pre_RMTp_Prepare(
  CTimeReqClockNanosleep_Context *ctx,
  CTimeReqClockNanosleep_Pre_RMTp state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Pre_RMTp_Valid: {
      /*
       * While the `rmtp` parameter references an object of type struct
       * timespec.
       */
      ctx->rmtp = &ctx->rmtp_obj;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RMTp_Null: {
      /*
       * While the `rmtp` parameter is equal to NULL.
       */
      ctx->rmtp = NULL;
      break;
    }

    case CTimeReqClockNanosleep_Pre_RMTp_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Post_Status_Check(
  CTimeReqClockNanosleep_Context    *ctx,
  CTimeReqClockNanosleep_Post_Status state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Post_Status_Zero: {
      /*
       * The return value of clock_nanosleep() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CTimeReqClockNanosleep_Post_Status_ENOTSUP: {
      /*
       * The return value of clock_nanosleep() shall be equal to ENOTSUP.
       */
      T_eq_int( ctx->status, ENOTSUP );
      break;
    }

    case CTimeReqClockNanosleep_Post_Status_EINVAL: {
      /*
       * The return value of clock_nanosleep() shall be equal to EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CTimeReqClockNanosleep_Post_Status_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Post_Timer_Check(
  CTimeReqClockNanosleep_Context   *ctx,
  CTimeReqClockNanosleep_Post_Timer state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case CTimeReqClockNanosleep_Post_Timer_Monotonic: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_MONOTONIC.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_MONOTONIC );
      break;
    }

    case CTimeReqClockNanosleep_Post_Timer_Realtime: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_REALTIME.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_REALTIME );
      break;
    }

    case CTimeReqClockNanosleep_Post_Timer_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Post_Expire_Check(
  CTimeReqClockNanosleep_Context    *ctx,
  CTimeReqClockNanosleep_Post_Expire state
)
{
  struct timespec expire;

  switch ( state ) {
    case CTimeReqClockNanosleep_Post_Expire_Last: {
      /*
       * The timer of the calling task shall expire at the last valid time
       * point of the clock specified by the `clock_id` parameter.
       */
      T_eq_u64( ctx->timer_info.expire_ticks, 0xffffffffffffffff );
      break;
    }

    case CTimeReqClockNanosleep_Post_Expire_Absolute: {
      /*
       * The timer of the calling task shall expire at the time point specified
       * by the `rqtp` parameter.
       */
      T_eq_i64( ctx->timer_info.expire_timespec.tv_sec, ctx->rqtp_obj.tv_sec );
      T_eq_long(
        ctx->timer_info.expire_timespec.tv_nsec,
        ctx->rqtp_obj.tv_nsec
      );
      break;
    }

    case CTimeReqClockNanosleep_Post_Expire_Relative: {
      /*
       * The timer of the calling task shall expire at the time point specified
       * by the sum of the current time of the clock specified by
       * CLOCK_MONOTONIC and the interval specified by the `rqtp` parameter.
       */
      expire = ctx->now_monotonic;
      expire.tv_sec += ctx->rqtp_obj.tv_sec;
      expire.tv_nsec += ctx->rqtp_obj.tv_nsec;

      if ( expire.tv_nsec >= 1000000000 ) {
        ++expire.tv_sec;
        expire.tv_nsec -= 1000000000;
      }

      T_eq_i64( ctx->timer_info.expire_timespec.tv_sec, expire.tv_sec );
      T_eq_long( ctx->timer_info.expire_timespec.tv_nsec, expire.tv_nsec );
      break;
    }

    case CTimeReqClockNanosleep_Post_Expire_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Post_Scheduler_Check(
  CTimeReqClockNanosleep_Context       *ctx,
  CTimeReqClockNanosleep_Post_Scheduler state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Post_Scheduler_Block: {
      /*
       * The calling task shall be blocked by the scheduler exactly once by the
       * clock_nanosleep() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
      T_eq_int( ctx->scheduler_log.events[ 0 ].operation, T_SCHEDULER_BLOCK );
      break;
    }

    case CTimeReqClockNanosleep_Post_Scheduler_BlockUnblock: {
      /*
       * The calling task shall be blocked exactly once by the scheduler and
       * then unblocked in the same thread dispatch critical section by the
       * clock_nanosleep() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 2 );
      T_eq_int( ctx->scheduler_log.events[ 0 ].operation, T_SCHEDULER_BLOCK );
      T_eq_int(
        ctx->scheduler_log.events[ 1 ].operation,
        T_SCHEDULER_UNBLOCK
      );
      break;
    }

    case CTimeReqClockNanosleep_Post_Scheduler_Nop: {
      /*
       * The calling task shall not be altered by the scheduler by the
       * clock_nanosleep() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      break;
    }

    case CTimeReqClockNanosleep_Post_Scheduler_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Post_RMTp_Check(
  CTimeReqClockNanosleep_Context  *ctx,
  CTimeReqClockNanosleep_Post_RMTp state
)
{
  switch ( state ) {
    case CTimeReqClockNanosleep_Post_RMTp_Zero: {
      /*
       * The object referenced by the `rmtp` parameter shall be cleared to zero
       * after the return of the clock_nanosleep() call.
       */
      T_eq_i64( ctx->rmtp_obj.tv_sec, 0 );
      T_eq_long( ctx->rmtp_obj.tv_nsec, 0 );
      break;
    }

    case CTimeReqClockNanosleep_Post_RMTp_Nop: {
      /*
       * Objects referenced by the `rmtp` parameter in past calls to
       * clock_nanosleep() shall not be accessed by the clock_nanosleep() call.
       */
      T_eq_i64( ctx->rmtp_obj.tv_sec, -1 );
      T_eq_long( ctx->rmtp_obj.tv_nsec, -1 );
      break;
    }

    case CTimeReqClockNanosleep_Post_RMTp_NA:
      break;
  }
}

static void CTimeReqClockNanosleep_Setup( CTimeReqClockNanosleep_Context *ctx )
{
  rtems_time_of_day now = { 1988, 1, 1, 0, 0, 0, 0 };
  T_rsc_success( rtems_clock_set( &now ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void CTimeReqClockNanosleep_Setup_Wrap( void *arg )
{
  CTimeReqClockNanosleep_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CTimeReqClockNanosleep_Setup( ctx );
}

static void CTimeReqClockNanosleep_Teardown(
  CTimeReqClockNanosleep_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void CTimeReqClockNanosleep_Teardown_Wrap( void *arg )
{
  CTimeReqClockNanosleep_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CTimeReqClockNanosleep_Teardown( ctx );
}

static void CTimeReqClockNanosleep_Prepare(
  CTimeReqClockNanosleep_Context *ctx
)
{
  ctx->status = -1;
  ctx->flags = 0;
  ctx->rmtp_obj.tv_sec = -1;
  ctx->rmtp_obj.tv_nsec = -1;
}

static void CTimeReqClockNanosleep_Action(
  CTimeReqClockNanosleep_Context *ctx
)
{
  ResumeTask( ctx->worker_id );
  (void) T_scheduler_record( NULL );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );
  ClockTick();
  FinalClockTick();
}

/* clang-format off */

static const CTimeReqClockNanosleep_Entry
CTimeReqClockNanosleep_Entries[] = {
  { 0, 0, 0, 0, 1, 1, 0, CTimeReqClockNanosleep_Post_Status_EINVAL,
    CTimeReqClockNanosleep_Post_Timer_Inactive,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 1, 1, 0, CTimeReqClockNanosleep_Post_Status_ENOTSUP,
    CTimeReqClockNanosleep_Post_Timer_Inactive,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_Nop,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_EINVAL,
    CTimeReqClockNanosleep_Post_Timer_Inactive,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 1, 1, 0, CTimeReqClockNanosleep_Post_Status_EINVAL,
    CTimeReqClockNanosleep_Post_Timer_Inactive,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_ENOTSUP,
    CTimeReqClockNanosleep_Post_Timer_Inactive,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_Nop,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 1, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_NA,
    CTimeReqClockNanosleep_Post_Timer_NA,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_NA,
    CTimeReqClockNanosleep_Post_RMTp_NA },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Inactive,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_EINVAL,
    CTimeReqClockNanosleep_Post_Timer_Inactive,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Monotonic,
    CTimeReqClockNanosleep_Post_Expire_Last,
    CTimeReqClockNanosleep_Post_Scheduler_Block,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Monotonic,
    CTimeReqClockNanosleep_Post_Expire_Absolute,
    CTimeReqClockNanosleep_Post_Scheduler_Block,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Monotonic,
    CTimeReqClockNanosleep_Post_Expire_Last,
    CTimeReqClockNanosleep_Post_Scheduler_Block,
    CTimeReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Monotonic,
    CTimeReqClockNanosleep_Post_Expire_Relative,
    CTimeReqClockNanosleep_Post_Scheduler_Block,
    CTimeReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Monotonic,
    CTimeReqClockNanosleep_Post_Expire_Relative,
    CTimeReqClockNanosleep_Post_Scheduler_Block,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Inactive,
    CTimeReqClockNanosleep_Post_Expire_NA,
    CTimeReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CTimeReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Realtime,
    CTimeReqClockNanosleep_Post_Expire_Last,
    CTimeReqClockNanosleep_Post_Scheduler_Block,
    CTimeReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CTimeReqClockNanosleep_Post_Status_Zero,
    CTimeReqClockNanosleep_Post_Timer_Realtime,
    CTimeReqClockNanosleep_Post_Expire_Absolute,
    CTimeReqClockNanosleep_Post_Scheduler_Block,
    CTimeReqClockNanosleep_Post_RMTp_Nop }
};

static const uint8_t
CTimeReqClockNanosleep_Map[] = {
  6, 6, 8, 8, 9, 9, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 7, 2, 10, 8, 11, 12, 13, 6, 7, 2, 7, 2, 7, 2, 7, 2, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 6, 6, 14, 14, 15, 15, 6, 6, 2, 2,
  2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 2, 10,
  8, 11, 12, 13, 6, 7, 2, 7, 2, 7, 2, 7, 2, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

/* clang-format on */

static size_t CTimeReqClockNanosleep_Scope( void *arg, char *buf, size_t n )
{
  CTimeReqClockNanosleep_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CTimeReqClockNanosleep_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CTimeReqClockNanosleep_Fixture = {
  .setup = CTimeReqClockNanosleep_Setup_Wrap,
  .stop = NULL,
  .teardown = CTimeReqClockNanosleep_Teardown_Wrap,
  .scope = CTimeReqClockNanosleep_Scope,
  .initial_context = &CTimeReqClockNanosleep_Instance
};

static inline CTimeReqClockNanosleep_Entry CTimeReqClockNanosleep_PopEntry(
  CTimeReqClockNanosleep_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CTimeReqClockNanosleep_Entries[ CTimeReqClockNanosleep_Map[ index ] ];
}

static void CTimeReqClockNanosleep_SetPreConditionStates(
  CTimeReqClockNanosleep_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_RQTpNSec_NA ) {
    ctx->Map.pcs[ 3 ] = CTimeReqClockNanosleep_Pre_RQTpNSec_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_RQTpSec_NA ) {
    ctx->Map.pcs[ 4 ] = CTimeReqClockNanosleep_Pre_RQTpSec_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
}

static void CTimeReqClockNanosleep_TestVariant(
  CTimeReqClockNanosleep_Context *ctx
)
{
  CTimeReqClockNanosleep_Pre_ClockId_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CTimeReqClockNanosleep_Pre_Abstime_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CTimeReqClockNanosleep_Pre_RQTp_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CTimeReqClockNanosleep_Pre_RQTpNSec_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CTimeReqClockNanosleep_Pre_RQTpSec_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CTimeReqClockNanosleep_Pre_RMTp_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CTimeReqClockNanosleep_Action( ctx );
  CTimeReqClockNanosleep_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CTimeReqClockNanosleep_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  CTimeReqClockNanosleep_Post_Expire_Check( ctx, ctx->Map.entry.Post_Expire );
  CTimeReqClockNanosleep_Post_Scheduler_Check(
    ctx,
    ctx->Map.entry.Post_Scheduler
  );
  CTimeReqClockNanosleep_Post_RMTp_Check( ctx, ctx->Map.entry.Post_RMTp );
}

/**
 * @fn void T_case_body_CTimeReqClockNanosleep( void )
 */
T_TEST_CASE_FIXTURE( CTimeReqClockNanosleep, &CTimeReqClockNanosleep_Fixture )
{
  CTimeReqClockNanosleep_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CTimeReqClockNanosleep_Pre_ClockId_Monotonic;
    ctx->Map.pci[ 0 ] < CTimeReqClockNanosleep_Pre_ClockId_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CTimeReqClockNanosleep_Pre_Abstime_Yes;
      ctx->Map.pci[ 1 ] < CTimeReqClockNanosleep_Pre_Abstime_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CTimeReqClockNanosleep_Pre_RQTp_Valid;
        ctx->Map.pci[ 2 ] < CTimeReqClockNanosleep_Pre_RQTp_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CTimeReqClockNanosleep_Pre_RQTpNSec_Valid;
          ctx->Map.pci[ 3 ] < CTimeReqClockNanosleep_Pre_RQTpNSec_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CTimeReqClockNanosleep_Pre_RQTpSec_Negative;
            ctx->Map.pci[ 4 ] < CTimeReqClockNanosleep_Pre_RQTpSec_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CTimeReqClockNanosleep_Pre_RMTp_Valid;
              ctx->Map.pci[ 5 ] < CTimeReqClockNanosleep_Pre_RMTp_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = CTimeReqClockNanosleep_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              CTimeReqClockNanosleep_SetPreConditionStates( ctx );
              CTimeReqClockNanosleep_Prepare( ctx );
              CTimeReqClockNanosleep_TestVariant( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
