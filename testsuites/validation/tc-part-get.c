/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsPartReqGetBuffer
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
 * @defgroup RtemsPartReqGetBuffer spec:/rtems/part/req/get-buffer
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsPartReqGetBuffer_Pre_Id_NoObj,
  RtemsPartReqGetBuffer_Pre_Id_Part,
  RtemsPartReqGetBuffer_Pre_Id_NA
} RtemsPartReqGetBuffer_Pre_Id;

typedef enum {
  RtemsPartReqGetBuffer_Pre_Buf_Valid,
  RtemsPartReqGetBuffer_Pre_Buf_Null,
  RtemsPartReqGetBuffer_Pre_Buf_NA
} RtemsPartReqGetBuffer_Pre_Buf;

typedef enum {
  RtemsPartReqGetBuffer_Pre_Avail_Yes,
  RtemsPartReqGetBuffer_Pre_Avail_No,
  RtemsPartReqGetBuffer_Pre_Avail_NA
} RtemsPartReqGetBuffer_Pre_Avail;

typedef enum {
  RtemsPartReqGetBuffer_Post_Status_Ok,
  RtemsPartReqGetBuffer_Post_Status_InvId,
  RtemsPartReqGetBuffer_Post_Status_InvAddr,
  RtemsPartReqGetBuffer_Post_Status_Unsat,
  RtemsPartReqGetBuffer_Post_Status_NA
} RtemsPartReqGetBuffer_Post_Status;

typedef enum {
  RtemsPartReqGetBuffer_Post_BufVar_Set,
  RtemsPartReqGetBuffer_Post_BufVar_Nop,
  RtemsPartReqGetBuffer_Post_BufVar_NA
} RtemsPartReqGetBuffer_Post_BufVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Buf_NA : 1;
  uint16_t Pre_Avail_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_BufVar : 2;
} RtemsPartReqGetBuffer_Entry;

/**
 * @brief Test context for spec:/rtems/part/req/get-buffer test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_id id;

  rtems_id id_value;

  void **buffer;

  void *buffer_pointer;

  void *stolen_buffer;

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
    RtemsPartReqGetBuffer_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsPartReqGetBuffer_Context;

static RtemsPartReqGetBuffer_Context
  RtemsPartReqGetBuffer_Instance;

static const char * const RtemsPartReqGetBuffer_PreDesc_Id[] = {
  "NoObj",
  "Part",
  "NA"
};

static const char * const RtemsPartReqGetBuffer_PreDesc_Buf[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsPartReqGetBuffer_PreDesc_Avail[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsPartReqGetBuffer_PreDesc[] = {
  RtemsPartReqGetBuffer_PreDesc_Id,
  RtemsPartReqGetBuffer_PreDesc_Buf,
  RtemsPartReqGetBuffer_PreDesc_Avail,
  NULL
};

#define BUFFER_COUNT 1

#define BUFFER_SIZE ( 2 * sizeof( void * ) )

static RTEMS_ALIGNED( RTEMS_PARTITION_ALIGNMENT ) uint8_t
  buffers[ BUFFER_COUNT ][ BUFFER_SIZE ];

static void RtemsPartReqGetBuffer_Pre_Id_Prepare(
  RtemsPartReqGetBuffer_Context *ctx,
  RtemsPartReqGetBuffer_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsPartReqGetBuffer_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with a partition.
       */
      ctx->id = 0xffffffff;
      break;
    }

    case RtemsPartReqGetBuffer_Pre_Id_Part: {
      /*
       * While the ``id`` parameter is associated with a partition.
       */
      ctx->id = ctx->id_value;
      break;
    }

    case RtemsPartReqGetBuffer_Pre_Id_NA:
      break;
  }
}

static void RtemsPartReqGetBuffer_Pre_Buf_Prepare(
  RtemsPartReqGetBuffer_Context *ctx,
  RtemsPartReqGetBuffer_Pre_Buf  state
)
{
  switch ( state ) {
    case RtemsPartReqGetBuffer_Pre_Buf_Valid: {
      /*
       * While the ``buffer`` parameter references an object of type ``void
       * *``.
       */
      ctx->buffer = &ctx->buffer_pointer;
      break;
    }

    case RtemsPartReqGetBuffer_Pre_Buf_Null: {
      /*
       * While the ``buffer`` parameter is NULL.
       */
      ctx->buffer = NULL;
      break;
    }

    case RtemsPartReqGetBuffer_Pre_Buf_NA:
      break;
  }
}

static void RtemsPartReqGetBuffer_Pre_Avail_Prepare(
  RtemsPartReqGetBuffer_Context  *ctx,
  RtemsPartReqGetBuffer_Pre_Avail state
)
{
  rtems_status_code sc;

  switch ( state ) {
    case RtemsPartReqGetBuffer_Pre_Avail_Yes: {
      /*
       * While the partition has at least one free buffer available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsPartReqGetBuffer_Pre_Avail_No: {
      /*
       * While the partition does not have a buffer available.
       */
      sc = rtems_partition_get_buffer( ctx->id_value, &ctx->stolen_buffer );
      T_rsc_success( sc );
      break;
    }

    case RtemsPartReqGetBuffer_Pre_Avail_NA:
      break;
  }
}

static void RtemsPartReqGetBuffer_Post_Status_Check(
  RtemsPartReqGetBuffer_Context    *ctx,
  RtemsPartReqGetBuffer_Post_Status state
)
{
  switch ( state ) {
    case RtemsPartReqGetBuffer_Post_Status_Ok: {
      /*
       * The return status of rtems_partition_get_buffer() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      T_eq_ptr( ctx->buffer_pointer, buffers );
      break;
    }

    case RtemsPartReqGetBuffer_Post_Status_InvId: {
      /*
       * The return status of rtems_partition_get_buffer() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      T_eq_ptr( ctx->buffer_pointer, (void *) (uintptr_t) 1 );
      break;
    }

    case RtemsPartReqGetBuffer_Post_Status_InvAddr: {
      /*
       * The return status of rtems_partition_get_buffer() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsPartReqGetBuffer_Post_Status_Unsat: {
      /*
       * The return status of rtems_partition_get_buffer() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED );
      T_eq_ptr( ctx->buffer_pointer, (void *) (uintptr_t) 1 );
      break;
    }

    case RtemsPartReqGetBuffer_Post_Status_NA:
      break;
  }
}

static void RtemsPartReqGetBuffer_Post_BufVar_Check(
  RtemsPartReqGetBuffer_Context    *ctx,
  RtemsPartReqGetBuffer_Post_BufVar state
)
{
  switch ( state ) {
    case RtemsPartReqGetBuffer_Post_BufVar_Set: {
      /*
       * The value of the object referenced by the ``starting_address``
       * parameter shall be set to the object identifier of the begin address
       * of the returned buffer after the return of the
       * rtems_partition_create() call.
       */
      T_eq_ptr( ctx->buffer, &ctx->buffer_pointer );
      T_eq_ptr( ctx->buffer_pointer, buffers );
      break;
    }

    case RtemsPartReqGetBuffer_Post_BufVar_Nop: {
      /*
       * Objects referenced by the ``buffer`` parameter in past calls to
       * rtems_partition_get_buffer() shall not be accessed by the
       * rtems_partition_get_buffer() call.
       */
      T_eq_ptr( ctx->buffer_pointer, (void *) (uintptr_t) 1 );
      break;
    }

    case RtemsPartReqGetBuffer_Post_BufVar_NA:
      break;
  }
}

static void RtemsPartReqGetBuffer_Setup( RtemsPartReqGetBuffer_Context *ctx )
{
  rtems_status_code sc;

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
}

static void RtemsPartReqGetBuffer_Setup_Wrap( void *arg )
{
  RtemsPartReqGetBuffer_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsPartReqGetBuffer_Setup( ctx );
}

static void RtemsPartReqGetBuffer_Teardown(
  RtemsPartReqGetBuffer_Context *ctx
)
{
  if ( ctx->id_value != 0 ) {
    rtems_status_code sc;

    sc = rtems_partition_delete( ctx->id_value );
    T_rsc_success( sc );
  }
}

static void RtemsPartReqGetBuffer_Teardown_Wrap( void *arg )
{
  RtemsPartReqGetBuffer_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsPartReqGetBuffer_Teardown( ctx );
}

static void RtemsPartReqGetBuffer_Prepare( RtemsPartReqGetBuffer_Context *ctx )
{
  ctx->buffer_pointer = (void *) (uintptr_t) 1;
  ctx->stolen_buffer = NULL;
}

static void RtemsPartReqGetBuffer_Action( RtemsPartReqGetBuffer_Context *ctx )
{
  ctx->status = rtems_partition_get_buffer( ctx->id, ctx->buffer );
}

static void RtemsPartReqGetBuffer_Cleanup( RtemsPartReqGetBuffer_Context *ctx )
{
  rtems_status_code sc;

  if ( (uintptr_t) ctx->buffer_pointer != 1 ) {
    sc = rtems_partition_return_buffer( ctx->id_value, ctx->buffer_pointer );
    T_rsc_success( sc );
  }

  if ( ctx->stolen_buffer != NULL ) {
    sc = rtems_partition_return_buffer( ctx->id_value, ctx->stolen_buffer );
    T_rsc_success( sc );
  }
}

static const RtemsPartReqGetBuffer_Entry
RtemsPartReqGetBuffer_Entries[] = {
  { 0, 0, 0, 0, RtemsPartReqGetBuffer_Post_Status_InvAddr,
    RtemsPartReqGetBuffer_Post_BufVar_Nop },
  { 0, 0, 0, 0, RtemsPartReqGetBuffer_Post_Status_InvId,
    RtemsPartReqGetBuffer_Post_BufVar_Nop },
  { 0, 0, 0, 0, RtemsPartReqGetBuffer_Post_Status_Ok,
    RtemsPartReqGetBuffer_Post_BufVar_Set },
  { 0, 0, 0, 0, RtemsPartReqGetBuffer_Post_Status_Unsat,
    RtemsPartReqGetBuffer_Post_BufVar_Nop }
};

static const uint8_t
RtemsPartReqGetBuffer_Map[] = {
  1, 1, 0, 0, 2, 3, 0, 0
};

static size_t RtemsPartReqGetBuffer_Scope( void *arg, char *buf, size_t n )
{
  RtemsPartReqGetBuffer_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsPartReqGetBuffer_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsPartReqGetBuffer_Fixture = {
  .setup = RtemsPartReqGetBuffer_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsPartReqGetBuffer_Teardown_Wrap,
  .scope = RtemsPartReqGetBuffer_Scope,
  .initial_context = &RtemsPartReqGetBuffer_Instance
};

static inline RtemsPartReqGetBuffer_Entry RtemsPartReqGetBuffer_PopEntry(
  RtemsPartReqGetBuffer_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsPartReqGetBuffer_Entries[
    RtemsPartReqGetBuffer_Map[ index ]
  ];
}

static void RtemsPartReqGetBuffer_TestVariant(
  RtemsPartReqGetBuffer_Context *ctx
)
{
  RtemsPartReqGetBuffer_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsPartReqGetBuffer_Pre_Buf_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsPartReqGetBuffer_Pre_Avail_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsPartReqGetBuffer_Action( ctx );
  RtemsPartReqGetBuffer_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsPartReqGetBuffer_Post_BufVar_Check( ctx, ctx->Map.entry.Post_BufVar );
}

/**
 * @fn void T_case_body_RtemsPartReqGetBuffer( void )
 */
T_TEST_CASE_FIXTURE( RtemsPartReqGetBuffer, &RtemsPartReqGetBuffer_Fixture )
{
  RtemsPartReqGetBuffer_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsPartReqGetBuffer_Pre_Id_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsPartReqGetBuffer_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsPartReqGetBuffer_Pre_Buf_Valid;
      ctx->Map.pcs[ 1 ] < RtemsPartReqGetBuffer_Pre_Buf_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsPartReqGetBuffer_Pre_Avail_Yes;
        ctx->Map.pcs[ 2 ] < RtemsPartReqGetBuffer_Pre_Avail_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsPartReqGetBuffer_PopEntry( ctx );
        RtemsPartReqGetBuffer_Prepare( ctx );
        RtemsPartReqGetBuffer_TestVariant( ctx );
        RtemsPartReqGetBuffer_Cleanup( ctx );
      }
    }
  }
}

/** @} */
