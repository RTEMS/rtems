/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqVectorEnable
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
 * @defgroup RtemsIntrReqVectorEnable spec:/rtems/intr/req/vector-enable
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqVectorEnable_Pre_Vector_Valid,
  RtemsIntrReqVectorEnable_Pre_Vector_Invalid,
  RtemsIntrReqVectorEnable_Pre_Vector_NA
} RtemsIntrReqVectorEnable_Pre_Vector;

typedef enum {
  RtemsIntrReqVectorEnable_Pre_IsEnabled_Yes,
  RtemsIntrReqVectorEnable_Pre_IsEnabled_No,
  RtemsIntrReqVectorEnable_Pre_IsEnabled_NA
} RtemsIntrReqVectorEnable_Pre_IsEnabled;

typedef enum {
  RtemsIntrReqVectorEnable_Pre_CanEnable_Yes,
  RtemsIntrReqVectorEnable_Pre_CanEnable_Maybe,
  RtemsIntrReqVectorEnable_Pre_CanEnable_No,
  RtemsIntrReqVectorEnable_Pre_CanEnable_NA
} RtemsIntrReqVectorEnable_Pre_CanEnable;

typedef enum {
  RtemsIntrReqVectorEnable_Post_Status_Ok,
  RtemsIntrReqVectorEnable_Post_Status_InvId,
  RtemsIntrReqVectorEnable_Post_Status_Unsat,
  RtemsIntrReqVectorEnable_Post_Status_NA
} RtemsIntrReqVectorEnable_Post_Status;

typedef enum {
  RtemsIntrReqVectorEnable_Post_IsEnabled_Nop,
  RtemsIntrReqVectorEnable_Post_IsEnabled_Yes,
  RtemsIntrReqVectorEnable_Post_IsEnabled_Maybe,
  RtemsIntrReqVectorEnable_Post_IsEnabled_NA
} RtemsIntrReqVectorEnable_Post_IsEnabled;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Vector_NA : 1;
  uint8_t Pre_IsEnabled_NA : 1;
  uint8_t Pre_CanEnable_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_IsEnabled : 2;
} RtemsIntrReqVectorEnable_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/vector-enable test case.
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
   * @brief This member may contain the return value of the
   *   rtems_interrupt_vector_enable() call.
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
    RtemsIntrReqVectorEnable_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqVectorEnable_Context;

static RtemsIntrReqVectorEnable_Context
  RtemsIntrReqVectorEnable_Instance;

static const char * const RtemsIntrReqVectorEnable_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqVectorEnable_PreDesc_IsEnabled[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const RtemsIntrReqVectorEnable_PreDesc_CanEnable[] = {
  "Yes",
  "Maybe",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqVectorEnable_PreDesc[] = {
  RtemsIntrReqVectorEnable_PreDesc_Vector,
  RtemsIntrReqVectorEnable_PreDesc_IsEnabled,
  RtemsIntrReqVectorEnable_PreDesc_CanEnable,
  NULL
};

typedef RtemsIntrReqVectorEnable_Context Context;

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

  sc = rtems_interrupt_vector_enable( ctx->vector );
  T_rsc( sc, RTEMS_UNSATISFIED );

  enabled_after = !enabled_before;
  sc = rtems_interrupt_vector_is_enabled( ctx->vector, &enabled_after );
  T_rsc_success( sc );

  T_eq( enabled_before, enabled_after );
}

static void CheckVectorEnable(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr,
  bool                              has_installed_entries
)
{
  rtems_status_code sc;

  if ( !attr->maybe_enable ) {
    CheckUnsatisfied( ctx );
  } else if ( has_installed_entries ) {
    T_true( IsEnabled( ctx ) );

    if ( attr->can_disable ) {
      Disable( ctx );
      T_false( IsEnabled( ctx ) );
    }

    Enable( ctx );
    T_true( IsEnabled( ctx ) );
  } else if ( attr->is_maskable && attr->can_disable ) {
    rtems_interrupt_entry entry;
    bool                  enabled;

    ctx->interrupt_occurred = false;
    rtems_interrupt_entry_initialize( &entry, EntryRoutine, ctx, "Info" );
    sc = rtems_interrupt_entry_install(
      ctx->vector,
      RTEMS_INTERRUPT_UNIQUE,
      &entry
    );
    T_rsc_success( sc );

    if ( attr->can_disable ) {
      Disable( ctx );
      T_false( IsEnabled( ctx ) );
    }

    Enable( ctx );
    Enable( ctx );

    enabled = false;
    sc = rtems_interrupt_vector_is_enabled( ctx->vector, &enabled );
    T_rsc_success( sc );
    T_true( enabled || ctx->interrupt_occurred || attr->maybe_enable );

    sc = rtems_interrupt_entry_remove( ctx->vector, &entry );
    T_rsc_success( sc );

    T_true( !attr->can_disable || !IsEnabled( ctx ) );
  }
}

static void RtemsIntrReqVectorEnable_Pre_Vector_Prepare(
  RtemsIntrReqVectorEnable_Context   *ctx,
  RtemsIntrReqVectorEnable_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorEnable_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqVectorEnable_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqVectorEnable_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqVectorEnable_Pre_IsEnabled_Prepare(
  RtemsIntrReqVectorEnable_Context      *ctx,
  RtemsIntrReqVectorEnable_Pre_IsEnabled state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorEnable_Pre_IsEnabled_Yes: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter is
       * enabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorEnable().
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Pre_IsEnabled_No: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter is
       * disabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorEnable().
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Pre_IsEnabled_NA:
      break;
  }
}

static void RtemsIntrReqVectorEnable_Pre_CanEnable_Prepare(
  RtemsIntrReqVectorEnable_Context      *ctx,
  RtemsIntrReqVectorEnable_Pre_CanEnable state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorEnable_Pre_CanEnable_Yes: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * can be enabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorEnable().
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Pre_CanEnable_Maybe: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * may be enabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorEnable().
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Pre_CanEnable_No: {
      /*
       * While the interrupt vector associated with the ``vector`` parameter
       * cannot be enabled.
       */
      /*
       * This pre-condition depends on the attributes of an interrupt vector,
       * see CheckVectorEnable().
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Pre_CanEnable_NA:
      break;
  }
}

static void RtemsIntrReqVectorEnable_Post_Status_Check(
  RtemsIntrReqVectorEnable_Context    *ctx,
  RtemsIntrReqVectorEnable_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorEnable_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_vector_enable() shall be
       * RTEMS_SUCCESSFUL.
       */
      /*
       * Validation is done by CheckVectorEnable() for each interrupt
       * vector.
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_vector_enable() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqVectorEnable_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_vector_enable() shall be
       * RTEMS_UNSATISFIED.
       */
      /*
       * Validation is done by CheckVectorEnable() for each interrupt
       * vector.
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqVectorEnable_Post_IsEnabled_Check(
  RtemsIntrReqVectorEnable_Context       *ctx,
  RtemsIntrReqVectorEnable_Post_IsEnabled state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorEnable_Post_IsEnabled_Nop: {
      /*
       * The enabled status of the interrupt vector specified by ``vector``
       * shall not be modified by the rtems_interrupt_vector_enable() call.
       */
      /*
       * Validation is done by CheckUnsatisfied() for each interrupt
       * vector which cannot be enabled.
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Post_IsEnabled_Yes: {
      /*
       * The interrupt vector specified by ``vector`` shall be enabled.
       */
      /*
       * Validation is done by CheckVectorEnable() for each interrupt
       * vector.
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Post_IsEnabled_Maybe: {
      /*
       * The interrupt vector specified by ``vector`` may be enabled.
       */
      /*
       * Validation is done by CheckVectorEnable() for each interrupt
       * vector.
       */
      break;
    }

    case RtemsIntrReqVectorEnable_Post_IsEnabled_NA:
      break;
  }
}

static void RtemsIntrReqVectorEnable_Action(
  RtemsIntrReqVectorEnable_Context *ctx
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
      CheckVectorEnable( ctx, &attr, has_installed_entries );
    }
  } else {
    ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    ctx->status = rtems_interrupt_vector_enable( ctx->vector );
  }
}

static const RtemsIntrReqVectorEnable_Entry
RtemsIntrReqVectorEnable_Entries[] = {
  { 0, 0, 1, 1, RtemsIntrReqVectorEnable_Post_Status_InvId,
    RtemsIntrReqVectorEnable_Post_IsEnabled_NA },
  { 0, 0, 0, 0, RtemsIntrReqVectorEnable_Post_Status_Ok,
    RtemsIntrReqVectorEnable_Post_IsEnabled_Yes },
  { 0, 0, 0, 0, RtemsIntrReqVectorEnable_Post_Status_Ok,
    RtemsIntrReqVectorEnable_Post_IsEnabled_Maybe },
  { 0, 0, 0, 0, RtemsIntrReqVectorEnable_Post_Status_Unsat,
    RtemsIntrReqVectorEnable_Post_IsEnabled_Nop }
};

static const uint8_t
RtemsIntrReqVectorEnable_Map[] = {
  1, 2, 3, 1, 2, 3, 0, 0, 0, 0, 0, 0
};

static size_t RtemsIntrReqVectorEnable_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqVectorEnable_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqVectorEnable_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqVectorEnable_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqVectorEnable_Scope,
  .initial_context = &RtemsIntrReqVectorEnable_Instance
};

static inline RtemsIntrReqVectorEnable_Entry RtemsIntrReqVectorEnable_PopEntry(
  RtemsIntrReqVectorEnable_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqVectorEnable_Entries[
    RtemsIntrReqVectorEnable_Map[ index ]
  ];
}

static void RtemsIntrReqVectorEnable_SetPreConditionStates(
  RtemsIntrReqVectorEnable_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_IsEnabled_NA ) {
    ctx->Map.pcs[ 1 ] = RtemsIntrReqVectorEnable_Pre_IsEnabled_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }

  if ( ctx->Map.entry.Pre_CanEnable_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsIntrReqVectorEnable_Pre_CanEnable_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void RtemsIntrReqVectorEnable_TestVariant(
  RtemsIntrReqVectorEnable_Context *ctx
)
{
  RtemsIntrReqVectorEnable_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqVectorEnable_Pre_IsEnabled_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqVectorEnable_Pre_CanEnable_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqVectorEnable_Action( ctx );
  RtemsIntrReqVectorEnable_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsIntrReqVectorEnable_Post_IsEnabled_Check(
    ctx,
    ctx->Map.entry.Post_IsEnabled
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqVectorEnable( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqVectorEnable,
  &RtemsIntrReqVectorEnable_Fixture
)
{
  RtemsIntrReqVectorEnable_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqVectorEnable_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqVectorEnable_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqVectorEnable_Pre_IsEnabled_Yes;
      ctx->Map.pci[ 1 ] < RtemsIntrReqVectorEnable_Pre_IsEnabled_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqVectorEnable_Pre_CanEnable_Yes;
        ctx->Map.pci[ 2 ] < RtemsIntrReqVectorEnable_Pre_CanEnable_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqVectorEnable_PopEntry( ctx );
        RtemsIntrReqVectorEnable_SetPreConditionStates( ctx );
        RtemsIntrReqVectorEnable_TestVariant( ctx );
      }
    }
  }
}

/** @} */
