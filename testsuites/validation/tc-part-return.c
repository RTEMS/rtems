/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsPartReqReturnBuffer
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
 * @defgroup RtemsPartReqReturnBuffer spec:/rtems/part/req/return-buffer
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsPartReqReturnBuffer_Pre_Id_NoObj,
  RtemsPartReqReturnBuffer_Pre_Id_Part,
  RtemsPartReqReturnBuffer_Pre_Id_NA
} RtemsPartReqReturnBuffer_Pre_Id;

typedef enum {
  RtemsPartReqReturnBuffer_Pre_Buf_Valid,
  RtemsPartReqReturnBuffer_Pre_Buf_BadAlign,
  RtemsPartReqReturnBuffer_Pre_Buf_BelowArea,
  RtemsPartReqReturnBuffer_Pre_Buf_AboveArea,
  RtemsPartReqReturnBuffer_Pre_Buf_NA
} RtemsPartReqReturnBuffer_Pre_Buf;

typedef enum {
  RtemsPartReqReturnBuffer_Post_Status_Ok,
  RtemsPartReqReturnBuffer_Post_Status_InvId,
  RtemsPartReqReturnBuffer_Post_Status_InvAddr,
  RtemsPartReqReturnBuffer_Post_Status_NA
} RtemsPartReqReturnBuffer_Post_Status;

typedef enum {
  RtemsPartReqReturnBuffer_Post_Buf_Free,
  RtemsPartReqReturnBuffer_Post_Buf_InUse,
  RtemsPartReqReturnBuffer_Post_Buf_NA
} RtemsPartReqReturnBuffer_Post_Buf;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Pre_Buf_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Buf : 2;
} RtemsPartReqReturnBuffer_Entry;

/**
 * @brief Test context for spec:/rtems/part/req/return-buffer test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_id id;

  rtems_id id_value;

  void *buffer;

  void *buffer_in_use;

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
    RtemsPartReqReturnBuffer_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsPartReqReturnBuffer_Context;

static RtemsPartReqReturnBuffer_Context
  RtemsPartReqReturnBuffer_Instance;

static const char * const RtemsPartReqReturnBuffer_PreDesc_Id[] = {
  "NoObj",
  "Part",
  "NA"
};

static const char * const RtemsPartReqReturnBuffer_PreDesc_Buf[] = {
  "Valid",
  "BadAlign",
  "BelowArea",
  "AboveArea",
  "NA"
};

static const char * const * const RtemsPartReqReturnBuffer_PreDesc[] = {
  RtemsPartReqReturnBuffer_PreDesc_Id,
  RtemsPartReqReturnBuffer_PreDesc_Buf,
  NULL
};

#define BUFFER_COUNT 1

#define BUFFER_SIZE ( 2 * sizeof( void * ) )

static RTEMS_ALIGNED( RTEMS_PARTITION_ALIGNMENT ) uint8_t
  buffers[ BUFFER_COUNT ][ BUFFER_SIZE ];

static void RtemsPartReqReturnBuffer_Pre_Id_Prepare(
  RtemsPartReqReturnBuffer_Context *ctx,
  RtemsPartReqReturnBuffer_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsPartReqReturnBuffer_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is invalid.
       */
      ctx->id = 0xffffffff;
      break;
    }

    case RtemsPartReqReturnBuffer_Pre_Id_Part: {
      /*
       * While the ``id`` parameter is associated with a partition.
       */
      ctx->id = ctx->id_value;
      break;
    }

    case RtemsPartReqReturnBuffer_Pre_Id_NA:
      break;
  }
}

static void RtemsPartReqReturnBuffer_Pre_Buf_Prepare(
  RtemsPartReqReturnBuffer_Context *ctx,
  RtemsPartReqReturnBuffer_Pre_Buf  state
)
{
  switch ( state ) {
    case RtemsPartReqReturnBuffer_Pre_Buf_Valid: {
      /*
       * While the ``buffer`` parameter references a buffer previously returned
       * by rtems_partition_get_buffer().
       */
      ctx->buffer = ctx->buffer_in_use;
      break;
    }

    case RtemsPartReqReturnBuffer_Pre_Buf_BadAlign: {
      /*
       * While the ``buffer`` parameter is an address inside the buffer area of
       * the partition, while the address is not on a valid buffer boundary.
       */
      ctx->buffer = (void *) ( (uintptr_t) ctx->buffer_in_use + 1 );
      break;
    }

    case RtemsPartReqReturnBuffer_Pre_Buf_BelowArea: {
      /*
       * While the ``buffer`` parameter is an address below the buffer area of
       * the partition.
       */
      ctx->buffer = (void *) ( (uintptr_t) buffers - 1 );
      break;
    }

    case RtemsPartReqReturnBuffer_Pre_Buf_AboveArea: {
      /*
       * While the ``buffer`` parameter is an address above the buffer area of
       * the partition.
       */
      ctx->buffer = (void *) ( (uintptr_t) buffers + sizeof( buffers ) );
      break;
    }

    case RtemsPartReqReturnBuffer_Pre_Buf_NA:
      break;
  }
}

static void RtemsPartReqReturnBuffer_Post_Status_Check(
  RtemsPartReqReturnBuffer_Context    *ctx,
  RtemsPartReqReturnBuffer_Post_Status state
)
{
  switch ( state ) {
    case RtemsPartReqReturnBuffer_Post_Status_Ok: {
      /*
       * The return status of rtems_partition_return_buffer() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsPartReqReturnBuffer_Post_Status_InvId: {
      /*
       * The return status of rtems_partition_return_buffer() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsPartReqReturnBuffer_Post_Status_InvAddr: {
      /*
       * The return status of rtems_partition_return_buffer() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsPartReqReturnBuffer_Post_Status_NA:
      break;
  }
}

static void RtemsPartReqReturnBuffer_Post_Buf_Check(
  RtemsPartReqReturnBuffer_Context *ctx,
  RtemsPartReqReturnBuffer_Post_Buf state
)
{
  rtems_status_code sc;
  void             *no_buffer;

  switch ( state ) {
    case RtemsPartReqReturnBuffer_Post_Buf_Free: {
      /*
       * The buffer obtained from the partition shall be made available for
       * re-use by the rtems_partition_return_buffer() call.
       */
      sc = rtems_partition_get_buffer( ctx->id_value, &ctx->buffer_in_use );
      T_rsc_success( sc );
      T_eq_ptr( ctx->buffer_in_use, buffers );
      break;
    }

    case RtemsPartReqReturnBuffer_Post_Buf_InUse: {
      /*
       * The buffer obtained from the partition shall be still in use after the
       * rtems_partition_return_buffer() call.
       */
      sc = rtems_partition_get_buffer( ctx->id_value, &no_buffer );
      T_rsc( sc, RTEMS_UNSATISFIED );
      break;
    }

    case RtemsPartReqReturnBuffer_Post_Buf_NA:
      break;
  }
}

static void RtemsPartReqReturnBuffer_Setup(
  RtemsPartReqReturnBuffer_Context *ctx
)
{
  rtems_status_code sc;

  ctx->buffer_in_use = NULL;
  ctx->id_value = 0;

  sc = rtems_partition_create(
    rtems_build_name( 'N', 'A', 'M', 'E' ),
    buffers,
    sizeof( buffers ),
    sizeof( buffers[ 0 ] ),
    RTEMS_DEFAULT_ATTRIBUTES,
    &ctx->id_value
  );
  T_assert_rsc_success( sc );

  sc = rtems_partition_get_buffer( ctx->id_value, &ctx->buffer_in_use );
  T_assert_rsc_success( sc );
  T_assert_eq_ptr( ctx->buffer_in_use, buffers );
}

static void RtemsPartReqReturnBuffer_Setup_Wrap( void *arg )
{
  RtemsPartReqReturnBuffer_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsPartReqReturnBuffer_Setup( ctx );
}

static void RtemsPartReqReturnBuffer_Teardown(
  RtemsPartReqReturnBuffer_Context *ctx
)
{
  rtems_status_code sc;

  if ( ctx->buffer_in_use != NULL ) {
    sc = rtems_partition_return_buffer( ctx->id_value, ctx->buffer_in_use );
    T_rsc_success( sc );
  }

  if ( ctx->id_value != 0 ) {
    sc = rtems_partition_delete( ctx->id_value );
    T_rsc_success( sc );
  }
}

static void RtemsPartReqReturnBuffer_Teardown_Wrap( void *arg )
{
  RtemsPartReqReturnBuffer_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsPartReqReturnBuffer_Teardown( ctx );
}

static void RtemsPartReqReturnBuffer_Action(
  RtemsPartReqReturnBuffer_Context *ctx
)
{
  ctx->status = rtems_partition_return_buffer( ctx->id, ctx->buffer );
}

static const RtemsPartReqReturnBuffer_Entry
RtemsPartReqReturnBuffer_Entries[] = {
  { 0, 0, 0, RtemsPartReqReturnBuffer_Post_Status_InvId,
    RtemsPartReqReturnBuffer_Post_Buf_InUse },
  { 0, 0, 0, RtemsPartReqReturnBuffer_Post_Status_InvAddr,
    RtemsPartReqReturnBuffer_Post_Buf_InUse },
  { 0, 0, 0, RtemsPartReqReturnBuffer_Post_Status_Ok,
    RtemsPartReqReturnBuffer_Post_Buf_Free }
};

static const uint8_t
RtemsPartReqReturnBuffer_Map[] = {
  0, 0, 0, 0, 2, 1, 1, 1
};

static size_t RtemsPartReqReturnBuffer_Scope( void *arg, char *buf, size_t n )
{
  RtemsPartReqReturnBuffer_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsPartReqReturnBuffer_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsPartReqReturnBuffer_Fixture = {
  .setup = RtemsPartReqReturnBuffer_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsPartReqReturnBuffer_Teardown_Wrap,
  .scope = RtemsPartReqReturnBuffer_Scope,
  .initial_context = &RtemsPartReqReturnBuffer_Instance
};

static inline RtemsPartReqReturnBuffer_Entry RtemsPartReqReturnBuffer_PopEntry(
  RtemsPartReqReturnBuffer_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsPartReqReturnBuffer_Entries[
    RtemsPartReqReturnBuffer_Map[ index ]
  ];
}

static void RtemsPartReqReturnBuffer_TestVariant(
  RtemsPartReqReturnBuffer_Context *ctx
)
{
  RtemsPartReqReturnBuffer_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsPartReqReturnBuffer_Pre_Buf_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsPartReqReturnBuffer_Action( ctx );
  RtemsPartReqReturnBuffer_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsPartReqReturnBuffer_Post_Buf_Check( ctx, ctx->Map.entry.Post_Buf );
}

/**
 * @fn void T_case_body_RtemsPartReqReturnBuffer( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsPartReqReturnBuffer,
  &RtemsPartReqReturnBuffer_Fixture
)
{
  RtemsPartReqReturnBuffer_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsPartReqReturnBuffer_Pre_Id_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsPartReqReturnBuffer_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsPartReqReturnBuffer_Pre_Buf_Valid;
      ctx->Map.pcs[ 1 ] < RtemsPartReqReturnBuffer_Pre_Buf_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsPartReqReturnBuffer_PopEntry( ctx );
      RtemsPartReqReturnBuffer_TestVariant( ctx );
    }
  }
}

/** @} */
