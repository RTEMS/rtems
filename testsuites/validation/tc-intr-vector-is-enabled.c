/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqVectorIsEnabled
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
 * @defgroup RtemsIntrReqVectorIsEnabled spec:/rtems/intr/req/vector-is-enabled
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqVectorIsEnabled_Pre_Vector_Valid,
  RtemsIntrReqVectorIsEnabled_Pre_Vector_Invalid,
  RtemsIntrReqVectorIsEnabled_Pre_Vector_NA
} RtemsIntrReqVectorIsEnabled_Pre_Vector;

typedef enum {
  RtemsIntrReqVectorIsEnabled_Pre_Enabled_Obj,
  RtemsIntrReqVectorIsEnabled_Pre_Enabled_Null,
  RtemsIntrReqVectorIsEnabled_Pre_Enabled_NA
} RtemsIntrReqVectorIsEnabled_Pre_Enabled;

typedef enum {
  RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_Yes,
  RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_No,
  RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_NA
} RtemsIntrReqVectorIsEnabled_Pre_IsEnabled;

typedef enum {
  RtemsIntrReqVectorIsEnabled_Post_Status_Ok,
  RtemsIntrReqVectorIsEnabled_Post_Status_InvAddr,
  RtemsIntrReqVectorIsEnabled_Post_Status_InvId,
  RtemsIntrReqVectorIsEnabled_Post_Status_NA
} RtemsIntrReqVectorIsEnabled_Post_Status;

typedef enum {
  RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Nop,
  RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Yes,
  RtemsIntrReqVectorIsEnabled_Post_IsEnabled_No,
  RtemsIntrReqVectorIsEnabled_Post_IsEnabled_NA
} RtemsIntrReqVectorIsEnabled_Post_IsEnabled;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Vector_NA : 1;
  uint8_t Pre_Enabled_NA : 1;
  uint8_t Pre_IsEnabled_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_IsEnabled : 2;
} RtemsIntrReqVectorIsEnabled_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/vector-is-enabled test case.
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
   * @brief If this member is true, then the interrupt vector shall be enabled
   *   before the rtems_interrupt_vector_is_enabled() call.
   */
  bool is_enabled;

  /**
   * @brief This member provides the ``bool`` object.
   */
  bool enabled_obj;

  /**
   * @brief If this member is true, then the ``vector`` parameter shall be
   *   valid.
   */
  bool valid_vector;

  /**
   * @brief This member specifies if the ``enabled`` parameter value.
   */
  bool *enabled;

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_vector_is_enabled() call.
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
    RtemsIntrReqVectorIsEnabled_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqVectorIsEnabled_Context;

static RtemsIntrReqVectorIsEnabled_Context
  RtemsIntrReqVectorIsEnabled_Instance;

static const char * const RtemsIntrReqVectorIsEnabled_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqVectorIsEnabled_PreDesc_Enabled[] = {
  "Obj",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqVectorIsEnabled_PreDesc_IsEnabled[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqVectorIsEnabled_PreDesc[] = {
  RtemsIntrReqVectorIsEnabled_PreDesc_Vector,
  RtemsIntrReqVectorIsEnabled_PreDesc_Enabled,
  RtemsIntrReqVectorIsEnabled_PreDesc_IsEnabled,
  NULL
};

typedef RtemsIntrReqVectorIsEnabled_Context Context;

static void CheckIsEnabled( Context *ctx, bool expected )
{
  ctx->enabled_obj = !expected;
  ctx->status = rtems_interrupt_vector_is_enabled(
    ctx->vector,
    ctx->enabled
  );
  T_rsc_success( ctx->status );
  T_eq( ctx->enabled_obj, expected );
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

  CheckIsEnabled( ctx, true );
  Disable( ctx );
  ctx->interrupt_occurred = true;
}

static void WhileIsEnabled(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr,
  bool                              has_installed_entries
)
{
  rtems_status_code sc;

  if ( has_installed_entries ) {
    CheckIsEnabled( ctx, true );
  } else if ( attr->can_enable && attr->can_disable && attr->is_maskable ) {
    rtems_interrupt_entry entry;

    ctx->interrupt_occurred = false;
    rtems_interrupt_entry_initialize( &entry, EntryRoutine, ctx, "Info" );
    sc = rtems_interrupt_entry_install(
      ctx->vector,
      RTEMS_INTERRUPT_UNIQUE,
      &entry
    );
    T_rsc_success( sc );

    ctx->enabled_obj = false;
    sc = rtems_interrupt_vector_is_enabled( ctx->vector, ctx->enabled );
    T_rsc_success( sc );
    T_true( ctx->enabled_obj || ctx->interrupt_occurred );

    sc = rtems_interrupt_entry_remove( ctx->vector, &entry );
    T_rsc_success( sc );

    CheckIsEnabled( ctx, false );
  } else {
    sc = rtems_interrupt_vector_is_enabled( ctx->vector, ctx->enabled );
    T_rsc_success( sc );
  }
}

static void WhileIsDisabled(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr,
  bool                              has_installed_entries
)
{
  if ( has_installed_entries ) {
    if ( attr->can_enable && attr->can_disable ) {
      Disable( ctx );
      CheckIsEnabled( ctx, false );
      Enable( ctx );
    }
  } else if ( attr->can_disable ) {
    CheckIsEnabled( ctx, false );
  } else {
    rtems_status_code sc;

    sc = rtems_interrupt_vector_is_enabled( ctx->vector, ctx->enabled );
    T_rsc_success( sc );
  }
}

static void RtemsIntrReqVectorIsEnabled_Pre_Vector_Prepare(
  RtemsIntrReqVectorIsEnabled_Context   *ctx,
  RtemsIntrReqVectorIsEnabled_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorIsEnabled_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqVectorIsEnabled_Pre_Enabled_Prepare(
  RtemsIntrReqVectorIsEnabled_Context    *ctx,
  RtemsIntrReqVectorIsEnabled_Pre_Enabled state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorIsEnabled_Pre_Enabled_Obj: {
      /*
       * While the ``enabled`` parameter references an object of type ``bool``.
       */
      ctx->enabled = &ctx->enabled_obj;
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Pre_Enabled_Null: {
      /*
       * While the ``enabled`` parameter is equal to NULL.
       */
      ctx->enabled = NULL;
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Pre_Enabled_NA:
      break;
  }
}

static void RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_Prepare(
  RtemsIntrReqVectorIsEnabled_Context      *ctx,
  RtemsIntrReqVectorIsEnabled_Pre_IsEnabled state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_Yes: {
      /*
       * While the interrupt associated with the interrupt vector specified by
       * ``vector`` was enabled for the processor executing the
       * rtems_interrupt_vector_is_enabled() call at some time point during the
       * call.
       */
      ctx->is_enabled = true;
      ctx->enabled_obj = false;
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_No: {
      /*
       * While the interrupt associated with the interrupt vector specified by
       * ``vector`` was disabled for the processor executing the
       * rtems_interrupt_vector_is_enabled() call at some time point during the
       * call.
       */
      ctx->is_enabled = false;
      ctx->enabled_obj = true;
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_NA:
      break;
  }
}

static void RtemsIntrReqVectorIsEnabled_Post_Status_Check(
  RtemsIntrReqVectorIsEnabled_Context    *ctx,
  RtemsIntrReqVectorIsEnabled_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorIsEnabled_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_vector_is_enabled() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_vector_is_enabled() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_vector_is_enabled() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Check(
  RtemsIntrReqVectorIsEnabled_Context       *ctx,
  RtemsIntrReqVectorIsEnabled_Post_IsEnabled state
)
{
  switch ( state ) {
    case RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Nop: {
      /*
       * Objects referenced by the ``enabled`` parameter in past calls to
       * rtems_interrupt_vector_is_enabled() shall not be accessed by the
       * rtems_interrupt_vector_is_enabled() call.
       */
      T_eq( ctx->is_enabled, !ctx->enabled_obj );
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Yes: {
      /*
       * The value of the object referenced by the ``enabled`` parameter shall
       * be set to true.
       */
      /* Validation is done by WhileIsEnabled() for each interrupt vector */
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Post_IsEnabled_No: {
      /*
       * The value of the object referenced by the ``enabled`` parameter shall
       * be set to false.
       */
      /* Validation is done by WhileIsDisabled() for each interrupt vector */
      break;
    }

    case RtemsIntrReqVectorIsEnabled_Post_IsEnabled_NA:
      break;
  }
}

static void RtemsIntrReqVectorIsEnabled_Action(
  RtemsIntrReqVectorIsEnabled_Context *ctx
)
{
  if ( ctx->valid_vector && ctx->enabled != NULL ) {
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

      if ( ctx->is_enabled ) {
        WhileIsEnabled( ctx, &attr, has_installed_entries );
      } else {
        WhileIsDisabled( ctx, &attr, has_installed_entries );
      }
    }
  } else {
    if ( ctx->valid_vector ) {
      ctx->vector = 0;
    } else {
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    }

    ctx->status = rtems_interrupt_vector_is_enabled(
      ctx->vector,
      ctx->enabled
    );
  }
}

static const RtemsIntrReqVectorIsEnabled_Entry
RtemsIntrReqVectorIsEnabled_Entries[] = {
  { 0, 0, 0, 0, RtemsIntrReqVectorIsEnabled_Post_Status_InvAddr,
    RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Nop },
  { 0, 0, 0, 1, RtemsIntrReqVectorIsEnabled_Post_Status_InvId,
    RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Nop },
  { 0, 0, 0, 1, RtemsIntrReqVectorIsEnabled_Post_Status_InvAddr,
    RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Nop },
  { 0, 0, 0, 0, RtemsIntrReqVectorIsEnabled_Post_Status_Ok,
    RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Yes },
  { 0, 0, 0, 0, RtemsIntrReqVectorIsEnabled_Post_Status_Ok,
    RtemsIntrReqVectorIsEnabled_Post_IsEnabled_No }
};

static const uint8_t
RtemsIntrReqVectorIsEnabled_Map[] = {
  3, 4, 0, 0, 1, 1, 2, 2
};

static size_t RtemsIntrReqVectorIsEnabled_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  RtemsIntrReqVectorIsEnabled_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqVectorIsEnabled_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqVectorIsEnabled_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqVectorIsEnabled_Scope,
  .initial_context = &RtemsIntrReqVectorIsEnabled_Instance
};

static inline RtemsIntrReqVectorIsEnabled_Entry
RtemsIntrReqVectorIsEnabled_PopEntry(
  RtemsIntrReqVectorIsEnabled_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqVectorIsEnabled_Entries[
    RtemsIntrReqVectorIsEnabled_Map[ index ]
  ];
}

static void RtemsIntrReqVectorIsEnabled_SetPreConditionStates(
  RtemsIntrReqVectorIsEnabled_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_IsEnabled_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void RtemsIntrReqVectorIsEnabled_TestVariant(
  RtemsIntrReqVectorIsEnabled_Context *ctx
)
{
  RtemsIntrReqVectorIsEnabled_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqVectorIsEnabled_Pre_Enabled_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqVectorIsEnabled_Action( ctx );
  RtemsIntrReqVectorIsEnabled_Post_Status_Check(
    ctx,
    ctx->Map.entry.Post_Status
  );
  RtemsIntrReqVectorIsEnabled_Post_IsEnabled_Check(
    ctx,
    ctx->Map.entry.Post_IsEnabled
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqVectorIsEnabled( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqVectorIsEnabled,
  &RtemsIntrReqVectorIsEnabled_Fixture
)
{
  RtemsIntrReqVectorIsEnabled_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqVectorIsEnabled_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqVectorIsEnabled_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqVectorIsEnabled_Pre_Enabled_Obj;
      ctx->Map.pci[ 1 ] < RtemsIntrReqVectorIsEnabled_Pre_Enabled_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_Yes;
        ctx->Map.pci[ 2 ] < RtemsIntrReqVectorIsEnabled_Pre_IsEnabled_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqVectorIsEnabled_PopEntry( ctx );
        RtemsIntrReqVectorIsEnabled_SetPreConditionStates( ctx );
        RtemsIntrReqVectorIsEnabled_TestVariant( ctx );
      }
    }
  }
}

/** @} */
