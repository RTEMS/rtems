/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsUserextReqDelete
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

#include <rtems/test.h>

/**
 * @defgroup RtemsUserextReqDelete spec:/rtems/userext/req/delete
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsUserextReqDelete_Pre_Id_NoObj,
  RtemsUserextReqDelete_Pre_Id_ExtTdSw,
  RtemsUserextReqDelete_Pre_Id_ExtNoTdSw,
  RtemsUserextReqDelete_Pre_Id_NA
} RtemsUserextReqDelete_Pre_Id;

typedef enum {
  RtemsUserextReqDelete_Post_Status_Ok,
  RtemsUserextReqDelete_Post_Status_InvId,
  RtemsUserextReqDelete_Post_Status_NA
} RtemsUserextReqDelete_Post_Status;

typedef enum {
  RtemsUserextReqDelete_Post_Name_Valid,
  RtemsUserextReqDelete_Post_Name_Invalid,
  RtemsUserextReqDelete_Post_Name_NA
} RtemsUserextReqDelete_Post_Name;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Id_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Name : 2;
} RtemsUserextReqDelete_Entry;

/**
 * @brief Test context for spec:/rtems/userext/req/delete test case.
 */
typedef struct {
  rtems_id extension_id;

  rtems_extensions_table table;

  rtems_id id;

  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    RtemsUserextReqDelete_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsUserextReqDelete_Context;

static RtemsUserextReqDelete_Context
  RtemsUserextReqDelete_Instance;

static const char * const RtemsUserextReqDelete_PreDesc_Id[] = {
  "NoObj",
  "ExtTdSw",
  "ExtNoTdSw",
  "NA"
};

static const char * const * const RtemsUserextReqDelete_PreDesc[] = {
  RtemsUserextReqDelete_PreDesc_Id,
  NULL
};

#define NAME rtems_build_name( 'T', 'E', 'S', 'T' )

static void ThreadSwitch( rtems_tcb *executing,  rtems_tcb *heir)
{
  (void) executing;
  (void) heir;
}

static void RtemsUserextReqDelete_Pre_Id_Prepare(
  RtemsUserextReqDelete_Context *ctx,
  RtemsUserextReqDelete_Pre_Id   state
)
{
  rtems_status_code sc;
  bool              valid_id;

  valid_id = false;

  switch ( state ) {
    case RtemsUserextReqDelete_Pre_Id_NoObj: {
      /*
       * While the ``id`` parameter is not associated with an extension set.
       */
      /* Already set by prologue */
      break;
    }

    case RtemsUserextReqDelete_Pre_Id_ExtTdSw: {
      /*
       * While the ``id`` parameter is associated with an extension set with a
       * thread switch extension.
       */
      valid_id = true;
      ctx->table.thread_switch = ThreadSwitch;
      break;
    }

    case RtemsUserextReqDelete_Pre_Id_ExtNoTdSw: {
      /*
       * While the ``id`` parameter is associated with an extension set without
       * a thread switch extension.
       */
      valid_id = true;
      ctx->table.thread_switch = NULL;
      break;
    }

    case RtemsUserextReqDelete_Pre_Id_NA:
      break;
  }

  sc = rtems_extension_create(
    NAME,
    &ctx->table,
    &ctx->extension_id
  );
  T_rsc_success( sc );

  if ( valid_id ) {
    ctx->id = ctx->extension_id;
  } else {
    ctx->id = 0;
  }
}

static void RtemsUserextReqDelete_Post_Status_Check(
  RtemsUserextReqDelete_Context    *ctx,
  RtemsUserextReqDelete_Post_Status state
)
{
  switch ( state ) {
    case RtemsUserextReqDelete_Post_Status_Ok: {
      /*
       * The return status of rtems_extension_delete() shall be
       * RTEMS_SUCCESSFUL.
       */
      ctx->extension_id = 0;
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsUserextReqDelete_Post_Status_InvId: {
      /*
       * The return status of rtems_extension_delete() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsUserextReqDelete_Post_Status_NA:
      break;
  }
}

static void RtemsUserextReqDelete_Post_Name_Check(
  RtemsUserextReqDelete_Context  *ctx,
  RtemsUserextReqDelete_Post_Name state
)
{
  rtems_status_code sc;
  rtems_id          id;

  switch ( state ) {
    case RtemsUserextReqDelete_Post_Name_Valid: {
      /*
       * The unique object name shall identify an extension set.
       */
      id = 0;
      sc = rtems_extension_ident( NAME, &id );
      T_rsc_success( sc );
      T_eq_u32( id, ctx->extension_id );
      break;
    }

    case RtemsUserextReqDelete_Post_Name_Invalid: {
      /*
       * The unique object name shall not identify an extension set.
       */
      sc = rtems_extension_ident( NAME, &id );
      T_rsc( sc, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsUserextReqDelete_Post_Name_NA:
      break;
  }
}

static void RtemsUserextReqDelete_Setup( RtemsUserextReqDelete_Context *ctx )
{
  memset( ctx, 0, sizeof( *ctx ) );
}

static void RtemsUserextReqDelete_Setup_Wrap( void *arg )
{
  RtemsUserextReqDelete_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsUserextReqDelete_Setup( ctx );
}

static void RtemsUserextReqDelete_Action( RtemsUserextReqDelete_Context *ctx )
{
  ctx->status = rtems_extension_delete( ctx->id );
}

static void RtemsUserextReqDelete_Cleanup( RtemsUserextReqDelete_Context *ctx )
{
  if ( ctx->extension_id != 0 ) {
    rtems_status_code sc;

    sc = rtems_extension_delete( ctx->extension_id );
    T_rsc_success( sc );
  }
}

static const RtemsUserextReqDelete_Entry
RtemsUserextReqDelete_Entries[] = {
  { 0, 0, RtemsUserextReqDelete_Post_Status_Ok,
    RtemsUserextReqDelete_Post_Name_Invalid },
  { 0, 0, RtemsUserextReqDelete_Post_Status_InvId,
    RtemsUserextReqDelete_Post_Name_Valid }
};

static const uint8_t
RtemsUserextReqDelete_Map[] = {
  1, 0, 0
};

static size_t RtemsUserextReqDelete_Scope( void *arg, char *buf, size_t n )
{
  RtemsUserextReqDelete_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsUserextReqDelete_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsUserextReqDelete_Fixture = {
  .setup = RtemsUserextReqDelete_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsUserextReqDelete_Scope,
  .initial_context = &RtemsUserextReqDelete_Instance
};

static inline RtemsUserextReqDelete_Entry RtemsUserextReqDelete_PopEntry(
  RtemsUserextReqDelete_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsUserextReqDelete_Entries[
    RtemsUserextReqDelete_Map[ index ]
  ];
}

static void RtemsUserextReqDelete_TestVariant(
  RtemsUserextReqDelete_Context *ctx
)
{
  RtemsUserextReqDelete_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsUserextReqDelete_Action( ctx );
  RtemsUserextReqDelete_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsUserextReqDelete_Post_Name_Check( ctx, ctx->Map.entry.Post_Name );
}

/**
 * @fn void T_case_body_RtemsUserextReqDelete( void )
 */
T_TEST_CASE_FIXTURE( RtemsUserextReqDelete, &RtemsUserextReqDelete_Fixture )
{
  RtemsUserextReqDelete_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsUserextReqDelete_Pre_Id_NoObj;
    ctx->Map.pcs[ 0 ] < RtemsUserextReqDelete_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsUserextReqDelete_PopEntry( ctx );
    RtemsUserextReqDelete_TestVariant( ctx );
    RtemsUserextReqDelete_Cleanup( ctx );
  }
}

/** @} */
