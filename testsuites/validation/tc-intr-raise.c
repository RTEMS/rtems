/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqRaise
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
 * @defgroup RtemsIntrReqRaise spec:/rtems/intr/req/raise
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqRaise_Pre_Vector_Valid,
  RtemsIntrReqRaise_Pre_Vector_Invalid,
  RtemsIntrReqRaise_Pre_Vector_NA
} RtemsIntrReqRaise_Pre_Vector;

typedef enum {
  RtemsIntrReqRaise_Pre_CanRaise_Yes,
  RtemsIntrReqRaise_Pre_CanRaise_No,
  RtemsIntrReqRaise_Pre_CanRaise_NA
} RtemsIntrReqRaise_Pre_CanRaise;

typedef enum {
  RtemsIntrReqRaise_Post_Status_Ok,
  RtemsIntrReqRaise_Post_Status_InvId,
  RtemsIntrReqRaise_Post_Status_Unsat,
  RtemsIntrReqRaise_Post_Status_NA
} RtemsIntrReqRaise_Post_Status;

typedef enum {
  RtemsIntrReqRaise_Post_Pending_Yes,
  RtemsIntrReqRaise_Post_Pending_No,
  RtemsIntrReqRaise_Post_Pending_NA
} RtemsIntrReqRaise_Post_Pending;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Vector_NA : 1;
  uint8_t Pre_CanRaise_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Pending : 2;
} RtemsIntrReqRaise_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/raise test case.
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
   *   rtems_interrupt_raise() call.
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
    RtemsIntrReqRaise_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqRaise_Context;

static RtemsIntrReqRaise_Context
  RtemsIntrReqRaise_Instance;

static const char * const RtemsIntrReqRaise_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqRaise_PreDesc_CanRaise[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqRaise_PreDesc[] = {
  RtemsIntrReqRaise_PreDesc_Vector,
  RtemsIntrReqRaise_PreDesc_CanRaise,
  NULL
};

typedef RtemsIntrReqRaise_Context Context;

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

  sc = rtems_interrupt_raise( ctx->vector );
  T_rsc( sc, RTEMS_UNSATISFIED );

  pending_after = !pending_before;
  sc = rtems_interrupt_is_pending( ctx->vector, &pending_after );
  T_rsc_success( sc );

  T_eq( pending_before, pending_after );
}

static void CheckRaise(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr,
  bool                              has_installed_entries
)
{
  rtems_status_code sc;

  if ( !attr->can_raise ) {
    CheckUnsatisfied( ctx );
  } else if ( has_installed_entries ) {
    /* We cannot test this vector, since it is used by a device driver */
  } else if ( !attr->is_maskable ) {
    /* We can only safely test maskable interrupts */
  } else if ( IsPending( ctx ) ) {
    /*
     * If there is already an interrupt pending, then it is probably raised
     * by a peripheral which we cannot control.
     */
  } else if (
    attr->can_disable && ( attr->can_clear || attr->cleared_by_acknowledge )
  ) {
    rtems_interrupt_entry entry;
    rtems_interrupt_level level;

    ctx->interrupt_count = 0;
    ctx->do_clear = attr->can_clear && !attr->cleared_by_acknowledge;
    rtems_interrupt_entry_initialize( &entry, EntryRoutine, ctx, "Info" );
    sc = rtems_interrupt_entry_install(
      ctx->vector,
      RTEMS_INTERRUPT_UNIQUE,
      &entry
    );
    T_rsc_success( sc );

    if ( !IsPending( ctx) && ( attr->can_enable || IsEnabled( ctx ) ) ) {
      Disable( ctx );
      Raise( ctx );

      /*
       * Some interrupt controllers will signal a pending interrupt if it is
       * disabled (for example ARM GIC), others will not signal a pending
       * interrupt if it is disabled (for example Freescale/NXP MPIC).
       */
      (void) IsPending( ctx );

      sc = rtems_interrupt_vector_enable( ctx->vector );
      T_rsc_success( sc );

      while ( ctx->interrupt_count < 1 ) {
        /* Wait */
      }

      T_false( IsPending( ctx ) );

      rtems_interrupt_local_disable( level );
      Raise( ctx );
      T_true( IsPending( ctx ) );
      rtems_interrupt_local_enable( level );

      while ( ctx->interrupt_count < 2 ) {
        /* Wait */
      }

      T_false( IsPending( ctx ) );
    }

    sc = rtems_interrupt_entry_remove( ctx->vector, &entry );
    T_rsc_success( sc );
  }
}

static void RtemsIntrReqRaise_Pre_Vector_Prepare(
  RtemsIntrReqRaise_Context   *ctx,
  RtemsIntrReqRaise_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqRaise_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqRaise_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqRaise_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqRaise_Pre_CanRaise_Prepare(
  RtemsIntrReqRaise_Context     *ctx,
  RtemsIntrReqRaise_Pre_CanRaise state
)
{
  switch ( state ) {
    case RtemsIntrReqRaise_Pre_CanRaise_Yes: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * can be raised.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckRaise().
       */
      break;
    }

    case RtemsIntrReqRaise_Pre_CanRaise_No: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * cannot be raised.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckRaise().
       */
      break;
    }

    case RtemsIntrReqRaise_Pre_CanRaise_NA:
      break;
  }
}

static void RtemsIntrReqRaise_Post_Status_Check(
  RtemsIntrReqRaise_Context    *ctx,
  RtemsIntrReqRaise_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqRaise_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_raise() shall be
       * RTEMS_SUCCESSFUL.
       */
      /* Validation is done by CheckRaise() for each interrupt vector */
      break;
    }

    case RtemsIntrReqRaise_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_raise() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqRaise_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_raise() shall be
       * RTEMS_UNSATISFIED.
       */
      /* Validation is done by CheckRaise() for each interrupt vector */
      break;
    }

    case RtemsIntrReqRaise_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqRaise_Post_Pending_Check(
  RtemsIntrReqRaise_Context     *ctx,
  RtemsIntrReqRaise_Post_Pending state
)
{
  switch ( state ) {
    case RtemsIntrReqRaise_Post_Pending_Yes: {
      /*
       * The interrupt associated with the interrupt vector specified by
       * ``vector`` shall be made pending by the rtems_interrupt_raise() call.
       */
      /* Validation is done by CheckRaise() for each interrupt vector */
      break;
    }

    case RtemsIntrReqRaise_Post_Pending_No: {
      /*
       * The interrupt associated with the interrupt vector specified by
       * ``vector`` shall not be made pending by the rtems_interrupt_raise()
       * call.
       */
      /* Validation is done by CheckRaise() for each interrupt vector */
      break;
    }

    case RtemsIntrReqRaise_Post_Pending_NA:
      break;
  }
}

static void RtemsIntrReqRaise_Action( RtemsIntrReqRaise_Context *ctx )
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
      CheckRaise( ctx, &attr, has_installed_entries );
    }
  } else {
    ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    ctx->status = rtems_interrupt_raise( ctx->vector );
  }
}

static const RtemsIntrReqRaise_Entry
RtemsIntrReqRaise_Entries[] = {
  { 0, 0, 1, RtemsIntrReqRaise_Post_Status_InvId,
    RtemsIntrReqRaise_Post_Pending_NA },
  { 0, 0, 0, RtemsIntrReqRaise_Post_Status_Ok,
    RtemsIntrReqRaise_Post_Pending_Yes },
  { 0, 0, 0, RtemsIntrReqRaise_Post_Status_Unsat,
    RtemsIntrReqRaise_Post_Pending_No }
};

static const uint8_t
RtemsIntrReqRaise_Map[] = {
  1, 2, 0, 0
};

static size_t RtemsIntrReqRaise_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqRaise_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsIntrReqRaise_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsIntrReqRaise_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqRaise_Scope,
  .initial_context = &RtemsIntrReqRaise_Instance
};

static inline RtemsIntrReqRaise_Entry RtemsIntrReqRaise_PopEntry(
  RtemsIntrReqRaise_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqRaise_Entries[
    RtemsIntrReqRaise_Map[ index ]
  ];
}

static void RtemsIntrReqRaise_SetPreConditionStates(
  RtemsIntrReqRaise_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_CanRaise_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsIntrReqRaise_Pre_CanRaise_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void RtemsIntrReqRaise_TestVariant( RtemsIntrReqRaise_Context *ctx )
{
  RtemsIntrReqRaise_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqRaise_Pre_CanRaise_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqRaise_Action( ctx );
  RtemsIntrReqRaise_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsIntrReqRaise_Post_Pending_Check( ctx, ctx->Map.entry.Post_Pending );
}

/**
 * @fn void T_case_body_RtemsIntrReqRaise( void )
 */
T_TEST_CASE_FIXTURE( RtemsIntrReqRaise, &RtemsIntrReqRaise_Fixture )
{
  RtemsIntrReqRaise_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqRaise_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqRaise_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqRaise_Pre_CanRaise_Yes;
      ctx->Map.pci[ 1 ] < RtemsIntrReqRaise_Pre_CanRaise_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = RtemsIntrReqRaise_PopEntry( ctx );
      RtemsIntrReqRaise_SetPreConditionStates( ctx );
      RtemsIntrReqRaise_TestVariant( ctx );
    }
  }
}

/** @} */
