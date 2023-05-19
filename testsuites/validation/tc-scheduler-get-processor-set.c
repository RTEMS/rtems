/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsSchedulerReqGetProcessorSet
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
 * @defgroup RtemsSchedulerReqGetProcessorSet \
 *   spec:/rtems/scheduler/req/get-processor-set
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsSchedulerReqGetProcessorSet_Pre_Id_Invalid,
  RtemsSchedulerReqGetProcessorSet_Pre_Id_Scheduler,
  RtemsSchedulerReqGetProcessorSet_Pre_Id_NA
} RtemsSchedulerReqGetProcessorSet_Pre_Id;

typedef enum {
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_Valid,
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_TooSmall,
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_Askew,
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_NA
} RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize;

typedef enum {
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_Valid,
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_Null,
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_NA
} RtemsSchedulerReqGetProcessorSet_Pre_CPUSet;

typedef enum {
  RtemsSchedulerReqGetProcessorSet_Post_Status_Ok,
  RtemsSchedulerReqGetProcessorSet_Post_Status_InvAddr,
  RtemsSchedulerReqGetProcessorSet_Post_Status_InvId,
  RtemsSchedulerReqGetProcessorSet_Post_Status_InvSize,
  RtemsSchedulerReqGetProcessorSet_Post_Status_NA
} RtemsSchedulerReqGetProcessorSet_Post_Status;

typedef enum {
  RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Set,
  RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Nop,
  RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_NA
} RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Id_NA : 1;
  uint16_t Pre_CPUSetSize_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_CPUSetVar : 2;
} RtemsSchedulerReqGetProcessorSet_Entry;

/**
 * @brief Test context for spec:/rtems/scheduler/req/get-processor-set test
 *   case.
 */
typedef struct {
  /**
   * @brief This member contains the identifier of a scheduler.
   */
  rtems_id scheduler_id;

  /**
   * @brief This member provides the object referenced by the ``cpusetsize``
   *   parameter.
   */
  cpu_set_t cpuset_value;

  /**
   * @brief This member contains the return value of the
   *   rtems_scheduler_get_processor_set() call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``scheduler_id`` parameter value.
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
    RtemsSchedulerReqGetProcessorSet_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsSchedulerReqGetProcessorSet_Context;

static RtemsSchedulerReqGetProcessorSet_Context
  RtemsSchedulerReqGetProcessorSet_Instance;

static const char * const RtemsSchedulerReqGetProcessorSet_PreDesc_Id[] = {
  "Invalid",
  "Scheduler",
  "NA"
};

static const char * const RtemsSchedulerReqGetProcessorSet_PreDesc_CPUSetSize[] = {
  "Valid",
  "TooSmall",
  "Askew",
  "NA"
};

static const char * const RtemsSchedulerReqGetProcessorSet_PreDesc_CPUSet[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const * const RtemsSchedulerReqGetProcessorSet_PreDesc[] = {
  RtemsSchedulerReqGetProcessorSet_PreDesc_Id,
  RtemsSchedulerReqGetProcessorSet_PreDesc_CPUSetSize,
  RtemsSchedulerReqGetProcessorSet_PreDesc_CPUSet,
  NULL
};

static void RtemsSchedulerReqGetProcessorSet_Pre_Id_Prepare(
  RtemsSchedulerReqGetProcessorSet_Context *ctx,
  RtemsSchedulerReqGetProcessorSet_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsSchedulerReqGetProcessorSet_Pre_Id_Invalid: {
      /*
       * While the ``scheduler_id`` parameter is not associated with a
       * scheduler.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Pre_Id_Scheduler: {
      /*
       * While the ``scheduler_id`` parameter is associated with a scheduler.
       */
      ctx->id = ctx->scheduler_id;
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Pre_Id_NA:
      break;
  }
}

static void RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_Prepare(
  RtemsSchedulerReqGetProcessorSet_Context       *ctx,
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize state
)
{
  switch ( state ) {
    case RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_Valid: {
      /*
       * While the ``cpusetsize`` parameter is an integral multiple of the size
       * of long, while the ``cpusetsize`` parameter specifies a processor set
       * which is large enough to contain the processor set of the scheduler.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_value );
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_TooSmall: {
      /*
       * While the ``cpusetsize`` parameter is an integral multiple of the size
       * of long, while the ``cpusetsize`` parameter specifies a processor set
       * which is not large enough to contain the processor set of the
       * scheduler.
       */
      ctx->cpusetsize = 0;
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_Askew: {
      /*
       * While the ``cpusetsize`` parameter is not an integral multiple of the
       * size of long.
       */
      ctx->cpusetsize = SIZE_MAX;
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_NA:
      break;
  }
}

static void RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_Prepare(
  RtemsSchedulerReqGetProcessorSet_Context   *ctx,
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSet state
)
{
  switch ( state ) {
    case RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_Valid: {
      /*
       * While the ``cpuset`` parameter references an object of type cpu_set_t.
       */
      ctx->cpuset = &ctx->cpuset_value;
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_Null: {
      /*
       * While the ``cpuset`` parameter is equal to NULL.
       */
      ctx->cpuset = NULL;
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_NA:
      break;
  }
}

static void RtemsSchedulerReqGetProcessorSet_Post_Status_Check(
  RtemsSchedulerReqGetProcessorSet_Context    *ctx,
  RtemsSchedulerReqGetProcessorSet_Post_Status state
)
{
  switch ( state ) {
    case RtemsSchedulerReqGetProcessorSet_Post_Status_Ok: {
      /*
       * The return status of rtems_scheduler_get_processor_set() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Post_Status_InvAddr: {
      /*
       * The return status of rtems_scheduler_get_processor_set() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Post_Status_InvId: {
      /*
       * The return status of rtems_scheduler_get_processor_set() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Post_Status_InvSize: {
      /*
       * The return status of rtems_scheduler_get_processor_set() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Post_Status_NA:
      break;
  }
}

static void RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Check(
  RtemsSchedulerReqGetProcessorSet_Context       *ctx,
  RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar state
)
{
  cpu_set_t set;

  switch ( state ) {
    case RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Set: {
      /*
       * The value of the object referenced by the ``cpuset`` parameter shall
       * be set to the processor set owned by the scheduler specified by the
       * ``scheduler_id`` parameter at some point during the call after the
       * return of the rtems_scheduler_get_processor_set() call.
       */
      CPU_ZERO( &set );
      CPU_SET( 0, &set );
      T_eq_int( CPU_CMP( &ctx->cpuset_value, &set ), 0 );
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Nop: {
      /*
       * Objects referenced by the ``cpuset`` parameter in past calls to
       * rtems_scheduler_get_processor_set() shall not be accessed by the
       * rtems_scheduler_get_processor_set() call.
       */
      CPU_FILL( &set );
      T_eq_int( CPU_CMP( &ctx->cpuset_value, &set ), 0 );
      break;
    }

    case RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_NA:
      break;
  }
}

static void RtemsSchedulerReqGetProcessorSet_Setup(
  RtemsSchedulerReqGetProcessorSet_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_scheduler_ident(
    TEST_SCHEDULER_A_NAME,
    &ctx->scheduler_id
  );
  T_rsc_success( sc );
}

static void RtemsSchedulerReqGetProcessorSet_Setup_Wrap( void *arg )
{
  RtemsSchedulerReqGetProcessorSet_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsSchedulerReqGetProcessorSet_Setup( ctx );
}

static void RtemsSchedulerReqGetProcessorSet_Prepare(
  RtemsSchedulerReqGetProcessorSet_Context *ctx
)
{
  CPU_FILL( &ctx->cpuset_value );
}

static void RtemsSchedulerReqGetProcessorSet_Action(
  RtemsSchedulerReqGetProcessorSet_Context *ctx
)
{
  ctx->status = rtems_scheduler_get_processor_set(
    ctx->id,
    ctx->cpusetsize,
    ctx->cpuset
  );
}

static const RtemsSchedulerReqGetProcessorSet_Entry
RtemsSchedulerReqGetProcessorSet_Entries[] = {
  { 0, 0, 0, 0, RtemsSchedulerReqGetProcessorSet_Post_Status_InvAddr,
    RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Nop },
  { 0, 0, 0, 0, RtemsSchedulerReqGetProcessorSet_Post_Status_InvId,
    RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Nop },
  { 0, 0, 0, 0, RtemsSchedulerReqGetProcessorSet_Post_Status_InvSize,
    RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Nop },
  { 0, 0, 0, 0, RtemsSchedulerReqGetProcessorSet_Post_Status_Ok,
    RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Set }
};

static const uint8_t
RtemsSchedulerReqGetProcessorSet_Map[] = {
  1, 0, 1, 0, 1, 0, 3, 0, 2, 0, 2, 0
};

static size_t RtemsSchedulerReqGetProcessorSet_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsSchedulerReqGetProcessorSet_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsSchedulerReqGetProcessorSet_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsSchedulerReqGetProcessorSet_Fixture = {
  .setup = RtemsSchedulerReqGetProcessorSet_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsSchedulerReqGetProcessorSet_Scope,
  .initial_context = &RtemsSchedulerReqGetProcessorSet_Instance
};

static inline RtemsSchedulerReqGetProcessorSet_Entry
RtemsSchedulerReqGetProcessorSet_PopEntry(
  RtemsSchedulerReqGetProcessorSet_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsSchedulerReqGetProcessorSet_Entries[
    RtemsSchedulerReqGetProcessorSet_Map[ index ]
  ];
}

static void RtemsSchedulerReqGetProcessorSet_TestVariant(
  RtemsSchedulerReqGetProcessorSet_Context *ctx
)
{
  RtemsSchedulerReqGetProcessorSet_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_Prepare(
    ctx,
    ctx->Map.pcs[ 2 ]
  );
  RtemsSchedulerReqGetProcessorSet_Action( ctx );
  RtemsSchedulerReqGetProcessorSet_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsSchedulerReqGetProcessorSet_Post_CPUSetVar_Check(
    ctx,
    ctx->Map.entry.Post_CPUSetVar
  );
}

/**
 * @fn void T_case_body_RtemsSchedulerReqGetProcessorSet( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsSchedulerReqGetProcessorSet,
  &RtemsSchedulerReqGetProcessorSet_Fixture
)
{
  RtemsSchedulerReqGetProcessorSet_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsSchedulerReqGetProcessorSet_Pre_Id_Invalid;
    ctx->Map.pcs[ 0 ] < RtemsSchedulerReqGetProcessorSet_Pre_Id_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_Valid;
      ctx->Map.pcs[ 1 ] < RtemsSchedulerReqGetProcessorSet_Pre_CPUSetSize_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_Valid;
        ctx->Map.pcs[ 2 ] < RtemsSchedulerReqGetProcessorSet_Pre_CPUSet_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        ctx->Map.entry = RtemsSchedulerReqGetProcessorSet_PopEntry( ctx );
        RtemsSchedulerReqGetProcessorSet_Prepare( ctx );
        RtemsSchedulerReqGetProcessorSet_TestVariant( ctx );
      }
    }
  }
}

/** @} */
