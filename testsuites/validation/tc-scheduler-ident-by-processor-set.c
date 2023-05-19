/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerReqIdentByProcessorSet
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
 * @defgroup RtemsSchedulerReqIdentByProcessorSet \
 *   spec:/rtems/scheduler/req/ident-by-processor-set
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_Yes,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_No,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_NA
} RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler;

typedef enum {
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_Invalid,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_Valid,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_NA
} RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj;

typedef enum {
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_Valid,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_Invalid,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_NA
} RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize;

typedef enum {
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_Valid,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_Null,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_NA
} RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet;

typedef enum {
  RtemsSchedulerReqIdentByProcessorSet_Pre_Id_Valid,
  RtemsSchedulerReqIdentByProcessorSet_Pre_Id_Null,
  RtemsSchedulerReqIdentByProcessorSet_Pre_Id_NA
} RtemsSchedulerReqIdentByProcessorSet_Pre_Id;

typedef enum {
  RtemsSchedulerReqIdentByProcessorSet_Post_Status_Ok,
  RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvAddr,
  RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvSize,
  RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvName,
  RtemsSchedulerReqIdentByProcessorSet_Post_Status_IncStat,
  RtemsSchedulerReqIdentByProcessorSet_Post_Status_NA
} RtemsSchedulerReqIdentByProcessorSet_Post_Status;

typedef enum {
  RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Set,
  RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop,
  RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_NA
} RtemsSchedulerReqIdentByProcessorSet_Post_IdVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_CPUOwnedByScheduler_NA : 1;
  uint16_t Pre_CPUSetObj_NA : 1;
  uint16_t Pre_CPUSetSize_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_IdVar : 2;
} RtemsSchedulerReqIdentByProcessorSet_Entry;

/**
 * @brief Test context for spec:/rtems/scheduler/req/ident-by-processor-set
 *   test case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of a second scheduler.
   */
  rtems_id second_scheduler_id;

  /**
   * @brief This member provides the object referenced by the ``cpuset``
   *   parameter.
   */
  cpu_set_t cpuset_value;

  /**
   * @brief This member provides the object referenced by the ``id`` parameter.
   */
  rtems_id id_value;

  /**
   * @brief If this member is true, then the processor specified by the
   *   ``cpusetsize`` parameter shall be owned by a scheduler.
   */
  bool cpu_has_scheduler;

  /**
   * @brief This member contains the return value of the
   *   rtems_scheduler_ident_by_processor_set() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``cpusetsize`` parameter value.
   */
  size_t cpusetsize;

  /**
   * @brief This member specifies if the ``cpuset`` parameter value.
   */
  const cpu_set_t *cpuset;

  /**
   * @brief This member specifies if the ``id`` parameter value.
   */
  rtems_id *id;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 5 ];

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
    RtemsSchedulerReqIdentByProcessorSet_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSchedulerReqIdentByProcessorSet_Context;

static RtemsSchedulerReqIdentByProcessorSet_Context
  RtemsSchedulerReqIdentByProcessorSet_Instance;

static const char * const RtemsSchedulerReqIdentByProcessorSet_PreDesc_CPUOwnedByScheduler[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsSchedulerReqIdentByProcessorSet_PreDesc_CPUSetObj[] = {
  "Invalid",
  "Valid",
  "NA"
};

static const char * const RtemsSchedulerReqIdentByProcessorSet_PreDesc_CPUSetSize[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsSchedulerReqIdentByProcessorSet_PreDesc_CPUSet[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsSchedulerReqIdentByProcessorSet_PreDesc_Id[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsSchedulerReqIdentByProcessorSet_PreDesc[] = {
  RtemsSchedulerReqIdentByProcessorSet_PreDesc_CPUOwnedByScheduler,
  RtemsSchedulerReqIdentByProcessorSet_PreDesc_CPUSetObj,
  RtemsSchedulerReqIdentByProcessorSet_PreDesc_CPUSetSize,
  RtemsSchedulerReqIdentByProcessorSet_PreDesc_CPUSet,
  RtemsSchedulerReqIdentByProcessorSet_PreDesc_Id,
  NULL
};

static void
RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_Prepare(
  RtemsSchedulerReqIdentByProcessorSet_Context                *ctx,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_Yes: {
      /*
       * While the highest numbered online processor specified by the processor
       * set is owned by a scheduler.
       */
      ctx->cpu_has_scheduler = true;
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_No: {
      /*
       * While the highest numbered online processor specified by the processor
       * set is not owned by a scheduler.
       */
      ctx->cpu_has_scheduler = false;
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_Prepare(
  RtemsSchedulerReqIdentByProcessorSet_Context      *ctx,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_Invalid: {
      /*
       * While the processor set contains no online processor.
       */
      CPU_ZERO( &ctx->cpuset_value );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_Valid: {
      /*
       * While the processor set contains at least one online processor.
       */
      CPU_ZERO( &ctx->cpuset_value );

      if ( ctx->cpu_has_scheduler ) {
        CPU_SET( 0, &ctx->cpuset_value );
      } else {
        CPU_SET( 1, &ctx->cpuset_value );
      }
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_Prepare(
  RtemsSchedulerReqIdentByProcessorSet_Context       *ctx,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_Valid: {
      /*
       * While the ``cpusetsize`` parameter is an integral multiple of the size
       * of long.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_value );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_Invalid: {
      /*
       * While the ``cpusetsize`` parameter is not an integral multiple of the
       * size of long.
       */
      ctx->cpusetsize = 1;
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_Prepare(
  RtemsSchedulerReqIdentByProcessorSet_Context   *ctx,
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_Valid: {
      /*
       * While the ``cpuset`` parameter references an object of type cpu_set_t.
       */
      ctx->cpuset = &ctx->cpuset_value;
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_Null: {
      /*
       * While the ``cpuset`` parameter is equal to NULL.
       */
      ctx->cpuset = NULL;
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessorSet_Pre_Id_Prepare(
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx,
  RtemsSchedulerReqIdentByProcessorSet_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessorSet_Pre_Id_Valid: {
      /*
       * While the ``id`` parameter references an object of type rtems_id.
       */
      ctx->id_value = INVALID_ID;
      ctx->id = &ctx->id_value;
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_Id_Null: {
      /*
       * While the ``id`` parameter is equal to NULL.
       */
      ctx->id = NULL;
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Pre_Id_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessorSet_Post_Status_Check(
  RtemsSchedulerReqIdentByProcessorSet_Context    *ctx,
  RtemsSchedulerReqIdentByProcessorSet_Post_Status state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessorSet_Post_Status_Ok: {
      /*
       * The return status of rtems_scheduler_ident_by_processor_set() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvAddr: {
      /*
       * The return status of rtems_scheduler_ident_by_processor_set() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvSize: {
      /*
       * The return status of rtems_scheduler_ident_by_processor_set() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvName: {
      /*
       * The return status of rtems_scheduler_ident_by_processor_set() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NAME );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Post_Status_IncStat: {
      /*
       * The return status of rtems_scheduler_ident_by_processor_set() shall be
       * RTEMS_INVALID_NAME.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Post_Status_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Check(
  RtemsSchedulerReqIdentByProcessorSet_Context   *ctx,
  RtemsSchedulerReqIdentByProcessorSet_Post_IdVar state
)
{
  switch ( state ) {
    case RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Set: {
      /*
       * The value of the object referenced by the ``id`` parameter shall be
       * set to the identifier of the scheduler which owned the highest
       * numbered online processor specified by the ``cpusetsize`` ``cpuset``
       * parameters at some point during the call after the return of the
       * rtems_scheduler_ident_by_processor_set() call.
       */
      T_eq_u32( ctx->id_value, 0x0f010001 );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop: {
      /*
       * Objects referenced by the ``id`` parameter in past calls to
       * rtems_scheduler_ident_by_processor_set() shall not be accessed by the
       * rtems_scheduler_ident_by_processor_set() call.
       */
      T_eq_u32( ctx->id_value, INVALID_ID );
      break;
    }

    case RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_NA:
      break;
  }
}

static void RtemsSchedulerReqIdentByProcessorSet_Setup(
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx
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

static void RtemsSchedulerReqIdentByProcessorSet_Setup_Wrap( void *arg )
{
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSchedulerReqIdentByProcessorSet_Setup( ctx );
}

static void RtemsSchedulerReqIdentByProcessorSet_Prepare(
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx
)
{
  ctx->id_value = INVALID_ID;
}

static void RtemsSchedulerReqIdentByProcessorSet_Action(
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx
)
{
  #if defined(RTEMS_SMP)
  rtems_status_code sc;

  if ( !ctx->cpu_has_scheduler ) {
    sc = rtems_scheduler_remove_processor( ctx->second_scheduler_id, 1 );
    T_rsc_success( sc );
  }
  #endif

  ctx->status = rtems_scheduler_ident_by_processor_set(
    ctx->cpusetsize,
    ctx->cpuset,
    ctx->id
  );

  #if defined(RTEMS_SMP)
  if ( !ctx->cpu_has_scheduler ) {
    sc = rtems_scheduler_add_processor( ctx->second_scheduler_id, 1 );
    T_rsc_success( sc );
  }
  #endif
}

static const RtemsSchedulerReqIdentByProcessorSet_Entry
RtemsSchedulerReqIdentByProcessorSet_Entries[] = {
  { 0, 1, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvAddr,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvAddr,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvAddr,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_NA,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_NA },
#endif
  { 0, 1, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvName,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop },
  { 0, 1, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvSize,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_Ok,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Set },
  { 0, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvSize,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_IncStat,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop },
#else
  { 1, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_NA,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_InvSize,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Nop }
#else
  { 1, 0, 0, 0, 0, 0, RtemsSchedulerReqIdentByProcessorSet_Post_Status_NA,
    RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_NA }
#endif
};

static const uint8_t
RtemsSchedulerReqIdentByProcessorSet_Map[] = {
  3, 0, 0, 0, 4, 0, 0, 0, 5, 1, 1, 1, 6, 1, 1, 1, 3, 0, 0, 0, 4, 0, 0, 0, 7, 2,
  2, 2, 8, 2, 2, 2
};

static size_t RtemsSchedulerReqIdentByProcessorSet_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsSchedulerReqIdentByProcessorSet_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsSchedulerReqIdentByProcessorSet_Fixture = {
  .setup = RtemsSchedulerReqIdentByProcessorSet_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsSchedulerReqIdentByProcessorSet_Scope,
  .initial_context = &RtemsSchedulerReqIdentByProcessorSet_Instance
};

static inline RtemsSchedulerReqIdentByProcessorSet_Entry
RtemsSchedulerReqIdentByProcessorSet_PopEntry(
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSchedulerReqIdentByProcessorSet_Entries[
    RtemsSchedulerReqIdentByProcessorSet_Map[ index ]
  ];
}

static void RtemsSchedulerReqIdentByProcessorSet_SetPreConditionStates(
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx
)
{
  if ( ctx->Map.entry.Pre_CPUOwnedByScheduler_NA ) {
    ctx->Map.pcs[ 0 ] = RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_NA;
  } else {
    ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  }

  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
}

static void RtemsSchedulerReqIdentByProcessorSet_TestVariant(
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx
)
{
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_Prepare(
    ctx,
    ctx->Map.pcs[ 3 ]
  );
  RtemsSchedulerReqIdentByProcessorSet_Pre_Id_Prepare(
    ctx,
    ctx->Map.pcs[ 4 ]
  );
  RtemsSchedulerReqIdentByProcessorSet_Action( ctx );
  RtemsSchedulerReqIdentByProcessorSet_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsSchedulerReqIdentByProcessorSet_Post_IdVar_Check(
    ctx,
    ctx->Map.entry.Post_IdVar
  );
}

/**
 * @fn void T_case_body_RtemsSchedulerReqIdentByProcessorSet( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsSchedulerReqIdentByProcessorSet,
  &RtemsSchedulerReqIdentByProcessorSet_Fixture
)
{
  RtemsSchedulerReqIdentByProcessorSet_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_Yes;
    ctx->Map.pci[ 0 ] < RtemsSchedulerReqIdentByProcessorSet_Pre_CPUOwnedByScheduler_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_Invalid;
      ctx->Map.pci[ 1 ] < RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetObj_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_Valid;
        ctx->Map.pci[ 2 ] < RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSetSize_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_Valid;
          ctx->Map.pci[ 3 ] < RtemsSchedulerReqIdentByProcessorSet_Pre_CPUSet_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsSchedulerReqIdentByProcessorSet_Pre_Id_Valid;
            ctx->Map.pci[ 4 ] < RtemsSchedulerReqIdentByProcessorSet_Pre_Id_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = RtemsSchedulerReqIdentByProcessorSet_PopEntry(
              ctx
            );

            if ( ctx->Map.entry.Skip ) {
              continue;
            }

            RtemsSchedulerReqIdentByProcessorSet_SetPreConditionStates( ctx );
            RtemsSchedulerReqIdentByProcessorSet_Prepare( ctx );
            RtemsSchedulerReqIdentByProcessorSet_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
