/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqGetAffinity
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

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsTaskReqGetAffinity spec:/rtems/task/req/get-affinity
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqGetAffinity_Pre_Id_Invalid,
  RtemsTaskReqGetAffinity_Pre_Id_Task,
  RtemsTaskReqGetAffinity_Pre_Id_NA
} RtemsTaskReqGetAffinity_Pre_Id;

typedef enum {
  RtemsTaskReqGetAffinity_Pre_CPUSetSize_Valid,
  RtemsTaskReqGetAffinity_Pre_CPUSetSize_TooSmall,
  RtemsTaskReqGetAffinity_Pre_CPUSetSize_Askew,
  RtemsTaskReqGetAffinity_Pre_CPUSetSize_NA
} RtemsTaskReqGetAffinity_Pre_CPUSetSize;

typedef enum {
  RtemsTaskReqGetAffinity_Pre_CPUSet_Valid,
  RtemsTaskReqGetAffinity_Pre_CPUSet_Null,
  RtemsTaskReqGetAffinity_Pre_CPUSet_NA
} RtemsTaskReqGetAffinity_Pre_CPUSet;

typedef enum {
  RtemsTaskReqGetAffinity_Post_Status_Ok,
  RtemsTaskReqGetAffinity_Post_Status_InvAddr,
  RtemsTaskReqGetAffinity_Post_Status_InvId,
  RtemsTaskReqGetAffinity_Post_Status_InvSize,
  RtemsTaskReqGetAffinity_Post_Status_NA
} RtemsTaskReqGetAffinity_Post_Status;

typedef enum {
  RtemsTaskReqGetAffinity_Post_CPUSetObj_Set,
  RtemsTaskReqGetAffinity_Post_CPUSetObj_Nop,
  RtemsTaskReqGetAffinity_Post_CPUSetObj_NA
} RtemsTaskReqGetAffinity_Post_CPUSetObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_CPUSetSize_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_CPUSetObj : 2;
} RtemsTaskReqGetAffinity_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/get-affinity test case.
 */
typedef struct {
  /**
   * @brief This member provides the object referenced by the ``cpuset``
   *   parameter.
   */
  cpu_set_t cpuset_obj;

  /**
   * @brief This member contains the return value of the
   *   rtems_task_get_affinity() call.
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
    RtemsTaskReqGetAffinity_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqGetAffinity_Context;

static RtemsTaskReqGetAffinity_Context
  RtemsTaskReqGetAffinity_Instance;

static const char * const RtemsTaskReqGetAffinity_PreDesc_Id[] = {
  "Invalid",
  "Task",
  "NA"
};

static const char * const RtemsTaskReqGetAffinity_PreDesc_CPUSetSize[] = {
  "Valid",
  "TooSmall",
  "Askew",
  "NA"
};

static const char * const RtemsTaskReqGetAffinity_PreDesc_CPUSet[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsTaskReqGetAffinity_PreDesc[] = {
  RtemsTaskReqGetAffinity_PreDesc_Id,
  RtemsTaskReqGetAffinity_PreDesc_CPUSetSize,
  RtemsTaskReqGetAffinity_PreDesc_CPUSet,
  NULL
};

static void RtemsTaskReqGetAffinity_Pre_Id_Prepare(
  RtemsTaskReqGetAffinity_Context *ctx,
  RtemsTaskReqGetAffinity_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqGetAffinity_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqGetAffinity_Pre_Id_Task: {
      /*
       * While the ``id`` parameter is associated with a task.
       */
      ctx->id = RTEMS_SELF;
      break;
    }

    case RtemsTaskReqGetAffinity_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqGetAffinity_Pre_CPUSetSize_Prepare(
  RtemsTaskReqGetAffinity_Context       *ctx,
  RtemsTaskReqGetAffinity_Pre_CPUSetSize state
)
{
  switch ( state ) {
    case RtemsTaskReqGetAffinity_Pre_CPUSetSize_Valid: {
      /*
       * While the ``cpusetsize`` parameter is an integral multiple of the size
       * of long, while the ``cpusetsize`` parameter specifies a processor set
       * which is large enough to contain the processor affinity set of the
       * task.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj );
      break;
    }

    case RtemsTaskReqGetAffinity_Pre_CPUSetSize_TooSmall: {
      /*
       * While the ``cpusetsize`` parameter is an integral multiple of the size
       * of long, while the ``cpusetsize`` parameter specifies a processor set
       * which is not large enough to contain the processor affinity set of the
       * task.
       */
      ctx->cpusetsize = 0;
      break;
    }

    case RtemsTaskReqGetAffinity_Pre_CPUSetSize_Askew: {
      /*
       * While the ``cpusetsize`` parameter is not an integral multiple of the
       * size of long.
       */
      ctx->cpusetsize = SIZE_MAX;
      break;
    }

    case RtemsTaskReqGetAffinity_Pre_CPUSetSize_NA:
      break;
  }
}

static void RtemsTaskReqGetAffinity_Pre_CPUSet_Prepare(
  RtemsTaskReqGetAffinity_Context   *ctx,
  RtemsTaskReqGetAffinity_Pre_CPUSet state
)
{
  switch ( state ) {
    case RtemsTaskReqGetAffinity_Pre_CPUSet_Valid: {
      /*
       * While the ``cpuset`` parameter references an object of type cpu_set_t.
       */
      ctx->cpuset = &ctx->cpuset_obj;
      break;
    }

    case RtemsTaskReqGetAffinity_Pre_CPUSet_Null: {
      /*
       * While the ``cpuset`` parameter is equal to NULL.
       */
      ctx->cpuset = NULL;
      break;
    }

    case RtemsTaskReqGetAffinity_Pre_CPUSet_NA:
      break;
  }
}

static void RtemsTaskReqGetAffinity_Post_Status_Check(
  RtemsTaskReqGetAffinity_Context    *ctx,
  RtemsTaskReqGetAffinity_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqGetAffinity_Post_Status_Ok: {
      /*
       * The return status of rtems_task_get_affinity() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqGetAffinity_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_get_affinity() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqGetAffinity_Post_Status_InvId: {
      /*
       * The return status of rtems_task_get_affinity() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqGetAffinity_Post_Status_InvSize: {
      /*
       * The return status of rtems_task_get_affinity() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsTaskReqGetAffinity_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqGetAffinity_Post_CPUSetObj_Check(
  RtemsTaskReqGetAffinity_Context       *ctx,
  RtemsTaskReqGetAffinity_Post_CPUSetObj state
)
{
  cpu_set_t set;
  uint32_t  cpu_index;
  uint32_t  cpu_max;

  switch ( state ) {
    case RtemsTaskReqGetAffinity_Post_CPUSetObj_Set: {
      /*
       * The value of the object referenced by the ``cpuset`` parameter shall
       * be set to the processor affinity set of the task specified by the
       * ``id`` parameter at some point during the call after the return of the
       * rtems_task_get_affinity() call.
       */
      CPU_ZERO( &set );

      cpu_max = rtems_scheduler_get_processor_maximum();

      /* We need the online processors */
      if ( cpu_max > 4 ) {
        cpu_max = 4;
      }

      for ( cpu_index = 0; cpu_index < cpu_max; ++cpu_index ) {
        CPU_SET( (int) cpu_index, &set );
      }

      T_eq_int( CPU_CMP( &ctx->cpuset_obj, &set ), 0 );
      break;
    }

    case RtemsTaskReqGetAffinity_Post_CPUSetObj_Nop: {
      /*
       * Objects referenced by the ``cpuset`` parameter in past calls to
       * rtems_task_get_affinity() shall not be accessed by the
       * rtems_task_get_affinity() call.
       */
      CPU_ZERO( &set );
      T_eq_int( CPU_CMP( &ctx->cpuset_obj, &set ), 0 );
      break;
    }

    case RtemsTaskReqGetAffinity_Post_CPUSetObj_NA:
      break;
  }
}

static void RtemsTaskReqGetAffinity_Prepare(
  RtemsTaskReqGetAffinity_Context *ctx
)
{
  CPU_ZERO( &ctx->cpuset_obj );
}

static void RtemsTaskReqGetAffinity_Action(
  RtemsTaskReqGetAffinity_Context *ctx
)
{
  ctx->status = rtems_task_get_affinity(
    ctx->id,
    ctx->cpusetsize,
    ctx->cpuset
  );
}

static const RtemsTaskReqGetAffinity_Entry
RtemsTaskReqGetAffinity_Entries[] = {
  { 0, 0, 0, 0, RtemsTaskReqGetAffinity_Post_Status_InvAddr,
    RtemsTaskReqGetAffinity_Post_CPUSetObj_Nop },
  { 0, 0, 0, 0, RtemsTaskReqGetAffinity_Post_Status_InvId,
    RtemsTaskReqGetAffinity_Post_CPUSetObj_Nop },
  { 0, 0, 0, 0, RtemsTaskReqGetAffinity_Post_Status_InvSize,
    RtemsTaskReqGetAffinity_Post_CPUSetObj_Nop },
  { 0, 0, 0, 0, RtemsTaskReqGetAffinity_Post_Status_Ok,
    RtemsTaskReqGetAffinity_Post_CPUSetObj_Set }
};

static const uint8_t
RtemsTaskReqGetAffinity_Map[] = {
  1, 0, 1, 0, 1, 0, 3, 0, 2, 0, 2, 0
};

static size_t RtemsTaskReqGetAffinity_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqGetAffinity_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsTaskReqGetAffinity_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsTaskReqGetAffinity_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsTaskReqGetAffinity_Scope,
  .initial_context = &RtemsTaskReqGetAffinity_Instance
};

static inline RtemsTaskReqGetAffinity_Entry RtemsTaskReqGetAffinity_PopEntry(
  RtemsTaskReqGetAffinity_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqGetAffinity_Entries[
    RtemsTaskReqGetAffinity_Map[ index ]
  ];
}

static void RtemsTaskReqGetAffinity_TestVariant(
  RtemsTaskReqGetAffinity_Context *ctx
)
{
  RtemsTaskReqGetAffinity_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqGetAffinity_Pre_CPUSetSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqGetAffinity_Pre_CPUSet_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqGetAffinity_Action( ctx );
  RtemsTaskReqGetAffinity_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqGetAffinity_Post_CPUSetObj_Check(
    ctx,
    ctx->Map.entry.Post_CPUSetObj
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqGetAffinity( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsTaskReqGetAffinity,
  &RtemsTaskReqGetAffinity_Fixture
)
{
  RtemsTaskReqGetAffinity_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsTaskReqGetAffinity_Pre_Id_Invalid;
    ctx->Map.pcs[ 0 ] < RtemsTaskReqGetAffinity_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsTaskReqGetAffinity_Pre_CPUSetSize_Valid;
      ctx->Map.pcs[ 1 ] < RtemsTaskReqGetAffinity_Pre_CPUSetSize_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsTaskReqGetAffinity_Pre_CPUSet_Valid;
        ctx->Map.pcs[ 2 ] < RtemsTaskReqGetAffinity_Pre_CPUSet_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsTaskReqGetAffinity_PopEntry( ctx );
        RtemsTaskReqGetAffinity_Prepare( ctx );
        RtemsTaskReqGetAffinity_TestVariant( ctx );
      }
    }
  }
}

/** @} */
