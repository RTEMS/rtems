/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqIsPending
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
 * @defgroup RtemsIntrReqIsPending spec:/rtems/intr/req/is-pending
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqIsPending_Pre_Vector_Valid,
  RtemsIntrReqIsPending_Pre_Vector_Invalid,
  RtemsIntrReqIsPending_Pre_Vector_NA
} RtemsIntrReqIsPending_Pre_Vector;

typedef enum {
  RtemsIntrReqIsPending_Pre_Pending_Obj,
  RtemsIntrReqIsPending_Pre_Pending_Null,
  RtemsIntrReqIsPending_Pre_Pending_NA
} RtemsIntrReqIsPending_Pre_Pending;

typedef enum {
  RtemsIntrReqIsPending_Pre_IsPending_Yes,
  RtemsIntrReqIsPending_Pre_IsPending_No,
  RtemsIntrReqIsPending_Pre_IsPending_NA
} RtemsIntrReqIsPending_Pre_IsPending;

typedef enum {
  RtemsIntrReqIsPending_Post_Status_Ok,
  RtemsIntrReqIsPending_Post_Status_InvAddr,
  RtemsIntrReqIsPending_Post_Status_InvId,
  RtemsIntrReqIsPending_Post_Status_NA
} RtemsIntrReqIsPending_Post_Status;

typedef enum {
  RtemsIntrReqIsPending_Post_IsPending_Nop,
  RtemsIntrReqIsPending_Post_IsPending_Yes,
  RtemsIntrReqIsPending_Post_IsPending_No,
  RtemsIntrReqIsPending_Post_IsPending_NA
} RtemsIntrReqIsPending_Post_IsPending;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Vector_NA : 1;
  uint8_t Pre_Pending_NA : 1;
  uint8_t Pre_IsPending_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_IsPending : 2;
} RtemsIntrReqIsPending_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/is-pending test case.
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
   * @brief This member provides the ``bool`` object.
   */
  bool pending_obj;

  /**
   * @brief If this member is true, then the ``vector`` parameter shall be
   *   valid.
   */
  bool valid_vector;

  /**
   * @brief This member specifies if the ``pending`` parameter value.
   */
  bool *pending;

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_is_pending() call.
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
    RtemsIntrReqIsPending_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqIsPending_Context;

static RtemsIntrReqIsPending_Context
  RtemsIntrReqIsPending_Instance;

static const char * const RtemsIntrReqIsPending_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqIsPending_PreDesc_Pending[] = {
  "Obj",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqIsPending_PreDesc_IsPending[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqIsPending_PreDesc[] = {
  RtemsIntrReqIsPending_PreDesc_Vector,
  RtemsIntrReqIsPending_PreDesc_Pending,
  RtemsIntrReqIsPending_PreDesc_IsPending,
  NULL
};

typedef RtemsIntrReqIsPending_Context Context;

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

static void CheckIsPending(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr,
  bool                              has_installed_entries
)
{
  rtems_status_code sc;

  if ( has_installed_entries ) {
    /*
     * We cannot test this vector thoroughly, since it is used by a device
     * driver.  It may be pending or not.  For example in SMP configurations,
     * it may be pending while being serviced right now on another processor.
     */
    (void) IsPending( ctx );
  } else if ( !attr->is_maskable ) {
    /* We can only safely test maskable interrupts */
    T_false( IsPending( ctx ) );
  } else if ( IsPending( ctx ) ) {
    /*
     * If there is already an interrupt pending, then it is probably raised
     * by a peripheral which we cannot control.
     */
  } else if (
    attr->can_raise && attr->can_disable &&
    ( attr->can_clear || attr->cleared_by_acknowledge )
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

      rtems_interrupt_local_disable( level );
      Raise( ctx );
      T_true( IsPending( ctx ) );
      rtems_interrupt_local_enable( level );

      while ( ctx->interrupt_count < 2 ) {
        /* Wait */
      }
    }

    sc = rtems_interrupt_entry_remove( ctx->vector, &entry );
    T_rsc_success( sc );
  }
}

static void RtemsIntrReqIsPending_Pre_Vector_Prepare(
  RtemsIntrReqIsPending_Context   *ctx,
  RtemsIntrReqIsPending_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqIsPending_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqIsPending_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqIsPending_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqIsPending_Pre_Pending_Prepare(
  RtemsIntrReqIsPending_Context    *ctx,
  RtemsIntrReqIsPending_Pre_Pending state
)
{
  switch ( state ) {
    case RtemsIntrReqIsPending_Pre_Pending_Obj: {
      /*
       * While the ``pending`` parameter references an object of type ``bool``.
       */
      ctx->pending = &ctx->pending_obj;
      break;
    }

    case RtemsIntrReqIsPending_Pre_Pending_Null: {
      /*
       * While the ``pending`` parameter is equal to NULL.
       */
      ctx->pending = NULL;
      break;
    }

    case RtemsIntrReqIsPending_Pre_Pending_NA:
      break;
  }
}

static void RtemsIntrReqIsPending_Pre_IsPending_Prepare(
  RtemsIntrReqIsPending_Context      *ctx,
  RtemsIntrReqIsPending_Pre_IsPending state
)
{
  switch ( state ) {
    case RtemsIntrReqIsPending_Pre_IsPending_Yes: {
      /*
       * While the interrupt associated with the interrupt vector specified by
       * ``vector`` was pending for the processor executing the
       * rtems_interrupt_is_pending() call at some time point during the call.
       */
      /* Validation is done by CheckIsPending() for each interrupt vector */
      break;
    }

    case RtemsIntrReqIsPending_Pre_IsPending_No: {
      /*
       * While the interrupt associated with the interrupt vector specified by
       * ``vector`` was not pending for the processor executing the
       * rtems_interrupt_is_pending() call at some time point during the call.
       */
      /* Validation is done by CheckIsPending() for each interrupt vector */
      break;
    }

    case RtemsIntrReqIsPending_Pre_IsPending_NA:
      break;
  }
}

static void RtemsIntrReqIsPending_Post_Status_Check(
  RtemsIntrReqIsPending_Context    *ctx,
  RtemsIntrReqIsPending_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqIsPending_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_is_pending() shall be
       * RTEMS_SUCCESSFUL.
       */
      T_rsc_success( ctx->status );
      break;
    }

    case RtemsIntrReqIsPending_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_is_pending() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqIsPending_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_is_pending() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqIsPending_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqIsPending_Post_IsPending_Check(
  RtemsIntrReqIsPending_Context       *ctx,
  RtemsIntrReqIsPending_Post_IsPending state
)
{
  bool pending;

  switch ( state ) {
    case RtemsIntrReqIsPending_Post_IsPending_Nop: {
      /*
       * Objects referenced by the ``pending`` parameter in past calls to
       * rtems_interrupt_is_pending() shall not be accessed by the
       * rtems_interrupt_is_pending() call.
       */
      memset( &pending, 0xa5, sizeof( pending ) );
      T_eq_mem( &ctx->pending_obj, &pending, sizeof( pending ) );
      break;
    }

    case RtemsIntrReqIsPending_Post_IsPending_Yes: {
      /*
       * The value of the object referenced by the ``pending`` parameter shall
       * be set to true.
       */
      /* Validation is done by CheckIsPending() for each interrupt vector */
      break;
    }

    case RtemsIntrReqIsPending_Post_IsPending_No: {
      /*
       * The value of the object referenced by the ``pending`` parameter shall
       * be set to false.
       */
      /* Validation is done by CheckIsPending() for each interrupt vector */
      break;
    }

    case RtemsIntrReqIsPending_Post_IsPending_NA:
      break;
  }
}

static void RtemsIntrReqIsPending_Action( RtemsIntrReqIsPending_Context *ctx )
{
  if ( ctx->valid_vector && ctx->pending != NULL ) {
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
      CheckIsPending( ctx, &attr, has_installed_entries );
    }
  } else {
    if ( ctx->valid_vector ) {
      ctx->vector = 0;
    } else {
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    }

    memset( &ctx->pending_obj, 0xa5, sizeof( ctx->pending_obj ) );

    ctx->status = rtems_interrupt_is_pending( ctx->vector, ctx->pending );
  }
}

static const RtemsIntrReqIsPending_Entry
RtemsIntrReqIsPending_Entries[] = {
  { 0, 0, 0, 0, RtemsIntrReqIsPending_Post_Status_InvAddr,
    RtemsIntrReqIsPending_Post_IsPending_Nop },
  { 0, 0, 0, 1, RtemsIntrReqIsPending_Post_Status_InvId,
    RtemsIntrReqIsPending_Post_IsPending_Nop },
  { 0, 0, 0, 1, RtemsIntrReqIsPending_Post_Status_InvAddr,
    RtemsIntrReqIsPending_Post_IsPending_Nop },
  { 0, 0, 0, 0, RtemsIntrReqIsPending_Post_Status_Ok,
    RtemsIntrReqIsPending_Post_IsPending_Yes },
  { 0, 0, 0, 0, RtemsIntrReqIsPending_Post_Status_Ok,
    RtemsIntrReqIsPending_Post_IsPending_No }
};

static const uint8_t
RtemsIntrReqIsPending_Map[] = {
  3, 4, 0, 0, 1, 1, 2, 2
};

static size_t RtemsIntrReqIsPending_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqIsPending_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsIntrReqIsPending_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsIntrReqIsPending_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqIsPending_Scope,
  .initial_context = &RtemsIntrReqIsPending_Instance
};

static inline RtemsIntrReqIsPending_Entry RtemsIntrReqIsPending_PopEntry(
  RtemsIntrReqIsPending_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqIsPending_Entries[
    RtemsIntrReqIsPending_Map[ index ]
  ];
}

static void RtemsIntrReqIsPending_SetPreConditionStates(
  RtemsIntrReqIsPending_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_IsPending_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsIntrReqIsPending_Pre_IsPending_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }
}

static void RtemsIntrReqIsPending_TestVariant(
  RtemsIntrReqIsPending_Context *ctx
)
{
  RtemsIntrReqIsPending_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqIsPending_Pre_Pending_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqIsPending_Pre_IsPending_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqIsPending_Action( ctx );
  RtemsIntrReqIsPending_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsIntrReqIsPending_Post_IsPending_Check(
    ctx,
    ctx->Map.entry.Post_IsPending
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqIsPending( void )
 */
T_TEST_CASE_FIXTURE( RtemsIntrReqIsPending, &RtemsIntrReqIsPending_Fixture )
{
  RtemsIntrReqIsPending_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqIsPending_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqIsPending_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqIsPending_Pre_Pending_Obj;
      ctx->Map.pci[ 1 ] < RtemsIntrReqIsPending_Pre_Pending_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqIsPending_Pre_IsPending_Yes;
        ctx->Map.pci[ 2 ] < RtemsIntrReqIsPending_Pre_IsPending_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        ctx->Map.entry = RtemsIntrReqIsPending_PopEntry( ctx );
        RtemsIntrReqIsPending_SetPreConditionStates( ctx );
        RtemsIntrReqIsPending_TestVariant( ctx );
      }
    }
  }
}

/** @} */
