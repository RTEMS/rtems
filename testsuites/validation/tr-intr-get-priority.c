/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqGetPriority
 */

/*
 * Copyright (C) 2024 embedded brains GmbH & Co. KG
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
#include <bsp/irq-generic.h>

#include "tr-intr-get-priority.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqGetPriority spec:/rtems/intr/req/get-priority
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Vector_NA : 1;
  uint16_t Pre_Priority_NA : 1;
  uint16_t Pre_CanGetPriority_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_PriorityObj : 2;
} RtemsIntrReqGetPriority_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/get-priority test case.
 */
typedef struct {
  /**
   * @brief This member specifies the ``vector`` parameter value.
   */
  rtems_vector_number vector;

  /**
   * @brief This member provides the object referenced by the ``priority``
   *   parameter.
   */
  uint32_t priority_obj;

  /**
   * @brief This member specifies the ``priority`` parameter value.
   */
  uint32_t *priority;

  /**
   * @brief This member contains the return status.
   */
  rtems_status_code status;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsIntrReqGetPriority_Run() parameter.
   */
  rtems_vector_number valid_vector;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsIntrReqGetPriority_Run() parameter.
   */
  bool can_get_priority;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 3 ];

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
    RtemsIntrReqGetPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqGetPriority_Context;

static RtemsIntrReqGetPriority_Context
  RtemsIntrReqGetPriority_Instance;

static const char * const RtemsIntrReqGetPriority_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqGetPriority_PreDesc_Priority[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqGetPriority_PreDesc_CanGetPriority[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqGetPriority_PreDesc[] = {
  RtemsIntrReqGetPriority_PreDesc_Vector,
  RtemsIntrReqGetPriority_PreDesc_Priority,
  RtemsIntrReqGetPriority_PreDesc_CanGetPriority,
  NULL
};

#define PRIORITY_UNSET (UINT32_MAX - 1234)

static void RtemsIntrReqGetPriority_Pre_Vector_Prepare(
  RtemsIntrReqGetPriority_Context   *ctx,
  RtemsIntrReqGetPriority_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqGetPriority_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->vector = ctx->valid_vector;
      break;
    }

    case RtemsIntrReqGetPriority_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
      break;
    }

    case RtemsIntrReqGetPriority_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqGetPriority_Pre_Priority_Prepare(
  RtemsIntrReqGetPriority_Context     *ctx,
  RtemsIntrReqGetPriority_Pre_Priority state
)
{
  switch ( state ) {
    case RtemsIntrReqGetPriority_Pre_Priority_Valid: {
      /*
       * While the ``priority`` parameter references an object of type
       * uint32_t.
       */
      ctx->priority = &ctx->priority_obj;
      break;
    }

    case RtemsIntrReqGetPriority_Pre_Priority_Null: {
      /*
       * While the ``priority`` parameter is equal to NULL.
       */
      ctx->priority = NULL;
      break;
    }

    case RtemsIntrReqGetPriority_Pre_Priority_NA:
      break;
  }
}

static void RtemsIntrReqGetPriority_Pre_CanGetPriority_Prepare(
  RtemsIntrReqGetPriority_Context           *ctx,
  RtemsIntrReqGetPriority_Pre_CanGetPriority state
)
{
  switch ( state ) {
    case RtemsIntrReqGetPriority_Pre_CanGetPriority_Yes: {
      /*
       * While getting the priority for the interrupt vector specified by
       * ``vector`` parameter is supported.
       */
      if ( !ctx->can_get_priority ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqGetPriority_Pre_CanGetPriority_No: {
      /*
       * While getting the priority for the interrupt vector specified by
       * ``vector`` parameter is not supported.
       */
      if ( ctx->can_get_priority ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqGetPriority_Pre_CanGetPriority_NA:
      break;
  }
}

static void RtemsIntrReqGetPriority_Post_Status_Check(
  RtemsIntrReqGetPriority_Context    *ctx,
  RtemsIntrReqGetPriority_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqGetPriority_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_get_priority() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqGetPriority_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_get_priority() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqGetPriority_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_get_priority() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqGetPriority_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_get_priority() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED );
      break;
    }

    case RtemsIntrReqGetPriority_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqGetPriority_Post_PriorityObj_Check(
  RtemsIntrReqGetPriority_Context         *ctx,
  RtemsIntrReqGetPriority_Post_PriorityObj state
)
{
  switch ( state ) {
    case RtemsIntrReqGetPriority_Post_PriorityObj_Set: {
      /*
       * The value of the object referenced by the ``priority`` parameter shall
       * be set by the directive call to a priority which the interrupt
       * specified by the ``vector`` parameter had at some time point during
       * the directive call.
       */
      T_ne_u32( ctx->priority_obj, PRIORITY_UNSET );
      break;
    }

    case RtemsIntrReqGetPriority_Post_PriorityObj_Nop: {
      /*
       * The value of the object referenced by the ``priority`` parameter shall
       * not be changed by the directive call.
       */
      T_eq_u32( ctx->priority_obj, PRIORITY_UNSET );
      break;
    }

    case RtemsIntrReqGetPriority_Post_PriorityObj_NA:
      break;
  }
}

static void RtemsIntrReqGetPriority_Action(
  RtemsIntrReqGetPriority_Context *ctx
)
{
  ctx->priority_obj = PRIORITY_UNSET;
  ctx->status = rtems_interrupt_get_priority( ctx->vector, ctx->priority );
}

static const RtemsIntrReqGetPriority_Entry
RtemsIntrReqGetPriority_Entries[] = {
  { 0, 0, 0, 0, RtemsIntrReqGetPriority_Post_Status_InvAddr,
    RtemsIntrReqGetPriority_Post_PriorityObj_NA },
  { 0, 0, 0, 1, RtemsIntrReqGetPriority_Post_Status_InvId,
    RtemsIntrReqGetPriority_Post_PriorityObj_Nop },
  { 0, 0, 0, 1, RtemsIntrReqGetPriority_Post_Status_InvAddr,
    RtemsIntrReqGetPriority_Post_PriorityObj_NA },
  { 0, 0, 0, 0, RtemsIntrReqGetPriority_Post_Status_Ok,
    RtemsIntrReqGetPriority_Post_PriorityObj_Set },
  { 0, 0, 0, 0, RtemsIntrReqGetPriority_Post_Status_Unsat,
    RtemsIntrReqGetPriority_Post_PriorityObj_Nop }
};

static const uint8_t
RtemsIntrReqGetPriority_Map[] = {
  3, 4, 0, 0, 1, 1, 2, 2
};

static size_t RtemsIntrReqGetPriority_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqGetPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqGetPriority_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqGetPriority_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqGetPriority_Scope,
  .initial_context = &RtemsIntrReqGetPriority_Instance
};

static const uint8_t RtemsIntrReqGetPriority_Weights[] = {
  4, 2, 1
};

static void RtemsIntrReqGetPriority_Skip(
  RtemsIntrReqGetPriority_Context *ctx,
  size_t                           index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = RtemsIntrReqGetPriority_Pre_Priority_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = RtemsIntrReqGetPriority_Pre_CanGetPriority_NA - 1;
      break;
  }
}

static inline RtemsIntrReqGetPriority_Entry RtemsIntrReqGetPriority_PopEntry(
  RtemsIntrReqGetPriority_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += RtemsIntrReqGetPriority_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return RtemsIntrReqGetPriority_Entries[
    RtemsIntrReqGetPriority_Map[ index ]
  ];
}

static void RtemsIntrReqGetPriority_SetPreConditionStates(
  RtemsIntrReqGetPriority_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_CanGetPriority_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsIntrReqGetPriority_Pre_CanGetPriority_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void RtemsIntrReqGetPriority_TestVariant(
  RtemsIntrReqGetPriority_Context *ctx
)
{
  RtemsIntrReqGetPriority_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqGetPriority_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqGetPriority_Pre_CanGetPriority_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    RtemsIntrReqGetPriority_Skip( ctx, 2 );
    return;
  }

  RtemsIntrReqGetPriority_Action( ctx );
  RtemsIntrReqGetPriority_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsIntrReqGetPriority_Post_PriorityObj_Check(
    ctx,
    ctx->Map.entry.Post_PriorityObj
  );
}

static T_fixture_node RtemsIntrReqGetPriority_Node;

static T_remark RtemsIntrReqGetPriority_Remark = {
  .next = NULL,
  .remark = "RtemsIntrReqGetPriority"
};

void RtemsIntrReqGetPriority_Run(
  rtems_vector_number valid_vector,
  bool                can_get_priority
)
{
  RtemsIntrReqGetPriority_Context *ctx;

  ctx = &RtemsIntrReqGetPriority_Instance;
  ctx->valid_vector = valid_vector;
  ctx->can_get_priority = can_get_priority;

  ctx = T_push_fixture(
    &RtemsIntrReqGetPriority_Node,
    &RtemsIntrReqGetPriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqGetPriority_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqGetPriority_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqGetPriority_Pre_Priority_Valid;
      ctx->Map.pci[ 1 ] < RtemsIntrReqGetPriority_Pre_Priority_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqGetPriority_Pre_CanGetPriority_Yes;
        ctx->Map.pci[ 2 ] < RtemsIntrReqGetPriority_Pre_CanGetPriority_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqGetPriority_PopEntry( ctx );
        RtemsIntrReqGetPriority_SetPreConditionStates( ctx );
        RtemsIntrReqGetPriority_TestVariant( ctx );
      }
    }
  }

  T_add_remark( &RtemsIntrReqGetPriority_Remark );
  T_pop_fixture();
}

/** @} */
