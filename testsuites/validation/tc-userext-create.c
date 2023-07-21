/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsUserextReqCreate
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
 * @defgroup RtemsUserextReqCreate spec:/rtems/userext/req/create
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsUserextReqCreate_Pre_Name_Valid,
  RtemsUserextReqCreate_Pre_Name_Invalid,
  RtemsUserextReqCreate_Pre_Name_NA
} RtemsUserextReqCreate_Pre_Name;

typedef enum {
  RtemsUserextReqCreate_Pre_Id_Valid,
  RtemsUserextReqCreate_Pre_Id_Null,
  RtemsUserextReqCreate_Pre_Id_NA
} RtemsUserextReqCreate_Pre_Id;

typedef enum {
  RtemsUserextReqCreate_Pre_Table_TdSw,
  RtemsUserextReqCreate_Pre_Table_NoTdSw,
  RtemsUserextReqCreate_Pre_Table_Null,
  RtemsUserextReqCreate_Pre_Table_NA
} RtemsUserextReqCreate_Pre_Table;

typedef enum {
  RtemsUserextReqCreate_Pre_Free_Yes,
  RtemsUserextReqCreate_Pre_Free_No,
  RtemsUserextReqCreate_Pre_Free_NA
} RtemsUserextReqCreate_Pre_Free;

typedef enum {
  RtemsUserextReqCreate_Post_Status_Ok,
  RtemsUserextReqCreate_Post_Status_InvName,
  RtemsUserextReqCreate_Post_Status_InvAddr,
  RtemsUserextReqCreate_Post_Status_TooMany,
  RtemsUserextReqCreate_Post_Status_NA
} RtemsUserextReqCreate_Post_Status;

typedef enum {
  RtemsUserextReqCreate_Post_Name_Valid,
  RtemsUserextReqCreate_Post_Name_Invalid,
  RtemsUserextReqCreate_Post_Name_NA
} RtemsUserextReqCreate_Post_Name;

typedef enum {
  RtemsUserextReqCreate_Post_IdVar_Set,
  RtemsUserextReqCreate_Post_IdVar_Nop,
  RtemsUserextReqCreate_Post_IdVar_NA
} RtemsUserextReqCreate_Post_IdVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Name_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_Table_NA : 1;
  uint16_t Pre_Free_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Name : 2;
  uint16_t Post_IdVar : 2;
} RtemsUserextReqCreate_Entry;

/**
 * @brief Test context for spec:/rtems/userext/req/create test case.
 */
typedef struct {
  void *seized_objects;

  rtems_extensions_table table_variable;

  rtems_id id_value;

  rtems_name name;

  rtems_extensions_table *table;

  rtems_id *id;

  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 4 ];

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
    RtemsUserextReqCreate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsUserextReqCreate_Context;

static RtemsUserextReqCreate_Context
  RtemsUserextReqCreate_Instance;

static const char * const RtemsUserextReqCreate_PreDesc_Name[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsUserextReqCreate_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsUserextReqCreate_PreDesc_Table[] = {
  "TdSw",
  "NoTdSw",
  "Null",
  "NA"
};

static const char * const RtemsUserextReqCreate_PreDesc_Free[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsUserextReqCreate_PreDesc[] = {
  RtemsUserextReqCreate_PreDesc_Name,
  RtemsUserextReqCreate_PreDesc_Id,
  RtemsUserextReqCreate_PreDesc_Table,
  RtemsUserextReqCreate_PreDesc_Free,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

static rtems_status_code Create( void *arg, uint32_t *id )
{
  static const rtems_extensions_table table;

  return rtems_extension_create(
    rtems_build_name( 'S', 'I', 'Z', 'E' ),
    &table,
    id
  );
}

static void ThreadSwitch( rtems_tcb *executing,  rtems_tcb *heir)
{
  (void) executing;
  (void) heir;
}

static void RtemsUserextReqCreate_Pre_Name_Prepare(
  RtemsUserextReqCreate_Context *ctx,
  RtemsUserextReqCreate_Pre_Name state
)
{
  switch ( state ) {
    case RtemsUserextReqCreate_Pre_Name_Valid: {
      /*
       * While the ``name`` parameter is valid.
       */
      ctx->name = NAME;
      break;
    }

    case RtemsUserextReqCreate_Pre_Name_Invalid: {
      /*
       * While the ``name`` parameter is invalid.
       */
      ctx->name = 0;
      break;
    }

    case RtemsUserextReqCreate_Pre_Name_NA:
      break;
  }
}

static void RtemsUserextReqCreate_Pre_Id_Prepare(
  RtemsUserextReqCreate_Context *ctx,
  RtemsUserextReqCreate_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsUserextReqCreate_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsUserextReqCreate_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsUserextReqCreate_Pre_Id_NA:
      break;
  }
}

static void RtemsUserextReqCreate_Pre_Table_Prepare(
  RtemsUserextReqCreate_Context  *ctx,
  RtemsUserextReqCreate_Pre_Table state
)
{
  switch ( state ) {
    case RtemsUserextReqCreate_Pre_Table_TdSw: {
      /*
       * While the ``extension_table`` parameter references an object of type
       * rtems_extensions_table, while all extensions except the thread switch
       * extension of the referenced object are set to NULL or the address of a
       * corresponding extension, while the thread switch extension of the
       * referenced object is set to the address of a thread switch extension.
       */
      ctx->table = &ctx->table_variable;
      ctx->table_variable.thread_switch = ThreadSwitch;
      break;
    }

    case RtemsUserextReqCreate_Pre_Table_NoTdSw: {
      /*
       * While the ``extension_table`` parameter references an object of type
       * rtems_id, while all extensions except the thread switch extension of
       * the referenced object are set to NULL or the address of a
       * corresponding extension, while the thread switch extension of the
       * referenced object is set to NULL.
       */
      ctx->table = &ctx->table_variable;
      ctx->table_variable.thread_switch = NULL;
      break;
    }

    case RtemsUserextReqCreate_Pre_Table_Null: {
      /*
       * While the ``extension_table`` parameter is NULL.
       */
      ctx->table = NULL;
      break;
    }

    case RtemsUserextReqCreate_Pre_Table_NA:
      break;
  }
}

static void RtemsUserextReqCreate_Pre_Free_Prepare(
  RtemsUserextReqCreate_Context *ctx,
  RtemsUserextReqCreate_Pre_Free state
)
{
  switch ( state ) {
    case RtemsUserextReqCreate_Pre_Free_Yes: {
      /*
       * While the system has at least one inactive extension set object
       * available.
       */
      /* Nothing to do */
      break;
    }

    case RtemsUserextReqCreate_Pre_Free_No: {
      /*
       * While the system has no inactive extension set object available.
       */
      ctx->seized_objects = T_seize_objects( Create, NULL );
      break;
    }

    case RtemsUserextReqCreate_Pre_Free_NA:
      break;
  }
}

static void RtemsUserextReqCreate_Post_Status_Check(
  RtemsUserextReqCreate_Context    *ctx,
  RtemsUserextReqCreate_Post_Status state
)
{
  switch ( state ) {
    case RtemsUserextReqCreate_Post_Status_Ok: {
      /*
       * The return status of rtems_extension_create() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsUserextReqCreate_Post_Status_InvName: {
      /*
       * The return status of rtems_extension_create() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsUserextReqCreate_Post_Status_InvAddr: {
      /*
       * The return status of rtems_extension_create() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsUserextReqCreate_Post_Status_TooMany: {
      /*
       * The return status of rtems_extension_create() shall be RTEMS_TOO_MANY.
       */
      T_rsc( ctx->status, RTEMS_TOO_MANY );
      break;
    }

    case RtemsUserextReqCreate_Post_Status_NA:
      break;
  }
}

static void RtemsUserextReqCreate_Post_Name_Check(
  RtemsUserextReqCreate_Context  *ctx,
  RtemsUserextReqCreate_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsUserextReqCreate_Post_Name_Valid: {
      /*
       * The unique object name shall identify the extension set created by the
       * rtems_extension_create() call.
       */
      id = 0;
      sc = rtems_extension_ident( NAME, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->id_value );
      break;
    }

    case RtemsUserextReqCreate_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify an extension set.
       */
      sc = rtems_extension_ident( NAME, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsUserextReqCreate_Post_Name_NA:
      break;
  }
}

static void RtemsUserextReqCreate_Post_IdVar_Check(
  RtemsUserextReqCreate_Context   *ctx,
  RtemsUserextReqCreate_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsUserextReqCreate_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``extension_table``
       * parameter shall be set to the object identifier of the created
       * extension set after the return of the rtems_extension_create() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_ne_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsUserextReqCreate_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``extension_table`` parameter in past calls
       * to rtems_extension_create() shall not be accessed by the
       * rtems_extension_create() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsUserextReqCreate_Post_IdVar_NA:
      break;
  }
}

static void RtemsUserextReqCreate_Setup( RtemsUserextReqCreate_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
  ctx->id_value = INVALID_ID;
}

static void RtemsUserextReqCreate_Setup_Wrap( void *arg )
{
  RtemsUserextReqCreate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsUserextReqCreate_Setup( ctx );
}

static void RtemsUserextReqCreate_Action( RtemsUserextReqCreate_Context *ctx )
{
  ctx->status = rtems_extension_create( ctx->name, ctx->table, ctx->id );
}

static void RtemsUserextReqCreate_Cleanup( RtemsUserextReqCreate_Context *ctx )
{
  if ( ctx->id_value != INVALID_ID ) {
    rtems_status_code sc;

    sc = rtems_extension_delete( ctx->id_value );
    T_rsc_success( sc );

    ctx->id_value = INVALID_ID;
  }

  T_surrender_objects( &ctx->seized_objects, rtems_extension_delete );
}

static const RtemsUserextReqCreate_Entry
RtemsUserextReqCreate_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsUserextReqCreate_Post_Status_InvName,
    RtemsUserextReqCreate_Post_Name_Invalid,
    RtemsUserextReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, RtemsUserextReqCreate_Post_Status_InvAddr,
    RtemsUserextReqCreate_Post_Name_Invalid,
    RtemsUserextReqCreate_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, RtemsUserextReqCreate_Post_Status_Ok,
    RtemsUserextReqCreate_Post_Name_Valid, RtemsUserextReqCreate_Post_IdVar_Set },
  { 0, 0, 0, 0, 0, RtemsUserextReqCreate_Post_Status_TooMany,
    RtemsUserextReqCreate_Post_Name_Invalid,
    RtemsUserextReqCreate_Post_IdVar_Nop }
};

static const uint8_t
RtemsUserextReqCreate_Map[] = {
  2, 3, 2, 3, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static size_t RtemsUserextReqCreate_Scope( void *arg, char *buf, size_t n )
{
  RtemsUserextReqCreate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsUserextReqCreate_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsUserextReqCreate_Fixture = {
  .setup = RtemsUserextReqCreate_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsUserextReqCreate_Scope,
  .initial_context = &RtemsUserextReqCreate_Instance
};

static inline RtemsUserextReqCreate_Entry RtemsUserextReqCreate_PopEntry(
  RtemsUserextReqCreate_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsUserextReqCreate_Entries[
    RtemsUserextReqCreate_Map[ index ]
  ];
}

static void RtemsUserextReqCreate_TestVariant(
  RtemsUserextReqCreate_Context *ctx
)
{
  RtemsUserextReqCreate_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsUserextReqCreate_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsUserextReqCreate_Pre_Table_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsUserextReqCreate_Pre_Free_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsUserextReqCreate_Action( ctx );
  RtemsUserextReqCreate_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsUserextReqCreate_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
  RtemsUserextReqCreate_Post_IdVar_Check( ctx, ctx->Map.entry.Post_IdVar );
}

/**
 * @fn void T_case_body_RtemsUserextReqCreate( void )
 */
T_TEST_CASE_FIXTURE( RtemsUserextReqCreate, &RtemsUserextReqCreate_Fixture )
{
  RtemsUserextReqCreate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsUserextReqCreate_Pre_Name_Valid;
    ctx->Map.pcs[ 0 ] < RtemsUserextReqCreate_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsUserextReqCreate_Pre_Id_Valid;
      ctx->Map.pcs[ 1 ] < RtemsUserextReqCreate_Pre_Id_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsUserextReqCreate_Pre_Table_TdSw;
        ctx->Map.pcs[ 2 ] < RtemsUserextReqCreate_Pre_Table_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsUserextReqCreate_Pre_Free_Yes;
          ctx->Map.pcs[ 3 ] < RtemsUserextReqCreate_Pre_Free_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsUserextReqCreate_PopEntry( ctx );
          RtemsUserextReqCreate_TestVariant( ctx );
          RtemsUserextReqCreate_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
