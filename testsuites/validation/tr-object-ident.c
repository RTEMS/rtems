/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsReqIdent
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

#include "tr-object-ident.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsReqIdent spec:/rtems/req/ident
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Node_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Id : 3;
} RtemsReqIdent_Entry;

/**
 * @brief Test context for spec:/rtems/req/ident test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_name name;

  uint32_t node;

  rtems_id *id;

  rtems_id id_value;

  rtems_id id_remote_object;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsReqIdent_Run() parameter.
   */
  rtems_id id_local_object;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsReqIdent_Run() parameter.
   */
  rtems_name name_local_object;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsReqIdent_Run() parameter.
   */
  rtems_status_code ( *action )( rtems_name, uint32_t, rtems_id * );

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
    RtemsReqIdent_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsReqIdent_Context;

static RtemsReqIdent_Context
  RtemsReqIdent_Instance;

static const char * const RtemsReqIdent_PreDesc_Name[] = {
  "Invalid",
  "Valid",
  "NA"
};

static const char * const RtemsReqIdent_PreDesc_Node[] = {
  "Local",
  "Remote",
  "Invalid",
  "SearchAll",
  "SearchOther",
  "SearchLocal",
  "NA"
};

static const char * const RtemsReqIdent_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsReqIdent_PreDesc[] = {
  RtemsReqIdent_PreDesc_Name,
  RtemsReqIdent_PreDesc_Node,
  RtemsReqIdent_PreDesc_Id,
  NULL
};

static void RtemsReqIdent_Pre_Name_Prepare(
  RtemsReqIdent_Context *ctx,
  RtemsReqIdent_Pre_Name state
)
{
  switch ( state ) {
    case RtemsReqIdent_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is not associated with an active object
       * of the specified class .
       */
      ctx->name = 1;
      break;
    }

    case RtemsReqIdent_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is associated with an active object of
       * the specified class .
       */
      ctx->name = ctx->name_local_object;
      break;
    }

    case RtemsReqIdent_Pre_Name_NA:
      break;
  }
}

static void RtemsReqIdent_Pre_Node_Prepare(
  RtemsReqIdent_Context *ctx,
  RtemsReqIdent_Pre_Node state
)
{
  switch ( state ) {
    case RtemsReqIdent_Pre_Node_Local: {
      /*
       * While the ``node`` parameter is the local node number.
       */
      ctx->node = 1;
      break;
    }

    case RtemsReqIdent_Pre_Node_Remote: {
      /*
       * While the ``node`` parameter is a remote node number.
       */
      ctx->node = 2;
      break;
    }

    case RtemsReqIdent_Pre_Node_Invalid: {
      /*
       * While the ``node`` parameter is an invalid node number.
       */
      ctx->node = 256;
      break;
    }

    case RtemsReqIdent_Pre_Node_SearchAll: {
      /*
       * While the ``node`` parameter is RTEMS_SEARCH_ALL_NODES.
       */
      ctx->node = RTEMS_SEARCH_ALL_NODES;
      break;
    }

    case RtemsReqIdent_Pre_Node_SearchOther: {
      /*
       * While the ``node`` parameter is RTEMS_SEARCH_OTHER_NODES.
       */
      ctx->node = RTEMS_SEARCH_OTHER_NODES;
      break;
    }

    case RtemsReqIdent_Pre_Node_SearchLocal: {
      /*
       * While the ``node`` parameter is RTEMS_SEARCH_LOCAL_NODE.
       */
      ctx->node = RTEMS_SEARCH_LOCAL_NODE;
      break;
    }

    case RtemsReqIdent_Pre_Node_NA:
      break;
  }
}

static void RtemsReqIdent_Pre_Id_Prepare(
  RtemsReqIdent_Context *ctx,
  RtemsReqIdent_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsReqIdent_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id_value = 0xffffffff;
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsReqIdent_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsReqIdent_Pre_Id_NA:
      break;
  }
}

static void RtemsReqIdent_Post_Status_Check(
  RtemsReqIdent_Context    *ctx,
  RtemsReqIdent_Post_Status state
)
{
  switch ( state ) {
    case RtemsReqIdent_Post_Status_Ok: {
      /*
       * The return status shall be RTEMS_SUCCESSFUL.
       */
      T_rsc(ctx->status, RTEMS_SUCCESSFUL);
      break;
    }

    case RtemsReqIdent_Post_Status_InvAddr: {
      /*
       * The return status shall be RTEMS_INVALID_ADDRESS.
       */
      T_rsc(ctx->status, RTEMS_INVALID_ADDRESS);
      break;
    }

    case RtemsReqIdent_Post_Status_InvName: {
      /*
       * The return status shall be RTEMS_INVALID_NAME.
       */
      T_rsc(ctx->status, RTEMS_INVALID_NAME);
      break;
    }

    case RtemsReqIdent_Post_Status_InvNode: {
      /*
       * The return status shall be RTEMS_INVALID_NODE.
       */
      T_rsc(ctx->status, RTEMS_INVALID_NODE);
      break;
    }

    case RtemsReqIdent_Post_Status_NA:
      break;
  }
}

static void RtemsReqIdent_Post_Id_Check(
  RtemsReqIdent_Context *ctx,
  RtemsReqIdent_Post_Id  state
)
{
  switch ( state ) {
    case RtemsReqIdent_Post_Id_Nop: {
      /*
       * The value of the object identifier referenced by the id parameter
       * shall be the value before the action.
       */
      T_eq_ptr(ctx->id, &ctx->id_value);
      T_eq_u32(ctx->id_value, 0xffffffff);
      break;
    }

    case RtemsReqIdent_Post_Id_Null: {
      /*
       * While the id is NULL.
       */
      T_null(ctx->id)
      break;
    }

    case RtemsReqIdent_Post_Id_LocalObj: {
      /*
       * The value of the object identifier referenced by the id parameter
       * shall be the identifier of a local object of the specified class with
       * a name equal to the name parameter.  If more than one local object of
       * the specified class with such a name exists, then it shall be the
       * identifier of the object with the lowest object index.
       */
      T_eq_ptr(ctx->id, &ctx->id_value);
      T_eq_u32(ctx->id_value, ctx->id_local_object);
      break;
    }

    case RtemsReqIdent_Post_Id_RemoteObj: {
      /*
       * The value of the object identifier referenced by the id parameter
       * shall be the identifier of a remote object of the specified class on a
       * eligible node defined by the node parameter with a name equal to the
       * name parameter.  If more than one local object of the specified class
       * with such a name exists, then it shall be the identifier of the object
       * with the lowest object index.  Otherwise, if more than one object of
       * the specified class with such a name exists on remote eligible nodes,
       * then it shall be the identifier of the object with the lowest node
       * index and the lowest object index on this node.
       */
      T_eq_ptr(ctx->id, &ctx->id_value);
      T_eq_u32(ctx->id_value, ctx->id_remote_object);
      break;
    }

    case RtemsReqIdent_Post_Id_NA:
      break;
  }
}

static void RtemsReqIdent_Action( RtemsReqIdent_Context *ctx )
{
  ctx->status = ( *ctx->action )( ctx->name, ctx->node, ctx->id );
}

static const RtemsReqIdent_Entry
RtemsReqIdent_Entries[] = {
  { 0, 0, 0, 0, RtemsReqIdent_Post_Status_InvAddr, RtemsReqIdent_Post_Id_Null },
  { 0, 0, 0, 0, RtemsReqIdent_Post_Status_InvName, RtemsReqIdent_Post_Id_Nop },
  { 0, 0, 0, 0, RtemsReqIdent_Post_Status_Ok, RtemsReqIdent_Post_Id_LocalObj },
#if defined(RTEMS_MULTIPROCESSING)
  { 0, 0, 0, 0, RtemsReqIdent_Post_Status_Ok, RtemsReqIdent_Post_Id_RemoteObj }
#else
  { 0, 0, 0, 0, RtemsReqIdent_Post_Status_InvName, RtemsReqIdent_Post_Id_Nop }
#endif
};

static const uint8_t
RtemsReqIdent_Map[] = {
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 2, 0, 3, 0, 1, 0, 2, 0, 3, 0, 2, 0
};

static size_t RtemsReqIdent_Scope( void *arg, char *buf, size_t n )
{
  RtemsReqIdent_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsReqIdent_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsReqIdent_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsReqIdent_Scope,
  .initial_context = &RtemsReqIdent_Instance
};

static inline RtemsReqIdent_Entry RtemsReqIdent_PopEntry(
  RtemsReqIdent_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsReqIdent_Entries[
    RtemsReqIdent_Map[ index ]
  ];
}

static void RtemsReqIdent_TestVariant( RtemsReqIdent_Context *ctx )
{
  RtemsReqIdent_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsReqIdent_Pre_Node_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsReqIdent_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsReqIdent_Action( ctx );
  RtemsReqIdent_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsReqIdent_Post_Id_Check( ctx, ctx->Map.entry.Post_Id );
}

static T_fixture_node RtemsReqIdent_Node;

static T_remark RtemsReqIdent_Remark = {
  .next = NULL,
  .remark = "RtemsReqIdent"
};

void RtemsReqIdent_Run(
  rtems_id             id_local_object,
  rtems_name           name_local_object,
  rtems_status_code ( *action )( rtems_name, uint32_t, rtems_id * )
)
{
  RtemsReqIdent_Context *ctx;

  ctx = &RtemsReqIdent_Instance;
  ctx->id_local_object = id_local_object;
  ctx->name_local_object = name_local_object;
  ctx->action = action;

  ctx = T_push_fixture( &RtemsReqIdent_Node, &RtemsReqIdent_Fixture );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsReqIdent_Pre_Name_Invalid;
    ctx->Map.pcs[ 0 ] < RtemsReqIdent_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsReqIdent_Pre_Node_Local;
      ctx->Map.pcs[ 1 ] < RtemsReqIdent_Pre_Node_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsReqIdent_Pre_Id_Valid;
        ctx->Map.pcs[ 2 ] < RtemsReqIdent_Pre_Id_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsReqIdent_PopEntry( ctx );
        RtemsReqIdent_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &RtemsReqIdent_Remark );
  T_pop_fixture();
}

/** @} */
