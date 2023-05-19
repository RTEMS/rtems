/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsRatemonReqCreate
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
 * @defgroup RtemsRatemonReqCreate spec:/rtems/ratemon/req/create
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsRatemonReqCreate_Pre_Name_Valid,
  RtemsRatemonReqCreate_Pre_Name_Invalid,
  RtemsRatemonReqCreate_Pre_Name_NA
} RtemsRatemonReqCreate_Pre_Name;

typedef enum {
  RtemsRatemonReqCreate_Pre_Id_Valid,
  RtemsRatemonReqCreate_Pre_Id_Null,
  RtemsRatemonReqCreate_Pre_Id_NA
} RtemsRatemonReqCreate_Pre_Id;

typedef enum {
  RtemsRatemonReqCreate_Pre_Free_Yes,
  RtemsRatemonReqCreate_Pre_Free_No,
  RtemsRatemonReqCreate_Pre_Free_NA
} RtemsRatemonReqCreate_Pre_Free;

typedef enum {
  RtemsRatemonReqCreate_Post_Status_Ok,
  RtemsRatemonReqCreate_Post_Status_InvName,
  RtemsRatemonReqCreate_Post_Status_InvAddr,
  RtemsRatemonReqCreate_Post_Status_TooMany,
  RtemsRatemonReqCreate_Post_Status_NA
} RtemsRatemonReqCreate_Post_Status;

typedef enum {
  RtemsRatemonReqCreate_Post_Name_Valid,
  RtemsRatemonReqCreate_Post_Name_Invalid,
  RtemsRatemonReqCreate_Post_Name_NA
} RtemsRatemonReqCreate_Post_Name;

typedef enum {
  RtemsRatemonReqCreate_Post_IdVar_Set,
  RtemsRatemonReqCreate_Post_IdVar_Nop,
  RtemsRatemonReqCreate_Post_IdVar_NA
} RtemsRatemonReqCreate_Post_IdVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Free_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Name : 2;
  uint16_t Post_IdVar : 2;
} RtemsRatemonReqCreate_Entry;

/**
 * @brief Test context for spec:/rtems/ratemon/req/create test case.
 */
typedef struct {
  void *seized_objects;

  rtems_id id_value;

  rtems_name name;

  rtems_id *id;

  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 3 ];

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
    RtemsRatemonReqCreate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsRatemonReqCreate_Context;

static RtemsRatemonReqCreate_Context
  RtemsRatemonReqCreate_Instance;

static const char * const RtemsRatemonReqCreate_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsRatemonReqCreate_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsRatemonReqCreate_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsRatemonReqCreate_PreDesc[] = {
  RtemsRatemonReqCreate_PreDesc_Name,
  RtemsRatemonReqCreate_PreDesc_Id,
  RtemsRatemonReqCreate_PreDesc_Free,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

static rtems_status_code Create( void *arg, uint32_t *id )
{
  return rtems_rate_monotonic_create(
    rtems_build_name( 'S', 'I', 'Z', 'E' ),
    id
  );
}

static void RtemsRatemonReqCreate_Pre_Name_Prepare(
  RtemsRatemonReqCreate_Context *ctx,
  RtemsRatemonReqCreate_Pre_Name state
)
{
  switch ( state ) {
    case RtemsRatemonReqCreate_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is valid.
       */
      ctx->name = NAME;
      break;
    }

    case RtemsRatemonReqCreate_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is invalid.
       */
      ctx->name = 0;
      break;
    }

    case RtemsRatemonReqCreate_Pre_Name_NA:
      break;
  }
}

static void RtemsRatemonReqCreate_Pre_Id_Prepare(
  RtemsRatemonReqCreate_Context *ctx,
  RtemsRatemonReqCreate_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsRatemonReqCreate_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsRatemonReqCreate_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsRatemonReqCreate_Pre_Id_NA:
      break;
  }
}

static void RtemsRatemonReqCreate_Pre_Free_Prepare(
  RtemsRatemonReqCreate_Context *ctx,
  RtemsRatemonReqCreate_Pre_Free state
)
{
  switch ( state ) {
    case RtemsRatemonReqCreate_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive period object available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsRatemonReqCreate_Pre_Free_No: {
      /*
       * While the system has no inactive period object available.
       */
      ctx->seized_objects = T_seize_objects( Create, NULL );
      break;
    }

    case RtemsRatemonReqCreate_Pre_Free_NA:
      break;
  }
}

static void RtemsRatemonReqCreate_Post_Status_Check(
  RtemsRatemonReqCreate_Context    *ctx,
  RtemsRatemonReqCreate_Post_Status state
)
{
  switch ( state ) {
    case RtemsRatemonReqCreate_Post_Status_Ok: {
      /*
       * The return status of rtems_rate_monotonic_create() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsRatemonReqCreate_Post_Status_InvName: {
      /*
       * The return status of rtems_rate_monotonic_create() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsRatemonReqCreate_Post_Status_InvAddr: {
      /*
       * The return status of rtems_rate_monotonic_create() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsRatemonReqCreate_Post_Status_TooMany: {
      /*
       * The return status of rtems_rate_monotonic_create() shall be
       * RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsRatemonReqCreate_Post_Status_NA:
      break;
  }
}

static void RtemsRatemonReqCreate_Post_Name_Check(
  RtemsRatemonReqCreate_Context  *ctx,
  RtemsRatemonReqCreate_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsRatemonReqCreate_Post_Name_Valid: {
      /*
       * The unique object name shall identify the period created by the
       * rtems_rate_monotonic_create() call.
       */
      id = 0;
      sc = rtems_rate_monotonic_ident( NAME, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsRatemonReqCreate_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a period.
       */
      sc = rtems_rate_monotonic_ident( NAME, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsRatemonReqCreate_Post_Name_NA:
      break;
  }
}

static void RtemsRatemonReqCreate_Post_IdVar_Check(
  RtemsRatemonReqCreate_Context   *ctx,
  RtemsRatemonReqCreate_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsRatemonReqCreate_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the object identifier of the created period after the return of
       * the rtems_rate_monotonic_create() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsRatemonReqCreate_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_rate_monotonic_create() shall not be accessed by the
       * rtems_rate_monotonic_create() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsRatemonReqCreate_Post_IdVar_NA:
      break;
  }
}

static void RtemsRatemonReqCreate_Setup( RtemsRatemonReqCreate_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->id_value = INVALID_ID;
}

static void RtemsRatemonReqCreate_Setup_Wrap( void *arg )
{
  RtemsRatemonReqCreate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsRatemonReqCreate_Setup( ctx );
}

static void RtemsRatemonReqCreate_Action( RtemsRatemonReqCreate_Context *ctx )
{
  ctx->status = rtems_rate_monotonic_create( ctx->name, ctx->id );
}

static void RtemsRatemonReqCreate_Cleanup( RtemsRatemonReqCreate_Context *ctx )
{
  if ( ctx->id_value != INVALID_ID ) {
    rtems_status_code sc;

    sc = rtems_rate_monotonic_delete( ctx->id_value );
    T_rsc_success( sc );

    ctx->id_value = INVALID_ID;
  }

  T_surrender_objects( &ctx->seized_objects, rtems_rate_monotonic_delete );
}

static const RtemsRatemonReqCreate_Entry
RtemsRatemonReqCreate_Entries[] = {
  { 0, 0, 0, 0, RtemsRatemonReqCreate_Post_Status_InvName,
    RtemsRatemonReqCreate_Post_Name_Invalid,
    RtemsRatemonReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, RtemsRatemonReqCreate_Post_Status_InvAddr,
    RtemsRatemonReqCreate_Post_Name_Invalid,
    RtemsRatemonReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, RtemsRatemonReqCreate_Post_Status_Ok,
    RtemsRatemonReqCreate_Post_Name_Valid, RtemsRatemonReqCreate_Post_IdVar_Set },
  { 0, 0, 0, 0, RtemsRatemonReqCreate_Post_Status_TooMany,
    RtemsRatemonReqCreate_Post_Name_Invalid,
    RtemsRatemonReqCreate_Post_IdVar_Nop }
};

static const uint8_t
RtemsRatemonReqCreate_Map[] = {
  2, 3, 1, 1, 0, 0, 0, 0
};

static size_t RtemsRatemonReqCreate_Scope( void *arg, char *buf, size_t n )
{
  RtemsRatemonReqCreate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsRatemonReqCreate_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsRatemonReqCreate_Fixture = {
  .setup = RtemsRatemonReqCreate_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsRatemonReqCreate_Scope,
  .initial_context = &RtemsRatemonReqCreate_Instance
};

static inline RtemsRatemonReqCreate_Entry RtemsRatemonReqCreate_PopEntry(
  RtemsRatemonReqCreate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsRatemonReqCreate_Entries[
    RtemsRatemonReqCreate_Map[ index ]
  ];
}

static void RtemsRatemonReqCreate_TestVariant(
  RtemsRatemonReqCreate_Context *ctx
)
{
  RtemsRatemonReqCreate_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsRatemonReqCreate_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsRatemonReqCreate_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsRatemonReqCreate_Action( ctx );
  RtemsRatemonReqCreate_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsRatemonReqCreate_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsRatemonReqCreate_Post_IdVar_Check( ctx, ctx->Map.entry.Post_IdVar );
}

/**
 * @fn void T_case_body_RtemsRatemonReqCreate( void )
 */
T_TEST_CASE_FIXTURE( RtemsRatemonReqCreate, &RtemsRatemonReqCreate_Fixture )
{
  RtemsRatemonReqCreate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsRatemonReqCreate_Pre_Name_Valid;
    ctx->Map.pcs[ 0 ] < RtemsRatemonReqCreate_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsRatemonReqCreate_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsRatemonReqCreate_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsRatemonReqCreate_Pre_Free_Yes;
        ctx->Map.pcs[ 2 ] < RtemsRatemonReqCreate_Pre_Free_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsRatemonReqCreate_PopEntry( ctx );
        RtemsRatemonReqCreate_TestVariant( ctx );
        RtemsRatemonReqCreate_Cleanup( ctx );
      }
    }
  }
}

/** @} */
