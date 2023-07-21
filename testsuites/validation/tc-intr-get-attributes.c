/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqGetAttributes
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

#include <string.h>
#include <bsp/irq-generic.h>
#include <rtems/irq-extension.h>

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqGetAttributes spec:/rtems/intr/req/get-attributes
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqGetAttributes_Pre_Vector_Valid,
  RtemsIntrReqGetAttributes_Pre_Vector_Invalid,
  RtemsIntrReqGetAttributes_Pre_Vector_NA
} RtemsIntrReqGetAttributes_Pre_Vector;

typedef enum {
  RtemsIntrReqGetAttributes_Pre_Attributes_Obj,
  RtemsIntrReqGetAttributes_Pre_Attributes_Null,
  RtemsIntrReqGetAttributes_Pre_Attributes_NA
} RtemsIntrReqGetAttributes_Pre_Attributes;

typedef enum {
  RtemsIntrReqGetAttributes_Post_Status_Ok,
  RtemsIntrReqGetAttributes_Post_Status_InvAddr,
  RtemsIntrReqGetAttributes_Post_Status_InvId,
  RtemsIntrReqGetAttributes_Post_Status_NA
} RtemsIntrReqGetAttributes_Post_Status;

typedef enum {
  RtemsIntrReqGetAttributes_Post_Attributes_Nop,
  RtemsIntrReqGetAttributes_Post_Attributes_Zero,
  RtemsIntrReqGetAttributes_Post_Attributes_Set,
  RtemsIntrReqGetAttributes_Post_Attributes_NA
} RtemsIntrReqGetAttributes_Post_Attributes;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Vector_NA : 1;
  uint8_t Pre_Attributes_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Attributes : 2;
} RtemsIntrReqGetAttributes_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/get-attributes test case.
 */
typedef struct {
  /**
   * @brief This member provides the rtems_interrupt_attributes object.
   */
  rtems_interrupt_attributes attributes_obj;

  /**
   * @brief If this member is true, then the ``vector`` parameter shall be
   *   valid.
   */
  bool valid_vector;

  /**
   * @brief This member specifies if the ``attributes`` parameter value.
   */
  rtems_interrupt_attributes *attributes;

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_get_attributes() call.
   */
  rtems_status_code status;

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
    RtemsIntrReqGetAttributes_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqGetAttributes_Context;

static RtemsIntrReqGetAttributes_Context
  RtemsIntrReqGetAttributes_Instance;

static const char * const RtemsIntrReqGetAttributes_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqGetAttributes_PreDesc_Attributes[] = {
  "Obj",
  "Null",
  "NA"
};

static const char * const * const RtemsIntrReqGetAttributes_PreDesc[] = {
  RtemsIntrReqGetAttributes_PreDesc_Vector,
  RtemsIntrReqGetAttributes_PreDesc_Attributes,
  NULL
};

static void RtemsIntrReqGetAttributes_Pre_Vector_Prepare(
  RtemsIntrReqGetAttributes_Context   *ctx,
  RtemsIntrReqGetAttributes_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAttributes_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqGetAttributes_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqGetAttributes_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqGetAttributes_Pre_Attributes_Prepare(
  RtemsIntrReqGetAttributes_Context       *ctx,
  RtemsIntrReqGetAttributes_Pre_Attributes state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAttributes_Pre_Attributes_Obj: {
      /*
       * While the ``attributes`` parameter references an object of type
       * rtems_interrupt_attributes.
       */
      ctx->attributes = &ctx->attributes_obj;
      break;
    }

    case RtemsIntrReqGetAttributes_Pre_Attributes_Null: {
      /*
       * While the ``attributes`` parameter is equal to NULL.
       */
      ctx->attributes = NULL;
      break;
    }

    case RtemsIntrReqGetAttributes_Pre_Attributes_NA:
      break;
  }
}

static void RtemsIntrReqGetAttributes_Post_Status_Check(
  RtemsIntrReqGetAttributes_Context    *ctx,
  RtemsIntrReqGetAttributes_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAttributes_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_get_attributes() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqGetAttributes_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_get_attributes() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqGetAttributes_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_get_attributes() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqGetAttributes_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqGetAttributes_Post_Attributes_Check(
  RtemsIntrReqGetAttributes_Context        *ctx,
  RtemsIntrReqGetAttributes_Post_Attributes state
)
{
  rtems_interrupt_attributes attr;

  switch ( state ) {
    case RtemsIntrReqGetAttributes_Post_Attributes_Nop: {
      /*
       * Objects referenced by the ``attributes`` parameter in past calls to
       * rtems_interrupt_get_attributes() shall not be accessed by the
       * rtems_interrupt_get_attributes() call.
       */
      memset( &attr, 0xa5, sizeof( attr ) );
      T_eq_mem( &ctx->attributes_obj, &attr, sizeof( attr ) );
      break;
    }

    case RtemsIntrReqGetAttributes_Post_Attributes_Zero: {
      /*
       * The object referenced by the ``attributes`` parameter shall cleared to
       * zero.
       */
      memset( &attr, 0, sizeof( attr ) );
      T_eq_mem( &ctx->attributes_obj, &attr, sizeof( attr ) );
      break;
    }

    case RtemsIntrReqGetAttributes_Post_Attributes_Set: {
      /*
       * The members of the object referenced by the ``attributes`` parameter
       * shall be set to the attributes of the interrupt vector specified by
       * ``vector``.
       */
      memset( &attr, 0xa5, sizeof( attr ) );
      T_ne_mem( &ctx->attributes_obj, &attr, sizeof( attr ) );
      break;
    }

    case RtemsIntrReqGetAttributes_Post_Attributes_NA:
      break;
  }
}

static void RtemsIntrReqGetAttributes_Prepare(
  RtemsIntrReqGetAttributes_Context *ctx
)
{
  memset( &ctx->attributes_obj, 0xa5, sizeof( ctx->attributes_obj ) );
}

static void RtemsIntrReqGetAttributes_Action(
  RtemsIntrReqGetAttributes_Context *ctx
)
{
  rtems_vector_number vector;

  if ( ctx->valid_vector && ctx->attributes != NULL ) {
    ctx->status = RTEMS_INTERNAL_ERROR;

    for ( vector = 0; vector < BSP_INTERRUPT_VECTOR_COUNT; ++vector ) {
      rtems_status_code sc;

      memset( &ctx->attributes_obj, 0xff, sizeof( ctx->attributes_obj ) );
      sc = rtems_interrupt_get_attributes( vector, ctx->attributes );

      if ( sc == RTEMS_INVALID_ID ) {
        continue;
      }

      T_rsc_success( sc );
      ctx->status = sc;

      if ( ctx->attributes_obj.can_enable ) {
        T_true( ctx->attributes_obj.maybe_enable );
      }

      if ( ctx->attributes_obj.can_disable ) {
        T_true( ctx->attributes_obj.maybe_disable );
        T_true(
          ctx->attributes_obj.can_enable || ctx->attributes_obj.maybe_enable
        );
      }
    }
  } else {
    if ( ctx->valid_vector ) {
      vector = 0;
    } else {
      vector = BSP_INTERRUPT_VECTOR_COUNT;
    }

    ctx->status = rtems_interrupt_get_attributes( vector, ctx->attributes );
  }
}

static const RtemsIntrReqGetAttributes_Entry
RtemsIntrReqGetAttributes_Entries[] = {
  { 0, 0, 0, RtemsIntrReqGetAttributes_Post_Status_InvAddr,
    RtemsIntrReqGetAttributes_Post_Attributes_Nop },
  { 0, 0, 0, RtemsIntrReqGetAttributes_Post_Status_Ok,
    RtemsIntrReqGetAttributes_Post_Attributes_Set },
  { 0, 0, 0, RtemsIntrReqGetAttributes_Post_Status_InvId,
    RtemsIntrReqGetAttributes_Post_Attributes_Zero }
};

static const uint8_t
RtemsIntrReqGetAttributes_Map[] = {
  1, 0, 2, 0
};

static size_t RtemsIntrReqGetAttributes_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqGetAttributes_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqGetAttributes_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqGetAttributes_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqGetAttributes_Scope,
  .initial_context = &RtemsIntrReqGetAttributes_Instance
};

static inline RtemsIntrReqGetAttributes_Entry
RtemsIntrReqGetAttributes_PopEntry( RtemsIntrReqGetAttributes_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqGetAttributes_Entries[
    RtemsIntrReqGetAttributes_Map[ index ]
  ];
}

static void RtemsIntrReqGetAttributes_TestVariant(
  RtemsIntrReqGetAttributes_Context *ctx
)
{
  RtemsIntrReqGetAttributes_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqGetAttributes_Pre_Attributes_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqGetAttributes_Action( ctx );
  RtemsIntrReqGetAttributes_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsIntrReqGetAttributes_Post_Attributes_Check(
    ctx,
    ctx->Map.entry.Post_Attributes
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqGetAttributes( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqGetAttributes,
  &RtemsIntrReqGetAttributes_Fixture
)
{
  RtemsIntrReqGetAttributes_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsIntrReqGetAttributes_Pre_Vector_Valid;
    ctx->Map.pcs[ 0 ] < RtemsIntrReqGetAttributes_Pre_Vector_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsIntrReqGetAttributes_Pre_Attributes_Obj;
      ctx->Map.pcs[ 1 ] < RtemsIntrReqGetAttributes_Pre_Attributes_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      ctx->Map.entry = RtemsIntrReqGetAttributes_PopEntry( ctx );
      RtemsIntrReqGetAttributes_Prepare( ctx );
      RtemsIntrReqGetAttributes_TestVariant( ctx );
    }
  }
}

/** @} */
