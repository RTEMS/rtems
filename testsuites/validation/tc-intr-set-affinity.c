/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseRtemsIntrReqSetAffinity
 */

/*
 * Copyright (C) 2021 embedded brains GmbH (http://www.embedded-brains.de)
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
 * @defgroup RTEMSTestCaseRtemsIntrReqSetAffinity \
 *   spec:/rtems/intr/req/set-affinity
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidation0
 *
 * @{
 */

typedef enum {
  RtemsIntrReqSetAffinity_Pre_Vector_Valid,
  RtemsIntrReqSetAffinity_Pre_Vector_Invalid,
  RtemsIntrReqSetAffinity_Pre_Vector_NA
} RtemsIntrReqSetAffinity_Pre_Vector;

typedef enum {
  RtemsIntrReqSetAffinity_Pre_CPUSetKind_Valid,
  RtemsIntrReqSetAffinity_Pre_CPUSetKind_Huge,
  RtemsIntrReqSetAffinity_Pre_CPUSetKind_Askew,
  RtemsIntrReqSetAffinity_Pre_CPUSetKind_NA
} RtemsIntrReqSetAffinity_Pre_CPUSetKind;

typedef enum {
  RtemsIntrReqSetAffinity_Pre_CPUSet_Valid,
  RtemsIntrReqSetAffinity_Pre_CPUSet_Null,
  RtemsIntrReqSetAffinity_Pre_CPUSet_NA
} RtemsIntrReqSetAffinity_Pre_CPUSet;

typedef enum {
  RtemsIntrReqSetAffinity_Pre_CanSetAffinity_Yes,
  RtemsIntrReqSetAffinity_Pre_CanSetAffinity_No,
  RtemsIntrReqSetAffinity_Pre_CanSetAffinity_NA
} RtemsIntrReqSetAffinity_Pre_CanSetAffinity;

typedef enum {
  RtemsIntrReqSetAffinity_Post_Status_Ok,
  RtemsIntrReqSetAffinity_Post_Status_InvAddr,
  RtemsIntrReqSetAffinity_Post_Status_InvId,
  RtemsIntrReqSetAffinity_Post_Status_InvNum,
  RtemsIntrReqSetAffinity_Post_Status_Unsat,
  RtemsIntrReqSetAffinity_Post_Status_NA
} RtemsIntrReqSetAffinity_Post_Status;

typedef enum {
  RtemsIntrReqSetAffinity_Post_SetAffinity_Yes,
  RtemsIntrReqSetAffinity_Post_SetAffinity_Nop,
  RtemsIntrReqSetAffinity_Post_SetAffinity_NA
} RtemsIntrReqSetAffinity_Post_SetAffinity;

/**
 * @brief Test context for spec:/rtems/intr/req/set-affinity test case.
 */
typedef struct {
  /**
   * @brief This member contains the current vector number.
   */
  rtems_vector_number vector;

  /**
   * @brief This member contains some valid vector number.
   */
  rtems_vector_number some_vector;

  /**
   * @brief This member provides the object referenced by the ``affinity``
   *   parameter.
   */
  cpu_set_t cpuset_obj[ 2 ];

  /**
   * @brief This member contains the return value of the
   *   rtems_interrupt_set_affinity() call.
   */
  rtems_status_code status;

  /**
   * @brief If this member is true, then the ``vector`` parameter shall be
   *   valid.
   */
  bool valid_vector;

  /**
   * @brief This member specifies if the ``affinity_size`` parameter value.
   */
  size_t cpusetsize;

  /**
   * @brief This member specifies if the ``affinity`` parameter value.
   */
  cpu_set_t *cpuset;

  /**
   * @brief This member defines the pre-condition states for the next action.
   */
  size_t pcs[ 4 ];

  /**
   * @brief This member indicates if the test action loop is currently
   *   executed.
   */
  bool in_action_loop;
} RtemsIntrReqSetAffinity_Context;

static RtemsIntrReqSetAffinity_Context
  RtemsIntrReqSetAffinity_Instance;

static const char * const RtemsIntrReqSetAffinity_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqSetAffinity_PreDesc_CPUSetKind[] = {
  "Valid",
  "Huge",
  "Askew",
  "NA"
};

static const char * const RtemsIntrReqSetAffinity_PreDesc_CPUSet[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqSetAffinity_PreDesc_CanSetAffinity[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqSetAffinity_PreDesc[] = {
  RtemsIntrReqSetAffinity_PreDesc_Vector,
  RtemsIntrReqSetAffinity_PreDesc_CPUSetKind,
  RtemsIntrReqSetAffinity_PreDesc_CPUSet,
  RtemsIntrReqSetAffinity_PreDesc_CanSetAffinity,
  NULL
};

typedef RtemsIntrReqSetAffinity_Context Context;

static void CheckSetAffinity(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr
)
{
  rtems_status_code sc;
  cpu_set_t         set;
  cpu_set_t         set2;

  CPU_ZERO( &set );

  if ( attr->can_get_affinity ) {
    sc = rtems_interrupt_get_affinity( ctx->vector, sizeof( set ), &set );
    T_rsc_success( sc );
  } else {
    CPU_SET( 0, &set );
  }

  CPU_COPY( &set, &ctx->cpuset_obj[ 0 ] );

  sc = rtems_interrupt_set_affinity(
    ctx->vector,
    ctx->cpusetsize,
    ctx->cpuset
  );

  if ( attr->can_set_affinity ) {
    T_rsc_success( sc );
  } else {
    T_rsc( sc, RTEMS_UNSATISFIED );
  }

  if ( attr->can_get_affinity ) {
    CPU_FILL( &set2 );
    sc = rtems_interrupt_get_affinity( ctx->vector, sizeof( set2 ), &set2 );
    T_rsc_success( sc );
    T_eq_int( CPU_CMP( &set, &set2 ), 0 );
  }
}

static void RtemsIntrReqSetAffinity_Pre_Vector_Prepare(
  RtemsIntrReqSetAffinity_Context   *ctx,
  RtemsIntrReqSetAffinity_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqSetAffinity_Pre_CPUSetKind_Prepare(
  RtemsIntrReqSetAffinity_Context       *ctx,
  RtemsIntrReqSetAffinity_Pre_CPUSetKind state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Pre_CPUSetKind_Valid: {
      /*
       * While the ``affinity_size`` parameter is an integral multiple of the
       * size of long, while the ``affinity_size`` and ``affinity`` parameter
       * specify a processor set which is within the implementation limits.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj[ 0 ] );
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetKind_Huge: {
      /*
       * While the ``affinity_size`` parameter is an integral multiple of the
       * size of long, while the ``affinity_size`` and ``affinity`` parameter
       * specify a processor set which exceeds the implementation limits.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj );
      CPU_ZERO( &ctx->cpuset_obj[ 0 ] );
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetKind_Askew: {
      /*
       * While the ``affinity_size`` parameter is not an integral multiple of
       * the size of long.
       */
      ctx->cpusetsize = SIZE_MAX;
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetKind_NA:
      break;
  }
}

static void RtemsIntrReqSetAffinity_Pre_CPUSet_Prepare(
  RtemsIntrReqSetAffinity_Context   *ctx,
  RtemsIntrReqSetAffinity_Pre_CPUSet state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Pre_CPUSet_Valid: {
      /*
       * While the ``affinity`` parameter references an object of type
       * cpu_set_t.
       */
      ctx->cpuset = &ctx->cpuset_obj[ 0 ];
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSet_Null: {
      /*
       * While the ``affinity`` parameter is equal to NULL.
       */
      ctx->cpuset = NULL;
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSet_NA:
      break;
  }
}

static void RtemsIntrReqSetAffinity_Pre_CanSetAffinity_Prepare(
  RtemsIntrReqSetAffinity_Context           *ctx,
  RtemsIntrReqSetAffinity_Pre_CanSetAffinity state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Pre_CanSetAffinity_Yes: {
      /*
       * While setting the affinity for the interrupt vector specified by
       * ``vector`` parameter is supported.
       */
      /* Validation done by CheckSetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CanSetAffinity_No: {
      /*
       * While setting the affinity for the interrupt vector specified by
       * ``vector`` parameter is not supported.
       */
      /* Validation done by CheckSetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CanSetAffinity_NA:
      break;
  }
}

static void RtemsIntrReqSetAffinity_Post_Status_Check(
  RtemsIntrReqSetAffinity_Context    *ctx,
  RtemsIntrReqSetAffinity_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_set_affinity() shall be
       * RTEMS_SUCCESSFUL.
       */
      /* Validation done by CheckSetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqSetAffinity_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_set_affinity() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqSetAffinity_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_set_affinity() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqSetAffinity_Post_Status_InvNum: {
      /*
       * The return status of rtems_interrupt_set_affinity() shall be
       * RTEMS_INVALID_NUMBER.
       */
      T_rsc( ctx->status, RTEMS_INVALID_NUMBER );
      break;
    }

    case RtemsIntrReqSetAffinity_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_set_affinity() shall be
       * RTEMS_UNSATISFIED.
       */
      /* Validation done by CheckSetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqSetAffinity_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqSetAffinity_Post_SetAffinity_Check(
  RtemsIntrReqSetAffinity_Context         *ctx,
  RtemsIntrReqSetAffinity_Post_SetAffinity state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Post_SetAffinity_Yes: {
      /*
       * The affinity set of the interrupt specified by ``vector`` shall be set
       * to the processor set specified by ``affinity_size`` and ``affinity``
       * at some point during the rtems_interrupt_set_affinity() call.
       */
      /* Validation done by CheckSetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqSetAffinity_Post_SetAffinity_Nop: {
      /*
       * The affinity set of the interrupt specified by ``vector`` shall not be
       * modified by the rtems_interrupt_set_affinity() call.
       */
      /*
       * Validation done by CheckSetAffinity() for each interrupt vector and in
       * the action code.
       */
      break;
    }

    case RtemsIntrReqSetAffinity_Post_SetAffinity_NA:
      break;
  }
}

static void RtemsIntrReqSetAffinity_Setup(
  RtemsIntrReqSetAffinity_Context *ctx
)
{
  ctx->some_vector = GetValidInterruptVectorNumber( NULL );
}

static void RtemsIntrReqSetAffinity_Setup_Wrap( void *arg )
{
  RtemsIntrReqSetAffinity_Context *ctx;

  ctx = arg;
  ctx->in_action_loop = false;
  RtemsIntrReqSetAffinity_Setup( ctx );
}

static void RtemsIntrReqSetAffinity_Prepare(
  RtemsIntrReqSetAffinity_Context *ctx
)
{
  CPU_FILL_S( sizeof( ctx->cpuset_obj ), &ctx->cpuset_obj[ 0 ] );
}

static void RtemsIntrReqSetAffinity_Action(
  RtemsIntrReqSetAffinity_Context *ctx
)
{
  rtems_status_code sc;

  if (
    ctx->valid_vector && ctx->cpusetsize == sizeof( ctx->cpuset_obj[ 0 ] ) &&
    ctx->cpuset == &ctx->cpuset_obj[ 0 ]
  ) {
    for (
      ctx->vector = 0;
      ctx->vector < BSP_INTERRUPT_VECTOR_COUNT;
      ++ctx->vector
    ) {
      rtems_interrupt_attributes attr;

      memset( &attr, 0, sizeof( attr ) );
      sc = rtems_interrupt_get_attributes( ctx->vector, &attr );

      if ( sc == RTEMS_INVALID_ID ) {
        continue;
      }

      T_rsc_success( sc );

      CheckSetAffinity( ctx, &attr );
    }
  } else {
    cpu_set_t set;
    cpu_set_t set2;

    CPU_ZERO( &set );
    CPU_ZERO( &set2 );

    if ( ctx->valid_vector ) {
      ctx->vector = ctx->some_vector;

      sc = rtems_interrupt_get_affinity( ctx->vector, sizeof( set ), &set );
      T_true( sc == RTEMS_SUCCESSFUL || sc == RTEMS_UNSATISFIED );
    } else {
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    }

    CPU_ZERO( &ctx->cpuset_obj[ 0 ] );

    ctx->status = rtems_interrupt_set_affinity(
      ctx->vector,
      ctx->cpusetsize,
      ctx->cpuset
    );

    if ( ctx->valid_vector ) {
      sc = rtems_interrupt_get_affinity( ctx->vector, sizeof( set2 ), &set2 );
      T_true( sc == RTEMS_SUCCESSFUL || sc == RTEMS_UNSATISFIED );
      T_eq_int( CPU_CMP( &set, &set2 ), 0 );
    }
  }
}

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Vector_NA : 1;
  uint16_t Pre_CPUSetKind_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Pre_CanSetAffinity_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_SetAffinity : 2;
} RtemsIntrReqSetAffinity_Entry;

static const RtemsIntrReqSetAffinity_Entry
RtemsIntrReqSetAffinity_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsIntrReqSetAffinity_Post_Status_InvAddr,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 1, RtemsIntrReqSetAffinity_Post_Status_InvId,
    RtemsIntrReqSetAffinity_Post_SetAffinity_NA },
  { 0, 0, 0, 0, 1, RtemsIntrReqSetAffinity_Post_Status_InvAddr,
    RtemsIntrReqSetAffinity_Post_SetAffinity_NA },
  { 0, 0, 0, 0, 0, RtemsIntrReqSetAffinity_Post_Status_InvNum,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 0, RtemsIntrReqSetAffinity_Post_Status_Ok,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Yes },
  { 0, 0, 0, 0, 0, RtemsIntrReqSetAffinity_Post_Status_Unsat,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop }
};

static const uint8_t
RtemsIntrReqSetAffinity_Map[] = {
  4, 5, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2
};

static size_t RtemsIntrReqSetAffinity_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqSetAffinity_Context *ctx;

  ctx = arg;

  if ( ctx->in_action_loop ) {
    return T_get_scope( RtemsIntrReqSetAffinity_PreDesc, buf, n, ctx->pcs );
  }

  return 0;
}

static T_fixture RtemsIntrReqSetAffinity_Fixture = {
  .setup = RtemsIntrReqSetAffinity_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqSetAffinity_Scope,
  .initial_context = &RtemsIntrReqSetAffinity_Instance
};

static inline RtemsIntrReqSetAffinity_Entry RtemsIntrReqSetAffinity_GetEntry(
  size_t index
)
{
  return RtemsIntrReqSetAffinity_Entries[
    RtemsIntrReqSetAffinity_Map[ index ]
  ];
}

/**
 * @fn void T_case_body_RtemsIntrReqSetAffinity( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqSetAffinity,
  &RtemsIntrReqSetAffinity_Fixture
)
{
  RtemsIntrReqSetAffinity_Context *ctx;
  size_t index;

  ctx = T_fixture_context();
  ctx->in_action_loop = true;
  index = 0;

  for (
    ctx->pcs[ 0 ] = RtemsIntrReqSetAffinity_Pre_Vector_Valid;
    ctx->pcs[ 0 ] < RtemsIntrReqSetAffinity_Pre_Vector_NA;
    ++ctx->pcs[ 0 ]
  ) {
    for (
      ctx->pcs[ 1 ] = RtemsIntrReqSetAffinity_Pre_CPUSetKind_Valid;
      ctx->pcs[ 1 ] < RtemsIntrReqSetAffinity_Pre_CPUSetKind_NA;
      ++ctx->pcs[ 1 ]
    ) {
      for (
        ctx->pcs[ 2 ] = RtemsIntrReqSetAffinity_Pre_CPUSet_Valid;
        ctx->pcs[ 2 ] < RtemsIntrReqSetAffinity_Pre_CPUSet_NA;
        ++ctx->pcs[ 2 ]
      ) {
        for (
          ctx->pcs[ 3 ] = RtemsIntrReqSetAffinity_Pre_CanSetAffinity_Yes;
          ctx->pcs[ 3 ] < RtemsIntrReqSetAffinity_Pre_CanSetAffinity_NA;
          ++ctx->pcs[ 3 ]
        ) {
          RtemsIntrReqSetAffinity_Entry entry;
          size_t pcs[ 4 ];

          entry = RtemsIntrReqSetAffinity_GetEntry( index );
          ++index;

          memcpy( pcs, ctx->pcs, sizeof( pcs ) );

          if ( entry.Pre_CanSetAffinity_NA ) {
            ctx->pcs[ 3 ] = RtemsIntrReqSetAffinity_Pre_CanSetAffinity_NA;
          }

          RtemsIntrReqSetAffinity_Prepare( ctx );
          RtemsIntrReqSetAffinity_Pre_Vector_Prepare( ctx, ctx->pcs[ 0 ] );
          RtemsIntrReqSetAffinity_Pre_CPUSetKind_Prepare( ctx, ctx->pcs[ 1 ] );
          RtemsIntrReqSetAffinity_Pre_CPUSet_Prepare( ctx, ctx->pcs[ 2 ] );
          RtemsIntrReqSetAffinity_Pre_CanSetAffinity_Prepare(
            ctx,
            ctx->pcs[ 3 ]
          );
          RtemsIntrReqSetAffinity_Action( ctx );
          RtemsIntrReqSetAffinity_Post_Status_Check( ctx, entry.Post_Status );
          RtemsIntrReqSetAffinity_Post_SetAffinity_Check(
            ctx,
            entry.Post_SetAffinity
          );
          memcpy( ctx->pcs, pcs, sizeof( ctx->pcs ) );
        }
      }
    }
  }
}

/** @} */
