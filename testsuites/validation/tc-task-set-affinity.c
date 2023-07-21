/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqSetAffinity
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqSetAffinity spec:/rtems/task/req/set-affinity
 *
 * @ingroup TestsuitesValidationNoClock0
 * @ingroup TestsuitesValidationOneCpu0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqSetAffinity_Pre_Id_Invalid,
  RtemsTaskReqSetAffinity_Pre_Id_Task,
  RtemsTaskReqSetAffinity_Pre_Id_NA
} RtemsTaskReqSetAffinity_Pre_Id;

typedef enum {
  RtemsTaskReqSetAffinity_Pre_CPUSetSize_Askew,
  RtemsTaskReqSetAffinity_Pre_CPUSetSize_Normal,
  RtemsTaskReqSetAffinity_Pre_CPUSetSize_Huge,
  RtemsTaskReqSetAffinity_Pre_CPUSetSize_NA
} RtemsTaskReqSetAffinity_Pre_CPUSetSize;

typedef enum {
  RtemsTaskReqSetAffinity_Pre_CPUSetOnline_Supported,
  RtemsTaskReqSetAffinity_Pre_CPUSetOnline_Unsupported,
  RtemsTaskReqSetAffinity_Pre_CPUSetOnline_NA
} RtemsTaskReqSetAffinity_Pre_CPUSetOnline;

typedef enum {
  RtemsTaskReqSetAffinity_Pre_CPUSetHuge_NotZero,
  RtemsTaskReqSetAffinity_Pre_CPUSetHuge_Zero,
  RtemsTaskReqSetAffinity_Pre_CPUSetHuge_NA
} RtemsTaskReqSetAffinity_Pre_CPUSetHuge;

typedef enum {
  RtemsTaskReqSetAffinity_Pre_CPUSet_Valid,
  RtemsTaskReqSetAffinity_Pre_CPUSet_Null,
  RtemsTaskReqSetAffinity_Pre_CPUSet_NA
} RtemsTaskReqSetAffinity_Pre_CPUSet;

typedef enum {
  RtemsTaskReqSetAffinity_Post_Status_Ok,
  RtemsTaskReqSetAffinity_Post_Status_InvAddr,
  RtemsTaskReqSetAffinity_Post_Status_InvId,
  RtemsTaskReqSetAffinity_Post_Status_InvNum,
  RtemsTaskReqSetAffinity_Post_Status_NA
} RtemsTaskReqSetAffinity_Post_Status;

typedef enum {
  RtemsTaskReqSetAffinity_Post_SetAffinity_Set,
  RtemsTaskReqSetAffinity_Post_SetAffinity_Nop,
  RtemsTaskReqSetAffinity_Post_SetAffinity_NA
} RtemsTaskReqSetAffinity_Post_SetAffinity;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_CPUSetSize_NA : 1;
  uint16_t Pre_CPUSetOnline_NA : 1;
  uint16_t Pre_CPUSetHuge_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_SetAffinity : 2;
} RtemsTaskReqSetAffinity_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/set-affinity test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_2 scheduler_log;

  /**
   * @brief This member provides the object referenced by the ``cpuset``
   *   parameter.
   */
  cpu_set_t cpuset_obj[ 2 ];

  /**
   * @brief This member contains the return value of the
   *   rtems_task_set_affinity() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``id`` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member specifies if the ``cpusetsize`` parameter value.
   */
  size_t cpusetsize;

  /**
   * @brief This member specifies if the ``cpuset`` parameter value.
   */
  cpu_set_t *cpuset;

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
    RtemsTaskReqSetAffinity_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqSetAffinity_Context;

static RtemsTaskReqSetAffinity_Context
  RtemsTaskReqSetAffinity_Instance;

static const char * const RtemsTaskReqSetAffinity_PreDesc_Id[] = {
  "Invalid",
  "Task",
  "NA"
};

static const char * const RtemsTaskReqSetAffinity_PreDesc_CPUSetSize[] = {
  "Askew",
  "Normal",
  "Huge",
  "NA"
};

static const char * const RtemsTaskReqSetAffinity_PreDesc_CPUSetOnline[] = {
  "Supported",
  "Unsupported",
  "NA"
};

static const char * const RtemsTaskReqSetAffinity_PreDesc_CPUSetHuge[] = {
  "NotZero",
  "Zero",
  "NA"
};

static const char * const RtemsTaskReqSetAffinity_PreDesc_CPUSet[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsTaskReqSetAffinity_PreDesc[] = {
  RtemsTaskReqSetAffinity_PreDesc_Id,
  RtemsTaskReqSetAffinity_PreDesc_CPUSetSize,
  RtemsTaskReqSetAffinity_PreDesc_CPUSetOnline,
  RtemsTaskReqSetAffinity_PreDesc_CPUSetHuge,
  RtemsTaskReqSetAffinity_PreDesc_CPUSet,
  NULL
};

static void RtemsTaskReqSetAffinity_Pre_Id_Prepare(
  RtemsTaskReqSetAffinity_Context *ctx,
  RtemsTaskReqSetAffinity_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqSetAffinity_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_Id_Task: {
      /*
       * While the ``id`` parameter is associated with a task.
       */
      ctx->id = RTEMS_SELF;
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqSetAffinity_Pre_CPUSetSize_Prepare(
  RtemsTaskReqSetAffinity_Context       *ctx,
  RtemsTaskReqSetAffinity_Pre_CPUSetSize state
)
{
  switch ( state ) {
    case RtemsTaskReqSetAffinity_Pre_CPUSetSize_Askew: {
      /*
       * While the ``cpusetsize`` parameter is not an integral multiple of the
       * size of long.
       */
      ctx->cpusetsize = SIZE_MAX;
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSetSize_Normal: {
      /*
       * While the ``cpusetsize`` parameter is an integral multiple of the size
       * of long, while the ``cpusetsize`` parameter is less than or equal to
       * the maximum processor set size storable in the system.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj[ 0 ] );
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSetSize_Huge: {
      /*
       * While the ``cpusetsize`` parameter is an integral multiple of the size
       * of long, while the ``cpusetsize`` parameter is greater than the
       * maximum processor set size storable in the system.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj );
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSetSize_NA:
      break;
  }
}

static void RtemsTaskReqSetAffinity_Pre_CPUSetOnline_Prepare(
  RtemsTaskReqSetAffinity_Context         *ctx,
  RtemsTaskReqSetAffinity_Pre_CPUSetOnline state
)
{
  switch ( state ) {
    case RtemsTaskReqSetAffinity_Pre_CPUSetOnline_Supported: {
      /*
       * While the intersection of the processor set specified by the
       * ``cpusetsize`` and ``cpuset`` parameters and the set of online
       * processors represents an affinity set supported by the home scheduler
       * of the task specified by the ``id`` parameter at some point during the
       * rtems_task_set_affinity() call.
       */
      /* Already prepared */
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSetOnline_Unsupported: {
      /*
       * While the intersection of the processor set specified by the
       * ``cpusetsize`` and ``cpuset`` parameters and the set of online
       * processors represents an affinity set not supported by the home
       * scheduler of the task specified by the ``id`` parameter at some point
       * during the rtems_task_set_affinity() call.
       */
      CPU_CLR( 0, &ctx->cpuset_obj[ 0 ] );
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSetOnline_NA:
      break;
  }
}

static void RtemsTaskReqSetAffinity_Pre_CPUSetHuge_Prepare(
  RtemsTaskReqSetAffinity_Context       *ctx,
  RtemsTaskReqSetAffinity_Pre_CPUSetHuge state
)
{
  switch ( state ) {
    case RtemsTaskReqSetAffinity_Pre_CPUSetHuge_NotZero: {
      /*
       * While the processor set specified by the ``cpusetsize`` and ``cpuset``
       * parameters contains at least one processor which is not storable in a
       * processor set supported by the system.
       */
      /* Already prepared */
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSetHuge_Zero: {
      /*
       * While the processor set specified by the ``cpusetsize`` and ``cpuset``
       * parameters contains no processor which is not storable in a processor
       * set supported by the system.
       */
      CPU_ZERO( &ctx->cpuset_obj[ 1 ] );
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSetHuge_NA:
      break;
  }
}

static void RtemsTaskReqSetAffinity_Pre_CPUSet_Prepare(
  RtemsTaskReqSetAffinity_Context   *ctx,
  RtemsTaskReqSetAffinity_Pre_CPUSet state
)
{
  switch ( state ) {
    case RtemsTaskReqSetAffinity_Pre_CPUSet_Valid: {
      /*
       * While the ``cpuset`` parameter references an object of type cpu_set_t.
       */
      ctx->cpuset = &ctx->cpuset_obj[ 0 ];
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSet_Null: {
      /*
       * While the ``cpuset`` parameter is equal to NULL.
       */
      ctx->cpuset = NULL;
      break;
    }

    case RtemsTaskReqSetAffinity_Pre_CPUSet_NA:
      break;
  }
}

static void RtemsTaskReqSetAffinity_Post_Status_Check(
  RtemsTaskReqSetAffinity_Context    *ctx,
  RtemsTaskReqSetAffinity_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqSetAffinity_Post_Status_Ok: {
      /*
       * The return status of rtems_task_set_affinity() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqSetAffinity_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_set_affinity() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqSetAffinity_Post_Status_InvId: {
      /*
       * The return status of rtems_task_set_affinity() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqSetAffinity_Post_Status_InvNum: {
      /*
       * The return status of rtems_task_set_affinity() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsTaskReqSetAffinity_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqSetAffinity_Post_SetAffinity_Check(
  RtemsTaskReqSetAffinity_Context         *ctx,
  RtemsTaskReqSetAffinity_Post_SetAffinity state
)
{
  switch ( state ) {
    case RtemsTaskReqSetAffinity_Post_SetAffinity_Set: {
      /*
       * The affinity set of the task specified by the ``id`` parameter shall
       * be set with respect to the home scheduler of the task at some point
       * during the rtems_task_set_affinity() call.
       */
      #if defined(RTEMS_SMP)
      T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].operation,
        T_SCHEDULER_SET_AFFINITY
      );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].set_affinity.status,
        STATUS_SUCCESSFUL
      );
      #else
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      #endif
      break;
    }

    case RtemsTaskReqSetAffinity_Post_SetAffinity_Nop: {
      /*
       * No task affinity shall be modified by the rtems_task_set_affinity()
       * call.
       */
      #if defined(RTEMS_SMP)
      if ( ctx->scheduler_log.header.recorded == 1 ) {
        T_eq_int(
          ctx->scheduler_log.events[ 0 ].operation,
          T_SCHEDULER_SET_AFFINITY
        );
        T_eq_int(
          ctx->scheduler_log.events[ 0 ].set_affinity.status,
          STATUS_INVALID_NUMBER
        );
      } else {
        T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      }
      #else
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      #endif
      break;
    }

    case RtemsTaskReqSetAffinity_Post_SetAffinity_NA:
      break;
  }
}

static void RtemsTaskReqSetAffinity_Prepare(
  RtemsTaskReqSetAffinity_Context *ctx
)
{
  CPU_FILL_S( sizeof( ctx->cpuset_obj ), &ctx->cpuset_obj[ 0 ] );
}

static void RtemsTaskReqSetAffinity_Action(
  RtemsTaskReqSetAffinity_Context *ctx
)
{
  T_scheduler_log *log;

  log = T_scheduler_record_2( &ctx->scheduler_log );
  T_null( log );

  ctx->status = rtems_task_set_affinity(
    ctx->id,
    ctx->cpusetsize,
    ctx->cpuset
  );

  log = T_scheduler_record( NULL );
  T_eq_ptr( &log->header, &ctx->scheduler_log.header );
}

static const RtemsTaskReqSetAffinity_Entry
RtemsTaskReqSetAffinity_Entries[] = {
  { 0, 0, 0, 1, 1, 0, RtemsTaskReqSetAffinity_Post_Status_InvAddr,
    RtemsTaskReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 1, 1, 0, RtemsTaskReqSetAffinity_Post_Status_InvId,
    RtemsTaskReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 1, 0, RtemsTaskReqSetAffinity_Post_Status_InvNum,
    RtemsTaskReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 1, 0, 0, RtemsTaskReqSetAffinity_Post_Status_InvId,
    RtemsTaskReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 1, 0, RtemsTaskReqSetAffinity_Post_Status_Ok,
    RtemsTaskReqSetAffinity_Post_SetAffinity_Set },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqSetAffinity_Post_Status_Ok,
    RtemsTaskReqSetAffinity_Post_SetAffinity_Set },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqSetAffinity_Post_Status_InvNum,
    RtemsTaskReqSetAffinity_Post_SetAffinity_Nop }
};

static const uint8_t
RtemsTaskReqSetAffinity_Map[] = {
  1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 3, 0, 3, 0, 3, 0, 3, 0, 2, 0,
  2, 0, 2, 0, 2, 0, 4, 0, 4, 0, 2, 0, 2, 0, 5, 0, 5, 0, 6, 0, 6, 0
};

static size_t RtemsTaskReqSetAffinity_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqSetAffinity_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTaskReqSetAffinity_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTaskReqSetAffinity_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsTaskReqSetAffinity_Scope,
  .initial_context = &RtemsTaskReqSetAffinity_Instance
};

static inline RtemsTaskReqSetAffinity_Entry RtemsTaskReqSetAffinity_PopEntry(
  RtemsTaskReqSetAffinity_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqSetAffinity_Entries[
    RtemsTaskReqSetAffinity_Map[ index ]
  ];
}

static void RtemsTaskReqSetAffinity_SetPreConditionStates(
  RtemsTaskReqSetAffinity_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_CPUSetOnline_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsTaskReqSetAffinity_Pre_CPUSetOnline_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_CPUSetHuge_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsTaskReqSetAffinity_Pre_CPUSetHuge_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
}

static void RtemsTaskReqSetAffinity_TestVariant(
  RtemsTaskReqSetAffinity_Context *ctx
)
{
  RtemsTaskReqSetAffinity_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqSetAffinity_Pre_CPUSetSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqSetAffinity_Pre_CPUSetOnline_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqSetAffinity_Pre_CPUSetHuge_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqSetAffinity_Pre_CPUSet_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTaskReqSetAffinity_Action( ctx );
  RtemsTaskReqSetAffinity_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqSetAffinity_Post_SetAffinity_Check(
    ctx,
    ctx->Map.entry.Post_SetAffinity
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqSetAffinity( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqSetAffinity,
  &RtemsTaskReqSetAffinity_Fixture
)
{
  RtemsTaskReqSetAffinity_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqSetAffinity_Pre_Id_Invalid;
    ctx->Map.pci[ 0 ] < RtemsTaskReqSetAffinity_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqSetAffinity_Pre_CPUSetSize_Askew;
      ctx->Map.pci[ 1 ] < RtemsTaskReqSetAffinity_Pre_CPUSetSize_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTaskReqSetAffinity_Pre_CPUSetOnline_Supported;
        ctx->Map.pci[ 2 ] < RtemsTaskReqSetAffinity_Pre_CPUSetOnline_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsTaskReqSetAffinity_Pre_CPUSetHuge_NotZero;
          ctx->Map.pci[ 3 ] < RtemsTaskReqSetAffinity_Pre_CPUSetHuge_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsTaskReqSetAffinity_Pre_CPUSet_Valid;
            ctx->Map.pci[ 4 ] < RtemsTaskReqSetAffinity_Pre_CPUSet_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = RtemsTaskReqSetAffinity_PopEntry( ctx );
            RtemsTaskReqSetAffinity_SetPreConditionStates( ctx );
            RtemsTaskReqSetAffinity_Prepare( ctx );
            RtemsTaskReqSetAffinity_TestVariant( ctx );
          }
        }
      }
    }
  }
}

/** @} */
