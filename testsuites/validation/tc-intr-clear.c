/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqClear
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

#include <string.h>
#include <bsp/irq-generic.h>
#include <rtems/irq-extension.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsIntrReqClear spec:/rtems/intr/req/clear
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqClear_Pre_Vector_Valid,
  RtemsIntrReqClear_Pre_Vector_Invalid,
  RtemsIntrReqClear_Pre_Vector_NA
} RtemsIntrReqClear_Pre_Vector;

typedef enum {
  RtemsIntrReqClear_Pre_CanClear_Yes,
  RtemsIntrReqClear_Pre_CanClear_No,
  RtemsIntrReqClear_Pre_CanClear_NA
} RtemsIntrReqClear_Pre_CanClear;

typedef enum {
  RtemsIntrReqClear_Post_Status_Ok,
  RtemsIntrReqClear_Post_Status_InvId,
  RtemsIntrReqClear_Post_Status_Unsat,
  RtemsIntrReqClear_Post_Status_NA
} RtemsIntrReqClear_Post_Status;

typedef enum {
  RtemsIntrReqClear_Post_Cleared_Yes,
  RtemsIntrReqClear_Post_Cleared_No,
  RtemsIntrReqClear_Post_Cleared_NA
} RtemsIntrReqClear_Post_Cleared;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Vector_NA : 1;
  uint8_t Pre_CanClear_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Cleared : 2;
} RtemsIntrReqClear_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/clear test case.
 */
typedef struct {
  /**
   * @brief This member contains the count of serviced interrupts.
   */
  volatile uint32_t interrupt_count;

  /**
   * @brief If this member is true, then the interrupt shall be cleared.
   */
  bool do_clear;

  /**
   * @brief This member contains the current vector number.
   */
  rtems_vector_number vector;

  /**
   * @brief If this member is true, then the ``vector`` parameter shall be
   *   valid.
   */
  bool valid_vector;

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_clear() call.
   */
  rtems_status_code status;

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 2 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 2 ];

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
    RtemsIntrReqClear_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqClear_Context;

static RtemsIntrReqClear_Context
  RtemsIntrReqClear_Instance;

static const char * const RtemsIntrReqClear_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqClear_PreDesc_CanClear[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqClear_PreDesc[] = {
  RtemsIntrReqClear_PreDesc_Vector,
  RtemsIntrReqClear_PreDesc_CanClear,
  NULL
};

typedef RtemsIntrReqClear_Context Context;

static bool IsEnabled( const Context *ctx )
{
  rtems_status_code sc;
  bool              enabled;

  enabled = false;
  sc = rtems_interrupt_vector_is_enabled( ctx->vector, &enabled );
  T_rsc_success( sc );

  return enabled;
}

static bool IsPending( const Context *ctx )
{
  rtems_status_code sc;
  bool              pending;

  pending = false;
  sc = rtems_interrupt_is_pending( ctx->vector, &pending );
  T_rsc_success( sc );

  return pending;
}

static void Disable( const Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_interrupt_vector_disable( ctx->vector );
  T_rsc_success( sc );
}

static void Raise( const Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_interrupt_raise( ctx->vector );
  T_rsc_success( sc );
}

static void Clear( const Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_interrupt_clear( ctx->vector );
  T_rsc_success( sc );
}

static void EntryRoutine( void *arg )
{
  Context *ctx;
  uint32_t count;

  (void) arg;
  ctx = T_fixture_context();

  count = ctx->interrupt_count;
  ctx->interrupt_count = count + 1;

  if ( ctx->do_clear ) {
    rtems_status_code sc;

    sc = rtems_interrupt_clear( ctx->vector );
    T_rsc_success( sc );
  }

  if ( count > 2 ) {
    /* Some interrupts are probably cased by a peripheral */
    Disable( ctx );
  }
}

static void CheckUnsatisfied( const Context *ctx )
{
  rtems_status_code sc;
  bool              pending_before;
  bool              pending_after;

  pending_before = true;
  sc = rtems_interrupt_is_pending( ctx->vector, &pending_before );
  T_rsc_success( sc );

  sc = rtems_interrupt_clear( ctx->vector );
  T_rsc( sc, RTEMS_UNSATISFIED );

  pending_after = !pending_before;
  sc = rtems_interrupt_is_pending( ctx->vector, &pending_after );
  T_rsc_success( sc );

  T_eq( pending_before, pending_after );
}

static void CheckClear(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr,
  bool                              has_installed_entries
)
{
  rtems_status_code sc;

  if ( !attr->can_clear ) {
    CheckUnsatisfied( ctx );
  } else if ( has_installed_entries ) {
    /* We cannot test this vector, since it is used by a device driver */
  } else if ( !attr->is_maskable ) {
    /* We can only safely test maskable interrupts */
  } else if ( IsPending( ctx ) ) {
    /*
     * If there is already an interrupt pending, then it is probably cleard
     * by a peripheral which we cannot control.
     */
  } else if ( attr->can_disable ) {
    rtems_interrupt_entry entry;
    rtems_interrupt_level level;

    ctx->interrupt_count = 0;
    ctx->do_clear = !attr->cleared_by_acknowledge;
    rtems_interrupt_entry_initialize( &entry, EntryRoutine, ctx, "Info" );
    sc = rtems_interrupt_entry_install(
      ctx->vector,
      RTEMS_INTERRUPT_UNIQUE,
      &entry
    );
    T_rsc_success( sc );

    Clear( ctx );

    if ( !IsPending( ctx) && ( attr->can_enable || IsEnabled( ctx ) ) ) {
      T_false( IsPending( ctx ) );
      Clear( ctx );
      T_false( IsPending( ctx ) );

      if ( attr->can_disable ) {
        Disable( ctx );
        Raise( ctx );
        T_true( IsPending( ctx ) );
        Clear( ctx );
        T_false( IsPending( ctx ) );

        sc = rtems_interrupt_vector_enable( ctx->vector );
        T_rsc_success( sc );
      }

      T_false( IsPending( ctx ) );
      Clear( ctx );
      T_false( IsPending( ctx ) );

      rtems_interrupt_local_disable( level );
      Raise( ctx );
      T_true( IsPending( ctx ) );
      Clear( ctx );
      T_false( IsPending( ctx ) );
      rtems_interrupt_local_enable( level );

      T_false( IsPending( ctx ) );
      Clear( ctx );
      T_false( IsPending( ctx ) );
    }

    sc = rtems_interrupt_entry_remove( ctx->vector, &entry );
    T_rsc_success( sc );
  }
}

static void RtemsIntrReqClear_Pre_Vector_Prepare(
  RtemsIntrReqClear_Context   *ctx,
  RtemsIntrReqClear_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqClear_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqClear_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqClear_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqClear_Pre_CanClear_Prepare(
  RtemsIntrReqClear_Context     *ctx,
  RtemsIntrReqClear_Pre_CanClear state
)
{
  switch ( state ) {
    case RtemsIntrReqClear_Pre_CanClear_Yes: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * can be cleard.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckClear().
       */
      break;
    }

    case RtemsIntrReqClear_Pre_CanClear_No: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * cannot be cleard.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckClear().
       */
      break;
    }

    case RtemsIntrReqClear_Pre_CanClear_NA:
      break;
  }
}

static void RtemsIntrReqClear_Post_Status_Check(
  RtemsIntrReqClear_Context    *ctx,
  RtemsIntrReqClear_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqClear_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_clear() shall be
       * RTEMS_SUCCESSFUL.
       */
      /* Validation is done by CheckClear() for each interrupt vector */
      break;
    }

    case RtemsIntrReqClear_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_clear() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqClear_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_clear() shall be
       * RTEMS_UNSATISFIED.
       */
      /* Validation is done by CheckClear() for each interrupt vector */
      break;
    }

    case RtemsIntrReqClear_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqClear_Post_Cleared_Check(
  RtemsIntrReqClear_Context     *ctx,
  RtemsIntrReqClear_Post_Cleared state
)
{
  switch ( state ) {
    case RtemsIntrReqClear_Post_Cleared_Yes: {
      /*
       * The pending state of the interrupt associated with the interrupt
       * vector specified by ``vector`` shall be cleared for the processor
       * executing the rtems_interrupt_clear() call at some time point during
       * the call.
       */
      /* Validation is done by CheckClear() for each interrupt vector */
      break;
    }

    case RtemsIntrReqClear_Post_Cleared_No: {
      /*
       * The pending state of the interrupt associated with the interrupt
       * vector specified by ``vector`` shall not be cleared by the
       * rtems_interrupt_clear() call.
       */
      /* Validation is done by CheckClear() for each interrupt vector */
      break;
    }

    case RtemsIntrReqClear_Post_Cleared_NA:
      break;
  }
}

static void RtemsIntrReqClear_Action( RtemsIntrReqClear_Context *ctx )
{
  if ( ctx->valid_vector ) {
    for (
      ctx->vector = 0;
      ctx->vector < BSP_INTERRUPT_VECTOR_COUNT;
      ++ctx->vector
    ) {
      rtems_status_code          sc;
      rtems_interrupt_attributes attr;
      bool                       has_installed_entries;

      memset( &attr, 0, sizeof( attr ) );
      sc = rtems_interrupt_get_attributes( ctx->vector, &attr );

      if ( sc == RTEMS_INVALID_ID ) {
        continue;
      }

      T_rsc_success( sc );

      has_installed_entries = HasInterruptVectorEntriesInstalled( ctx->vector );
      CheckClear( ctx, &attr, has_installed_entries );
    }
  } else {
    ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    ctx->status = rtems_interrupt_clear( ctx->vector );
  }
}

static const RtemsIntrReqClear_Entry
RtemsIntrReqClear_Entries[] = {
  { 0, 0, 1, RtemsIntrReqClear_Post_Status_InvId,
    RtemsIntrReqClear_Post_Cleared_NA },
  { 0, 0, 0, RtemsIntrReqClear_Post_Status_Ok,
    RtemsIntrReqClear_Post_Cleared_Yes },
  { 0, 0, 0, RtemsIntrReqClear_Post_Status_Unsat,
    RtemsIntrReqClear_Post_Cleared_No }
};

static const uint8_t
RtemsIntrReqClear_Map[] = {
  1, 2, 0, 0
};

static size_t RtemsIntrReqClear_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqClear_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsIntrReqClear_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsIntrReqClear_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqClear_Scope,
  .initial_context = &RtemsIntrReqClear_Instance
};

static inline RtemsIntrReqClear_Entry RtemsIntrReqClear_PopEntry(
  RtemsIntrReqClear_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqClear_Entries[
    RtemsIntrReqClear_Map[ index ]
  ];
}

static void RtemsIntrReqClear_SetPreConditionStates(
  RtemsIntrReqClear_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_CanClear_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsIntrReqClear_Pre_CanClear_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void RtemsIntrReqClear_TestVariant( RtemsIntrReqClear_Context *ctx )
{
  RtemsIntrReqClear_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqClear_Pre_CanClear_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqClear_Action( ctx );
  RtemsIntrReqClear_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsIntrReqClear_Post_Cleared_Check( ctx, ctx->Map.entry.Post_Cleared );
}

/**
 * @fn void T_case_body_RtemsIntrReqClear( void )
 */
T_TEST_CASE_FIXTURE( RtemsIntrReqClear, &RtemsIntrReqClear_Fixture )
{
  RtemsIntrReqClear_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqClear_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqClear_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqClear_Pre_CanClear_Yes;
      ctx->Map.pci[ 1 ] < RtemsIntrReqClear_Pre_CanClear_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = RtemsIntrReqClear_PopEntry( ctx );
      RtemsIntrReqClear_SetPreConditionStates( ctx );
      RtemsIntrReqClear_TestVariant( ctx );
    }
  }
}

/** @} */
