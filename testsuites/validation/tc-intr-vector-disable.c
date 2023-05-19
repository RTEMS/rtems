/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqVectorDisable
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
 * @defgroup RtemsIntrReqVectorDisable spec:/rtems/intr/req/vector-disable
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqVectorDisable_Pre_Vector_Valid,
  RtemsIntrReqVectorDisable_Pre_Vector_Invalid,
  RtemsIntrReqVectorDisable_Pre_Vector_NA
} RtemsIntrReqVectorDisable_Pre_Vector;

typedef enum {
  RtemsIntrReqVectorDisable_Pre_IsEnabled_Yes,
  RtemsIntrReqVectorDisable_Pre_IsEnabled_No,
  RtemsIntrReqVectorDisable_Pre_IsEnabled_NA
} RtemsIntrReqVectorDisable_Pre_IsEnabled;

typedef enum {
  RtemsIntrReqVectorDisable_Pre_CanDisable_Yes,
  RtemsIntrReqVectorDisable_Pre_CanDisable_Maybe,
  RtemsIntrReqVectorDisable_Pre_CanDisable_No,
  RtemsIntrReqVectorDisable_Pre_CanDisable_NA
} RtemsIntrReqVectorDisable_Pre_CanDisable;

typedef enum {
  RtemsIntrReqVectorDisable_Post_Status_Ok,
  RtemsIntrReqVectorDisable_Post_Status_InvId,
  RtemsIntrReqVectorDisable_Post_Status_Unsat,
  RtemsIntrReqVectorDisable_Post_Status_NA
} RtemsIntrReqVectorDisable_Post_Status;

typedef enum {
  RtemsIntrReqVectorDisable_Post_IsEnabled_Nop,
  RtemsIntrReqVectorDisable_Post_IsEnabled_No,
  RtemsIntrReqVectorDisable_Post_IsEnabled_Maybe,
  RtemsIntrReqVectorDisable_Post_IsEnabled_NA
} RtemsIntrReqVectorDisable_Post_IsEnabled;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Vector_NA : 1;
  uint8_t Pre_IsEnabled_NA : 1;
  uint8_t Pre_CanDisable_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_IsEnabled : 2;
} RtemsIntrReqVectorDisable_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/vector-disable test case.
 */
typedef struct {
  /**
   * @brief If this member is true, then an interrupt occurred.
   */
  bool interrupt_occurred;

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
   *   rtems_interrupt_vector_disable() call.
   */
  rtems_status_code status;

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
    RtemsIntrReqVectorDisable_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqVectorDisable_Context;

static RtemsIntrReqVectorDisable_Context
  RtemsIntrReqVectorDisable_Instance;

static const char * const RtemsIntrReqVectorDisable_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqVectorDisable_PreDesc_IsEnabled[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsIntrReqVectorDisable_PreDesc_CanDisable[] = {
  "Yes",
  "Maybe",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqVectorDisable_PreDesc[] = {
  RtemsIntrReqVectorDisable_PreDesc_Vector,
  RtemsIntrReqVectorDisable_PreDesc_IsEnabled,
  RtemsIntrReqVectorDisable_PreDesc_CanDisable,
  NULL
};

typedef RtemsIntrReqVectorDisable_Context Context;

static bool IsEnabled( const Context *ctx )
{
  rtems_status_code sc;
  bool              enabled;

  enabled = false;
  sc = rtems_interrupt_vector_is_enabled( ctx->vector, &enabled );
  T_rsc_success( sc );

  return enabled;
}

static void Enable( const Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_interrupt_vector_enable( ctx->vector );
  T_rsc_success( sc );
}

static void Disable( const Context *ctx )
{
  rtems_status_code sc;

  sc = rtems_interrupt_vector_disable( ctx->vector );
  T_rsc_success( sc );
}

static void EntryRoutine( void *arg )
{
  Context *ctx;

  (void) arg;
  ctx = T_fixture_context();

  T_true( IsEnabled( ctx ) );
  Enable( ctx );
  T_true( IsEnabled( ctx ) );
  Disable( ctx );
  T_false( IsEnabled( ctx ) );
  ctx->interrupt_occurred = true;
}

static void CheckUnsatisfied( const Context *ctx )
{
  rtems_status_code sc;
  bool              enabled_before;
  bool              enabled_after;

  enabled_before = true;
  sc = rtems_interrupt_vector_is_enabled( ctx->vector, &enabled_before );
  T_rsc_success( sc );

  sc = rtems_interrupt_vector_disable( ctx->vector );
  T_rsc( sc, RTEMS_UNSATISFIED );

  enabled_after = !enabled_before;
  sc = rtems_interrupt_vector_is_enabled( ctx->vector, &enabled_after );
  T_rsc_success( sc );

  T_eq( enabled_before, enabled_after );
}

static void CheckVectorDisable(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr,
  bool                              has_installed_entries
)
{
  rtems_status_code sc;

  if ( !attr->maybe_disable ) {
    CheckUnsatisfied( ctx );
  } else if ( has_installed_entries ) {
    T_true( IsEnabled( ctx ) );
    Disable( ctx );
    T_true( !attr->can_disable || !IsEnabled( ctx ) );
    Enable( ctx );
    T_true( IsEnabled( ctx ) );
  } else if ( attr->is_maskable && attr->maybe_enable && attr->can_disable ) {
    rtems_interrupt_entry entry;

    ctx->interrupt_occurred = false;
    rtems_interrupt_entry_initialize( &entry, EntryRoutine, ctx, "Info" );
    sc = rtems_interrupt_entry_install(
      ctx->vector,
      RTEMS_INTERRUPT_UNIQUE,
      &entry
    );
    T_rsc_success( sc );

    if ( IsEnabled( ctx ) ) {
      bool enabled;

      Disable( ctx );
      T_false( IsEnabled( ctx ) );
      Enable( ctx );

      enabled = false;
      sc = rtems_interrupt_vector_is_enabled( ctx->vector, &enabled );
      T_rsc_success( sc );
      T_true( enabled || ctx->interrupt_occurred );
    }

    sc = rtems_interrupt_entry_remove( ctx->vector, &entry );
    T_rsc_success( sc );

    T_false( IsEnabled( ctx ) );
  }
}

static void RtemsIntrReqVectorDisable_Pre_Vector_Prepare(
  RtemsIntrReqVectorDisable_Context   *ctx,
  RtemsIntrReqVectorDisable_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorDisable_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqVectorDisable_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqVectorDisable_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqVectorDisable_Pre_IsEnabled_Prepare(
  RtemsIntrReqVectorDisable_Context      *ctx,
  RtemsIntrReqVectorDisable_Pre_IsEnabled state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorDisable_Pre_IsEnabled_Yes: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter is
       * enabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorDisable().
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Pre_IsEnabled_No: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter is
       * disabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorDisable().
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Pre_IsEnabled_NA:
      break;
  }
}

static void RtemsIntrReqVectorDisable_Pre_CanDisable_Prepare(
  RtemsIntrReqVectorDisable_Context       *ctx,
  RtemsIntrReqVectorDisable_Pre_CanDisable state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorDisable_Pre_CanDisable_Yes: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * can be disabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorDisable().
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Pre_CanDisable_Maybe: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * may be disabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorDisable().
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Pre_CanDisable_No: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * cannot be disabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorDisable().
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Pre_CanDisable_NA:
      break;
  }
}

static void RtemsIntrReqVectorDisable_Post_Status_Check(
  RtemsIntrReqVectorDisable_Context    *ctx,
  RtemsIntrReqVectorDisable_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorDisable_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_vector_disable() shall be
       * RTEMS_SUCCESSFUL.
       */
      /*
       * Validation is done by CheckVectorDisable() for each interrupt
       * vector.
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_vector_disable() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqVectorDisable_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_vector_disable() shall be
       * RTEMS_UNSATISFIED.
       */
      /*
       * Validation is done by CheckVectorDisable() for each interrupt
       * vector.
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqVectorDisable_Post_IsEnabled_Check(
  RtemsIntrReqVectorDisable_Context       *ctx,
  RtemsIntrReqVectorDisable_Post_IsEnabled state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorDisable_Post_IsEnabled_Nop: {
      /*
       * The enabled status of the interrupt vector specified by ``vector``
       * shall not be modified by the rtems_interrupt_vector_disable() call.
       */
      /*
       * Validation is done by CheckUnsatisfied() for each interrupt
       * vector which cannot be disabled.
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Post_IsEnabled_No: {
      /*
       * The interrupt vector specified by ``vector`` shall be disabled.
       */
      /*
       * Validation is done by CheckVectorDisable() for each interrupt
       * vector.
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Post_IsEnabled_Maybe: {
      /*
       * The interrupt vector specified by ``vector`` may be disabled.
       */
      /*
       * Validation is done by CheckVectorDisable() for each interrupt
       * vector.
       */
      break;
    }

    case RtemsIntrReqVectorDisable_Post_IsEnabled_NA:
      break;
  }
}

static void RtemsIntrReqVectorDisable_Action(
  RtemsIntrReqVectorDisable_Context *ctx
)
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
      CheckVectorDisable( ctx, &attr, has_installed_entries );
    }
  } else {
    ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    ctx->status = rtems_interrupt_vector_disable( ctx->vector );
  }
}

static const RtemsIntrReqVectorDisable_Entry
RtemsIntrReqVectorDisable_Entries[] = {
  { 0, 0, 1, 1, RtemsIntrReqVectorDisable_Post_Status_InvId,
    RtemsIntrReqVectorDisable_Post_IsEnabled_Nop },
  { 0, 0, 0, 0, RtemsIntrReqVectorDisable_Post_Status_Ok,
    RtemsIntrReqVectorDisable_Post_IsEnabled_No },
  { 0, 0, 0, 0, RtemsIntrReqVectorDisable_Post_Status_Ok,
    RtemsIntrReqVectorDisable_Post_IsEnabled_Maybe },
  { 0, 0, 0, 0, RtemsIntrReqVectorDisable_Post_Status_Unsat,
    RtemsIntrReqVectorDisable_Post_IsEnabled_Nop }
};

static const uint8_t
RtemsIntrReqVectorDisable_Map[] = {
  1, 2, 3, 1, 2, 3, 0, 0, 0, 0, 0, 0
};

static size_t RtemsIntrReqVectorDisable_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqVectorDisable_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqVectorDisable_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqVectorDisable_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqVectorDisable_Scope,
  .initial_context = &RtemsIntrReqVectorDisable_Instance
};

static inline RtemsIntrReqVectorDisable_Entry
RtemsIntrReqVectorDisable_PopEntry( RtemsIntrReqVectorDisable_Context *ctx )
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqVectorDisable_Entries[
    RtemsIntrReqVectorDisable_Map[ index ]
  ];
}

static void RtemsIntrReqVectorDisable_SetPreConditionStates(
  RtemsIntrReqVectorDisable_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_IsEnabled_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsIntrReqVectorDisable_Pre_IsEnabled_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_CanDisable_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsIntrReqVectorDisable_Pre_CanDisable_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void RtemsIntrReqVectorDisable_TestVariant(
  RtemsIntrReqVectorDisable_Context *ctx
)
{
  RtemsIntrReqVectorDisable_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqVectorDisable_Pre_IsEnabled_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqVectorDisable_Pre_CanDisable_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqVectorDisable_Action( ctx );
  RtemsIntrReqVectorDisable_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsIntrReqVectorDisable_Post_IsEnabled_Check(
    ctx,
    ctx->Map.entry.Post_IsEnabled
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqVectorDisable( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqVectorDisable,
  &RtemsIntrReqVectorDisable_Fixture
)
{
  RtemsIntrReqVectorDisable_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqVectorDisable_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqVectorDisable_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqVectorDisable_Pre_IsEnabled_Yes;
      ctx->Map.pci[ 1 ] < RtemsIntrReqVectorDisable_Pre_IsEnabled_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqVectorDisable_Pre_CanDisable_Yes;
        ctx->Map.pci[ 2 ] < RtemsIntrReqVectorDisable_Pre_CanDisable_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqVectorDisable_PopEntry( ctx );
        RtemsIntrReqVectorDisable_SetPreConditionStates( ctx );
        RtemsIntrReqVectorDisable_TestVariant( ctx );
      }
    }
  }
}

/** @} */
