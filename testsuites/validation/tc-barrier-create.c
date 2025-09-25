/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsBarrierReqCreate
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
 * @defgroup RtemsBarrierReqCreate spec:/rtems/barrier/req/create
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsBarrierReqCreate_Pre_Name_Valid,
  RtemsBarrierReqCreate_Pre_Name_Invalid,
  RtemsBarrierReqCreate_Pre_Name_NA
} RtemsBarrierReqCreate_Pre_Name;

typedef enum {
  RtemsBarrierReqCreate_Pre_Id_Valid,
  RtemsBarrierReqCreate_Pre_Id_Null,
  RtemsBarrierReqCreate_Pre_Id_NA
} RtemsBarrierReqCreate_Pre_Id;

typedef enum {
  RtemsBarrierReqCreate_Pre_Class_Default,
  RtemsBarrierReqCreate_Pre_Class_Manual,
  RtemsBarrierReqCreate_Pre_Class_Auto,
  RtemsBarrierReqCreate_Pre_Class_NA
} RtemsBarrierReqCreate_Pre_Class;

typedef enum {
  RtemsBarrierReqCreate_Pre_MaxWait_Zero,
  RtemsBarrierReqCreate_Pre_MaxWait_Positive,
  RtemsBarrierReqCreate_Pre_MaxWait_NA
} RtemsBarrierReqCreate_Pre_MaxWait;

typedef enum {
  RtemsBarrierReqCreate_Pre_Free_Yes,
  RtemsBarrierReqCreate_Pre_Free_No,
  RtemsBarrierReqCreate_Pre_Free_NA
} RtemsBarrierReqCreate_Pre_Free;

typedef enum {
  RtemsBarrierReqCreate_Post_Status_Ok,
  RtemsBarrierReqCreate_Post_Status_InvName,
  RtemsBarrierReqCreate_Post_Status_InvAddr,
  RtemsBarrierReqCreate_Post_Status_InvNum,
  RtemsBarrierReqCreate_Post_Status_TooMany,
  RtemsBarrierReqCreate_Post_Status_NA
} RtemsBarrierReqCreate_Post_Status;

typedef enum {
  RtemsBarrierReqCreate_Post_Name_Valid,
  RtemsBarrierReqCreate_Post_Name_Invalid,
  RtemsBarrierReqCreate_Post_Name_NA
} RtemsBarrierReqCreate_Post_Name;

typedef enum {
  RtemsBarrierReqCreate_Post_Class_Manual,
  RtemsBarrierReqCreate_Post_Class_Auto,
  RtemsBarrierReqCreate_Post_Class_NA
} RtemsBarrierReqCreate_Post_Class;

typedef enum {
  RtemsBarrierReqCreate_Post_IdVar_Set,
  RtemsBarrierReqCreate_Post_IdVar_Nop,
  RtemsBarrierReqCreate_Post_IdVar_NA
} RtemsBarrierReqCreate_Post_IdVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Class_NA : 1;
  uint16_t Pre_MaxWait_NA : 1;
  uint16_t Pre_Free_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Name : 2;
  uint16_t Post_Class : 2;
  uint16_t Post_IdVar : 2;
} RtemsBarrierReqCreate_Entry;

/**
 * @brief Test context for spec:/rtems/barrier/req/create test case.
 */
typedef struct {
  void *seized_objects;

  rtems_id worker_id;

  rtems_id id_value;

  uint32_t release_done;

  uint32_t release_expected;

  rtems_name name;

  rtems_attribute attribute_set;

  uint32_t maximum_waiters;

  rtems_id *id;

  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 5 ];

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
    RtemsBarrierReqCreate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsBarrierReqCreate_Context;

static RtemsBarrierReqCreate_Context
  RtemsBarrierReqCreate_Instance;

static const char * const RtemsBarrierReqCreate_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsBarrierReqCreate_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsBarrierReqCreate_PreDesc_Class[] = {
  "Default",
  "Manual",
  "Auto",
  "NA"
};

static const char * const RtemsBarrierReqCreate_PreDesc_MaxWait[] = {
  "Zero",
  "Positive",
  "NA"
};

static const char * const RtemsBarrierReqCreate_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsBarrierReqCreate_PreDesc[] = {
  RtemsBarrierReqCreate_PreDesc_Name,
  RtemsBarrierReqCreate_PreDesc_Id,
  RtemsBarrierReqCreate_PreDesc_Class,
  RtemsBarrierReqCreate_PreDesc_MaxWait,
  RtemsBarrierReqCreate_PreDesc_Free,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

typedef RtemsBarrierReqCreate_Context Context;

static void Worker( rtems_task_argument arg )
{
  Context *ctx;

  ctx = (Context *) arg;

  while ( true ) {
    rtems_status_code sc;
    uint32_t          released;

    ++ctx->release_done;

    released = 0;
    sc = rtems_barrier_release( ctx->id_value, &released );
    T_rsc_success( sc );
    T_eq_u32( released, 1 );
  }
}

static rtems_status_code Create( void *arg, uint32_t *id )
{
  (void) arg;

  return rtems_barrier_create(
    rtems_build_name( 'S', 'I', 'Z', 'E' ),
    RTEMS_DEFAULT_ATTRIBUTES,
    0,
    id
  );
}

static void RtemsBarrierReqCreate_Pre_Name_Prepare(
  RtemsBarrierReqCreate_Context *ctx,
  RtemsBarrierReqCreate_Pre_Name state
)
{
  switch ( state ) {
    case RtemsBarrierReqCreate_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is valid.
       */
      ctx->name = NAME;
      break;
    }

    case RtemsBarrierReqCreate_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is invalid.
       */
      ctx->name = 0;
      break;
    }

    case RtemsBarrierReqCreate_Pre_Name_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Pre_Id_Prepare(
  RtemsBarrierReqCreate_Context *ctx,
  RtemsBarrierReqCreate_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsBarrierReqCreate_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsBarrierReqCreate_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsBarrierReqCreate_Pre_Id_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Pre_Class_Prepare(
  RtemsBarrierReqCreate_Context  *ctx,
  RtemsBarrierReqCreate_Pre_Class state
)
{
  switch ( state ) {
    case RtemsBarrierReqCreate_Pre_Class_Default: {
      /*
       * While the ``attribute_set`` parameter specifies the default class.
       */
      /* Nothing to do */
      break;
    }

    case RtemsBarrierReqCreate_Pre_Class_Manual: {
      /*
       * While the ``attribute_set`` parameter specifies the manual release
       * class.
       */
      ctx->attribute_set |= RTEMS_BARRIER_MANUAL_RELEASE;
      break;
    }

    case RtemsBarrierReqCreate_Pre_Class_Auto: {
      /*
       * While the ``attribute_set`` parameter specifies the automatic release
       * class.
       */
      ctx->attribute_set |= RTEMS_BARRIER_AUTOMATIC_RELEASE;
      break;
    }

    case RtemsBarrierReqCreate_Pre_Class_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Pre_MaxWait_Prepare(
  RtemsBarrierReqCreate_Context    *ctx,
  RtemsBarrierReqCreate_Pre_MaxWait state
)
{
  switch ( state ) {
    case RtemsBarrierReqCreate_Pre_MaxWait_Zero: {
      /*
       * While the ``maximum_waiters`` parameter is zero.
       */
      ctx->maximum_waiters = 0;
      break;
    }

    case RtemsBarrierReqCreate_Pre_MaxWait_Positive: {
      /*
       * While the ``maximum_waiters`` parameter is positive.
       */
      ctx->maximum_waiters = 1;
      break;
    }

    case RtemsBarrierReqCreate_Pre_MaxWait_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Pre_Free_Prepare(
  RtemsBarrierReqCreate_Context *ctx,
  RtemsBarrierReqCreate_Pre_Free state
)
{
  switch ( state ) {
    case RtemsBarrierReqCreate_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive barrier object available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsBarrierReqCreate_Pre_Free_No: {
      /*
       * While the system has no inactive barrier object available.
       */
      ctx->seized_objects = T_seize_objects( Create, NULL );
      break;
    }

    case RtemsBarrierReqCreate_Pre_Free_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Post_Status_Check(
  RtemsBarrierReqCreate_Context    *ctx,
  RtemsBarrierReqCreate_Post_Status state
)
{
  switch ( state ) {
    case RtemsBarrierReqCreate_Post_Status_Ok: {
      /*
       * The return status of rtems_barrier_create() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsBarrierReqCreate_Post_Status_InvName: {
      /*
       * The return status of rtems_barrier_create() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsBarrierReqCreate_Post_Status_InvAddr: {
      /*
       * The return status of rtems_barrier_create() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsBarrierReqCreate_Post_Status_InvNum: {
      /*
       * The return status of rtems_barrier_create() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsBarrierReqCreate_Post_Status_TooMany: {
      /*
       * The return status of rtems_barrier_create() shall be RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsBarrierReqCreate_Post_Status_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Post_Name_Check(
  RtemsBarrierReqCreate_Context  *ctx,
  RtemsBarrierReqCreate_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsBarrierReqCreate_Post_Name_Valid: {
      /*
       * The unique object name shall identify the barrier created by the
       * rtems_barrier_create() call.
       */
      id = 0;
      sc = rtems_barrier_ident( NAME, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsBarrierReqCreate_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify a barrier.
       */
      sc = rtems_barrier_ident( NAME, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsBarrierReqCreate_Post_Name_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Post_Class_Check(
  RtemsBarrierReqCreate_Context   *ctx,
  RtemsBarrierReqCreate_Post_Class state
)
{
  rtems_status_code sc;

  switch ( state ) {
    case RtemsBarrierReqCreate_Post_Class_Manual: {
      /*
       * The class of the barrier created by the rtems_barrier_create() call
       * shall be manual release.
       */
      sc = rtems_barrier_wait( ctx->id_value, RTEMS_NO_TIMEOUT );
      T_rsc_success( sc );

      ++ctx->release_expected;
      T_eq_u32( ctx->release_done, ctx->release_expected );
      break;
    }

    case RtemsBarrierReqCreate_Post_Class_Auto: {
      /*
       * The class of the barrier created by the rtems_barrier_create() call
       * shall be automatic release.
       */
      sc = rtems_barrier_wait( ctx->id_value, RTEMS_NO_TIMEOUT );
      T_rsc_success( sc );

      T_eq_u32( ctx->release_done, ctx->release_expected );
      break;
    }

    case RtemsBarrierReqCreate_Post_Class_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Post_IdVar_Check(
  RtemsBarrierReqCreate_Context   *ctx,
  RtemsBarrierReqCreate_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsBarrierReqCreate_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the object identifier of the created barrier after the return
       * of the rtems_barrier_create() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsBarrierReqCreate_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_barrier_create() shall not be accessed by the
       * rtems_barrier_create() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsBarrierReqCreate_Post_IdVar_NA:
      break;
  }
}

static void RtemsBarrierReqCreate_Setup( RtemsBarrierReqCreate_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->id_value = INVALID_ID;
  ctx->worker_id = CreateTask( "WORK", PRIO_LOW );
  StartTask( ctx->worker_id, Worker, ctx );
}

static void RtemsBarrierReqCreate_Setup_Wrap( void *arg )
{
  RtemsBarrierReqCreate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsBarrierReqCreate_Setup( ctx );
}

static void RtemsBarrierReqCreate_Teardown(
  RtemsBarrierReqCreate_Context *ctx
)
{
  DeleteTask( ctx->worker_id );
}

static void RtemsBarrierReqCreate_Teardown_Wrap( void *arg )
{
  RtemsBarrierReqCreate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsBarrierReqCreate_Teardown( ctx );
}

static void RtemsBarrierReqCreate_Action( RtemsBarrierReqCreate_Context *ctx )
{
  ctx->status = rtems_barrier_create(
    ctx->name,
    ctx->attribute_set,
    ctx->maximum_waiters,
    ctx->id
  );
}

static void RtemsBarrierReqCreate_Cleanup( RtemsBarrierReqCreate_Context *ctx )
{
  if ( ctx->id_value != INVALID_ID ) {
    rtems_status_code sc;

    sc = rtems_barrier_delete( ctx->id_value );
    T_rsc_success( sc );

    ctx->id_value = INVALID_ID;
  }

  T_surrender_objects( &ctx->seized_objects, rtems_barrier_delete );
}

static const RtemsBarrierReqCreate_Entry
RtemsBarrierReqCreate_Entries[] = {
  { 0, 0, 0, 0, 0, 0, RtemsBarrierReqCreate_Post_Status_InvName,
    RtemsBarrierReqCreate_Post_Name_Invalid,
    RtemsBarrierReqCreate_Post_Class_NA, RtemsBarrierReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsBarrierReqCreate_Post_Status_InvAddr,
    RtemsBarrierReqCreate_Post_Name_Invalid,
    RtemsBarrierReqCreate_Post_Class_NA, RtemsBarrierReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsBarrierReqCreate_Post_Status_TooMany,
    RtemsBarrierReqCreate_Post_Name_Invalid,
    RtemsBarrierReqCreate_Post_Class_NA, RtemsBarrierReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsBarrierReqCreate_Post_Status_Ok,
    RtemsBarrierReqCreate_Post_Name_Valid,
    RtemsBarrierReqCreate_Post_Class_Manual,
    RtemsBarrierReqCreate_Post_IdVar_Set },
  { 0, 0, 0, 0, 0, 0, RtemsBarrierReqCreate_Post_Status_InvNum,
    RtemsBarrierReqCreate_Post_Name_Invalid,
    RtemsBarrierReqCreate_Post_Class_NA, RtemsBarrierReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsBarrierReqCreate_Post_Status_Ok,
    RtemsBarrierReqCreate_Post_Name_Valid,
    RtemsBarrierReqCreate_Post_Class_Auto, RtemsBarrierReqCreate_Post_IdVar_Set }
};

static const uint8_t
RtemsBarrierReqCreate_Map[] = {
  3, 2, 3, 2, 3, 2, 3, 2, 4, 4, 5, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static size_t RtemsBarrierReqCreate_Scope( void *arg, char *buf, size_t n )
{
  RtemsBarrierReqCreate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsBarrierReqCreate_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsBarrierReqCreate_Fixture = {
  .setup = RtemsBarrierReqCreate_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsBarrierReqCreate_Teardown_Wrap,
  .scope = RtemsBarrierReqCreate_Scope,
  .initial_context = &RtemsBarrierReqCreate_Instance
};

static inline RtemsBarrierReqCreate_Entry RtemsBarrierReqCreate_PopEntry(
  RtemsBarrierReqCreate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsBarrierReqCreate_Entries[
    RtemsBarrierReqCreate_Map[ index ]
  ];
}

static void RtemsBarrierReqCreate_TestVariant(
  RtemsBarrierReqCreate_Context *ctx
)
{
  RtemsBarrierReqCreate_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsBarrierReqCreate_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsBarrierReqCreate_Pre_Class_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsBarrierReqCreate_Pre_MaxWait_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsBarrierReqCreate_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsBarrierReqCreate_Action( ctx );
  RtemsBarrierReqCreate_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsBarrierReqCreate_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsBarrierReqCreate_Post_Class_Check( ctx, ctx->Map.entry.Post_Class );
  RtemsBarrierReqCreate_Post_IdVar_Check( ctx, ctx->Map.entry.Post_IdVar );
}

/**
 * @fn void T_case_body_RtemsBarrierReqCreate( void )
 */
T_TEST_CASE_FIXTURE( RtemsBarrierReqCreate, &RtemsBarrierReqCreate_Fixture )
{
  RtemsBarrierReqCreate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsBarrierReqCreate_Pre_Name_Valid;
    ctx->Map.pcs[ 0 ] < RtemsBarrierReqCreate_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsBarrierReqCreate_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsBarrierReqCreate_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsBarrierReqCreate_Pre_Class_Default;
        ctx->Map.pcs[ 2 ] < RtemsBarrierReqCreate_Pre_Class_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsBarrierReqCreate_Pre_MaxWait_Zero;
          ctx->Map.pcs[ 3 ] < RtemsBarrierReqCreate_Pre_MaxWait_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          for (
            ctx->Map.pcs[ 4 ] = RtemsBarrierReqCreate_Pre_Free_Yes;
            ctx->Map.pcs[ 4 ] < RtemsBarrierReqCreate_Pre_Free_NA;
            ++ctx->Map.pcs[ 4 ]
          ) {
            ctx->Map.entry = RtemsBarrierReqCreate_PopEntry( ctx );
            RtemsBarrierReqCreate_TestVariant( ctx );
            RtemsBarrierReqCreate_Cleanup( ctx );
          }
        }
      }
    }
  }
}

/** @} */
