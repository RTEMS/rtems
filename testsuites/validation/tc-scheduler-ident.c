/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerReqIdent
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

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSchedulerReqIdent spec:/rtems/scheduler/req/ident
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSchedulerReqIdent_Pre_Name_Invalid,
  RtemsSchedulerReqIdent_Pre_Name_Valid,
  RtemsSchedulerReqIdent_Pre_Name_NA
} RtemsSchedulerReqIdent_Pre_Name;

typedef enum {
  RtemsSchedulerReqIdent_Pre_Id_Valid,
  RtemsSchedulerReqIdent_Pre_Id_Null,
  RtemsSchedulerReqIdent_Pre_Id_NA
} RtemsSchedulerReqIdent_Pre_Id;

typedef enum {
  RtemsSchedulerReqIdent_Post_Status_Ok,
  RtemsSchedulerReqIdent_Post_Status_InvAddr,
  RtemsSchedulerReqIdent_Post_Status_InvName,
  RtemsSchedulerReqIdent_Post_Status_NA
} RtemsSchedulerReqIdent_Post_Status;

typedef enum {
  RtemsSchedulerReqIdent_Post_IdVar_Set,
  RtemsSchedulerReqIdent_Post_IdVar_Nop,
  RtemsSchedulerReqIdent_Post_IdVar_NA
} RtemsSchedulerReqIdent_Post_IdVar;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Name_NA : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_IdVar : 2;
} RtemsSchedulerReqIdent_Entry;

/**
 * @brief Test context for spec:/rtems/scheduler/req/ident test case.
 */
typedef struct {
  /**
   * @brief This member provides the object referenced by the ``id`` parameter.
   */
  rtems_id id_value;

  /**
   * @brief This member contains the return value of the
   *   rtems_scheduler_ident() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``name`` parameter value.
   */
  rtems_name name;

  /**
   * @brief This member specifies if the ``id`` parameter value.
   */
  rtems_id *id;

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
    RtemsSchedulerReqIdent_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSchedulerReqIdent_Context;

static RtemsSchedulerReqIdent_Context
  RtemsSchedulerReqIdent_Instance;

static const char * const RtemsSchedulerReqIdent_PreDesc_Name[] = {
  "Invalid",
  "Valid",
  "NA"
};

static const char * const RtemsSchedulerReqIdent_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsSchedulerReqIdent_PreDesc[] = {
  RtemsSchedulerReqIdent_PreDesc_Name,
  RtemsSchedulerReqIdent_PreDesc_Id,
  NULL
};

static void RtemsSchedulerReqIdent_Pre_Name_Prepare(
  RtemsSchedulerReqIdent_Context *ctx,
  RtemsSchedulerReqIdent_Pre_Name state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdent_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is not associated with a scheduler.
       */
      ctx->name = 0;
      break;
    }

    case RtemsSchedulerReqIdent_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is associated with a scheduler.
       */
      ctx->name = TEST_SCHEDULER_A_NAME;
      break;
    }

    case RtemsSchedulerReqIdent_Pre_Name_NA:
      break;
  }
}

static void RtemsSchedulerReqIdent_Pre_Id_Prepare(
  RtemsSchedulerReqIdent_Context *ctx,
  RtemsSchedulerReqIdent_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdent_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsSchedulerReqIdent_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is equal to NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsSchedulerReqIdent_Pre_Id_NA:
      break;
  }
}

static void RtemsSchedulerReqIdent_Post_Status_Check(
  RtemsSchedulerReqIdent_Context    *ctx,
  RtemsSchedulerReqIdent_Post_Status state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdent_Post_Status_Ok: {
      /*
       * The return status of rtems_scheduler_ident() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSchedulerReqIdent_Post_Status_InvAddr: {
      /*
       * The return status of rtems_scheduler_ident() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsSchedulerReqIdent_Post_Status_InvName: {
      /*
       * The return status of rtems_scheduler_ident() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsSchedulerReqIdent_Post_Status_NA:
      break;
  }
}

static void RtemsSchedulerReqIdent_Post_IdVar_Check(
  RtemsSchedulerReqIdent_Context   *ctx,
  RtemsSchedulerReqIdent_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdent_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the identifier of the scheduler with the lowest scheduler index
       * and a name equal to the ``name`` parameter after the return of the
       * rtems_scheduler_ident() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_eq_u32( ctx->id_value, 0x0f010001 );
      break;
    }

    case RtemsSchedulerReqIdent_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_scheduler_ident() shall not be accessed by the
       * rtems_scheduler_ident() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsSchedulerReqIdent_Post_IdVar_NA:
      break;
  }
}

static void RtemsSchedulerReqIdent_Prepare(
  RtemsSchedulerReqIdent_Context *ctx
)
{
  ctx->id_value = INVALID_ID;
}

static void RtemsSchedulerReqIdent_Action(
  RtemsSchedulerReqIdent_Context *ctx
)
{
  ctx->status = rtems_scheduler_ident( ctx->name, ctx->id );
}

static const RtemsSchedulerReqIdent_Entry
RtemsSchedulerReqIdent_Entries[] = {
  { 0, 0, 0, RtemsSchedulerReqIdent_Post_Status_InvAddr,
    RtemsSchedulerReqIdent_Post_IdVar_Nop },
  { 0, 0, 0, RtemsSchedulerReqIdent_Post_Status_InvName,
    RtemsSchedulerReqIdent_Post_IdVar_Nop },
  { 0, 0, 0, RtemsSchedulerReqIdent_Post_Status_Ok,
    RtemsSchedulerReqIdent_Post_IdVar_Set }
};

static const uint8_t
RtemsSchedulerReqIdent_Map[] = {
  1, 0, 2, 0
};

static size_t RtemsSchedulerReqIdent_Scope( void *arg, char *buf, size_t n )
{
  RtemsSchedulerReqIdent_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsSchedulerReqIdent_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsSchedulerReqIdent_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsSchedulerReqIdent_Scope,
  .initial_context = &RtemsSchedulerReqIdent_Instance
};

static inline RtemsSchedulerReqIdent_Entry RtemsSchedulerReqIdent_PopEntry(
  RtemsSchedulerReqIdent_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSchedulerReqIdent_Entries[
    RtemsSchedulerReqIdent_Map[ index ]
  ];
}

static void RtemsSchedulerReqIdent_TestVariant(
  RtemsSchedulerReqIdent_Context *ctx
)
{
  RtemsSchedulerReqIdent_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSchedulerReqIdent_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSchedulerReqIdent_Action( ctx );
  RtemsSchedulerReqIdent_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsSchedulerReqIdent_Post_IdVar_Check( ctx, ctx->Map.entry.Post_IdVar );
}

/**
 * @fn void T_case_body_RtemsSchedulerReqIdent( void )
 */
T_TEST_CASE_FIXTURE( RtemsSchedulerReqIdent, &RtemsSchedulerReqIdent_Fixture )
{
  RtemsSchedulerReqIdent_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSchedulerReqIdent_Pre_Name_Invalid;
    ctx->Map.pcs[ 0 ] < RtemsSchedulerReqIdent_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSchedulerReqIdent_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsSchedulerReqIdent_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsSchedulerReqIdent_PopEntry( ctx );
      RtemsSchedulerReqIdent_Prepare( ctx );
      RtemsSchedulerReqIdent_TestVariant( ctx );
    }
  }
}

/** @} */
