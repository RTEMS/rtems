/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqHandlerIterate
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

#include <bsp/irq-generic.h>
#include <rtems/irq-extension.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqHandlerIterate spec:/rtems/intr/req/handler-iterate
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqHandlerIterate_Pre_Vector_Valid,
  RtemsIntrReqHandlerIterate_Pre_Vector_Invalid,
  RtemsIntrReqHandlerIterate_Pre_Vector_NA
} RtemsIntrReqHandlerIterate_Pre_Vector;

typedef enum {
  RtemsIntrReqHandlerIterate_Pre_Routine_Valid,
  RtemsIntrReqHandlerIterate_Pre_Routine_Null,
  RtemsIntrReqHandlerIterate_Pre_Routine_NA
} RtemsIntrReqHandlerIterate_Pre_Routine;

typedef enum {
  RtemsIntrReqHandlerIterate_Pre_Init_Yes,
  RtemsIntrReqHandlerIterate_Pre_Init_No,
  RtemsIntrReqHandlerIterate_Pre_Init_NA
} RtemsIntrReqHandlerIterate_Pre_Init;

typedef enum {
  RtemsIntrReqHandlerIterate_Pre_ISR_Yes,
  RtemsIntrReqHandlerIterate_Pre_ISR_No,
  RtemsIntrReqHandlerIterate_Pre_ISR_NA
} RtemsIntrReqHandlerIterate_Pre_ISR;

typedef enum {
  RtemsIntrReqHandlerIterate_Post_Status_Ok,
  RtemsIntrReqHandlerIterate_Post_Status_InvAddr,
  RtemsIntrReqHandlerIterate_Post_Status_IncStat,
  RtemsIntrReqHandlerIterate_Post_Status_InvId,
  RtemsIntrReqHandlerIterate_Post_Status_CalledFromISR,
  RtemsIntrReqHandlerIterate_Post_Status_NA
} RtemsIntrReqHandlerIterate_Post_Status;

typedef enum {
  RtemsIntrReqHandlerIterate_Post_Visit_Yes,
  RtemsIntrReqHandlerIterate_Post_Visit_Nop,
  RtemsIntrReqHandlerIterate_Post_Visit_NA
} RtemsIntrReqHandlerIterate_Post_Visit;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Vector_NA : 1;
  uint16_t Pre_Routine_NA : 1;
  uint16_t Pre_Init_NA : 1;
  uint16_t Pre_ISR_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_Visit : 2;
} RtemsIntrReqHandlerIterate_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/handler-iterate test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then the service was initialized during
   *   setup.
   */
  bool initialized_during_setup;

  /**
   * @brief This member provides the vector number of a testable interrupt
   *   vector.
   */
  rtems_vector_number test_vector;

  /**
   * @brief If this member is true, then the testable interrupt vector was
   *   enabled at the test case begin.
   */
  bool test_vector_was_enabled;

  /**
   * @brief If this member is true, then the service shall be initialized.
   */
  bool initialized;

  /**
   * @brief If this member is true, then rtems_interrupt_handler_iterate()
   *   shall be called from within interrupt context.
   */
  bool isr;

  /**
   * @brief This member provides the count of visited entries.
   */
  uint32_t visited_entries;

  /**
   * @brief This member provides an rtems_interrupt_entry object.
   */
  rtems_interrupt_entry entry;

  /**
   * @brief This member specifies if the ``vector`` parameter value.
   */
  rtems_vector_number vector;

  /**
   * @brief This member specifies if the ``routine`` parameter value.
   */
  rtems_interrupt_per_handler_routine routine;

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_handler_iterate() call.
   */
  rtems_status_code status;

  struct {
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
    RtemsIntrReqHandlerIterate_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqHandlerIterate_Context;

static RtemsIntrReqHandlerIterate_Context
  RtemsIntrReqHandlerIterate_Instance;

static const char * const RtemsIntrReqHandlerIterate_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqHandlerIterate_PreDesc_Routine[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqHandlerIterate_PreDesc_Init[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsIntrReqHandlerIterate_PreDesc_ISR[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqHandlerIterate_PreDesc[] = {
  RtemsIntrReqHandlerIterate_PreDesc_Vector,
  RtemsIntrReqHandlerIterate_PreDesc_Routine,
  RtemsIntrReqHandlerIterate_PreDesc_Init,
  RtemsIntrReqHandlerIterate_PreDesc_ISR,
  NULL
};

typedef RtemsIntrReqHandlerIterate_Context Context;

static char entry_arg;

static char visitor_arg;

static const char entry_info[] = "Entry";

static void EntryRoutine( void *arg )
{
  Context          *ctx;
  rtems_status_code sc;

  (void) arg;
  ctx = T_fixture_context();
  sc = rtems_interrupt_vector_disable( ctx->test_vector );
  T_rsc_success( sc );

  T_eq_ptr( arg, &entry_arg );
}

static void VisitorRoutine(
  void                   *arg,
  const char             *info,
  rtems_option            options,
  rtems_interrupt_handler handler_routine,
  void                   *handler_arg
)
{
  Context *ctx;

  ctx = T_fixture_context();
  ++ctx->visited_entries;
  T_eq_ptr( arg, &visitor_arg );
  T_eq_ptr( info, entry_info );
  T_eq_u32( options, RTEMS_INTERRUPT_UNIQUE );
  T_eq_ptr( handler_routine, EntryRoutine );
  T_eq_ptr( handler_arg, &entry_arg );
}

static void Action( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->visited_entries = 0;

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized
  );

  ctx->status = rtems_interrupt_handler_iterate(
    ctx->vector,
    ctx->routine,
    &visitor_arg
  );

  bsp_interrupt_set_handler_unique(
    BSP_INTERRUPT_DISPATCH_TABLE_SIZE,
    ctx->initialized_during_setup
  );
}

static void RtemsIntrReqHandlerIterate_Pre_Vector_Prepare(
  RtemsIntrReqHandlerIterate_Context   *ctx,
  RtemsIntrReqHandlerIterate_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqHandlerIterate_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->vector = ctx->test_vector;
      break;
    }

    case RtemsIntrReqHandlerIterate_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
      break;
    }

    case RtemsIntrReqHandlerIterate_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqHandlerIterate_Pre_Routine_Prepare(
  RtemsIntrReqHandlerIterate_Context    *ctx,
  RtemsIntrReqHandlerIterate_Pre_Routine state
)
{
  switch ( state ) {
    case RtemsIntrReqHandlerIterate_Pre_Routine_Valid: {
      /*
       * While the visitor routine specified by the ``routine`` parameter is
       * valid.
       */
      ctx->routine = VisitorRoutine;
      break;
    }

    case RtemsIntrReqHandlerIterate_Pre_Routine_Null: {
      /*
       * While the visitor routine specified by the ``routine`` parameter is
       * equal to NULL.
       */
      ctx->routine = NULL;
      break;
    }

    case RtemsIntrReqHandlerIterate_Pre_Routine_NA:
      break;
  }
}

static void RtemsIntrReqHandlerIterate_Pre_Init_Prepare(
  RtemsIntrReqHandlerIterate_Context *ctx,
  RtemsIntrReqHandlerIterate_Pre_Init state
)
{
  switch ( state ) {
    case RtemsIntrReqHandlerIterate_Pre_Init_Yes: {
      /*
       * While the service is initialized.
       */
      ctx->initialized = true;
      break;
    }

    case RtemsIntrReqHandlerIterate_Pre_Init_No: {
      /*
       * While the service is not initialized.
       */
      ctx->initialized = false;
      break;
    }

    case RtemsIntrReqHandlerIterate_Pre_Init_NA:
      break;
  }
}

static void RtemsIntrReqHandlerIterate_Pre_ISR_Prepare(
  RtemsIntrReqHandlerIterate_Context *ctx,
  RtemsIntrReqHandlerIterate_Pre_ISR  state
)
{
  switch ( state ) {
    case RtemsIntrReqHandlerIterate_Pre_ISR_Yes: {
      /*
       * While rtems_interrupt_handler_iterate() is called from within
       * interrupt context.
       */
      ctx->isr = true;
      break;
    }

    case RtemsIntrReqHandlerIterate_Pre_ISR_No: {
      /*
       * While rtems_interrupt_handler_iterate() is not called from within
       * interrupt context.
       */
      ctx->isr = false;
      break;
    }

    case RtemsIntrReqHandlerIterate_Pre_ISR_NA:
      break;
  }
}

static void RtemsIntrReqHandlerIterate_Post_Status_Check(
  RtemsIntrReqHandlerIterate_Context    *ctx,
  RtemsIntrReqHandlerIterate_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqHandlerIterate_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_handler_iterate() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqHandlerIterate_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_handler_iterate() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqHandlerIterate_Post_Status_IncStat: {
      /*
       * The return status of rtems_interrupt_handler_iterate() shall be
       * RTEMS_INCORRECT_STATE.
       */
      T_rsc( ctx->status, RTEMS_INCORRECT_STATE );
      break;
    }

    case RtemsIntrReqHandlerIterate_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_handler_iterate() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqHandlerIterate_Post_Status_CalledFromISR: {
      /*
       * The return status of rtems_interrupt_handler_iterate() shall be
       * RTEMS_CALLED_FROM_ISR.
       */
      T_rsc( ctx->status, RTEMS_CALLED_FROM_ISR );
      break;
    }

    case RtemsIntrReqHandlerIterate_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqHandlerIterate_Post_Visit_Check(
  RtemsIntrReqHandlerIterate_Context   *ctx,
  RtemsIntrReqHandlerIterate_Post_Visit state
)
{
  switch ( state ) {
    case RtemsIntrReqHandlerIterate_Post_Visit_Yes: {
      /*
       * For each interrupt entry installed at the interrupt vector specified
       * by ``vector`` the visitor routine specified by ``routine`` shall be
       * called with the argument specified by ``arg``, the entry information,
       * the entry install options, the entry handler routine, and the entry
       * handler argument.
       */
      T_eq_u32( ctx->visited_entries, 1 );
      break;
    }

    case RtemsIntrReqHandlerIterate_Post_Visit_Nop: {
      /*
       * The visitor routine specified by ``routine`` shall not be called.
       */
      T_eq_u32( ctx->visited_entries, 0 );
      break;
    }

    case RtemsIntrReqHandlerIterate_Post_Visit_NA:
      break;
  }
}

static void RtemsIntrReqHandlerIterate_Setup(
  RtemsIntrReqHandlerIterate_Context *ctx
)
{
  rtems_status_code sc;

  ctx->initialized_during_setup = bsp_interrupt_is_initialized();
  ctx->test_vector = GetTestableInterruptVector( NULL );
  ctx->test_vector_was_enabled = false;
  (void) rtems_interrupt_vector_is_enabled(
    ctx->test_vector,
    &ctx->test_vector_was_enabled
  );
  rtems_interrupt_entry_initialize(
    &ctx->entry,
    EntryRoutine,
    &entry_arg,
    entry_info
  );
  sc = rtems_interrupt_entry_install(
    ctx->test_vector,
    RTEMS_INTERRUPT_UNIQUE,
    &ctx->entry
  );
  T_rsc_success( sc );
}

static void RtemsIntrReqHandlerIterate_Setup_Wrap( void *arg )
{
  RtemsIntrReqHandlerIterate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqHandlerIterate_Setup( ctx );
}

static void RtemsIntrReqHandlerIterate_Teardown(
  RtemsIntrReqHandlerIterate_Context *ctx
)
{
  rtems_status_code sc;

  sc = rtems_interrupt_entry_remove(
    ctx->test_vector,
    &ctx->entry
  );
  T_rsc_success( sc );

  if ( ctx->test_vector_was_enabled ) {
    (void) rtems_interrupt_vector_enable( ctx->test_vector );
  }
}

static void RtemsIntrReqHandlerIterate_Teardown_Wrap( void *arg )
{
  RtemsIntrReqHandlerIterate_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqHandlerIterate_Teardown( ctx );
}

static void RtemsIntrReqHandlerIterate_Action(
  RtemsIntrReqHandlerIterate_Context *ctx
)
{
  if ( ctx->isr ) {
    CallWithinISR( Action, ctx );
  } else {
    Action( ctx );
  }
}

static const RtemsIntrReqHandlerIterate_Entry
RtemsIntrReqHandlerIterate_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsIntrReqHandlerIterate_Post_Status_IncStat,
    RtemsIntrReqHandlerIterate_Post_Visit_Nop },
  { 0, 0, 0, 0, 0, RtemsIntrReqHandlerIterate_Post_Status_InvAddr,
    RtemsIntrReqHandlerIterate_Post_Visit_NA },
  { 0, 0, 0, 0, 0, RtemsIntrReqHandlerIterate_Post_Status_IncStat,
    RtemsIntrReqHandlerIterate_Post_Visit_NA },
  { 0, 0, 0, 0, 0, RtemsIntrReqHandlerIterate_Post_Status_InvId,
    RtemsIntrReqHandlerIterate_Post_Visit_Nop },
  { 0, 0, 0, 0, 0, RtemsIntrReqHandlerIterate_Post_Status_CalledFromISR,
    RtemsIntrReqHandlerIterate_Post_Visit_Nop },
  { 0, 0, 0, 0, 0, RtemsIntrReqHandlerIterate_Post_Status_Ok,
    RtemsIntrReqHandlerIterate_Post_Visit_Yes }
};

static const uint8_t
RtemsIntrReqHandlerIterate_Map[] = {
  4, 5, 0, 0, 1, 1, 2, 2, 3, 3, 0, 0, 1, 1, 2, 2
};

static size_t RtemsIntrReqHandlerIterate_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsIntrReqHandlerIterate_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqHandlerIterate_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqHandlerIterate_Fixture = {
  .setup = RtemsIntrReqHandlerIterate_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsIntrReqHandlerIterate_Teardown_Wrap,
  .scope = RtemsIntrReqHandlerIterate_Scope,
  .initial_context = &RtemsIntrReqHandlerIterate_Instance
};

static inline RtemsIntrReqHandlerIterate_Entry
RtemsIntrReqHandlerIterate_PopEntry( RtemsIntrReqHandlerIterate_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqHandlerIterate_Entries[
    RtemsIntrReqHandlerIterate_Map[ index ]
  ];
}

static void RtemsIntrReqHandlerIterate_TestVariant(
  RtemsIntrReqHandlerIterate_Context *ctx
)
{
  RtemsIntrReqHandlerIterate_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqHandlerIterate_Pre_Routine_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqHandlerIterate_Pre_Init_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqHandlerIterate_Pre_ISR_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsIntrReqHandlerIterate_Action( ctx );
  RtemsIntrReqHandlerIterate_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsIntrReqHandlerIterate_Post_Visit_Check(
    ctx,
    ctx->Map.entry.Post_Visit
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqHandlerIterate( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqHandlerIterate,
  &RtemsIntrReqHandlerIterate_Fixture
)
{
  RtemsIntrReqHandlerIterate_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsIntrReqHandlerIterate_Pre_Vector_Valid;
    ctx->Map.pcs[ 0 ] < RtemsIntrReqHandlerIterate_Pre_Vector_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    for (
      ctx->Map.pcs[ 1 ] = RtemsIntrReqHandlerIterate_Pre_Routine_Valid;
      ctx->Map.pcs[ 1 ] < RtemsIntrReqHandlerIterate_Pre_Routine_NA;
      ++ctx->Map.pcs[ 1 ]
    ) {
      for (
        ctx->Map.pcs[ 2 ] = RtemsIntrReqHandlerIterate_Pre_Init_Yes;
        ctx->Map.pcs[ 2 ] < RtemsIntrReqHandlerIterate_Pre_Init_NA;
        ++ctx->Map.pcs[ 2 ]
      ) {
        for (
          ctx->Map.pcs[ 3 ] = RtemsIntrReqHandlerIterate_Pre_ISR_Yes;
          ctx->Map.pcs[ 3 ] < RtemsIntrReqHandlerIterate_Pre_ISR_NA;
          ++ctx->Map.pcs[ 3 ]
        ) {
          ctx->Map.entry = RtemsIntrReqHandlerIterate_PopEntry( ctx );
          RtemsIntrReqHandlerIterate_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
