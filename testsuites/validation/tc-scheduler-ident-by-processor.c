/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerReqIdentByProcessor
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

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSchedulerReqIdentByProcessor \
 *   spec:/rtems/scheduler/req/ident-by-processor
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_Yes,
  RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_No,
  RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_NA
} RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler;

typedef enum {
  RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_Invalid,
  RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_Valid,
  RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_NA
} RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex;

typedef enum {
  RtemsSchedulerReqIdentByProcessor_Pre_Id_Valid,
  RtemsSchedulerReqIdentByProcessor_Pre_Id_Null,
  RtemsSchedulerReqIdentByProcessor_Pre_Id_NA
} RtemsSchedulerReqIdentByProcessor_Pre_Id;

typedef enum {
  RtemsSchedulerReqIdentByProcessor_Post_Status_Ok,
  RtemsSchedulerReqIdentByProcessor_Post_Status_InvAddr,
  RtemsSchedulerReqIdentByProcessor_Post_Status_InvName,
  RtemsSchedulerReqIdentByProcessor_Post_Status_IncStat,
  RtemsSchedulerReqIdentByProcessor_Post_Status_NA
} RtemsSchedulerReqIdentByProcessor_Post_Status;

typedef enum {
  RtemsSchedulerReqIdentByProcessor_Post_IdVar_Set,
  RtemsSchedulerReqIdentByProcessor_Post_IdVar_Nop,
  RtemsSchedulerReqIdentByProcessor_Post_IdVar_NA
} RtemsSchedulerReqIdentByProcessor_Post_IdVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_CPUOwnedByScheduler_NA : 1;
  uint16_t Pre_CPUIndex_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_IdVar : 2;
} RtemsSchedulerReqIdentByProcessor_Entry;

/**
 * @brief Test context for spec:/rtems/scheduler/req/ident-by-processor test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of a second scheduler.
   */
  rtems_id second_scheduler_id;

  /**
   * @brief This member provides the object referenced by the ``id`` parameter.
   */
  rtems_id id_value;

  /**
   * @brief If this member is true, then the processor specified by the
   *   ``cpu_index`` parameter shall be owned by a scheduler.
   */
  bool cpu_has_scheduler;

  /**
   * @brief This member contains the return value of the
   *   rtems_scheduler_ident_by_processor() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``cpu_index`` parameter value.
   */
  uint32_t cpu_index;

  /**
   * @brief This member specifies if the ``id`` parameter value.
   */
  rtems_id *id;

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
    RtemsSchedulerReqIdentByProcessor_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSchedulerReqIdentByProcessor_Context;

static RtemsSchedulerReqIdentByProcessor_Context
  RtemsSchedulerReqIdentByProcessor_Instance;

static const char * const RtemsSchedulerReqIdentByProcessor_PreDesc_CPUOwnedByScheduler[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSchedulerReqIdentByProcessor_PreDesc_CPUIndex[] = {
  "Invalid",
  "Valid",
  "NA"
};

static const char * const RtemsSchedulerReqIdentByProcessor_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsSchedulerReqIdentByProcessor_PreDesc[] = {
  RtemsSchedulerReqIdentByProcessor_PreDesc_CPUOwnedByScheduler,
  RtemsSchedulerReqIdentByProcessor_PreDesc_CPUIndex,
  RtemsSchedulerReqIdentByProcessor_PreDesc_Id,
  NULL
};

static void RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_Prepare(
  RtemsSchedulerReqIdentByProcessor_Context                *ctx,
  RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_Yes: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is owned
       * by a scheduler.
       */
      ctx->cpu_has_scheduler = true;
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_No: {
      /*
       * While the processor specified by the ``cpu_index`` parameter is not
       * owned by a scheduler.
       */
      ctx->cpu_has_scheduler = false;
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_Prepare(
  RtemsSchedulerReqIdentByProcessor_Context     *ctx,
  RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_Invalid: {
      /*
       * While the ``cpu_index`` parameter is greater than or equal to the
       * processor maximum.
       */
      ctx->cpu_index = rtems_scheduler_get_processor_maximum();
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_Valid: {
      /*
       * While the ``cpu_index`` parameter is less than the processor maximum.
       */
      if ( ctx->cpu_has_scheduler ) {
        ctx->cpu_index = 0;
      } else {
        ctx->cpu_index = 1;
      }
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessor_Pre_Id_Prepare(
  RtemsSchedulerReqIdentByProcessor_Context *ctx,
  RtemsSchedulerReqIdentByProcessor_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessor_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id_value = INVALID_ID;
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is equal to NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Pre_Id_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessor_Post_Status_Check(
  RtemsSchedulerReqIdentByProcessor_Context    *ctx,
  RtemsSchedulerReqIdentByProcessor_Post_Status state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessor_Post_Status_Ok: {
      /*
       * The return status of rtems_scheduler_ident_by_processor() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Post_Status_InvAddr: {
      /*
       * The return status of rtems_scheduler_ident_by_processor() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Post_Status_InvName: {
      /*
       * The return status of rtems_scheduler_ident_by_processor() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Post_Status_IncStat: {
      /*
       * The return status of rtems_scheduler_ident_by_processor() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Post_Status_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessor_Post_IdVar_Check(
  RtemsSchedulerReqIdentByProcessor_Context   *ctx,
  RtemsSchedulerReqIdentByProcessor_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessor_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the identifier of the scheduler which owned the processor
       * specified by the ``cpu_index`` parameter at some point during the call
       * after the return of the rtems_scheduler_ident_by_processor() call.
       */
      T_eq_ptr( ctx->id, &ctx->id_value );
      T_eq_u32( ctx->id_value, 0x0f010001 );
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_scheduler_ident_by_processor() shall not be accessed by the
       * rtems_scheduler_ident_by_processor() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsSchedulerReqIdentByProcessor_Post_IdVar_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessor_Setup(
  RtemsSchedulerReqIdentByProcessor_Context *ctx
)
{
  #if defined(RTEMS_SMP)
  rtems_status_code sc;

  sc = rtems_scheduler_ident(
    TEST_SCHEDULER_B_NAME,
    &ctx->second_scheduler_id
  );
  T_rsc_success( sc );
  #else
  ctx->second_scheduler_id = INVALID_ID;
  #endif
}

static void RtemsSchedulerReqIdentByProcessor_Setup_Wrap( void *arg )
{
  RtemsSchedulerReqIdentByProcessor_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSchedulerReqIdentByProcessor_Setup( ctx );
}

static void RtemsSchedulerReqIdentByProcessor_Prepare(
  RtemsSchedulerReqIdentByProcessor_Context *ctx
)
{
  ctx->id_value = INVALID_ID;
}

static void RtemsSchedulerReqIdentByProcessor_Action(
  RtemsSchedulerReqIdentByProcessor_Context *ctx
)
{
  #if defined(RTEMS_SMP)
  rtems_status_code sc;

  if ( !ctx->cpu_has_scheduler ) {
    sc = rtems_scheduler_remove_processor( ctx->second_scheduler_id, 1 );
    T_rsc_success( sc );
  }
  #endif

  ctx->status = rtems_scheduler_ident_by_processor( ctx->cpu_index, ctx->id );

  #if defined(RTEMS_SMP)
  if ( !ctx->cpu_has_scheduler ) {
    sc = rtems_scheduler_add_processor( ctx->second_scheduler_id, 1 );
    T_rsc_success( sc );
  }
  #endif
}

static const RtemsSchedulerReqIdentByProcessor_Entry
RtemsSchedulerReqIdentByProcessor_Entries[] = {
  { 0, 1, 0, 0, RtemsSchedulerReqIdentByProcessor_Post_Status_InvName,
    RtemsSchedulerReqIdentByProcessor_Post_IdVar_Nop },
  { 0, 1, 0, 0, RtemsSchedulerReqIdentByProcessor_Post_Status_InvAddr,
    RtemsSchedulerReqIdentByProcessor_Post_IdVar_Nop },
  { 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessor_Post_Status_Ok,
    RtemsSchedulerReqIdentByProcessor_Post_IdVar_Set },
  { 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessor_Post_Status_InvAddr,
    RtemsSchedulerReqIdentByProcessor_Post_IdVar_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessor_Post_Status_IncStat,
    RtemsSchedulerReqIdentByProcessor_Post_IdVar_Nop },
#else
  { 1, 0, 0, 0, RtemsSchedulerReqIdentByProcessor_Post_Status_NA,
    RtemsSchedulerReqIdentByProcessor_Post_IdVar_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessor_Post_Status_InvAddr,
    RtemsSchedulerReqIdentByProcessor_Post_IdVar_Nop }
#else
  { 1, 0, 0, 0, RtemsSchedulerReqIdentByProcessor_Post_Status_NA,
    RtemsSchedulerReqIdentByProcessor_Post_IdVar_NA }
#endif
};

static const uint8_t
RtemsSchedulerReqIdentByProcessor_Map[] = {
  0, 1, 2, 3, 0, 1, 4, 5
};

static size_t RtemsSchedulerReqIdentByProcessor_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsSchedulerReqIdentByProcessor_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsSchedulerReqIdentByProcessor_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsSchedulerReqIdentByProcessor_Fixture = {
  .setup = RtemsSchedulerReqIdentByProcessor_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsSchedulerReqIdentByProcessor_Scope,
  .initial_context = &RtemsSchedulerReqIdentByProcessor_Instance
};

static inline RtemsSchedulerReqIdentByProcessor_Entry
RtemsSchedulerReqIdentByProcessor_PopEntry(
  RtemsSchedulerReqIdentByProcessor_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSchedulerReqIdentByProcessor_Entries[
    RtemsSchedulerReqIdentByProcessor_Map[ index ]
  ];
}

static void RtemsSchedulerReqIdentByProcessor_SetPreConditionStates(
  RtemsSchedulerReqIdentByProcessor_Context *ctx
)
{
  if ( ctx->Map.entry.Pre_CPUOwnedByScheduler_NA ) {
    ctx->Map.pcs[ 0 ] = RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_NA;
  } else {
    ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  }

  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
}

static void RtemsSchedulerReqIdentByProcessor_TestVariant(
  RtemsSchedulerReqIdentByProcessor_Context *ctx
)
{
  RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  RtemsSchedulerReqIdentByProcessor_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSchedulerReqIdentByProcessor_Action( ctx );
  RtemsSchedulerReqIdentByProcessor_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsSchedulerReqIdentByProcessor_Post_IdVar_Check(
    ctx,
    ctx->Map.entry.Post_IdVar
  );
}

/**
 * @fn void T_case_body_RtemsSchedulerReqIdentByProcessor( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsSchedulerReqIdentByProcessor,
  &RtemsSchedulerReqIdentByProcessor_Fixture
)
{
  RtemsSchedulerReqIdentByProcessor_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_Yes;
    ctx->Map.pci[ 0 ] < RtemsSchedulerReqIdentByProcessor_Pre_CPUOwnedByScheduler_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_Invalid;
      ctx->Map.pci[ 1 ] < RtemsSchedulerReqIdentByProcessor_Pre_CPUIndex_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsSchedulerReqIdentByProcessor_Pre_Id_Valid;
        ctx->Map.pci[ 2 ] < RtemsSchedulerReqIdentByProcessor_Pre_Id_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsSchedulerReqIdentByProcessor_PopEntry( ctx );

        if ( ctx->Map.entry.Skip ) {
          continue;
        }

        RtemsSchedulerReqIdentByProcessor_SetPreConditionStates( ctx );
        RtemsSchedulerReqIdentByProcessor_Prepare( ctx );
        RtemsSchedulerReqIdentByProcessor_TestVariant( ctx );
      }
    }
  }
}

/** @} */
