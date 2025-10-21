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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqSetPriority spec:/rtems/intr/req/set-priority
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqSetPriority_Pre_Vector_Valid,
  RtemsIntrReqSetPriority_Pre_Vector_Invalid,
  RtemsIntrReqSetPriority_Pre_Vector_NA
} RtemsIntrReqSetPriority_Pre_Vector;

typedef enum {
  RtemsIntrReqSetPriority_Pre_Priority_Valid,
  RtemsIntrReqSetPriority_Pre_Priority_Invalid,
  RtemsIntrReqSetPriority_Pre_Priority_NA
} RtemsIntrReqSetPriority_Pre_Priority;

typedef enum {
  RtemsIntrReqSetPriority_Pre_CanSetPriority_Yes,
  RtemsIntrReqSetPriority_Pre_CanSetPriority_No,
  RtemsIntrReqSetPriority_Pre_CanSetPriority_NA
} RtemsIntrReqSetPriority_Pre_CanSetPriority;

typedef enum {
  RtemsIntrReqSetPriority_Post_Status_Ok,
  RtemsIntrReqSetPriority_Post_Status_InvId,
  RtemsIntrReqSetPriority_Post_Status_InvPrio,
  RtemsIntrReqSetPriority_Post_Status_Unsat,
  RtemsIntrReqSetPriority_Post_Status_NA
} RtemsIntrReqSetPriority_Post_Status;

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
   * @brief If this member is true, then the ``vector`` parameter shall be
   *   valid.
   */
  bool valid_vector;

  /**
   * @brief If this member is true, then the ``priority`` parameter shall be
   *   valid.
   */
  bool valid_priority;

  /**
   * @brief If this member is true, then setting the priority shall be
   *   supported.
   */
  bool can_set_priority;

  /**
   * @brief This member specifies the expected status.
   */
  rtems_status_code expected_status;

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

typedef RtemsIntrReqSetPriority_Context Context;

static void CheckSetPriority( Context *ctx, rtems_vector_number vector )
{
  rtems_status_code sc;
  uint32_t          priority;

  if ( ctx->valid_priority ) {
    (void) rtems_interrupt_get_priority( vector, &priority );
  } else {
    priority = UINT32_MAX;
  }

  sc = rtems_interrupt_set_priority( vector, priority );
  T_rsc( sc, ctx->expected_status );
}

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
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqSetPriority_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
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
      ctx->valid_priority = true;
      break;
    }

    case RtemsIntrReqSetPriority_Pre_Priority_Invalid: {
      /*
       * While the ``priority`` parameter is an invalid priority value.
       */
      ctx->valid_priority = false;
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
      ctx->can_set_priority = true;
      break;
    }

    case RtemsIntrReqSetPriority_Pre_CanSetPriority_No: {
      /*
       * While setting the priority for the interrupt vector specified by
       * ``vector`` parameter is not supported.
       */
      ctx->can_set_priority = false;
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
      ctx->expected_status = RTEMS_SUCCESSFUL;
      break;
    }

    case RtemsIntrReqSetPriority_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_set_priority() shall be
       * RTEMS_INVALID_ID.
       */
      ctx->expected_status = RTEMS_INVALID_ID;
      break;
    }

    case RtemsIntrReqSetPriority_Post_Status_InvPrio: {
      /*
       * The return status of rtems_interrupt_set_priority() shall be
       * RTEMS_INVALID_PRIORITY.
       */
      ctx->expected_status = RTEMS_INVALID_PRIORITY;
      break;
    }

    case RtemsIntrReqSetPriority_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_set_priority() shall be
       * RTEMS_UNSATISFIED.
       */
      ctx->expected_status = RTEMS_UNSATISFIED;
      break;
    }

    case RtemsIntrReqSetPriority_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqSetPriority_Action( void )
{
  /* Action carried out by CheckSetPriority() */
}

static void RtemsIntrReqSetPriority_Cleanup(
  RtemsIntrReqSetPriority_Context *ctx
)
{
  if ( ctx->valid_vector ) {
    rtems_vector_number vector;

    for (
      vector = 0;
      vector < BSP_INTERRUPT_VECTOR_COUNT;
      ++vector
    ) {
      rtems_interrupt_attributes attr;
      rtems_status_code          sc;

      memset( &attr, 0, sizeof( attr ) );
      sc = rtems_interrupt_get_attributes( vector, &attr );

      if ( sc == RTEMS_INVALID_ID ) {
        continue;
      }

      T_rsc_success( sc );

      if ( attr.can_set_priority != ctx->can_set_priority ) {
        continue;
      }

      CheckSetPriority( ctx, vector );
    }
  } else {
    CheckSetPriority( ctx, BSP_INTERRUPT_VECTOR_COUNT );
  }
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

static inline RtemsIntrReqSetPriority_Entry RtemsIntrReqSetPriority_PopEntry(
  RtemsIntrReqSetPriority_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
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
  RtemsIntrReqSetPriority_Pre_CanSetPriority_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqSetPriority_Action();
  RtemsIntrReqSetPriority_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
}

/**
 * @fn void T_case_body_RtemsIntrReqSetPriority( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqSetPriority,
  &RtemsIntrReqSetPriority_Fixture
)
{
  RtemsIntrReqSetPriority_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

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
        RtemsIntrReqSetPriority_TestVariant( ctx );
        RtemsIntrReqSetPriority_Cleanup( ctx );
      }
    }
  }
}

/** @} */
