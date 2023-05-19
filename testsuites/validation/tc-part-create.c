/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsPartReqCreate
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
#include <string.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsPartReqCreate spec:/rtems/part/req/create
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsPartReqCreate_Pre_Name_Valid,
  RtemsPartReqCreate_Pre_Name_Invalid,
  RtemsPartReqCreate_Pre_Name_NA
} RtemsPartReqCreate_Pre_Name;

typedef enum {
  RtemsPartReqCreate_Pre_Id_Valid,
  RtemsPartReqCreate_Pre_Id_Null,
  RtemsPartReqCreate_Pre_Id_NA
} RtemsPartReqCreate_Pre_Id;

typedef enum {
  RtemsPartReqCreate_Pre_Start_Valid,
  RtemsPartReqCreate_Pre_Start_Null,
  RtemsPartReqCreate_Pre_Start_BadAlign,
  RtemsPartReqCreate_Pre_Start_NA
} RtemsPartReqCreate_Pre_Start;

typedef enum {
  RtemsPartReqCreate_Pre_Length_Valid,
  RtemsPartReqCreate_Pre_Length_Zero,
  RtemsPartReqCreate_Pre_Length_Invalid,
  RtemsPartReqCreate_Pre_Length_NA
} RtemsPartReqCreate_Pre_Length;

typedef enum {
  RtemsPartReqCreate_Pre_Size_Valid,
  RtemsPartReqCreate_Pre_Size_Zero,
  RtemsPartReqCreate_Pre_Size_Skew,
  RtemsPartReqCreate_Pre_Size_Small,
  RtemsPartReqCreate_Pre_Size_NA
} RtemsPartReqCreate_Pre_Size;

typedef enum {
  RtemsPartReqCreate_Pre_Free_Yes,
  RtemsPartReqCreate_Pre_Free_No,
  RtemsPartReqCreate_Pre_Free_NA
} RtemsPartReqCreate_Pre_Free;

typedef enum {
  RtemsPartReqCreate_Post_Status_Ok,
  RtemsPartReqCreate_Post_Status_InvAddr,
  RtemsPartReqCreate_Post_Status_InvName,
  RtemsPartReqCreate_Post_Status_InvSize,
  RtemsPartReqCreate_Post_Status_TooMany,
  RtemsPartReqCreate_Post_Status_NA
} RtemsPartReqCreate_Post_Status;

typedef enum {
  RtemsPartReqCreate_Post_Name_Valid,
  RtemsPartReqCreate_Post_Name_Invalid,
  RtemsPartReqCreate_Post_Name_NA
} RtemsPartReqCreate_Post_Name;

typedef enum {
  RtemsPartReqCreate_Post_IdVar_Set,
  RtemsPartReqCreate_Post_IdVar_Nop,
  RtemsPartReqCreate_Post_IdVar_NA
} RtemsPartReqCreate_Post_IdVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Start_NA : 1;
  uint16_t Pre_Length_NA : 1;
  uint16_t Pre_Size_NA : 1;
  uint16_t Pre_Free_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Name : 2;
  uint16_t Post_IdVar : 2;
} RtemsPartReqCreate_Entry;

/**
 * @brief Test context for spec:/rtems/part/req/create test case.
 */
typedef struct {
  void *seized_objects;

  rtems_status_code status;

  rtems_name name;

  void *starting_address;

  uintptr_t length;

  size_t buffer_size;

  rtems_attribute attribute_set;

  rtems_id *id;

  rtems_id id_value;

  struct {
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
    RtemsPartReqCreate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsPartReqCreate_Context;

static RtemsPartReqCreate_Context
  RtemsPartReqCreate_Instance;

static const char * const RtemsPartReqCreate_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsPartReqCreate_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsPartReqCreate_PreDesc_Start[] = {
  "Valid",
  "Null",
  "BadAlign",
  "NA"
};

static const char * const RtemsPartReqCreate_PreDesc_Length[] = {
  "Valid",
  "Zero",
  "Invalid",
  "NA"
};

static const char * const RtemsPartReqCreate_PreDesc_Size[] = {
  "Valid",
  "Zero",
  "Skew",
  "Small",
  "NA"
};

static const char * const RtemsPartReqCreate_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsPartReqCreate_PreDesc[] = {
  RtemsPartReqCreate_PreDesc_Name,
  RtemsPartReqCreate_PreDesc_Id,
  RtemsPartReqCreate_PreDesc_Start,
  RtemsPartReqCreate_PreDesc_Length,
  RtemsPartReqCreate_PreDesc_Size,
  RtemsPartReqCreate_PreDesc_Free,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

#define MAX_PARTITIONS ( TEST_MAXIMUM_PARTITIONS + 1 )

#define BUFFER_COUNT 2

#define BUFFER_SIZE ( 2 * sizeof( void * ) )

static RTEMS_ALIGNED( RTEMS_PARTITION_ALIGNMENT ) uint8_t
  buffers_to_seize[ MAX_PARTITIONS ][ BUFFER_COUNT ][ BUFFER_SIZE ];

static RTEMS_ALIGNED( RTEMS_PARTITION_ALIGNMENT ) uint8_t
  buffers[ BUFFER_COUNT ][ BUFFER_SIZE ];

static rtems_status_code Create( void *arg, uint32_t *id )
{
  size_t *i;
  size_t  j;

  i = arg;
  j = *i;
  T_quiet_lt_sz( j, MAX_PARTITIONS );

  *i = j + 1;

  return rtems_partition_create(
    rtems_build_name( 'S', 'I', 'Z', 'E' ),
    buffers_to_seize[ j ],
    sizeof( buffers_to_seize[ j ] ),
    sizeof( buffers_to_seize[ j ][ 0 ] ),
    RTEMS_DEFAULT_ATTRIBUTES,
    id
  );
}

static void RtemsPartReqCreate_Pre_Name_Prepare(
  RtemsPartReqCreate_Context *ctx,
  RtemsPartReqCreate_Pre_Name state
)
{
  switch ( state ) {
    case RtemsPartReqCreate_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is valid.
       */
      ctx->name = NAME;
      break;
    }

    case RtemsPartReqCreate_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is invalid.
       */
      ctx->name = 0;
      break;
    }

    case RtemsPartReqCreate_Pre_Name_NA:
      break;
  }
}

static void RtemsPartReqCreate_Pre_Id_Prepare(
  RtemsPartReqCreate_Context *ctx,
  RtemsPartReqCreate_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsPartReqCreate_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsPartReqCreate_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsPartReqCreate_Pre_Id_NA:
      break;
  }
}

static void RtemsPartReqCreate_Pre_Start_Prepare(
  RtemsPartReqCreate_Context  *ctx,
  RtemsPartReqCreate_Pre_Start state
)
{
  switch ( state ) {
    case RtemsPartReqCreate_Pre_Start_Valid: {
      /*
       * While the ``starting_address`` parameter is valid.
       */
      ctx->starting_address = buffers;
      break;
    }

    case RtemsPartReqCreate_Pre_Start_Null: {
      /*
       * While the ``starting_address`` parameter is NULL.
       */
      ctx->starting_address = NULL;
      break;
    }

    case RtemsPartReqCreate_Pre_Start_BadAlign: {
      /*
       * While the ``starting_address`` parameter is misaligned.
       */
      ctx->starting_address = &buffers[ 0 ][ 1 ];
      break;
    }

    case RtemsPartReqCreate_Pre_Start_NA:
      break;
  }
}

static void RtemsPartReqCreate_Pre_Length_Prepare(
  RtemsPartReqCreate_Context   *ctx,
  RtemsPartReqCreate_Pre_Length state
)
{
  switch ( state ) {
    case RtemsPartReqCreate_Pre_Length_Valid: {
      /*
       * While the ``length`` parameter is valid.
       */
      ctx->length = sizeof( buffers );
      break;
    }

    case RtemsPartReqCreate_Pre_Length_Zero: {
      /*
       * While the ``length`` parameter is zero.
       */
      ctx->length = 0;
      break;
    }

    case RtemsPartReqCreate_Pre_Length_Invalid: {
      /*
       * While the ``length`` parameter is less than the buffer size.
       */
      ctx->length = sizeof( buffers[ 0 ] ) - 1;
      break;
    }

    case RtemsPartReqCreate_Pre_Length_NA:
      break;
  }
}

static void RtemsPartReqCreate_Pre_Size_Prepare(
  RtemsPartReqCreate_Context *ctx,
  RtemsPartReqCreate_Pre_Size state
)
{
  switch ( state ) {
    case RtemsPartReqCreate_Pre_Size_Valid: {
      /*
       * While the ``buffer_size`` parameter is valid.
       */
      ctx->buffer_size = sizeof( buffers[ 0 ] );
      break;
    }

    case RtemsPartReqCreate_Pre_Size_Zero: {
      /*
       * While the ``buffer_size`` parameter is zero.
       */
      ctx->buffer_size = 0;
      break;
    }

    case RtemsPartReqCreate_Pre_Size_Skew: {
      /*
       * The ``buffer_size`` parameter shall not an integral multiple of the
       * pointer size.
       */
      ctx->buffer_size = 1;
      break;
    }

    case RtemsPartReqCreate_Pre_Size_Small: {
      /*
       * The ``buffer_size`` parameter shall greater than zero and an integral
       * multiple of the pointer size and less than the size of two pointers.
       */
      ctx->buffer_size = sizeof( uintptr_t );
      break;
    }

    case RtemsPartReqCreate_Pre_Size_NA:
      break;
  }
}

static void RtemsPartReqCreate_Pre_Free_Prepare(
  RtemsPartReqCreate_Context *ctx,
  RtemsPartReqCreate_Pre_Free state
)
{
  size_t i;

  switch ( state ) {
    case RtemsPartReqCreate_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive partition object available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsPartReqCreate_Pre_Free_No: {
      /*
       * While the system has no inactive partition object available.
       */
      i = 0;
      ctx->seized_objects = T_seize_objects( Create, &i );
      break;
    }

    case RtemsPartReqCreate_Pre_Free_NA:
      break;
  }
}

static void RtemsPartReqCreate_Post_Status_Check(
  RtemsPartReqCreate_Context    *ctx,
  RtemsPartReqCreate_Post_Status state
)
{
  switch ( state ) {
    case RtemsPartReqCreate_Post_Status_Ok: {
      /*
       * The return status of rtems_partition_create() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsPartReqCreate_Post_Status_InvAddr: {
      /*
       * The return status of rtems_partition_create() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsPartReqCreate_Post_Status_InvName: {
      /*
       * The return status of rtems_partition_create() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsPartReqCreate_Post_Status_InvSize: {
      /*
       * The return status of rtems_partition_create() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsPartReqCreate_Post_Status_TooMany: {
      /*
       * The return status of rtems_partition_create() shall be RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsPartReqCreate_Post_Status_NA:
      break;
  }
}

static void RtemsPartReqCreate_Post_Name_Check(
  RtemsPartReqCreate_Context  *ctx,
  RtemsPartReqCreate_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsPartReqCreate_Post_Name_Valid: {
      /*
       * The unique object name shall identify the partition created by the
       * rtems_partition_create() call.
       */
      id = 0;
      sc = rtems_partition_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsPartReqCreate_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a partition.
       */
      sc = rtems_partition_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsPartReqCreate_Post_Name_NA:
      break;
  }
}

static void RtemsPartReqCreate_Post_IdVar_Check(
  RtemsPartReqCreate_Context   *ctx,
  RtemsPartReqCreate_Post_IdVar state
)
{
  rtems_status_code sc;
  size_t            i;
  void             *buffers[ BUFFER_COUNT ];
  void             *no_buffer;

  switch ( state ) {
    case RtemsPartReqCreate_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the object identifier of the created partition after the return
       * of the rtems_partition_create() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );

      for ( i = 0; i < BUFFER_COUNT; ++i) {
        sc = rtems_partition_get_buffer( ctx->id_value, &buffers[ i ] );
        T_rsc_success( sc );
        T_not_null( buffers[ i ] );
      }

      no_buffer = (void *) (uintptr_t) 1;
      sc = rtems_partition_get_buffer( ctx->id_value, &no_buffer );
      T_rsc( sc, RTEMS_UNSATISFIED );
      T_eq_ptr( no_buffer, (void *) (uintptr_t) 1 );

      for ( i = 0; i < BUFFER_COUNT; ++i) {
        sc = rtems_partition_return_buffer( ctx->id_value, buffers[ i ] );
        T_rsc_success( sc );
      }

      sc = rtems_partition_delete( ctx->id_value );
      T_rsc_success( sc );
      break;
    }

    case RtemsPartReqCreate_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_partition_create() shall not be accessed by the
       * rtems_partition_create() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsPartReqCreate_Post_IdVar_NA:
      break;
  }
}

static void RtemsPartReqCreate_Prepare( RtemsPartReqCreate_Context *ctx )
{
  rtems_status_code sc;
  rtems_id          id;

  ctx->id_value = INVALID_ID;
  ctx->attribute_set = RTEMS_DEFAULT_ATTRIBUTES;

  id = INVALID_ID;
  sc = rtems_partition_ident( NAME, RTEMS_SEARCH_LOCAL_NODE, &id );
  T_rsc( sc, RTEMS_INVALID_NAME );
  T_eq_u32( id, INVALID_ID );
}

static void RtemsPartReqCreate_Action( RtemsPartReqCreate_Context *ctx )
{
  ctx->status = rtems_partition_create(
    ctx->name,
    ctx->starting_address,
    ctx->length,
    ctx->buffer_size,
    ctx->attribute_set,
    ctx->id
  );
}

static void RtemsPartReqCreate_Cleanup( RtemsPartReqCreate_Context *ctx )
{
  T_surrender_objects( &ctx->seized_objects, rtems_partition_delete );
}

static const RtemsPartReqCreate_Entry
RtemsPartReqCreate_Entries[] = {
  { 0, 0, 0, 0, 0, 0, 0, RtemsPartReqCreate_Post_Status_InvName,
    RtemsPartReqCreate_Post_Name_Invalid, RtemsPartReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsPartReqCreate_Post_Status_InvAddr,
    RtemsPartReqCreate_Post_Name_Invalid, RtemsPartReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsPartReqCreate_Post_Status_InvSize,
    RtemsPartReqCreate_Post_Name_Invalid, RtemsPartReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsPartReqCreate_Post_Status_Ok,
    RtemsPartReqCreate_Post_Name_Valid, RtemsPartReqCreate_Post_IdVar_Set },
  { 0, 0, 0, 0, 0, 0, 0, RtemsPartReqCreate_Post_Status_TooMany,
    RtemsPartReqCreate_Post_Name_Invalid, RtemsPartReqCreate_Post_IdVar_Nop }
};

static const uint8_t
RtemsPartReqCreate_Map[] = {
  3, 4, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2,
  2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0
};

static size_t RtemsPartReqCreate_Scope( void *arg, char *buf, size_t n )
{
  RtemsPartReqCreate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsPartReqCreate_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsPartReqCreate_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsPartReqCreate_Scope,
  .initial_context = &RtemsPartReqCreate_Instance
};

static inline RtemsPartReqCreate_Entry RtemsPartReqCreate_PopEntry(
  RtemsPartReqCreate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsPartReqCreate_Entries[
    RtemsPartReqCreate_Map[ index ]
  ];
}

static void RtemsPartReqCreate_TestVariant( RtemsPartReqCreate_Context *ctx )
{
  RtemsPartReqCreate_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsPartReqCreate_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsPartReqCreate_Pre_Start_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsPartReqCreate_Pre_Length_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsPartReqCreate_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsPartReqCreate_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsPartReqCreate_Action( ctx );
  RtemsPartReqCreate_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsPartReqCreate_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsPartReqCreate_Post_IdVar_Check( ctx, ctx->Map.entry.Post_IdVar );
}

/**
 * @fn void T_case_body_RtemsPartReqCreate( void )
 */
T_TEST_CASE_FIXTURE( RtemsPartReqCreate, &RtemsPartReqCreate_Fixture )
{
  RtemsPartReqCreate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsPartReqCreate_Pre_Name_Valid;
    ctx->Map.pcs[ 0 ] < RtemsPartReqCreate_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsPartReqCreate_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsPartReqCreate_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsPartReqCreate_Pre_Start_Valid;
        ctx->Map.pcs[ 2 ] < RtemsPartReqCreate_Pre_Start_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsPartReqCreate_Pre_Length_Valid;
          ctx->Map.pcs[ 3 ] < RtemsPartReqCreate_Pre_Length_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsPartReqCreate_Pre_Size_Valid;
            ctx->Map.pcs[ 4 ] < RtemsPartReqCreate_Pre_Size_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            for (
              ctx->Map.pcs[ 5 ] = RtemsPartReqCreate_Pre_Free_Yes;
              ctx->Map.pcs[ 5 ] < RtemsPartReqCreate_Pre_Free_NA;
              ++ctx->Map.pcs[ 5 ]
            ) {
              ctx->Map.entry = RtemsPartReqCreate_PopEntry( ctx );
              RtemsPartReqCreate_Prepare( ctx );
              RtemsPartReqCreate_TestVariant( ctx );
              RtemsPartReqCreate_Cleanup( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
