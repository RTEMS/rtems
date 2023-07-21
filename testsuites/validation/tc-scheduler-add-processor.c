/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerReqAddProcessor
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
#include <rtems/test-scheduler.h>
#include <rtems/score/percpu.h>

#include "ts-config.h"
#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsSchedulerReqAddProcessor \
 *   spec:/rtems/scheduler/req/add-processor
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSchedulerReqAddProcessor_Pre_HasReady_Ready,
  RtemsSchedulerReqAddProcessor_Pre_HasReady_Empty,
  RtemsSchedulerReqAddProcessor_Pre_HasReady_NA
} RtemsSchedulerReqAddProcessor_Pre_HasReady;

typedef enum {
  RtemsSchedulerReqAddProcessor_Pre_Id_Invalid,
  RtemsSchedulerReqAddProcessor_Pre_Id_Scheduler,
  RtemsSchedulerReqAddProcessor_Pre_Id_NA
} RtemsSchedulerReqAddProcessor_Pre_Id;

typedef enum {
  RtemsSchedulerReqAddProcessor_Pre_CPUIndex_Valid,
  RtemsSchedulerReqAddProcessor_Pre_CPUIndex_Invalid,
  RtemsSchedulerReqAddProcessor_Pre_CPUIndex_NA
} RtemsSchedulerReqAddProcessor_Pre_CPUIndex;

typedef enum {
  RtemsSchedulerReqAddProcessor_Pre_CPUState_Idle,
  RtemsSchedulerReqAddProcessor_Pre_CPUState_InUse,
  RtemsSchedulerReqAddProcessor_Pre_CPUState_NotOnline,
  RtemsSchedulerReqAddProcessor_Pre_CPUState_NotUsable,
  RtemsSchedulerReqAddProcessor_Pre_CPUState_NA
} RtemsSchedulerReqAddProcessor_Pre_CPUState;

typedef enum {
  RtemsSchedulerReqAddProcessor_Post_Status_Ok,
  RtemsSchedulerReqAddProcessor_Post_Status_InvId,
  RtemsSchedulerReqAddProcessor_Post_Status_NotConf,
  RtemsSchedulerReqAddProcessor_Post_Status_IncStat,
  RtemsSchedulerReqAddProcessor_Post_Status_InUse,
  RtemsSchedulerReqAddProcessor_Post_Status_NA
} RtemsSchedulerReqAddProcessor_Post_Status;

typedef enum {
  RtemsSchedulerReqAddProcessor_Post_Added_Yes,
  RtemsSchedulerReqAddProcessor_Post_Added_Nop,
  RtemsSchedulerReqAddProcessor_Post_Added_NA
} RtemsSchedulerReqAddProcessor_Post_Added;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_HasReady_NA : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_CPUIndex_NA : 1;
  uint16_t Pre_CPUState_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Added : 2;
} RtemsSchedulerReqAddProcessor_Entry;

/**
 * @brief Test context for spec:/rtems/scheduler/req/add-processor test case.
 */
typedef struct {
  /**
   * @brief This member specifies the scheduler used to add the processor.
   */
  rtems_id scheduler_id;

  /**
   * @brief This member contains the identifier of scheduler A.
   */
  rtems_id scheduler_a_id;

  /**
   * @brief This member contains the identifier of scheduler B.
   */
  rtems_id scheduler_b_id;

  /**
   * @brief This member contains the identifier of scheduler C.
   */
  rtems_id scheduler_c_id;

  /**
   * @brief This member references the processor control of the processor to
   *   add.
   */
  Per_CPU_Control *cpu;

  /**
   * @brief This member contains the online status of the processor to add
   *   before the rtems_scheduler_add_processor() call is prepared.
   */
  bool online;

  /**
   * @brief If this member is true, then the processor should be added to the
   *   scheduler B during cleanup.
   */
  bool add_cpu_to_scheduler_b;

  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_2 scheduler_log;

  /**
   * @brief This member contains the return value of the
   *   rtems_scheduler_add_processor() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``scheduler_id`` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member specifies if the ``cpu_index`` parameter value.
   */
  uint32_t cpu_index;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

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
    RtemsSchedulerReqAddProcessor_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSchedulerReqAddProcessor_Context;

static RtemsSchedulerReqAddProcessor_Context
  RtemsSchedulerReqAddProcessor_Instance;

static const char * const RtemsSchedulerReqAddProcessor_PreDesc_HasReady[] = {
  "Ready",
  "Empty",
  "NA"
};

static const char * const RtemsSchedulerReqAddProcessor_PreDesc_Id[] = {
  "Invalid",
  "Scheduler",
  "NA"
};

static const char * const RtemsSchedulerReqAddProcessor_PreDesc_CPUIndex[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsSchedulerReqAddProcessor_PreDesc_CPUState[] = {
  "Idle",
  "InUse",
  "NotOnline",
  "NotUsable",
  "NA"
};

static const char * const * const RtemsSchedulerReqAddProcessor_PreDesc[] = {
  RtemsSchedulerReqAddProcessor_PreDesc_HasReady,
  RtemsSchedulerReqAddProcessor_PreDesc_Id,
  RtemsSchedulerReqAddProcessor_PreDesc_CPUIndex,
  RtemsSchedulerReqAddProcessor_PreDesc_CPUState,
  NULL
};

#define CPU_TO_ADD 1

static void RtemsSchedulerReqAddProcessor_Pre_HasReady_Prepare(
  RtemsSchedulerReqAddProcessor_Context     *ctx,
  RtemsSchedulerReqAddProcessor_Pre_HasReady state
)
{
  switch ( state ) {
    case RtemsSchedulerReqAddProcessor_Pre_HasReady_Ready: {
      /*
       * While the scheduler has at least one ready thread.
       */
      ctx->scheduler_id = ctx->scheduler_a_id;
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_HasReady_Empty: {
      /*
       * While the scheduler has no ready threads.
       */
      #if defined(RTEMS_SMP)
      ctx->scheduler_id = ctx->scheduler_c_id;
      #else
      ctx->scheduler_id = ctx->scheduler_a_id;
      #endif
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_HasReady_NA:
      break;
  }
}

static void RtemsSchedulerReqAddProcessor_Pre_Id_Prepare(
  RtemsSchedulerReqAddProcessor_Context *ctx,
  RtemsSchedulerReqAddProcessor_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSchedulerReqAddProcessor_Pre_Id_Invalid: {
      /*
       * While the ``scheduler_id`` parameter is not associated with a
       * scheduler.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_Id_Scheduler: {
      /*
       * While the ``scheduler_id`` parameter is associated with a scheduler.
       */
      ctx->id = ctx->scheduler_id;
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_Id_NA:
      break;
  }
}

static void RtemsSchedulerReqAddProcessor_Pre_CPUIndex_Prepare(
  RtemsSchedulerReqAddProcessor_Context     *ctx,
  RtemsSchedulerReqAddProcessor_Pre_CPUIndex state
)
{
  switch ( state ) {
    case RtemsSchedulerReqAddProcessor_Pre_CPUIndex_Valid: {
      /*
       * While the ``cpu_index`` parameter is less than the configured
       * processor maximum.
       */
      #if defined(RTEMS_SMP)
      ctx->cpu_index = CPU_TO_ADD;
      #else
      ctx->cpu_index = 0;
      #endif
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_CPUIndex_Invalid: {
      /*
       * While the ``cpu_index`` parameter is greater than or equal to the
       * configured processor maximum.
       */
      ctx->cpu_index = rtems_configuration_get_maximum_processors();
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_CPUIndex_NA:
      break;
  }
}

static void RtemsSchedulerReqAddProcessor_Pre_CPUState_Prepare(
  RtemsSchedulerReqAddProcessor_Context     *ctx,
  RtemsSchedulerReqAddProcessor_Pre_CPUState state
)
{
  rtems_status_code sc;

  switch ( state ) {
    case RtemsSchedulerReqAddProcessor_Pre_CPUState_Idle: {
      /*
       * While the processor associated with the ``cpu_index`` parameter is
       * configured to be used by a scheduler, while the processor associated
       * with the ``cpu_index`` parameter is online, while the processor
       * associated with the ``cpu_index`` parameter is not owned by a
       * scheduler.
       */
      sc = rtems_scheduler_remove_processor(
        ctx->scheduler_b_id,
        CPU_TO_ADD
      );
      T_rsc_success( sc );
      ctx->add_cpu_to_scheduler_b = true;
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_CPUState_InUse: {
      /*
       * While the processor associated with the ``cpu_index`` parameter is
       * owned by a scheduler.
       */
      /* Nothing to do */
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_CPUState_NotOnline: {
      /*
       * While the processor associated with the ``cpu_index`` parameter is not
       * online.
       */
      sc = rtems_scheduler_remove_processor(
        ctx->scheduler_b_id,
        CPU_TO_ADD
      );
      T_rsc_success( sc );
      ctx->add_cpu_to_scheduler_b = true;
      #if defined(RTEMS_SMP)
      ctx->cpu->online = false;
      #endif
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_CPUState_NotUsable: {
      /*
       * While the processor associated with the ``cpu_index`` parameter is not
       * configured to be used by a scheduler.
       */
      ctx->cpu_index = rtems_configuration_get_maximum_processors() - 1;
      break;
    }

    case RtemsSchedulerReqAddProcessor_Pre_CPUState_NA:
      break;
  }
}

static void RtemsSchedulerReqAddProcessor_Post_Status_Check(
  RtemsSchedulerReqAddProcessor_Context    *ctx,
  RtemsSchedulerReqAddProcessor_Post_Status state
)
{
  switch ( state ) {
    case RtemsSchedulerReqAddProcessor_Post_Status_Ok: {
      /*
       * The return status of rtems_scheduler_add_processor() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSchedulerReqAddProcessor_Post_Status_InvId: {
      /*
       * The return status of rtems_scheduler_add_processor() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSchedulerReqAddProcessor_Post_Status_NotConf: {
      /*
       * The return status of rtems_scheduler_add_processor() shall be
       * RTEMS_NOT_CONFIGURED.
       */
      T_rsc( ctx->status, RTEMS_NOT_CONFIGURED );
      break;
    }

    case RtemsSchedulerReqAddProcessor_Post_Status_IncStat: {
      /*
       * The return status of rtems_scheduler_add_processor() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsSchedulerReqAddProcessor_Post_Status_InUse: {
      /*
       * The return status of rtems_scheduler_add_processor() shall be
       * RTEMS_RESOURCE_IN_USE.
       */
      T_rsc( ctx->status, RTEMS_RESOURCE_IN_USE );
      break;
    }

    case RtemsSchedulerReqAddProcessor_Post_Status_NA:
      break;
  }
}

static void RtemsSchedulerReqAddProcessor_Post_Added_Check(
  RtemsSchedulerReqAddProcessor_Context   *ctx,
  RtemsSchedulerReqAddProcessor_Post_Added state
)
{
  rtems_status_code   sc;
  cpu_set_t           set;
  rtems_task_priority priority;

  switch ( state ) {
    case RtemsSchedulerReqAddProcessor_Post_Added_Yes: {
      /*
       * The processor specified by the ``cpu_index`` parameter shall be added
       * to the scheduler specified by the ``scheduler_id`` by the
       * rtems_scheduler_add_processor() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 2 );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].operation,
        T_SCHEDULER_MAP_PRIORITY
      );
      T_eq_int(
        ctx->scheduler_log.events[ 1 ].operation,
        T_SCHEDULER_ADD_PROCESSOR
      );

      priority = GetSelfPriority();

      if ( ctx->scheduler_id == ctx->scheduler_c_id ) {
        SetSelfScheduler( ctx->scheduler_c_id, priority );
      }

      SetSelfAffinityOne( CPU_TO_ADD );
      T_eq_u32( rtems_scheduler_get_processor(), CPU_TO_ADD );
      SetSelfAffinityAll();

      if ( ctx->scheduler_id == ctx->scheduler_c_id ) {
        SetSelfScheduler( ctx->scheduler_a_id, priority );
      }
      break;
    }

    case RtemsSchedulerReqAddProcessor_Post_Added_Nop: {
      /*
       * No processor shall be added to a scheduler by the
       * rtems_scheduler_add_processor() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );

      CPU_ZERO( &set );
      CPU_SET( CPU_TO_ADD, &set );
      sc = rtems_task_set_affinity( RTEMS_SELF, sizeof( set ), &set );
      T_rsc( sc, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsSchedulerReqAddProcessor_Post_Added_NA:
      break;
  }
}

static void RtemsSchedulerReqAddProcessor_Setup(
  RtemsSchedulerReqAddProcessor_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_scheduler_ident(
    TEST_SCHEDULER_A_NAME,
    &ctx->scheduler_a_id
  );
  T_rsc_success( sc );

  #if defined(RTEMS_SMP)
  ctx->cpu = _Per_CPU_Get_by_index( CPU_TO_ADD );

  sc = rtems_scheduler_ident( TEST_SCHEDULER_B_NAME, &ctx->scheduler_b_id );
  T_rsc_success( sc );

  sc = rtems_scheduler_ident( TEST_SCHEDULER_C_NAME, &ctx->scheduler_c_id );
  T_rsc_success( sc );
  #else
  ctx->scheduler_b_id = INVALID_ID;
  ctx->scheduler_c_id = INVALID_ID;
  #endif
}

static void RtemsSchedulerReqAddProcessor_Setup_Wrap( void *arg )
{
  RtemsSchedulerReqAddProcessor_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSchedulerReqAddProcessor_Setup( ctx );
}

static void RtemsSchedulerReqAddProcessor_Prepare(
  RtemsSchedulerReqAddProcessor_Context *ctx
)
{
  #if defined(RTEMS_SMP)
  ctx->add_cpu_to_scheduler_b = false;
  ctx->online = _Per_CPU_Is_processor_online( ctx->cpu );
  #endif
}

static void RtemsSchedulerReqAddProcessor_Action(
  RtemsSchedulerReqAddProcessor_Context *ctx
)
{
  T_scheduler_log *log;

  log = T_scheduler_record_2( &ctx->scheduler_log );
  T_null( log );

  ctx->status = rtems_scheduler_add_processor( ctx->id, ctx->cpu_index );

  log = T_scheduler_record( NULL );
  T_eq_ptr( &log->header, &ctx->scheduler_log.header );
}

static void RtemsSchedulerReqAddProcessor_Cleanup(
  RtemsSchedulerReqAddProcessor_Context *ctx
)
{
  #if defined(RTEMS_SMP)
  rtems_status_code sc;

  ctx->cpu->online = ctx->online;

  if ( ctx->status == RTEMS_SUCCESSFUL ) {
    sc = rtems_scheduler_remove_processor( ctx->scheduler_id, CPU_TO_ADD );
    T_rsc_success( sc );
  }

  if ( ctx->add_cpu_to_scheduler_b ) {
    sc = rtems_scheduler_add_processor( ctx->scheduler_b_id, CPU_TO_ADD );
    T_rsc_success( sc );
  }
  #endif
}

static const RtemsSchedulerReqAddProcessor_Entry
RtemsSchedulerReqAddProcessor_Entries[] = {
  { 0, 0, 0, 0, 1, RtemsSchedulerReqAddProcessor_Post_Status_InvId,
    RtemsSchedulerReqAddProcessor_Post_Added_Nop },
  { 0, 0, 0, 0, 1, RtemsSchedulerReqAddProcessor_Post_Status_NotConf,
    RtemsSchedulerReqAddProcessor_Post_Added_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_InvId,
    RtemsSchedulerReqAddProcessor_Post_Added_Nop },
#else
  { 1, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_NA,
    RtemsSchedulerReqAddProcessor_Post_Added_NA },
#endif
  { 0, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_InvId,
    RtemsSchedulerReqAddProcessor_Post_Added_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_Ok,
    RtemsSchedulerReqAddProcessor_Post_Added_Yes },
#else
  { 1, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_NA,
    RtemsSchedulerReqAddProcessor_Post_Added_NA },
#endif
  { 0, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_InUse,
    RtemsSchedulerReqAddProcessor_Post_Added_Nop },
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_IncStat,
    RtemsSchedulerReqAddProcessor_Post_Added_Nop },
#else
  { 1, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_NA,
    RtemsSchedulerReqAddProcessor_Post_Added_NA },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_NotConf,
    RtemsSchedulerReqAddProcessor_Post_Added_Nop }
#else
  { 1, 0, 0, 0, 0, RtemsSchedulerReqAddProcessor_Post_Status_NA,
    RtemsSchedulerReqAddProcessor_Post_Added_NA }
#endif
};

static const uint8_t
RtemsSchedulerReqAddProcessor_Map[] = {
  2, 3, 2, 2, 0, 0, 0, 0, 4, 5, 6, 7, 1, 1, 1, 1, 2, 3, 2, 2, 0, 0, 0, 0, 4, 5,
  6, 7, 1, 1, 1, 1
};

static size_t RtemsSchedulerReqAddProcessor_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsSchedulerReqAddProcessor_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsSchedulerReqAddProcessor_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsSchedulerReqAddProcessor_Fixture = {
  .setup = RtemsSchedulerReqAddProcessor_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsSchedulerReqAddProcessor_Scope,
  .initial_context = &RtemsSchedulerReqAddProcessor_Instance
};

static inline RtemsSchedulerReqAddProcessor_Entry
RtemsSchedulerReqAddProcessor_PopEntry(
  RtemsSchedulerReqAddProcessor_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSchedulerReqAddProcessor_Entries[
    RtemsSchedulerReqAddProcessor_Map[ index ]
  ];
}

static void RtemsSchedulerReqAddProcessor_SetPreConditionStates(
  RtemsSchedulerReqAddProcessor_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_CPUState_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsSchedulerReqAddProcessor_Pre_CPUState_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void RtemsSchedulerReqAddProcessor_TestVariant(
  RtemsSchedulerReqAddProcessor_Context *ctx
)
{
  RtemsSchedulerReqAddProcessor_Pre_HasReady_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSchedulerReqAddProcessor_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsSchedulerReqAddProcessor_Pre_CPUIndex_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsSchedulerReqAddProcessor_Pre_CPUState_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsSchedulerReqAddProcessor_Action( ctx );
  RtemsSchedulerReqAddProcessor_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsSchedulerReqAddProcessor_Post_Added_Check(
    ctx,
    ctx->Map.entry.Post_Added
  );
}

/**
 * @fn void T_case_body_RtemsSchedulerReqAddProcessor( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsSchedulerReqAddProcessor,
  &RtemsSchedulerReqAddProcessor_Fixture
)
{
  RtemsSchedulerReqAddProcessor_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsSchedulerReqAddProcessor_Pre_HasReady_Ready;
    ctx->Map.pci[ 0 ] < RtemsSchedulerReqAddProcessor_Pre_HasReady_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsSchedulerReqAddProcessor_Pre_Id_Invalid;
      ctx->Map.pci[ 1 ] < RtemsSchedulerReqAddProcessor_Pre_Id_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsSchedulerReqAddProcessor_Pre_CPUIndex_Valid;
        ctx->Map.pci[ 2 ] < RtemsSchedulerReqAddProcessor_Pre_CPUIndex_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsSchedulerReqAddProcessor_Pre_CPUState_Idle;
          ctx->Map.pci[ 3 ] < RtemsSchedulerReqAddProcessor_Pre_CPUState_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = RtemsSchedulerReqAddProcessor_PopEntry( ctx );

          if ( ctx->Map.entry.Skip ) {
            continue;
          }

          RtemsSchedulerReqAddProcessor_SetPreConditionStates( ctx );
          RtemsSchedulerReqAddProcessor_Prepare( ctx );
          RtemsSchedulerReqAddProcessor_TestVariant( ctx );
          RtemsSchedulerReqAddProcessor_Cleanup( ctx );
        }
      }
    }
  }
}

/** @} */
