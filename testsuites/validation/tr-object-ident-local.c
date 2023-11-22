/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsReqIdentLocal
 */

/*
 * Copyright (C) 2020 embedded brains GmbH & Co. KG
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

#include "tr-object-ident-local.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsReqIdentLocal spec:/rtems/req/ident-local
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Name_NA : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Id : 2;
} RtemsReqIdentLocal_Entry;

/**
 * @brief Test context for spec:/rtems/req/ident-local test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_name name;

  rtems_id *id;

  rtems_id id_value;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsReqIdentLocal_Run() parameter.
   */
  rtems_id id_local_object;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsReqIdentLocal_Run() parameter.
   */
  rtems_name name_local_object;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsReqIdentLocal_Run() parameter.
   */
  rtems_status_code ( *action )( rtems_name, rtems_id * );

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
    RtemsReqIdentLocal_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsReqIdentLocal_Context;

static RtemsReqIdentLocal_Context
  RtemsReqIdentLocal_Instance;

static const char * const RtemsReqIdentLocal_PreDesc_Name[] = {
  "Invalid",
  "Valid",
  "NA"
};

static const char * const RtemsReqIdentLocal_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsReqIdentLocal_PreDesc[] = {
  RtemsReqIdentLocal_PreDesc_Name,
  RtemsReqIdentLocal_PreDesc_Id,
  NULL
};

static void RtemsReqIdentLocal_Pre_Name_Prepare(
  RtemsReqIdentLocal_Context *ctx,
  RtemsReqIdentLocal_Pre_Name state
)
{
  switch ( state ) {
    case RtemsReqIdentLocal_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is not associated with an active object
       * of the specified class .
       */
      ctx->name = 1;
      break;
    }

    case RtemsReqIdentLocal_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is associated with an active object of
       * the specified class .
       */
      ctx->name = ctx->name_local_object;
      break;
    }

    case RtemsReqIdentLocal_Pre_Name_NA:
      break;
  }
}

static void RtemsReqIdentLocal_Pre_Id_Prepare(
  RtemsReqIdentLocal_Context *ctx,
  RtemsReqIdentLocal_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsReqIdentLocal_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id_value = 0xffffffff;
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsReqIdentLocal_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsReqIdentLocal_Pre_Id_NA:
      break;
  }
}

static void RtemsReqIdentLocal_Post_Status_Check(
  RtemsReqIdentLocal_Context    *ctx,
  RtemsReqIdentLocal_Post_Status state
)
{
  switch ( state ) {
    case RtemsReqIdentLocal_Post_Status_Ok: {
      /*
       * The return status shall be RTEMS_SUCCESSFUL.
       */
      T_rsc( ctx->status, RTEMS_SUCCESSFUL );
      break;
    }

    case RtemsReqIdentLocal_Post_Status_InvAddr: {
      /*
       * The return status shall be RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsReqIdentLocal_Post_Status_InvName: {
      /*
       * The return status shall be RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsReqIdentLocal_Post_Status_NA:
      break;
  }
}

static void RtemsReqIdentLocal_Post_Id_Check(
  RtemsReqIdentLocal_Context *ctx,
  RtemsReqIdentLocal_Post_Id  state
)
{
  switch ( state ) {
    case RtemsReqIdentLocal_Post_Id_Nop: {
      /*
       * The value of the object identifier referenced by the id parameter
       * shall be the value before the action.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_eq_u32( ctx->id_value, 0xffffffff );
      break;
    }

    case RtemsReqIdentLocal_Post_Id_Null: {
      /*
       * While the id is NULL.
       */
      T_null( ctx->id )
      break;
    }

    case RtemsReqIdentLocal_Post_Id_Id: {
      /*
       * The value of the object identifier referenced by the id parameter
       * shall be the identifier of a local object of the specified class with
       * a name equal to the name parameter.  If more than one local object of
       * the specified class with such a name exists, then it shall be the
       * identifier of the object with the lowest object index.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_eq_u32( ctx->id_value, ctx->id_local_object );
      break;
    }

    case RtemsReqIdentLocal_Post_Id_NA:
      break;
  }
}

static void RtemsReqIdentLocal_Action( RtemsReqIdentLocal_Context *ctx )
{
  ctx->status = ( *ctx->action )( ctx->name, ctx->id );
}

static const RtemsReqIdentLocal_Entry
RtemsReqIdentLocal_Entries[] = {
  { 0, 0, 0, RtemsReqIdentLocal_Post_Status_InvAddr,
    RtemsReqIdentLocal_Post_Id_Null },
  { 0, 0, 0, RtemsReqIdentLocal_Post_Status_InvName,
    RtemsReqIdentLocal_Post_Id_Nop },
  { 0, 0, 0, RtemsReqIdentLocal_Post_Status_Ok, RtemsReqIdentLocal_Post_Id_Id }
};

static const uint8_t
RtemsReqIdentLocal_Map[] = {
  1, 0, 2, 0
};

static size_t RtemsReqIdentLocal_Scope( void *arg, char *buf, size_t n )
{
  RtemsReqIdentLocal_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsReqIdentLocal_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsReqIdentLocal_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsReqIdentLocal_Scope,
  .initial_context = &RtemsReqIdentLocal_Instance
};

static inline RtemsReqIdentLocal_Entry RtemsReqIdentLocal_PopEntry(
  RtemsReqIdentLocal_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsReqIdentLocal_Entries[
    RtemsReqIdentLocal_Map[ index ]
  ];
}

static void RtemsReqIdentLocal_TestVariant( RtemsReqIdentLocal_Context *ctx )
{
  RtemsReqIdentLocal_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsReqIdentLocal_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsReqIdentLocal_Action( ctx );
  RtemsReqIdentLocal_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsReqIdentLocal_Post_Id_Check( ctx, ctx->Map.entry.Post_Id );
}

static T_fixture_node RtemsReqIdentLocal_Node;

static T_remark RtemsReqIdentLocal_Remark = {
  .next = NULL,
  .remark = "RtemsReqIdentLocal"
};

void RtemsReqIdentLocal_Run(
  rtems_id             id_local_object,
  rtems_name           name_local_object,
  rtems_status_code ( *action )( rtems_name, rtems_id * )
)
{
  RtemsReqIdentLocal_Context *ctx;

  ctx = &RtemsReqIdentLocal_Instance;
  ctx->id_local_object = id_local_object;
  ctx->name_local_object = name_local_object;
  ctx->action = action;

  ctx = T_push_fixture( &RtemsReqIdentLocal_Node, &RtemsReqIdentLocal_Fixture );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsReqIdentLocal_Pre_Name_Invalid;
    ctx->Map.pcs[ 0 ] < RtemsReqIdentLocal_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsReqIdentLocal_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsReqIdentLocal_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsReqIdentLocal_PopEntry( ctx );
      RtemsReqIdentLocal_TestVariant( ctx );
    }
  }

  T_add_remark( &RtemsReqIdentLocal_Remark );
  T_pop_fixture();
}

/** @} */
