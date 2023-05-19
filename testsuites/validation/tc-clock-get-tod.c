/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsClockReqGetTod
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsClockReqGetTod spec:/rtems/clock/req/get-tod
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsClockReqGetTod_Pre_ToD_Arbitrary,
  RtemsClockReqGetTod_Pre_ToD_Leap4,
  RtemsClockReqGetTod_Pre_ToD_Leap400,
  RtemsClockReqGetTod_Pre_ToD_Youngest,
  RtemsClockReqGetTod_Pre_ToD_Oldest,
  RtemsClockReqGetTod_Pre_ToD_NotSet,
  RtemsClockReqGetTod_Pre_ToD_NA
} RtemsClockReqGetTod_Pre_ToD;

typedef enum {
  RtemsClockReqGetTod_Pre_Param_Valid,
  RtemsClockReqGetTod_Pre_Param_Null,
  RtemsClockReqGetTod_Pre_Param_NA
} RtemsClockReqGetTod_Pre_Param;

typedef enum {
  RtemsClockReqGetTod_Post_Status_Ok,
  RtemsClockReqGetTod_Post_Status_InvAddr,
  RtemsClockReqGetTod_Post_Status_NotDef,
  RtemsClockReqGetTod_Post_Status_NA
} RtemsClockReqGetTod_Post_Status;

typedef enum {
  RtemsClockReqGetTod_Post_Value_TimeOfDay,
  RtemsClockReqGetTod_Post_Value_Unchanged,
  RtemsClockReqGetTod_Post_Value_NA
} RtemsClockReqGetTod_Post_Value;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_ToD_NA : 1;
  uint8_t Pre_Param_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Value : 2;
} RtemsClockReqGetTod_Entry;

/**
 * @brief Test context for spec:/rtems/clock/req/get-tod test case.
 */
typedef struct {
  rtems_status_code set_tod_status;

  rtems_time_of_day set_tod_value;

  rtems_time_of_day *get_tod_ref;

  rtems_time_of_day get_tod_value;

  rtems_status_code get_tod_status;

  bool isDef;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    RtemsClockReqGetTod_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsClockReqGetTod_Context;

static RtemsClockReqGetTod_Context
  RtemsClockReqGetTod_Instance;

static const char * const RtemsClockReqGetTod_PreDesc_ToD[] = {
  "Arbitrary",
  "Leap4",
  "Leap400",
  "Youngest",
  "Oldest",
  "NotSet",
  "NA"
};

static const char * const RtemsClockReqGetTod_PreDesc_Param[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsClockReqGetTod_PreDesc[] = {
  RtemsClockReqGetTod_PreDesc_ToD,
  RtemsClockReqGetTod_PreDesc_Param,
  NULL
};

static void RtemsClockReqGetTod_Pre_ToD_Prepare(
  RtemsClockReqGetTod_Context *ctx,
  RtemsClockReqGetTod_Pre_ToD  state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_ToD_Arbitrary: {
      /*
       * While the CLOCK_REALTIME indicates an arbitrary valid date and time
       * between 1988-01-01T00:00:00.000000000Z and
       * 2514-05-30T01:53:03.999999999Z.
       */
      ctx->set_tod_value =
        (rtems_time_of_day) { 2023, 12, 27, 6, 7, 8,
        rtems_clock_get_ticks_per_second() / 4 };
      break;
    }

    case RtemsClockReqGetTod_Pre_ToD_Leap4: {
      /*
       * While the CLOCK_REALTIME indicates a date for a leap year with the
       * value of 29th of February.
       */
      ctx->set_tod_value =
        (rtems_time_of_day) { 2096, 2, 29, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqGetTod_Pre_ToD_Leap400: {
      /*
       * While the CLOCK_REALTIME indicates a date for a leap year with the
       * value of 29th of February.
       */
      ctx->set_tod_value =
        (rtems_time_of_day) { 2000, 2, 29, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqGetTod_Pre_ToD_Youngest: {
      /*
       * While the CLOCK_REALTIME indicates the youngest date and time accepted
       * (1988-01-01T00:00:00.000000000Z).
       */
      ctx->set_tod_value =
        (rtems_time_of_day) { 1988, 1, 1, 0, 0, 0, 0 };
      break;
    }

    case RtemsClockReqGetTod_Pre_ToD_Oldest: {
      /*
       * While the CLOCK_REALTIME indicates the oldest date and time accepted
       * (2099-12-31T23:59:59.999999999Z).
       */
      ctx->set_tod_value =
        (rtems_time_of_day) { 2099, 12, 31, 23, 59, 59,
        rtems_clock_get_ticks_per_second() - 1 };
      break;
    }

    case RtemsClockReqGetTod_Pre_ToD_NotSet: {
      /*
       * While the CLOCK_REALTIME has not been set before.
       */
      ctx->isDef = false;
      break;
    }

    case RtemsClockReqGetTod_Pre_ToD_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Pre_Param_Prepare(
  RtemsClockReqGetTod_Context  *ctx,
  RtemsClockReqGetTod_Pre_Param state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Pre_Param_Valid: {
      /*
       * While the ``time_of_day`` parameter references an object of type
       * rtems_time_of_day.
       */
      ctx->get_tod_ref = &ctx->get_tod_value;
      break;
    }

    case RtemsClockReqGetTod_Pre_Param_Null: {
      /*
       * While the ``time_of_day`` parameter is NULL.
       */
      ctx->get_tod_ref = NULL;
      break;
    }

    case RtemsClockReqGetTod_Pre_Param_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Post_Status_Check(
  RtemsClockReqGetTod_Context    *ctx,
  RtemsClockReqGetTod_Post_Status state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Post_Status_Ok: {
      /*
       * The return status of rtems_clock_get_tod() shall be RTEMS_SUCCESSFUL
       */
      T_rsc_success( ctx->set_tod_status );
      T_rsc_success( ctx->get_tod_status );
      break;
    }

    case RtemsClockReqGetTod_Post_Status_InvAddr: {
      /*
       * The return status of rtems_clock_get_tod() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->get_tod_status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsClockReqGetTod_Post_Status_NotDef: {
      /*
       * The return status of rtems_clock_get_tod() shall be RTEMS_NOT_DEFINED.
       */
      T_rsc( ctx->get_tod_status, RTEMS_NOT_DEFINED );
      break;
    }

    case RtemsClockReqGetTod_Post_Status_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Post_Value_Check(
  RtemsClockReqGetTod_Context   *ctx,
  RtemsClockReqGetTod_Post_Value state
)
{
  switch ( state ) {
    case RtemsClockReqGetTod_Post_Value_TimeOfDay: {
      /*
       * The value of the object referenced by the ``time_of_day`` parameter
       * shall be set to the value of the CLOCK_REALTIME at a point in time
       * during the call to rtems_clock_get_tod().
       */
      T_eq_ptr( ctx->get_tod_ref, &ctx->get_tod_value );
      T_eq_u32( ctx->get_tod_value.year,   ctx->set_tod_value.year );
      T_eq_u32( ctx->get_tod_value.month,  ctx->set_tod_value.month );
      T_eq_u32( ctx->get_tod_value.day,    ctx->set_tod_value.day );
      T_eq_u32( ctx->get_tod_value.hour,   ctx->set_tod_value.hour );
      T_eq_u32( ctx->get_tod_value.minute, ctx->set_tod_value.minute );
      T_eq_u32( ctx->get_tod_value.second, ctx->set_tod_value.second );
      /* rtems_clock_set() or rtems_clock_get_tod() cause an error of 1 tick */
      T_ge_u32( ctx->get_tod_value.ticks + 1,  ctx->set_tod_value.ticks );
      T_le_u32( ctx->get_tod_value.ticks, ctx->set_tod_value.ticks );
      break;
    }

    case RtemsClockReqGetTod_Post_Value_Unchanged: {
      /*
       * Object referenced by the ``time_of_day`` parameter in past call to
       * rtems_clock_get_tod() shall not be modified by the
       * rtems_clock_get_tod() call.
       */
      T_eq_u32( ctx->get_tod_value.year,   1 );
      T_eq_u32( ctx->get_tod_value.month,  1 );
      T_eq_u32( ctx->get_tod_value.day,    1 );
      T_eq_u32( ctx->get_tod_value.hour,   1 );
      T_eq_u32( ctx->get_tod_value.minute, 1 );
      T_eq_u32( ctx->get_tod_value.second, 1 );
      T_eq_u32( ctx->get_tod_value.ticks,  1 );
      break;
    }

    case RtemsClockReqGetTod_Post_Value_NA:
      break;
  }
}

static void RtemsClockReqGetTod_Prepare( RtemsClockReqGetTod_Context *ctx )
{
  ctx->get_tod_value = (rtems_time_of_day) { 1, 1, 1, 1, 1, 1, 1 };
  ctx->get_tod_ref = &ctx->get_tod_value;
  ctx->set_tod_value = (rtems_time_of_day) { 2023, 4, 5, 6, 7, 8, 0 };
  ctx->isDef = true;
}

static void RtemsClockReqGetTod_Action( RtemsClockReqGetTod_Context *ctx )
{
  if ( ctx->isDef ) {
    ctx->set_tod_status = rtems_clock_set( &ctx->set_tod_value );
    ctx->get_tod_status = rtems_clock_get_tod( ctx->get_tod_ref );
  } else {
    UnsetClock();
    ctx->get_tod_status = rtems_clock_get_tod( ctx->get_tod_ref );
  }
}

static void RtemsClockReqGetTod_Cleanup( RtemsClockReqGetTod_Context *ctx )
{
  UnsetClock();
}

static const RtemsClockReqGetTod_Entry
RtemsClockReqGetTod_Entries[] = {
  { 0, 0, 0, RtemsClockReqGetTod_Post_Status_InvAddr,
    RtemsClockReqGetTod_Post_Value_Unchanged },
  { 0, 0, 0, RtemsClockReqGetTod_Post_Status_Ok,
    RtemsClockReqGetTod_Post_Value_TimeOfDay },
  { 0, 0, 0, RtemsClockReqGetTod_Post_Status_NotDef,
    RtemsClockReqGetTod_Post_Value_Unchanged }
};

static const uint8_t
RtemsClockReqGetTod_Map[] = {
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0
};

static size_t RtemsClockReqGetTod_Scope( void *arg, char *buf, size_t n )
{
  RtemsClockReqGetTod_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsClockReqGetTod_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsClockReqGetTod_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsClockReqGetTod_Scope,
  .initial_context = &RtemsClockReqGetTod_Instance
};

static inline RtemsClockReqGetTod_Entry RtemsClockReqGetTod_PopEntry(
  RtemsClockReqGetTod_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsClockReqGetTod_Entries[
    RtemsClockReqGetTod_Map[ index ]
  ];
}

static void RtemsClockReqGetTod_TestVariant( RtemsClockReqGetTod_Context *ctx )
{
  RtemsClockReqGetTod_Pre_ToD_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsClockReqGetTod_Pre_Param_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsClockReqGetTod_Action( ctx );
  RtemsClockReqGetTod_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsClockReqGetTod_Post_Value_Check( ctx, ctx->Map.entry.Post_Value );
}

/**
 * @fn void T_case_body_RtemsClockReqGetTod( void )
 */
T_TEST_CASE_FIXTURE( RtemsClockReqGetTod, &RtemsClockReqGetTod_Fixture )
{
  RtemsClockReqGetTod_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsClockReqGetTod_Pre_ToD_Arbitrary;
    ctx->Map.pcs[ 0 ] < RtemsClockReqGetTod_Pre_ToD_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsClockReqGetTod_Pre_Param_Valid;
      ctx->Map.pcs[ 1 ] < RtemsClockReqGetTod_Pre_Param_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsClockReqGetTod_PopEntry( ctx );
      RtemsClockReqGetTod_Prepare( ctx );
      RtemsClockReqGetTod_TestVariant( ctx );
      RtemsClockReqGetTod_Cleanup( ctx );
    }
  }
}

/** @} */
