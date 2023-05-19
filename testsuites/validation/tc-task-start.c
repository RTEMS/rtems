/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsTaskReqStart
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
 * @defgroup RtemsTaskReqStart spec:/rtems/task/req/start
 *
 * @ingroup TestsuitesValidationNoClock0
 *
 * @{
 */

typedef enum {
  RtemsTaskReqStart_Pre_Id_Invalid,
  RtemsTaskReqStart_Pre_Id_Task,
  RtemsTaskReqStart_Pre_Id_NA
} RtemsTaskReqStart_Pre_Id;

typedef enum {
  RtemsTaskReqStart_Pre_EntryPoint_Valid,
  RtemsTaskReqStart_Pre_EntryPoint_Null,
  RtemsTaskReqStart_Pre_EntryPoint_NA
} RtemsTaskReqStart_Pre_EntryPoint;

typedef enum {
  RtemsTaskReqStart_Pre_Argument_Pointer,
  RtemsTaskReqStart_Pre_Argument_Number,
  RtemsTaskReqStart_Pre_Argument_NA
} RtemsTaskReqStart_Pre_Argument;

typedef enum {
  RtemsTaskReqStart_Pre_Dormant_Yes,
  RtemsTaskReqStart_Pre_Dormant_No,
  RtemsTaskReqStart_Pre_Dormant_NA
} RtemsTaskReqStart_Pre_Dormant;

typedef enum {
  RtemsTaskReqStart_Pre_Suspended_Yes,
  RtemsTaskReqStart_Pre_Suspended_No,
  RtemsTaskReqStart_Pre_Suspended_NA
} RtemsTaskReqStart_Pre_Suspended;

typedef enum {
  RtemsTaskReqStart_Post_Status_Ok,
  RtemsTaskReqStart_Post_Status_InvAddr,
  RtemsTaskReqStart_Post_Status_InvId,
  RtemsTaskReqStart_Post_Status_IncStat,
  RtemsTaskReqStart_Post_Status_NA
} RtemsTaskReqStart_Post_Status;

typedef enum {
  RtemsTaskReqStart_Post_EntryPoint_Set,
  RtemsTaskReqStart_Post_EntryPoint_Nop,
  RtemsTaskReqStart_Post_EntryPoint_NA
} RtemsTaskReqStart_Post_EntryPoint;

typedef enum {
  RtemsTaskReqStart_Post_Argument_Set,
  RtemsTaskReqStart_Post_Argument_Nop,
  RtemsTaskReqStart_Post_Argument_NA
} RtemsTaskReqStart_Post_Argument;

typedef enum {
  RtemsTaskReqStart_Post_Unblock_Yes,
  RtemsTaskReqStart_Post_Unblock_Nop,
  RtemsTaskReqStart_Post_Unblock_NA
} RtemsTaskReqStart_Post_Unblock;

typedef enum {
  RtemsTaskReqStart_Post_StartExtensions_Yes,
  RtemsTaskReqStart_Post_StartExtensions_Nop,
  RtemsTaskReqStart_Post_StartExtensions_NA
} RtemsTaskReqStart_Post_StartExtensions;

typedef struct {
  uint32_t Skip : 1;
  uint32_t Pre_Id_NA : 1;
  uint32_t Pre_EntryPoint_NA : 1;
  uint32_t Pre_Argument_NA : 1;
  uint32_t Pre_Dormant_NA : 1;
  uint32_t Pre_Suspended_NA : 1;
  uint32_t Post_Status : 3;
  uint32_t Post_EntryPoint : 2;
  uint32_t Post_Argument : 2;
  uint32_t Post_Unblock : 2;
  uint32_t Post_StartExtensions : 2;
} RtemsTaskReqStart_Entry;

/**
 * @brief Test context for spec:/rtems/task/req/start test case.
 */
typedef struct {
  /**
   * @brief This member provides the scheduler operation records.
   */
  T_scheduler_log_2 scheduler_log;

  /**
   * @brief This member contains the identifier of a task.
   */
  rtems_id worker_id;

  /**
   * @brief This member contains the identifier of the test user extensions.
   */
  rtems_id extension_id;

  /**
   * @brief This member contains the count of thread start extension calls.
   */
  uint32_t start_extension_calls;

  /**
   * @brief This member contains the actual argument passed to the entry point.
   */
  rtems_task_argument actual_argument;

  /**
   * @brief This member contains the entry point counter.
   */
  uint32_t counter;

  /**
   * @brief If this member is true, then the worker is started before the
   *   rtems_task_start() call.
   */
  bool start;

  /**
   * @brief If this member is true, then the worker is suspended before the
   *   rtems_task_start() call.
   */
  bool suspend;

  /**
   * @brief This member contains the return value of the rtems_task_start()
   *   call.
   */
  rtems_status_code status;

  /**
   * @brief This member specifies if the ``id`` parameter value.
   */
  rtems_id id;

  /**
   * @brief This member specifies if the ``entry_point`` parameter value.
   */
  rtems_task_entry entry_point;

  /**
   * @brief This member specifies if the ``argument`` parameter value.
   */
  rtems_task_argument argument;

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
    RtemsTaskReqStart_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsTaskReqStart_Context;

static RtemsTaskReqStart_Context
  RtemsTaskReqStart_Instance;

static const char * const RtemsTaskReqStart_PreDesc_Id[] = {
  "Invalid",
  "Task",
  "NA"
};

static const char * const RtemsTaskReqStart_PreDesc_EntryPoint[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsTaskReqStart_PreDesc_Argument[] = {
  "Pointer",
  "Number",
  "NA"
};

static const char * const RtemsTaskReqStart_PreDesc_Dormant[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsTaskReqStart_PreDesc_Suspended[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsTaskReqStart_PreDesc[] = {
  RtemsTaskReqStart_PreDesc_Id,
  RtemsTaskReqStart_PreDesc_EntryPoint,
  RtemsTaskReqStart_PreDesc_Argument,
  RtemsTaskReqStart_PreDesc_Dormant,
  RtemsTaskReqStart_PreDesc_Suspended,
  NULL
};

typedef RtemsTaskReqStart_Context Context;

static void WorkerA( rtems_task_argument arg )
{
  Context *ctx;

  ctx = &RtemsTaskReqStart_Instance;

  while ( true ) {
    ctx->actual_argument += arg;
    ++ctx->counter;
    Yield();
  }
}

static void WorkerB( rtems_task_argument arg )
{
  Context *ctx;

  ctx = &RtemsTaskReqStart_Instance;

  while ( true ) {
    ctx->actual_argument += arg;
    Yield();
  }
}

static void ThreadStart( rtems_tcb *executing, rtems_tcb *started )
{
  (void) executing;
  (void) started;

  ++RtemsTaskReqStart_Instance.start_extension_calls;
}

static const rtems_extensions_table extensions = {
  .thread_start = ThreadStart
};

static void RtemsTaskReqStart_Pre_Id_Prepare(
  RtemsTaskReqStart_Context *ctx,
  RtemsTaskReqStart_Pre_Id   state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Pre_Id_Invalid: {
      /*
       * While the ``id`` parameter is not associated with a task.
       */
      ctx->id = INVALID_ID;
      break;
    }

    case RtemsTaskReqStart_Pre_Id_Task: {
      /*
       * While the ``id`` parameter is associated with a task.
       */
      ctx->id = ctx->worker_id;
      break;
    }

    case RtemsTaskReqStart_Pre_Id_NA:
      break;
  }
}

static void RtemsTaskReqStart_Pre_EntryPoint_Prepare(
  RtemsTaskReqStart_Context       *ctx,
  RtemsTaskReqStart_Pre_EntryPoint state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Pre_EntryPoint_Valid: {
      /*
       * While the task entry point specified by the ``entry_point`` parameter
       * is valid.
       */
      ctx->entry_point = WorkerA;
      break;
    }

    case RtemsTaskReqStart_Pre_EntryPoint_Null: {
      /*
       * While the task entry point specified by the ``entry_point`` parameter
       * is equal to NULL.
       */
      ctx->entry_point = NULL;
      break;
    }

    case RtemsTaskReqStart_Pre_EntryPoint_NA:
      break;
  }
}

static void RtemsTaskReqStart_Pre_Argument_Prepare(
  RtemsTaskReqStart_Context     *ctx,
  RtemsTaskReqStart_Pre_Argument state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Pre_Argument_Pointer: {
      /*
       * While the entry point argument specified by the ``argument`` parameter
       * is a pointer.
       */
      ctx->argument = (rtems_task_argument) ctx;
      break;
    }

    case RtemsTaskReqStart_Pre_Argument_Number: {
      /*
       * While the entry point argument specified by the ``argument`` parameter
       * is a 32-bit number.
       */
      ctx->argument = UINT32_C( 0x87654321 );
      break;
    }

    case RtemsTaskReqStart_Pre_Argument_NA:
      break;
  }
}

static void RtemsTaskReqStart_Pre_Dormant_Prepare(
  RtemsTaskReqStart_Context    *ctx,
  RtemsTaskReqStart_Pre_Dormant state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Pre_Dormant_Yes: {
      /*
       * While the task specified by the ``id`` parameter is dormant.
       */
      ctx->start = false;
      break;
    }

    case RtemsTaskReqStart_Pre_Dormant_No: {
      /*
       * While the task specified by the ``id`` parameter is not dormant.
       */
      ctx->start = true;
      break;
    }

    case RtemsTaskReqStart_Pre_Dormant_NA:
      break;
  }
}

static void RtemsTaskReqStart_Pre_Suspended_Prepare(
  RtemsTaskReqStart_Context      *ctx,
  RtemsTaskReqStart_Pre_Suspended state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Pre_Suspended_Yes: {
      /*
       * While the task specified by the ``id`` parameter is suspended.
       */
      ctx->suspend = true;
      break;
    }

    case RtemsTaskReqStart_Pre_Suspended_No: {
      /*
       * While the task specified by the ``id`` parameter is not suspended.
       */
      ctx->suspend = false;
      break;
    }

    case RtemsTaskReqStart_Pre_Suspended_NA:
      break;
  }
}

static void RtemsTaskReqStart_Post_Status_Check(
  RtemsTaskReqStart_Context    *ctx,
  RtemsTaskReqStart_Post_Status state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Post_Status_Ok: {
      /*
       * The return status of rtems_task_start() shall be RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsTaskReqStart_Post_Status_InvAddr: {
      /*
       * The return status of rtems_task_start() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsTaskReqStart_Post_Status_InvId: {
      /*
       * The return status of rtems_task_start() shall be RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsTaskReqStart_Post_Status_IncStat: {
      /*
       * The return status of rtems_task_start() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsTaskReqStart_Post_Status_NA:
      break;
  }
}

static void RtemsTaskReqStart_Post_EntryPoint_Check(
  RtemsTaskReqStart_Context        *ctx,
  RtemsTaskReqStart_Post_EntryPoint state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Post_EntryPoint_Set: {
      /*
       * The entry point of the task specified by the ``id`` parameter shall be
       * set to the function specified by the ``entry_point`` parameter before
       * the task is unblocked by the rtems_task_start() call.
       */
      T_eq_u32( ctx->counter, 1 );
      break;
    }

    case RtemsTaskReqStart_Post_EntryPoint_Nop: {
      /*
       * No entry point of a task shall be modified by the rtems_task_start()
       * call.
       */
      T_eq_u32( ctx->counter, 0 );
      break;
    }

    case RtemsTaskReqStart_Post_EntryPoint_NA:
      break;
  }
}

static void RtemsTaskReqStart_Post_Argument_Check(
  RtemsTaskReqStart_Context      *ctx,
  RtemsTaskReqStart_Post_Argument state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Post_Argument_Set: {
      /*
       * The entry point argument of the task specified by the ``id`` parameter
       * shall be set to the value specified by the ``argument`` parameter
       * before the task is unblocked by the rtems_task_start() call.
       */
      T_eq_u32( ctx->actual_argument, ctx->argument );
      break;
    }

    case RtemsTaskReqStart_Post_Argument_Nop: {
      /*
       * No entry point argument of a task shall be modified by the
       * rtems_task_start() call.
       */
      T_eq_u32( ctx->actual_argument, 0 );
      break;
    }

    case RtemsTaskReqStart_Post_Argument_NA:
      break;
  }
}

static void RtemsTaskReqStart_Post_Unblock_Check(
  RtemsTaskReqStart_Context     *ctx,
  RtemsTaskReqStart_Post_Unblock state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Post_Unblock_Yes: {
      /*
       * The task specified by the ``id`` parameter shall be unblocked by the
       * rtems_task_start() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 1 );
      T_eq_int(
        ctx->scheduler_log.events[ 0 ].operation,
        T_SCHEDULER_UNBLOCK
      );
      break;
    }

    case RtemsTaskReqStart_Post_Unblock_Nop: {
      /*
       * No task shall be unblocked by the rtems_task_start() call.
       */
      T_eq_sz( ctx->scheduler_log.header.recorded, 0 );
      break;
    }

    case RtemsTaskReqStart_Post_Unblock_NA:
      break;
  }
}

static void RtemsTaskReqStart_Post_StartExtensions_Check(
  RtemsTaskReqStart_Context             *ctx,
  RtemsTaskReqStart_Post_StartExtensions state
)
{
  switch ( state ) {
    case RtemsTaskReqStart_Post_StartExtensions_Yes: {
      /*
       * The thread start user extensions shall be invoked by the
       * rtems_task_start() call.
       */
      T_eq_u32( ctx->start_extension_calls, 1 );
      break;
    }

    case RtemsTaskReqStart_Post_StartExtensions_Nop: {
      /*
       * The thread start user extensions shall not be invoked by the
       * rtems_task_start() call.
       */
      T_eq_u32( ctx->start_extension_calls, 0 );
      break;
    }

    case RtemsTaskReqStart_Post_StartExtensions_NA:
      break;
  }
}

static void RtemsTaskReqStart_Setup( RtemsTaskReqStart_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_extension_create(
    rtems_build_name( 'T', 'E', 'S', 'T' ),
    &extensions,
    &ctx->extension_id
  );
  T_rsc_success( sc );
}

static void RtemsTaskReqStart_Setup_Wrap( void *arg )
{
  RtemsTaskReqStart_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqStart_Setup( ctx );
}

static void RtemsTaskReqStart_Teardown( RtemsTaskReqStart_Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_extension_delete( ctx->extension_id );
  T_rsc_success( sc );
}

static void RtemsTaskReqStart_Teardown_Wrap( void *arg )
{
  RtemsTaskReqStart_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsTaskReqStart_Teardown( ctx );
}

static void RtemsTaskReqStart_Prepare( RtemsTaskReqStart_Context *ctx )
{
  ctx->actual_argument = 0;
  ctx->counter = 0;
  ctx->worker_id = CreateTask( "WORK", PRIO_DEFAULT );
}

static void RtemsTaskReqStart_Action( RtemsTaskReqStart_Context *ctx )
{
  T_scheduler_log *log;

  if ( ctx->start ) {
    StartTask( ctx->worker_id, WorkerB, 0 );
  }

  if ( ctx->suspend ) {
    SuspendTask( ctx->worker_id );
  }

  ctx->start_extension_calls = 0;

  log = T_scheduler_record_2( &ctx->scheduler_log );
  T_null( log );

  ctx->status = rtems_task_start( ctx->id, ctx->entry_point, ctx->argument );

  log = T_scheduler_record( NULL );
  T_eq_ptr( &log->header, &ctx->scheduler_log.header );

  Yield();
}

static void RtemsTaskReqStart_Cleanup( RtemsTaskReqStart_Context *ctx )
{
  DeleteTask( ctx->worker_id );
}

static const RtemsTaskReqStart_Entry
RtemsTaskReqStart_Entries[] = {
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqStart_Post_Status_InvAddr,
    RtemsTaskReqStart_Post_EntryPoint_Nop, RtemsTaskReqStart_Post_Argument_Nop,
    RtemsTaskReqStart_Post_Unblock_Nop,
    RtemsTaskReqStart_Post_StartExtensions_Nop },
  { 0, 0, 0, 0, 1, 1, RtemsTaskReqStart_Post_Status_InvId,
    RtemsTaskReqStart_Post_EntryPoint_Nop, RtemsTaskReqStart_Post_Argument_Nop,
    RtemsTaskReqStart_Post_Unblock_Nop,
    RtemsTaskReqStart_Post_StartExtensions_Nop },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqStart_Post_Status_Ok,
    RtemsTaskReqStart_Post_EntryPoint_Set, RtemsTaskReqStart_Post_Argument_Set,
    RtemsTaskReqStart_Post_Unblock_Yes,
    RtemsTaskReqStart_Post_StartExtensions_Yes },
  { 0, 0, 0, 0, 0, 0, RtemsTaskReqStart_Post_Status_IncStat,
    RtemsTaskReqStart_Post_EntryPoint_Nop, RtemsTaskReqStart_Post_Argument_Nop,
    RtemsTaskReqStart_Post_Unblock_Nop,
    RtemsTaskReqStart_Post_StartExtensions_Nop }
};

static const uint8_t
RtemsTaskReqStart_Map[] = {
  1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 3, 3, 2, 2, 3, 3, 0, 0,
  0, 0, 0, 0, 0, 0
};

static size_t RtemsTaskReqStart_Scope( void *arg, char *buf, size_t n )
{
  RtemsTaskReqStart_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsTaskReqStart_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsTaskReqStart_Fixture = {
  .setup = RtemsTaskReqStart_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsTaskReqStart_Teardown_Wrap,
  .scope = RtemsTaskReqStart_Scope,
  .initial_context = &RtemsTaskReqStart_Instance
};

static inline RtemsTaskReqStart_Entry RtemsTaskReqStart_PopEntry(
  RtemsTaskReqStart_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsTaskReqStart_Entries[
    RtemsTaskReqStart_Map[ index ]
  ];
}

static void RtemsTaskReqStart_SetPreConditionStates(
  RtemsTaskReqStart_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_Dormant_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsTaskReqStart_Pre_Dormant_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  if ( ctx->Map.entry.Pre_Suspended_NA ) {
    ctx->Map.pcs[ 4 ] = RtemsTaskReqStart_Pre_Suspended_NA;
  } else {
    ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];
  }
}

static void RtemsTaskReqStart_TestVariant( RtemsTaskReqStart_Context *ctx )
{
  RtemsTaskReqStart_Pre_Id_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsTaskReqStart_Pre_EntryPoint_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsTaskReqStart_Pre_Argument_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsTaskReqStart_Pre_Dormant_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsTaskReqStart_Pre_Suspended_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsTaskReqStart_Action( ctx );
  RtemsTaskReqStart_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsTaskReqStart_Post_EntryPoint_Check(
    ctx,
    ctx->Map.entry.Post_EntryPoint
  );
  RtemsTaskReqStart_Post_Argument_Check( ctx, ctx->Map.entry.Post_Argument );
  RtemsTaskReqStart_Post_Unblock_Check( ctx, ctx->Map.entry.Post_Unblock );
  RtemsTaskReqStart_Post_StartExtensions_Check(
    ctx,
    ctx->Map.entry.Post_StartExtensions
  );
}

/**
 * @fn void T_case_body_RtemsTaskReqStart( void )
 */
T_TEST_CASE_FIXTURE( RtemsTaskReqStart, &RtemsTaskReqStart_Fixture )
{
  RtemsTaskReqStart_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsTaskReqStart_Pre_Id_Invalid;
    ctx->Map.pci[ 0 ] < RtemsTaskReqStart_Pre_Id_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsTaskReqStart_Pre_EntryPoint_Valid;
      ctx->Map.pci[ 1 ] < RtemsTaskReqStart_Pre_EntryPoint_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsTaskReqStart_Pre_Argument_Pointer;
        ctx->Map.pci[ 2 ] < RtemsTaskReqStart_Pre_Argument_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsTaskReqStart_Pre_Dormant_Yes;
          ctx->Map.pci[ 3 ] < RtemsTaskReqStart_Pre_Dormant_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsTaskReqStart_Pre_Suspended_Yes;
            ctx->Map.pci[ 4 ] < RtemsTaskReqStart_Pre_Suspended_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            ctx->Map.entry = RtemsTaskReqStart_PopEntry( ctx );
            RtemsTaskReqStart_SetPreConditionStates( ctx );
            RtemsTaskReqStart_Prepare( ctx );
            RtemsTaskReqStart_TestVariant( ctx );
            RtemsTaskReqStart_Cleanup( ctx );
          }
        }
      }
    }
  }
}

/** @} */
