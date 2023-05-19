/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqIdent
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
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqIdent spec:/rtems/task/req/ident
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqIdent_Pre_Name_WhoAmI,
  RtemsTaskReqIdent_Pre_Name_NotWhoAmI,
  RtemsTaskReqIdent_Pre_Name_NA
} RtemsTaskReqIdent_Pre_Name;

typedef enum {
  RtemsTaskReqIdent_Post_Status_OkAndWhoAmI,
  RtemsTaskReqIdent_Post_Status_Skip,
  RtemsTaskReqIdent_Post_Status_NA
} RtemsTaskReqIdent_Post_Status;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Name_NA : 1;
  uint8_t Post_Status : 2;
} RtemsTaskReqIdent_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/ident test case.
 */
typedef struct {
  rtems_status_code status;

  rtems_id *id;

  rtems_id id_value;

  rtems_id id_local_object;

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
    RtemsTaskReqIdent_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqIdent_Context;

static RtemsTaskReqIdent_Context
  RtemsTaskReqIdent_Instance;

static const char * const RtemsTaskReqIdent_PreDesc_Name[] = {
  "WhoAmI",
  "NotWhoAmI",
  "NA"
};

static const char * const * const RtemsTaskReqIdent_PreDesc[] = {
  RtemsTaskReqIdent_PreDesc_Name,
  NULL
};

static rtems_status_code ClassicTaskIdentAction(
  rtems_name name,
  uint32_t   node,
  rtems_id  *id
)
{
  return rtems_task_ident( name, node, id );
}

static void RtemsTaskReqIdent_Pre_Name_Prepare(
  RtemsTaskReqIdent_Context *ctx,
  RtemsTaskReqIdent_Pre_Name state
)
{
  switch ( state ) {
    case RtemsTaskReqIdent_Pre_Name_WhoAmI: {
      /*
       * While the ``name`` parameter is equal to RTEMS_WHO_AM_I, while ``id``
       * parameter is not equal to NULL.
       */
      ctx->id_value = 0xffffffff;
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsTaskReqIdent_Pre_Name_NotWhoAmI: {
      /*
       * While the ``name`` is not equal to RTEMS_WHO_AM_I or ``id`` parameter
       * is equal to NULL, the behaviour of rtems_task_ident() shall be
       * specified by spec:/rtems/req/ident.
       */
      ctx->id = NULL;
      /* Preparation performed by RtemsReqIdent_Run() */
      break;
    }

    case RtemsTaskReqIdent_Pre_Name_NA:
      break;
  }
}

static void RtemsTaskReqIdent_Post_Status_Check(
  RtemsTaskReqIdent_Context    *ctx,
  RtemsTaskReqIdent_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqIdent_Post_Status_OkAndWhoAmI: {
      /*
       * The return status of rtems_task_ident() shall be RTEMS_SUCCESSFUL.
       * The value of the object identifier referenced by the ``name``
       * parameter shall be the identifier of the executing thread.
       */
      T_rsc( ctx->status, RTEMS_SUCCESSFUL );
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_eq_u32( ctx->id_value, rtems_task_self() );
      break;
    }

    case RtemsTaskReqIdent_Post_Status_Skip: {
      /*
       * There is no status to validate.
       */
      /* Checks performed by RtemsReqIdent_Run() */
      break;
    }

    case RtemsTaskReqIdent_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqIdent_Setup( RtemsTaskReqIdent_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_task_construct(
    &DefaultTaskConfig,
    &ctx->id_local_object
  );
  T_assert_rsc_success( sc );
}

static void RtemsTaskReqIdent_Setup_Wrap( void *arg )
{
  RtemsTaskReqIdent_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqIdent_Setup( ctx );
}

static void RtemsTaskReqIdent_Teardown( RtemsTaskReqIdent_Context *ctx )
{
  if ( ctx->id_local_object != 0 ) {
    rtems_status_code sc;

    sc = rtems_task_delete( ctx->id_local_object );
    T_rsc_success( sc );
  }
}

static void RtemsTaskReqIdent_Teardown_Wrap( void *arg )
{
  RtemsTaskReqIdent_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqIdent_Teardown( ctx );
}

static void RtemsTaskReqIdent_Action( RtemsTaskReqIdent_Context *ctx )
{
  if ( ctx->id != NULL ) {
    ctx->status = rtems_task_ident( RTEMS_SELF, 0xdeadbeef, ctx->id );
  } else {
    RtemsReqIdent_Run(
      ctx->id_local_object,
      DefaultTaskConfig.name,
      ClassicTaskIdentAction
    );
  }
}

static const RtemsTaskReqIdent_Entry
RtemsTaskReqIdent_Entries[] = {
  { 0, 0, RtemsTaskReqIdent_Post_Status_OkAndWhoAmI },
  { 0, 0, RtemsTaskReqIdent_Post_Status_Skip }
};

static const uint8_t
RtemsTaskReqIdent_Map[] = {
  0, 1
};

static size_t RtemsTaskReqIdent_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqIdent_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqIdent_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqIdent_Fixture = {
  .setup = RtemsTaskReqIdent_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqIdent_Teardown_Wrap,
  .scope = RtemsTaskReqIdent_Scope,
  .initial_context = &RtemsTaskReqIdent_Instance
};

static inline RtemsTaskReqIdent_Entry RtemsTaskReqIdent_PopEntry(
  RtemsTaskReqIdent_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqIdent_Entries[
    RtemsTaskReqIdent_Map[ index ]
  ];
}

static void RtemsTaskReqIdent_TestVariant( RtemsTaskReqIdent_Context *ctx )
{
  RtemsTaskReqIdent_Pre_Name_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqIdent_Action( ctx );
  RtemsTaskReqIdent_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_RtemsTaskReqIdent( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqIdent, &RtemsTaskReqIdent_Fixture )
{
  RtemsTaskReqIdent_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTaskReqIdent_Pre_Name_WhoAmI;
    ctx->Map.pcs[ 0 ] < RtemsTaskReqIdent_Pre_Name_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsTaskReqIdent_PopEntry( ctx );
    RtemsTaskReqIdent_TestVariant( ctx );
  }
}

/** @} */
