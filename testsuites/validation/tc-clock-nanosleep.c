/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup CReqClockNanosleep
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
#include <limits.h>
#include <rtems.h>
#include <time.h>
#include <rtems/test-scheduler.h>
#include <rtems/score/timecounter.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup CReqClockNanosleep spec:/c/req/clock-nanosleep
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  CReqClockNanosleep_Pre_ClockId_Monotonic,
  CReqClockNanosleep_Pre_ClockId_Realtime,
  CReqClockNanosleep_Pre_ClockId_Invalid,
  CReqClockNanosleep_Pre_ClockId_NA
} CReqClockNanosleep_Pre_ClockId;

typedef enum {
  CReqClockNanosleep_Pre_Abstime_Yes,
  CReqClockNanosleep_Pre_Abstime_No,
  CReqClockNanosleep_Pre_Abstime_NA
} CReqClockNanosleep_Pre_Abstime;

typedef enum {
  CReqClockNanosleep_Pre_RQTp_Valid,
  CReqClockNanosleep_Pre_RQTp_Null,
  CReqClockNanosleep_Pre_RQTp_NA
} CReqClockNanosleep_Pre_RQTp;

typedef enum {
  CReqClockNanosleep_Pre_RQTpNSec_Valid,
  CReqClockNanosleep_Pre_RQTpNSec_Invalid,
  CReqClockNanosleep_Pre_RQTpNSec_NA
} CReqClockNanosleep_Pre_RQTpNSec;

typedef enum {
  CReqClockNanosleep_Pre_RQTpSec_Negative,
  CReqClockNanosleep_Pre_RQTpSec_FarFuture,
  CReqClockNanosleep_Pre_RQTpSec_Future,
  CReqClockNanosleep_Pre_RQTpSec_PastOrNow,
  CReqClockNanosleep_Pre_RQTpSec_NA
} CReqClockNanosleep_Pre_RQTpSec;

typedef enum {
  CReqClockNanosleep_Pre_RMTp_Valid,
  CReqClockNanosleep_Pre_RMTp_Null,
  CReqClockNanosleep_Pre_RMTp_NA
} CReqClockNanosleep_Pre_RMTp;

typedef enum {
  CReqClockNanosleep_Post_Status_Zero,
  CReqClockNanosleep_Post_Status_ENOTSUP,
  CReqClockNanosleep_Post_Status_EINVAL,
  CReqClockNanosleep_Post_Status_NA
} CReqClockNanosleep_Post_Status;

typedef enum {
  CReqClockNanosleep_Post_Timer_Inactive,
  CReqClockNanosleep_Post_Timer_Monotonic,
  CReqClockNanosleep_Post_Timer_Realtime,
  CReqClockNanosleep_Post_Timer_NA
} CReqClockNanosleep_Post_Timer;

typedef enum {
  CReqClockNanosleep_Post_Expire_Last,
  CReqClockNanosleep_Post_Expire_Absolute,
  CReqClockNanosleep_Post_Expire_Relative,
  CReqClockNanosleep_Post_Expire_NA
} CReqClockNanosleep_Post_Expire;

typedef enum {
  CReqClockNanosleep_Post_Scheduler_Block,
  CReqClockNanosleep_Post_Scheduler_BlockUnblock,
  CReqClockNanosleep_Post_Scheduler_Nop,
  CReqClockNanosleep_Post_Scheduler_NA
} CReqClockNanosleep_Post_Scheduler;

typedef enum {
  CReqClockNanosleep_Post_RMTp_Zero,
  CReqClockNanosleep_Post_RMTp_Nop,
  CReqClockNanosleep_Post_RMTp_NA
} CReqClockNanosleep_Post_RMTp;

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
} CReqClockNanosleep_Entry;

/**
 * @brief Test context for spec:/c/req/clock-nanosleep test case.
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
   * @brief This member provides the object referenced by the ``rqtp``
   *   parameter.
   */
  struct timespec rqtp_obj;

  /**
   * @brief This member provides the object referenced by the ``rmtp``
   *   parameter.
   */
  struct timespec rmtp_obj;

  /**
   * @brief This member contains the return value of the clock_nanosleep()
   *   call.
   */
  int status;

  /**
   * @brief This member specifies the ``clock_id`` parameter value.
   */
  clockid_t clock_id;

  /**
   * @brief This member specifies the ``flags`` parameter value.
   */
  int flags;

  /**
   * @brief This member specifies the ``rqtp`` parameter value.
   */
  const struct timespec *rqtp;

  /**
   * @brief This member specifies the ``rmtp`` parameter value.
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
    CReqClockNanosleep_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} CReqClockNanosleep_Context;

static CReqClockNanosleep_Context
  CReqClockNanosleep_Instance;

static const char * const CReqClockNanosleep_PreDesc_ClockId[] = {
  "Monotonic",
  "Realtime",
  "Invalid",
  "NA"
};

static const char * const CReqClockNanosleep_PreDesc_Abstime[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const CReqClockNanosleep_PreDesc_RQTp[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const CReqClockNanosleep_PreDesc_RQTpNSec[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const CReqClockNanosleep_PreDesc_RQTpSec[] = {
  "Negative",
  "FarFuture",
  "Future",
  "PastOrNow",
  "NA"
};

static const char * const CReqClockNanosleep_PreDesc_RMTp[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const CReqClockNanosleep_PreDesc[] = {
  CReqClockNanosleep_PreDesc_ClockId,
  CReqClockNanosleep_PreDesc_Abstime,
  CReqClockNanosleep_PreDesc_RQTp,
  CReqClockNanosleep_PreDesc_RQTpNSec,
  CReqClockNanosleep_PreDesc_RQTpSec,
  CReqClockNanosleep_PreDesc_RMTp,
  NULL
};

typedef CReqClockNanosleep_Context Context;

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

static void CReqClockNanosleep_Pre_ClockId_Prepare(
  CReqClockNanosleep_Context    *ctx,
  CReqClockNanosleep_Pre_ClockId state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Pre_ClockId_Monotonic: {
      /*
       * While the ``clock_id`` parameter is equal to CLOCK_MONOTONIC.
       */
      ctx->clock_id = CLOCK_MONOTONIC;
      break;
    }

    case CReqClockNanosleep_Pre_ClockId_Realtime: {
      /*
       * While the ``clock_id`` parameter is equal to CLOCK_REALTIME.
       */
      ctx->clock_id = CLOCK_REALTIME;
      break;
    }

    case CReqClockNanosleep_Pre_ClockId_Invalid: {
      /*
       * While the ``clock_id`` parameter is an invalid clock identifier.
       */
      ctx->clock_id = INT_MAX;
      break;
    }

    case CReqClockNanosleep_Pre_ClockId_NA:
      break;
  }
}

static void CReqClockNanosleep_Pre_Abstime_Prepare(
  CReqClockNanosleep_Context    *ctx,
  CReqClockNanosleep_Pre_Abstime state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Pre_Abstime_Yes: {
      /*
       * While the ``flags`` parameter indicates an absolute time.
       */
      ctx->flags |= TIMER_ABSTIME;
      break;
    }

    case CReqClockNanosleep_Pre_Abstime_No: {
      /*
       * While the ``flags`` parameter does not indicate an absolute time.
       */
      /* This is the default */
      break;
    }

    case CReqClockNanosleep_Pre_Abstime_NA:
      break;
  }
}

static void CReqClockNanosleep_Pre_RQTp_Prepare(
  CReqClockNanosleep_Context *ctx,
  CReqClockNanosleep_Pre_RQTp state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Pre_RQTp_Valid: {
      /*
       * While the ``rqtp`` parameter references an object of type struct
       * timespec.
       */
      ctx->rqtp = &ctx->rqtp_obj;
      break;
    }

    case CReqClockNanosleep_Pre_RQTp_Null: {
      /*
       * While the ``rqtp`` parameter is equal to NULL.
       */
      ctx->rqtp = NULL;
      break;
    }

    case CReqClockNanosleep_Pre_RQTp_NA:
      break;
  }
}

static void CReqClockNanosleep_Pre_RQTpNSec_Prepare(
  CReqClockNanosleep_Context     *ctx,
  CReqClockNanosleep_Pre_RQTpNSec state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Pre_RQTpNSec_Valid: {
      /*
       * While the ``tv_nsec`` member of the object referenced by the ``rqtp``
       * parameter is a valid nanoseconds value.
       */
      ctx->rqtp_obj.tv_nsec = 999999999;
      break;
    }

    case CReqClockNanosleep_Pre_RQTpNSec_Invalid: {
      /*
       * While the ``tv_nsec`` member of the object referenced by the ``rqtp``
       * parameter is an invalid nanoseconds value.
       */
      ctx->rqtp_obj.tv_nsec = -1;
      break;
    }

    case CReqClockNanosleep_Pre_RQTpNSec_NA:
      break;
  }
}

static void CReqClockNanosleep_Pre_RQTpSec_Prepare(
  CReqClockNanosleep_Context    *ctx,
  CReqClockNanosleep_Pre_RQTpSec state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Pre_RQTpSec_Negative: {
      /*
       * While the ``tv_sec`` member of the object referenced by the ``rqtp``
       * parameter is negative.
       */
      ctx->rqtp_obj.tv_sec = -238479;
      break;
    }

    case CReqClockNanosleep_Pre_RQTpSec_FarFuture: {
      /*
       * While the ``tv_sec`` member of the object referenced by the ``rqtp``
       * parameter specifies a time point which is past the implementation
       * limit.
       */
      ctx->rqtp_obj.tv_sec = INT64_MAX;
      break;
    }

    case CReqClockNanosleep_Pre_RQTpSec_Future: {
      /*
       * While the ``tv_sec`` member of the object referenced by the ``rqtp``
       * parameter specifies a time point which is after the current time of
       * the clock specified by the ``clock_id`` parameter and is within the
       * implementation limits.
       */
      ctx->rqtp_obj.tv_sec = 1621322302;
      break;
    }

    case CReqClockNanosleep_Pre_RQTpSec_PastOrNow: {
      /*
       * While the ``tv_sec`` member of the object referenced by the ``rqtp``
       * parameter is non-negative and specifies a time point which is before
       * or at the current time of the clock specified by the ``clock_id``
       * parameter.
       */
      ctx->rqtp_obj.tv_sec = 0;

      if ( ctx->rqtp_obj.tv_nsec == 999999999 ) {
        ctx->rqtp_obj.tv_nsec = 0;
      }
      break;
    }

    case CReqClockNanosleep_Pre_RQTpSec_NA:
      break;
  }
}

static void CReqClockNanosleep_Pre_RMTp_Prepare(
  CReqClockNanosleep_Context *ctx,
  CReqClockNanosleep_Pre_RMTp state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Pre_RMTp_Valid: {
      /*
       * While the ``rmtp`` parameter references an object of type struct
       * timespec.
       */
      ctx->rmtp = &ctx->rmtp_obj;
      break;
    }

    case CReqClockNanosleep_Pre_RMTp_Null: {
      /*
       * While the ``rmtp`` parameter is equal to NULL.
       */
      ctx->rmtp = NULL;
      break;
    }

    case CReqClockNanosleep_Pre_RMTp_NA:
      break;
  }
}

static void CReqClockNanosleep_Post_Status_Check(
  CReqClockNanosleep_Context    *ctx,
  CReqClockNanosleep_Post_Status state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Post_Status_Zero: {
      /*
       * The return value of clock_nanosleep() shall be equal to zero.
       */
      T_eq_int( ctx->status, 0 );
      break;
    }

    case CReqClockNanosleep_Post_Status_ENOTSUP: {
      /*
       * The return value of clock_nanosleep() shall be equal to ENOTSUP.
       */
      T_eq_int( ctx->status, ENOTSUP );
      break;
    }

    case CReqClockNanosleep_Post_Status_EINVAL: {
      /*
       * The return value of clock_nanosleep() shall be equal to EINVAL.
       */
      T_eq_int( ctx->status, EINVAL );
      break;
    }

    case CReqClockNanosleep_Post_Status_NA:
      break;
  }
}

static void CReqClockNanosleep_Post_Timer_Check(
  CReqClockNanosleep_Context   *ctx,
  CReqClockNanosleep_Post_Timer state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Post_Timer_Inactive: {
      /*
       * The timer of the calling task shall be inactive.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_INACTIVE );
      break;
    }

    case CReqClockNanosleep_Post_Timer_Monotonic: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_MONOTONIC.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_MONOTONIC );
      break;
    }

    case CReqClockNanosleep_Post_Timer_Realtime: {
      /*
       * The timer of the calling task shall be active using the
       * CLOCK_REALTIME.
       */
      T_eq_int( ctx->timer_info.state, TASK_TIMER_REALTIME );
      break;
    }

    case CReqClockNanosleep_Post_Timer_NA:
      break;
  }
}

static void CReqClockNanosleep_Post_Expire_Check(
  CReqClockNanosleep_Context    *ctx,
  CReqClockNanosleep_Post_Expire state
)
{
  struct timespec expire;

  switch ( state ) {
    case CReqClockNanosleep_Post_Expire_Last: {
      /*
       * The timer of the calling task shall expire at the last valid time
       * point of the clock specified by the ``clock_id`` parameter.
       */
      T_eq_u64( ctx->timer_info.expire_ticks, 0xffffffffffffffff );
      break;
    }

    case CReqClockNanosleep_Post_Expire_Absolute: {
      /*
       * The timer of the calling task shall expire at the time point specified
       * by the ``rqtp`` parameter.
       */
      T_eq_i64( ctx->timer_info.expire_timespec.tv_sec, ctx->rqtp_obj.tv_sec );
      T_eq_long(
        ctx->timer_info.expire_timespec.tv_nsec,
        ctx->rqtp_obj.tv_nsec
      );
      break;
    }

    case CReqClockNanosleep_Post_Expire_Relative: {
      /*
       * The timer of the calling task shall expire at the time point specified
       * by the sum of the current time of the clock specified by
       * CLOCK_MONOTONIC and the interval specified by the ``rqtp`` parameter.
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

    case CReqClockNanosleep_Post_Expire_NA:
      break;
  }
}

static void CReqClockNanosleep_Post_Scheduler_Check(
  CReqClockNanosleep_Context       *ctx,
  CReqClockNanosleep_Post_Scheduler state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Post_Scheduler_Block: {
      /*
       * The calling task shall be blocked by the scheduler exactly once by the
       * clock_nanosleep() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].operation,
        T_SCHEDULER_BLOCK
      );
      break;
    }

    case CReqClockNanosleep_Post_Scheduler_BlockUnblock: {
      /*
       * The calling task shall be blocked exactly once by the scheduler and
       * then unblocked in the same thread dispatch critical section by the
       * clock_nanosleep() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 2 );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].operation,
        T_SCHEDULER_BLOCK
      );
      T_eq_int(
        ctx->scheduler_log.events[ 1 ].operation,
        T_SCHEDULER_UNBLOCK
      );
      break;
    }

    case CReqClockNanosleep_Post_Scheduler_Nop: {
      /*
       * The calling task shall not be altered by the scheduler by the
       * clock_nanosleep() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      break;
    }

    case CReqClockNanosleep_Post_Scheduler_NA:
      break;
  }
}

static void CReqClockNanosleep_Post_RMTp_Check(
  CReqClockNanosleep_Context  *ctx,
  CReqClockNanosleep_Post_RMTp state
)
{
  switch ( state ) {
    case CReqClockNanosleep_Post_RMTp_Zero: {
      /*
       * The object referenced by the ``rmtp`` parameter shall be cleared to
       * zero after the return of the clock_nanosleep() call.
       */
      T_eq_i64( ctx->rmtp_obj.tv_sec, 0 );
      T_eq_long( ctx->rmtp_obj.tv_nsec, 0 );
      break;
    }

    case CReqClockNanosleep_Post_RMTp_Nop: {
      /*
       * Objects referenced by the ``rmtp`` parameter in past calls to
       * clock_nanosleep() shall not be accessed by the clock_nanosleep() call.
       */
      T_eq_i64( ctx->rmtp_obj.tv_sec, -1 );
      T_eq_long( ctx->rmtp_obj.tv_nsec, -1 );
      break;
    }

    case CReqClockNanosleep_Post_RMTp_NA:
      break;
  }
}

static void CReqClockNanosleep_Setup( CReqClockNanosleep_Context *ctx )
{
  rtems_time_of_day now = { 1988, 1, 1, 0, 0, 0, 0 };
  T_rsc_success( rtems_clock_set( &now ) );
  SetSelfPriority( PRIO_NORMAL );
  ctx->worker_id = CreateTask( "WORK", PRIO_HIGH );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void CReqClockNanosleep_Setup_Wrap( void *arg )
{
  CReqClockNanosleep_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqClockNanosleep_Setup( ctx );
}

static void CReqClockNanosleep_Teardown( CReqClockNanosleep_Context *ctx )
{
  DeleteTask( ctx->worker_id );
  RestoreRunnerPriority();
}

static void CReqClockNanosleep_Teardown_Wrap( void *arg )
{
  CReqClockNanosleep_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  CReqClockNanosleep_Teardown( ctx );
}

static void CReqClockNanosleep_Prepare( CReqClockNanosleep_Context *ctx )
{
  ctx->status = -1;
  ctx->flags = 0;
  ctx->rmtp_obj.tv_sec = -1;
  ctx->rmtp_obj.tv_nsec = -1;
}

static void CReqClockNanosleep_Action( CReqClockNanosleep_Context *ctx )
{
  ResumeTask( ctx->worker_id );
  (void) T_scheduler_record( NULL );
  GetTaskTimerInfo( ctx->worker_id, &ctx->timer_info );
  ClockTick();
  FinalClockTick();
}

static const CReqClockNanosleep_Entry
CReqClockNanosleep_Entries[] = {
  { 0, 0, 0, 0, 1, 1, 0, CReqClockNanosleep_Post_Status_EINVAL,
    CReqClockNanosleep_Post_Timer_Inactive, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 1, 1, 0, CReqClockNanosleep_Post_Status_ENOTSUP,
    CReqClockNanosleep_Post_Timer_Inactive, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_Nop, CReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_EINVAL,
    CReqClockNanosleep_Post_Timer_Inactive, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 1, 1, 0, CReqClockNanosleep_Post_Status_EINVAL,
    CReqClockNanosleep_Post_Timer_Inactive, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_ENOTSUP,
    CReqClockNanosleep_Post_Timer_Inactive, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_Nop, CReqClockNanosleep_Post_RMTp_Nop },
  { 1, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_NA,
    CReqClockNanosleep_Post_Timer_NA, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_NA, CReqClockNanosleep_Post_RMTp_NA },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Inactive, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_EINVAL,
    CReqClockNanosleep_Post_Timer_Inactive, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Monotonic,
    CReqClockNanosleep_Post_Expire_Last,
    CReqClockNanosleep_Post_Scheduler_Block, CReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Monotonic,
    CReqClockNanosleep_Post_Expire_Absolute,
    CReqClockNanosleep_Post_Scheduler_Block, CReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Monotonic,
    CReqClockNanosleep_Post_Expire_Last,
    CReqClockNanosleep_Post_Scheduler_Block, CReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Monotonic,
    CReqClockNanosleep_Post_Expire_Relative,
    CReqClockNanosleep_Post_Scheduler_Block, CReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Monotonic,
    CReqClockNanosleep_Post_Expire_Relative,
    CReqClockNanosleep_Post_Scheduler_Block, CReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Inactive, CReqClockNanosleep_Post_Expire_NA,
    CReqClockNanosleep_Post_Scheduler_BlockUnblock,
    CReqClockNanosleep_Post_RMTp_Zero },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Realtime,
    CReqClockNanosleep_Post_Expire_Last,
    CReqClockNanosleep_Post_Scheduler_Block, CReqClockNanosleep_Post_RMTp_Nop },
  { 0, 0, 0, 0, 0, 0, 0, CReqClockNanosleep_Post_Status_Zero,
    CReqClockNanosleep_Post_Timer_Realtime,
    CReqClockNanosleep_Post_Expire_Absolute,
    CReqClockNanosleep_Post_Scheduler_Block, CReqClockNanosleep_Post_RMTp_Nop }
};

static const uint8_t
CReqClockNanosleep_Map[] = {
  6, 6, 8, 8, 9, 9, 6, 6, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 7, 2, 10, 8, 11, 12, 13, 6, 7, 2, 7, 2, 7, 2, 7, 2, 3, 0,
  3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 6, 6, 14, 14, 15, 15, 6, 6, 2, 2,
  2, 2, 2, 2, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 2, 10,
  8, 11, 12, 13, 6, 7, 2, 7, 2, 7, 2, 7, 2, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0, 3, 0,
  3, 0, 3, 0, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 4, 4, 4, 4, 5, 5, 5, 5, 4, 4, 4, 4, 5, 5, 5, 5,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
};

static size_t CReqClockNanosleep_Scope( void *arg, char *buf, size_t n )
{
  CReqClockNanosleep_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( CReqClockNanosleep_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture CReqClockNanosleep_Fixture = {
  .setup = CReqClockNanosleep_Setup_Wrap,
  .stop = NULL,
  .teardown = CReqClockNanosleep_Teardown_Wrap,
  .scope = CReqClockNanosleep_Scope,
  .initial_context = &CReqClockNanosleep_Instance
};

static inline CReqClockNanosleep_Entry CReqClockNanosleep_PopEntry(
  CReqClockNanosleep_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return CReqClockNanosleep_Entries[
    CReqClockNanosleep_Map[ index ]
  ];
}

static void CReqClockNanosleep_SetPreConditionStates(
  CReqClockNanosleep_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_RQTpNSec_NA ) {
    ctx->Map.pcs[ 3 ] = CReqClockNanosleep_Pre_RQTpNSec_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_RQTpSec_NA ) {
    ctx->Map.pcs[ 4 ] = CReqClockNanosleep_Pre_RQTpSec_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }

  ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
}

static void CReqClockNanosleep_TestVariant( CReqClockNanosleep_Context *ctx )
{
  CReqClockNanosleep_Pre_ClockId_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  CReqClockNanosleep_Pre_Abstime_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  CReqClockNanosleep_Pre_RQTp_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  CReqClockNanosleep_Pre_RQTpNSec_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  CReqClockNanosleep_Pre_RQTpSec_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  CReqClockNanosleep_Pre_RMTp_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  CReqClockNanosleep_Action( ctx );
  CReqClockNanosleep_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  CReqClockNanosleep_Post_Timer_Check( ctx, ctx->Map.entry.Post_Timer );
  CReqClockNanosleep_Post_Expire_Check( ctx, ctx->Map.entry.Post_Expire );
  CReqClockNanosleep_Post_Scheduler_Check(
    ctx,
    ctx->Map.entry.Post_Scheduler
  );
  CReqClockNanosleep_Post_RMTp_Check( ctx, ctx->Map.entry.Post_RMTp );
}

/**
 * @fn void T_case_body_CReqClockNanosleep( void )
 */
T_TEST_CASE_FIXTURE( CReqClockNanosleep, &CReqClockNanosleep_Fixture )
{
  CReqClockNanosleep_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = CReqClockNanosleep_Pre_ClockId_Monotonic;
    ctx->Map.pci[ 0 ] < CReqClockNanosleep_Pre_ClockId_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = CReqClockNanosleep_Pre_Abstime_Yes;
      ctx->Map.pci[ 1 ] < CReqClockNanosleep_Pre_Abstime_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = CReqClockNanosleep_Pre_RQTp_Valid;
        ctx->Map.pci[ 2 ] < CReqClockNanosleep_Pre_RQTp_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = CReqClockNanosleep_Pre_RQTpNSec_Valid;
          ctx->Map.pci[ 3 ] < CReqClockNanosleep_Pre_RQTpNSec_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = CReqClockNanosleep_Pre_RQTpSec_Negative;
            ctx->Map.pci[ 4 ] < CReqClockNanosleep_Pre_RQTpSec_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = CReqClockNanosleep_Pre_RMTp_Valid;
              ctx->Map.pci[ 5 ] < CReqClockNanosleep_Pre_RMTp_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = CReqClockNanosleep_PopEntry( ctx );

              if ( ctx->Map.entry.Skip ) {
                continue;
              }

              CReqClockNanosleep_SetPreConditionStates( ctx );
              CReqClockNanosleep_Prepare( ctx );
              CReqClockNanosleep_TestVariant( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
