/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsPartReqDelete
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

#include <rtems.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsPartReqDelete spec:/rtems/part/req/delete
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsPartReqDelete_Pre_Id_NoObj,
  RtemsPartReqDelete_Pre_Id_Part,
  RtemsPartReqDelete_Pre_Id_NA
} RtemsPartReqDelete_Pre_Id;

typedef enum {
  RtemsPartReqDelete_Pre_InUse_Yes,
  RtemsPartReqDelete_Pre_InUse_No,
  RtemsPartReqDelete_Pre_InUse_NA
} RtemsPartReqDelete_Pre_InUse;

typedef enum {
  RtemsPartReqDelete_Post_Status_Ok,
  RtemsPartReqDelete_Post_Status_InvId,
  RtemsPartReqDelete_Post_Status_InUse,
  RtemsPartReqDelete_Post_Status_NA
} RtemsPartReqDelete_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_InUse_NA : 1;
  uint8_t Post_Status : 2;
} RtemsPartReqDelete_Entry;

/**
 * @brief Test context for spec:/rtems/part/req/delete test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_id id;

  rtems_id id_value;

  void *buffer;

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
    RtemsPartReqDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsPartReqDelete_Context;

static RtemsPartReqDelete_Context
  RtemsPartReqDelete_Instance;

static const char * const RtemsPartReqDelete_PreDesc_Id[] = {
  "NoObj",
  "Part",
  "NA"
};

static const char * const RtemsPartReqDelete_PreDesc_InUse[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsPartReqDelete_PreDesc[] = {
  RtemsPartReqDelete_PreDesc_Id,
  RtemsPartReqDelete_PreDesc_InUse,
  NULL
};

#define PART_NAME rtems_build_name( 'N', 'A', 'M', 'E' )

#define BUFFER_COUNT 1

#define BUFFER_SIZE ( 2 * sizeof( void * ) )

static RTEMS_ALIGNED( RTEMS_PARTITION_ALIGNMENT ) uint8_t
  buffers[ BUFFER_COUNT ][ BUFFER_SIZE ];

static void RtemsPartReqDelete_Pre_Id_Prepare(
  RtemsPartReqDelete_Context *ctx,
  RtemsPartReqDelete_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsPartReqDelete_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a partition.
       */
      ctx->id = 0xffffffff;
      break;
    }

    case RtemsPartReqDelete_Pre_Id_Part: {
      /*
       * While the ``id`` parameter is associated with a partition.
       */
      ctx->id = ctx->id_value;
      break;
    }

    case RtemsPartReqDelete_Pre_Id_NA:
      break;
  }
}

static void RtemsPartReqDelete_Pre_InUse_Prepare(
  RtemsPartReqDelete_Context  *ctx,
  RtemsPartReqDelete_Pre_InUse state
)
{
  rtems_status_code sc;

  switch ( state ) {
    case RtemsPartReqDelete_Pre_InUse_Yes: {
      /*
       * While the partition has at least one buffer in use.
       */
      ctx->buffer = NULL;
      sc = rtems_partition_get_buffer( ctx->id_value, &ctx->buffer );
      T_rsc_success( sc );
      T_not_null( ctx->buffer );
      break;
    }

    case RtemsPartReqDelete_Pre_InUse_No: {
      /*
       * While the partition does not have a buffer in use.
       */
      ctx->buffer = NULL;
      break;
    }

    case RtemsPartReqDelete_Pre_InUse_NA:
      break;
  }
}

static void RtemsPartReqDelete_Post_Status_Check(
  RtemsPartReqDelete_Context    *ctx,
  RtemsPartReqDelete_Post_Status state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsPartReqDelete_Post_Status_Ok: {
      /*
       * The status shall be RTEMS_SUCCESSFUL.  The deleted partition object
       * shall be inactive.
       */
      T_rsc_success( ctx->status );
      ctx->id_value = 0xffffffff;

      id = 0xffffffff;
      sc = rtems_partition_ident( PART_NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      T_eq_u32( id, 0xffffffff );
      break;
    }

    case RtemsPartReqDelete_Post_Status_InvId: {
      /*
       * The status shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );

      id = 0xffffffff;
      sc = rtems_partition_ident( PART_NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc);
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsPartReqDelete_Post_Status_InUse: {
      /*
       * The status shall be RTEMS_RESOURCE_IN_USE.
       */
      T_rsc( ctx->status, RTEMS_RESOURCE_IN_USE );

      id = 0xffffffff;
      sc = rtems_partition_ident( PART_NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc);
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsPartReqDelete_Post_Status_NA:
      break;
  }
}

static void RtemsPartReqDelete_Prepare( RtemsPartReqDelete_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_partition_create(
    PART_NAME,
    buffers,
    sizeof( buffers ),
    sizeof( buffers[ 0 ] ),
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->id_value
  );
  T_rsc_success( sc );
}

static void RtemsPartReqDelete_Action( RtemsPartReqDelete_Context *ctx )
{
  ctx->status = rtems_partition_delete( ctx->id );
}

static void RtemsPartReqDelete_Cleanup( RtemsPartReqDelete_Context *ctx )
{
  rtems_status_code sc;

  if ( ctx->buffer != NULL ) {
    sc = rtems_partition_return_buffer( ctx->id_value, ctx->buffer );
    T_rsc_success( sc );
  }

  if ( ctx->id_value != 0xffffffff ) {
    sc = rtems_partition_delete( ctx->id_value );
    T_rsc_success( sc );
  }
}

static const RtemsPartReqDelete_Entry
RtemsPartReqDelete_Entries[] = {
  { 0, 0, 0, RtemsPartReqDelete_Post_Status_InvId },
  { 0, 0, 0, RtemsPartReqDelete_Post_Status_InUse },
  { 0, 0, 0, RtemsPartReqDelete_Post_Status_Ok }
};

static const uint8_t
RtemsPartReqDelete_Map[] = {
  0, 0, 1, 2
};

static size_t RtemsPartReqDelete_Scope( void *arg, char *buf, size_t n )
{
  RtemsPartReqDelete_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsPartReqDelete_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsPartReqDelete_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsPartReqDelete_Scope,
  .initial_context = &RtemsPartReqDelete_Instance
};

static inline RtemsPartReqDelete_Entry RtemsPartReqDelete_PopEntry(
  RtemsPartReqDelete_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsPartReqDelete_Entries[
    RtemsPartReqDelete_Map[ index ]
  ];
}

static void RtemsPartReqDelete_TestVariant( RtemsPartReqDelete_Context *ctx )
{
  RtemsPartReqDelete_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsPartReqDelete_Pre_InUse_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsPartReqDelete_Action( ctx );
  RtemsPartReqDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_RtemsPartReqDelete( void )
 */
T_TEST_CASE_FIXTURE( RtemsPartReqDelete, &RtemsPartReqDelete_Fixture )
{
  RtemsPartReqDelete_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsPartReqDelete_Pre_Id_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsPartReqDelete_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsPartReqDelete_Pre_InUse_Yes;
      ctx->Map.pcs[ 1 ] < RtemsPartReqDelete_Pre_InUse_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsPartReqDelete_PopEntry( ctx );
      RtemsPartReqDelete_Prepare( ctx );
      RtemsPartReqDelete_TestVariant( ctx );
      RtemsPartReqDelete_Cleanup( ctx );
    }
  }
}

/** @} */
