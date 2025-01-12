/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqSetPriority
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

#include "tr-intr-set-priority.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqSetPriority spec:/rtems/intr/req/set-priority
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Vector_NA : 1;
  uint8_t Pre_Priority_NA : 1;
  uint8_t Pre_CanSetPriority_NA : 1;
  uint8_t Post_Status : 3;
} RtemsIntrReqSetPriority_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/set-priority test case.
 */
typedef struct {
  /**
   * @brief This member contains the current priority value.
   */
  uint32_t current_priority;

  /**
   * @brief This member specifies the ``vector`` parameter value.
   */
  rtems_vector_number vector;

  /**
   * @brief This member specifies the ``priority`` parameter value.
   */
  uint32_t priority;

  /**
   * @brief This member contains the return status.
   */
  rtems_status_code status;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsIntrReqSetPriority_Run() parameter.
   */
  rtems_vector_number valid_vector;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsIntrReqSetPriority_Run() parameter.
   */
  uint32_t maximum_priority;

  /**
   * @brief This member contains a copy of the corresponding
   *   RtemsIntrReqSetPriority_Run() parameter.
   */
  bool can_set_priority;

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
    RtemsIntrReqSetPriority_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqSetPriority_Context;

static RtemsIntrReqSetPriority_Context
  RtemsIntrReqSetPriority_Instance;

static const char * const RtemsIntrReqSetPriority_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqSetPriority_PreDesc_Priority[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqSetPriority_PreDesc_CanSetPriority[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqSetPriority_PreDesc[] = {
  RtemsIntrReqSetPriority_PreDesc_Vector,
  RtemsIntrReqSetPriority_PreDesc_Priority,
  RtemsIntrReqSetPriority_PreDesc_CanSetPriority,
  NULL
};

static void RtemsIntrReqSetPriority_Pre_Vector_Prepare(
  RtemsIntrReqSetPriority_Context   *ctx,
  RtemsIntrReqSetPriority_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqSetPriority_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->vector = ctx->valid_vector;
      break;
    }

    case RtemsIntrReqSetPriority_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
      break;
    }

    case RtemsIntrReqSetPriority_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqSetPriority_Pre_Priority_Prepare(
  RtemsIntrReqSetPriority_Context     *ctx,
  RtemsIntrReqSetPriority_Pre_Priority state
)
{
  switch ( state ) {
    case RtemsIntrReqSetPriority_Pre_Priority_Valid: {
      /*
       * While the ``priority`` parameter is a valid priority value.
       */
      ctx->priority = ctx->current_priority;
      break;
    }

    case RtemsIntrReqSetPriority_Pre_Priority_Invalid: {
      /*
       * While the ``priority`` parameter is an invalid priority value.
       */
      if ( ctx->maximum_priority < UINT32_MAX ) {
        ctx->priority = UINT32_MAX;
      } else {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqSetPriority_Pre_Priority_NA:
      break;
  }
}

static void RtemsIntrReqSetPriority_Pre_CanSetPriority_Prepare(
  RtemsIntrReqSetPriority_Context           *ctx,
  RtemsIntrReqSetPriority_Pre_CanSetPriority state
)
{
  switch ( state ) {
    case RtemsIntrReqSetPriority_Pre_CanSetPriority_Yes: {
      /*
       * While setting the priority for the interrupt vector specified by
       * ``vector`` parameter is supported.
       */
      if ( !ctx->can_set_priority ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqSetPriority_Pre_CanSetPriority_No: {
      /*
       * While setting the priority for the interrupt vector specified by
       * ``vector`` parameter is not supported.
       */
      if ( ctx->can_set_priority ) {
        ctx->Map.skip = true;
      }
      break;
    }

    case RtemsIntrReqSetPriority_Pre_CanSetPriority_NA:
      break;
  }
}

static void RtemsIntrReqSetPriority_Post_Status_Check(
  RtemsIntrReqSetPriority_Context    *ctx,
  RtemsIntrReqSetPriority_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqSetPriority_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_set_priority() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqSetPriority_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_set_priority() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqSetPriority_Post_Status_InvPrio: {
      /*
       * The return status of rtems_interrupt_set_priority() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      T_rsc( ctx->status, RTEMS_INVALID_PRIORITY );
      break;
    }

    case RtemsIntrReqSetPriority_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_set_priority() shall be
       * RTEMS_UNSATISFIED.
       */
      T_rsc( ctx->status, RTEMS_UNSATISFIED );
      break;
    }

    case RtemsIntrReqSetPriority_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqSetPriority_Prepare(
  RtemsIntrReqSetPriority_Context *ctx
)
{
  ctx->current_priority = 0;
  (void) rtems_interrupt_get_priority(
    ctx->valid_vector,
    &ctx->current_priority
  );
}

static void RtemsIntrReqSetPriority_Action(
  RtemsIntrReqSetPriority_Context *ctx
)
{
  ctx->status = rtems_interrupt_set_priority( ctx->vector, ctx->priority );
}

static void RtemsIntrReqSetPriority_Cleanup(
  RtemsIntrReqSetPriority_Context *ctx
)
{
  (void) rtems_interrupt_set_priority( ctx->valid_vector, ctx->current_priority );
}

static const RtemsIntrReqSetPriority_Entry
RtemsIntrReqSetPriority_Entries[] = {
  { 0, 0, 1, 1, RtemsIntrReqSetPriority_Post_Status_InvId },
  { 0, 0, 1, 0, RtemsIntrReqSetPriority_Post_Status_Unsat },
  { 0, 0, 0, 0, RtemsIntrReqSetPriority_Post_Status_Ok },
  { 0, 0, 0, 0, RtemsIntrReqSetPriority_Post_Status_InvPrio }
};

static const uint8_t
RtemsIntrReqSetPriority_Map[] = {
  2, 1, 3, 1, 0, 0, 0, 0
};

static size_t RtemsIntrReqSetPriority_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqSetPriority_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqSetPriority_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqSetPriority_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqSetPriority_Scope,
  .initial_context = &RtemsIntrReqSetPriority_Instance
};

static const uint8_t RtemsIntrReqSetPriority_Weights[] = {
  4, 2, 1
};

static void RtemsIntrReqSetPriority_Skip(
  RtemsIntrReqSetPriority_Context *ctx,
  size_t                           index
)
{
  switch ( index + 1 ) {
    case 1:
      ctx->Map.pci[ 1 ] = RtemsIntrReqSetPriority_Pre_Priority_NA - 1;
      /* Fall through */
    case 2:
      ctx->Map.pci[ 2 ] = RtemsIntrReqSetPriority_Pre_CanSetPriority_NA - 1;
      break;
  }
}

static inline RtemsIntrReqSetPriority_Entry RtemsIntrReqSetPriority_PopEntry(
  RtemsIntrReqSetPriority_Context *ctx
)
{
  size_t index;

  if ( ctx->Map.skip ) {
    size_t i;

    ctx->Map.skip = false;
    index = 0;

    for ( i = 0; i < 3; ++i ) {
      index += RtemsIntrReqSetPriority_Weights[ i ] * ctx->Map.pci[ i ];
    }
  } else {
    index = ctx->Map.index;
  }

  ctx->Map.index = index + 1;

  return RtemsIntrReqSetPriority_Entries[
    RtemsIntrReqSetPriority_Map[ index ]
  ];
}

static void RtemsIntrReqSetPriority_SetPreConditionStates(
  RtemsIntrReqSetPriority_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_Priority_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsIntrReqSetPriority_Pre_Priority_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_CanSetPriority_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsIntrReqSetPriority_Pre_CanSetPriority_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void RtemsIntrReqSetPriority_TestVariant(
  RtemsIntrReqSetPriority_Context *ctx
)
{
  RtemsIntrReqSetPriority_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqSetPriority_Pre_Priority_Prepare( ctx, ctx->Map.pcs[ 1 ] );

  if ( ctx->Map.skip ) {
    RtemsIntrReqSetPriority_Skip( ctx, 1 );
    return;
  }

  RtemsIntrReqSetPriority_Pre_CanSetPriority_Prepare( ctx, ctx->Map.pcs[ 2 ] );

  if ( ctx->Map.skip ) {
    RtemsIntrReqSetPriority_Skip( ctx, 2 );
    return;
  }

  RtemsIntrReqSetPriority_Action( ctx );
  RtemsIntrReqSetPriority_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
}

static T_fixture_node RtemsIntrReqSetPriority_Node;

static T_remark RtemsIntrReqSetPriority_Remark = {
  .next = NULL,
  .remark = "RtemsIntrReqSetPriority"
};

void RtemsIntrReqSetPriority_Run(
  rtems_vector_number valid_vector,
  uint32_t            maximum_priority,
  bool                can_set_priority
)
{
  RtemsIntrReqSetPriority_Context *ctx;

  ctx = &RtemsIntrReqSetPriority_Instance;
  ctx->valid_vector = valid_vector;
  ctx->maximum_priority = maximum_priority;
  ctx->can_set_priority = can_set_priority;

  ctx = T_push_fixture(
    &RtemsIntrReqSetPriority_Node,
    &RtemsIntrReqSetPriority_Fixture
  );
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;
  ctx->Map.skip = false;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqSetPriority_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqSetPriority_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqSetPriority_Pre_Priority_Valid;
      ctx->Map.pci[ 1 ] < RtemsIntrReqSetPriority_Pre_Priority_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqSetPriority_Pre_CanSetPriority_Yes;
        ctx->Map.pci[ 2 ] < RtemsIntrReqSetPriority_Pre_CanSetPriority_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqSetPriority_PopEntry( ctx );
        RtemsIntrReqSetPriority_SetPreConditionStates( ctx );
        RtemsIntrReqSetPriority_Prepare( ctx );
        RtemsIntrReqSetPriority_TestVariant( ctx );
        RtemsIntrReqSetPriority_Cleanup( ctx );
      }
    }
  }

  T_add_remark( &RtemsIntrReqSetPriority_Remark );
  T_pop_fixture();
}

/** @} */
