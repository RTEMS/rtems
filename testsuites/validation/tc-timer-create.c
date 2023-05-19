/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTimerReqCreate
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
 * @defgroup RtemsTimerReqCreate spec:/rtems/timer/req/create
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTimerReqCreate_Pre_Name_Valid,
  RtemsTimerReqCreate_Pre_Name_Invalid,
  RtemsTimerReqCreate_Pre_Name_NA
} RtemsTimerReqCreate_Pre_Name;

typedef enum {
  RtemsTimerReqCreate_Pre_Id_Valid,
  RtemsTimerReqCreate_Pre_Id_Null,
  RtemsTimerReqCreate_Pre_Id_NA
} RtemsTimerReqCreate_Pre_Id;

typedef enum {
  RtemsTimerReqCreate_Pre_Free_Yes,
  RtemsTimerReqCreate_Pre_Free_No,
  RtemsTimerReqCreate_Pre_Free_NA
} RtemsTimerReqCreate_Pre_Free;

typedef enum {
  RtemsTimerReqCreate_Post_Status_Ok,
  RtemsTimerReqCreate_Post_Status_InvName,
  RtemsTimerReqCreate_Post_Status_InvAddr,
  RtemsTimerReqCreate_Post_Status_TooMany,
  RtemsTimerReqCreate_Post_Status_NA
} RtemsTimerReqCreate_Post_Status;

typedef enum {
  RtemsTimerReqCreate_Post_Name_Valid,
  RtemsTimerReqCreate_Post_Name_Invalid,
  RtemsTimerReqCreate_Post_Name_NA
} RtemsTimerReqCreate_Post_Name;

typedef enum {
  RtemsTimerReqCreate_Post_IdVar_Set,
  RtemsTimerReqCreate_Post_IdVar_Nop,
  RtemsTimerReqCreate_Post_IdVar_NA
} RtemsTimerReqCreate_Post_IdVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Free_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Name : 2;
  uint16_t Post_IdVar : 2;
} RtemsTimerReqCreate_Entry;

/**
 * @brief Test context for spec:/rtems/timer/req/create test case.
 */
typedef struct {
  /**
   * @brief This member is used by the T_seize_objects() and
   *   T_surrender_objects() support functions.
   */
  void *seized_objects;

  /**
   * @brief This member may contain the object identifier returned by
   *   rtems_timer_create().
   */
  rtems_id id_value;

  /**
   * @brief This member specifies the ``name`` parameter for the action.
   */
  rtems_name name;

  /**
   * @brief This member specifies the ``id`` parameter for the action.
   */
  rtems_id *id;

  /**
   * @brief This member contains the return status of the action.
   */
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
    RtemsTimerReqCreate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTimerReqCreate_Context;

static RtemsTimerReqCreate_Context
  RtemsTimerReqCreate_Instance;

static const char * const RtemsTimerReqCreate_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsTimerReqCreate_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsTimerReqCreate_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsTimerReqCreate_PreDesc[] = {
  RtemsTimerReqCreate_PreDesc_Name,
  RtemsTimerReqCreate_PreDesc_Id,
  RtemsTimerReqCreate_PreDesc_Free,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

static rtems_status_code Create( void *arg, uint32_t *id )
{
  return rtems_timer_create( rtems_build_name( 'S', 'I', 'Z', 'E' ), id );
}

static void RtemsTimerReqCreate_Pre_Name_Prepare(
  RtemsTimerReqCreate_Context *ctx,
  RtemsTimerReqCreate_Pre_Name state
)
{
  switch ( state ) {
    case RtemsTimerReqCreate_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is valid.
       */
      ctx->name = NAME;
      break;
    }

    case RtemsTimerReqCreate_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is invalid.
       */
      ctx->name = 0;
      break;
    }

    case RtemsTimerReqCreate_Pre_Name_NA:
      break;
  }
}

static void RtemsTimerReqCreate_Pre_Id_Prepare(
  RtemsTimerReqCreate_Context *ctx,
  RtemsTimerReqCreate_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTimerReqCreate_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsTimerReqCreate_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsTimerReqCreate_Pre_Id_NA:
      break;
  }
}

static void RtemsTimerReqCreate_Pre_Free_Prepare(
  RtemsTimerReqCreate_Context *ctx,
  RtemsTimerReqCreate_Pre_Free state
)
{
  switch ( state ) {
    case RtemsTimerReqCreate_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive timer object available.
       */
      /* Ensured by the test suite configuration */
      break;
    }

    case RtemsTimerReqCreate_Pre_Free_No: {
      /*
       * While the system has no inactive timer object available.
       */
      ctx->seized_objects = T_seize_objects( Create, NULL );
      break;
    }

    case RtemsTimerReqCreate_Pre_Free_NA:
      break;
  }
}

static void RtemsTimerReqCreate_Post_Status_Check(
  RtemsTimerReqCreate_Context    *ctx,
  RtemsTimerReqCreate_Post_Status state
)
{
  switch ( state ) {
    case RtemsTimerReqCreate_Post_Status_Ok: {
      /*
       * The return status of rtems_timer_create() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTimerReqCreate_Post_Status_InvName: {
      /*
       * The return status of rtems_timer_create() shall be RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsTimerReqCreate_Post_Status_InvAddr: {
      /*
       * The return status of rtems_timer_create() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTimerReqCreate_Post_Status_TooMany: {
      /*
       * The return status of rtems_timer_create() shall be RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsTimerReqCreate_Post_Status_NA:
      break;
  }
}

static void RtemsTimerReqCreate_Post_Name_Check(
  RtemsTimerReqCreate_Context  *ctx,
  RtemsTimerReqCreate_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsTimerReqCreate_Post_Name_Valid: {
      /*
       * The unique object name shall identify the timer created by the
       * rtems_timer_create() call.
       */
      id = 0;
      sc = rtems_timer_ident( NAME, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsTimerReqCreate_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a timer.
       */
      sc = rtems_timer_ident( NAME, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsTimerReqCreate_Post_Name_NA:
      break;
  }
}

static void RtemsTimerReqCreate_Post_IdVar_Check(
  RtemsTimerReqCreate_Context   *ctx,
  RtemsTimerReqCreate_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsTimerReqCreate_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the object identifier of the created timer after the return of
       * the rtems_timer_create() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsTimerReqCreate_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_timer_create() shall not be accessed by the rtems_timer_create()
       * call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsTimerReqCreate_Post_IdVar_NA:
      break;
  }
}

static void RtemsTimerReqCreate_Setup( RtemsTimerReqCreate_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->id_value = INVALID_ID;
}

static void RtemsTimerReqCreate_Setup_Wrap( void *arg )
{
  RtemsTimerReqCreate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTimerReqCreate_Setup( ctx );
}

static void RtemsTimerReqCreate_Action( RtemsTimerReqCreate_Context *ctx )
{
  ctx->status = rtems_timer_create( ctx->name, ctx->id );
}

static void RtemsTimerReqCreate_Cleanup( RtemsTimerReqCreate_Context *ctx )
{
  if ( ctx->id_value != INVALID_ID ) {
    rtems_status_code sc;

    sc = rtems_timer_delete( ctx->id_value );
    T_rsc_success( sc );

    ctx->id_value = INVALID_ID;
  }

  T_surrender_objects( &ctx->seized_objects, rtems_timer_delete );
}

static const RtemsTimerReqCreate_Entry
RtemsTimerReqCreate_Entries[] = {
  { 0, 0, 0, 0, RtemsTimerReqCreate_Post_Status_InvName,
    RtemsTimerReqCreate_Post_Name_Invalid, RtemsTimerReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, RtemsTimerReqCreate_Post_Status_InvAddr,
    RtemsTimerReqCreate_Post_Name_Invalid, RtemsTimerReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, RtemsTimerReqCreate_Post_Status_Ok,
    RtemsTimerReqCreate_Post_Name_Valid, RtemsTimerReqCreate_Post_IdVar_Set },
  { 0, 0, 0, 0, RtemsTimerReqCreate_Post_Status_TooMany,
    RtemsTimerReqCreate_Post_Name_Invalid, RtemsTimerReqCreate_Post_IdVar_Nop }
};

static const uint8_t
RtemsTimerReqCreate_Map[] = {
  2, 3, 1, 1, 0, 0, 0, 0
};

static size_t RtemsTimerReqCreate_Scope( void *arg, char *buf, size_t n )
{
  RtemsTimerReqCreate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTimerReqCreate_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTimerReqCreate_Fixture = {
  .setup = RtemsTimerReqCreate_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsTimerReqCreate_Scope,
  .initial_context = &RtemsTimerReqCreate_Instance
};

static inline RtemsTimerReqCreate_Entry RtemsTimerReqCreate_PopEntry(
  RtemsTimerReqCreate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTimerReqCreate_Entries[
    RtemsTimerReqCreate_Map[ index ]
  ];
}

static void RtemsTimerReqCreate_TestVariant( RtemsTimerReqCreate_Context *ctx )
{
  RtemsTimerReqCreate_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTimerReqCreate_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTimerReqCreate_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTimerReqCreate_Action( ctx );
  RtemsTimerReqCreate_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTimerReqCreate_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsTimerReqCreate_Post_IdVar_Check( ctx, ctx->Map.entry.Post_IdVar );
}

/**
 * @fn void T_case_body_RtemsTimerReqCreate( void )
 */
T_TEST_CASE_FIXTURE( RtemsTimerReqCreate, &RtemsTimerReqCreate_Fixture )
{
  RtemsTimerReqCreate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTimerReqCreate_Pre_Name_Valid;
    ctx->Map.pcs[ 0 ] < RtemsTimerReqCreate_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTimerReqCreate_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsTimerReqCreate_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTimerReqCreate_Pre_Free_Yes;
        ctx->Map.pcs[ 2 ] < RtemsTimerReqCreate_Pre_Free_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsTimerReqCreate_PopEntry( ctx );
        RtemsTimerReqCreate_TestVariant( ctx );
        RtemsTimerReqCreate_Cleanup( ctx );
      }
    }
  }
}

/** @} */
