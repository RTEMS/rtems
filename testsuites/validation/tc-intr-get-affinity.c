/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqGetAffinity
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
 * @defgroup RtemsIntrReqGetAffinity spec:/rtems/intr/req/get-affinity
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqGetAffinity_Pre_Vector_Valid,
  RtemsIntrReqGetAffinity_Pre_Vector_Invalid,
  RtemsIntrReqGetAffinity_Pre_Vector_NA
} RtemsIntrReqGetAffinity_Pre_Vector;

typedef enum {
  RtemsIntrReqGetAffinity_Pre_CPUSetSize_Valid,
  RtemsIntrReqGetAffinity_Pre_CPUSetSize_TooSmall,
  RtemsIntrReqGetAffinity_Pre_CPUSetSize_Askew,
  RtemsIntrReqGetAffinity_Pre_CPUSetSize_NA
} RtemsIntrReqGetAffinity_Pre_CPUSetSize;

typedef enum {
  RtemsIntrReqGetAffinity_Pre_CPUSet_Valid,
  RtemsIntrReqGetAffinity_Pre_CPUSet_Null,
  RtemsIntrReqGetAffinity_Pre_CPUSet_NA
} RtemsIntrReqGetAffinity_Pre_CPUSet;

typedef enum {
  RtemsIntrReqGetAffinity_Pre_CanGetAffinity_Yes,
  RtemsIntrReqGetAffinity_Pre_CanGetAffinity_No,
  RtemsIntrReqGetAffinity_Pre_CanGetAffinity_NA
} RtemsIntrReqGetAffinity_Pre_CanGetAffinity;

typedef enum {
  RtemsIntrReqGetAffinity_Post_Status_Ok,
  RtemsIntrReqGetAffinity_Post_Status_InvAddr,
  RtemsIntrReqGetAffinity_Post_Status_InvId,
  RtemsIntrReqGetAffinity_Post_Status_InvSize,
  RtemsIntrReqGetAffinity_Post_Status_Unsat,
  RtemsIntrReqGetAffinity_Post_Status_NA
} RtemsIntrReqGetAffinity_Post_Status;

typedef enum {
  RtemsIntrReqGetAffinity_Post_CPUSetObj_Set,
  RtemsIntrReqGetAffinity_Post_CPUSetObj_Partial,
  RtemsIntrReqGetAffinity_Post_CPUSetObj_Zero,
  RtemsIntrReqGetAffinity_Post_CPUSetObj_Nop,
  RtemsIntrReqGetAffinity_Post_CPUSetObj_NA
} RtemsIntrReqGetAffinity_Post_CPUSetObj;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Vector_NA : 1;
  uint16_t Pre_CPUSetSize_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Pre_CanGetAffinity_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_CPUSetObj : 3;
} RtemsIntrReqGetAffinity_Entry;

/**
 * @brief Test context for spec:/rtems/intr/req/get-affinity test case.
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
   *   rtems_interrupt_get_affinity() call.
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

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 4 ];

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
    RtemsIntrReqGetAffinity_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqGetAffinity_Context;

static RtemsIntrReqGetAffinity_Context
  RtemsIntrReqGetAffinity_Instance;

static const char * const RtemsIntrReqGetAffinity_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqGetAffinity_PreDesc_CPUSetSize[] = {
  "Valid",
  "TooSmall",
  "Askew",
  "NA"
};

static const char * const RtemsIntrReqGetAffinity_PreDesc_CPUSet[] = {
  "Valid",
  "Null",
  "NA"
};

static const char * const RtemsIntrReqGetAffinity_PreDesc_CanGetAffinity[] = {
  "Yes",
  "No",
  "NA"
};

static const char * const * const RtemsIntrReqGetAffinity_PreDesc[] = {
  RtemsIntrReqGetAffinity_PreDesc_Vector,
  RtemsIntrReqGetAffinity_PreDesc_CPUSetSize,
  RtemsIntrReqGetAffinity_PreDesc_CPUSet,
  RtemsIntrReqGetAffinity_PreDesc_CanGetAffinity,
  NULL
};

typedef RtemsIntrReqGetAffinity_Context Context;

static void CheckGetAffinity(
  Context                          *ctx,
  const rtems_interrupt_attributes *attr
)
{
  rtems_status_code sc;
  cpu_set_t         set[ 2 ];

  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Warray-bounds"

  if ( attr->can_get_affinity ) {
    CPU_ZERO_S( sizeof( ctx->cpuset_obj ), ctx->cpuset_obj );
  } else {
    CPU_FILL_S( sizeof( ctx->cpuset_obj ), ctx->cpuset_obj );
  }

  sc = rtems_interrupt_get_affinity(
    ctx->vector,
    ctx->cpusetsize,
    ctx->cpuset
  );

  CPU_ZERO_S( sizeof( set ), set );

  if ( attr->can_get_affinity ) {
    T_rsc_success( sc );
    T_false( CPU_EQUAL_S( sizeof( set ), ctx->cpuset_obj, set ) );
  } else {
    T_rsc( sc, RTEMS_UNSATISFIED );
    T_true( CPU_EQUAL_S( sizeof( set ), ctx->cpuset_obj, set ) );
  }

  #pragma GCC diagnostic pop
}

static void RtemsIntrReqGetAffinity_Pre_Vector_Prepare(
  RtemsIntrReqGetAffinity_Context   *ctx,
  RtemsIntrReqGetAffinity_Pre_Vector state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAffinity_Pre_Vector_Valid: {
      /*
       * While the ``vector`` parameter is associated with an interrupt vector.
       */
      ctx->valid_vector = true;
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_Vector_Invalid: {
      /*
       * While the ``vector`` parameter is not associated with an interrupt
       * vector.
       */
      ctx->valid_vector = false;
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_Vector_NA:
      break;
  }
}

static void RtemsIntrReqGetAffinity_Pre_CPUSetSize_Prepare(
  RtemsIntrReqGetAffinity_Context       *ctx,
  RtemsIntrReqGetAffinity_Pre_CPUSetSize state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAffinity_Pre_CPUSetSize_Valid: {
      /*
       * While the ``affinity_size`` parameter is an integral multiple of the
       * size of ``long``, while the ``affinity_size`` parameter specifies a
       * processor set which is large enough to contain the processor affinity
       * set of the interrupt specified by ``vector``.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj );
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_CPUSetSize_TooSmall: {
      /*
       * While the ``affinity_size`` parameter is an integral multiple of the
       * size of ``long``, while the ``affinity_size`` parameter specifies a
       * processor set which is not large enough to contain the processor
       * affinity set of the interrupt specified by ``vector``.
       */
      ctx->cpusetsize = 0;
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_CPUSetSize_Askew: {
      /*
       * While the ``affinity_size`` parameter is not an integral multiple of
       * the size of ``long``.
       */
      ctx->cpusetsize = SIZE_MAX;
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_CPUSetSize_NA:
      break;
  }
}

static void RtemsIntrReqGetAffinity_Pre_CPUSet_Prepare(
  RtemsIntrReqGetAffinity_Context   *ctx,
  RtemsIntrReqGetAffinity_Pre_CPUSet state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAffinity_Pre_CPUSet_Valid: {
      /*
       * While the ``affinity`` parameter references an object of type
       * cpu_set_t.
       */
      ctx->cpuset = ctx->cpuset_obj;
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_CPUSet_Null: {
      /*
       * While the ``affinity`` parameter is equal to NULL.
       */
      ctx->cpuset = NULL;
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_CPUSet_NA:
      break;
  }
}

static void RtemsIntrReqGetAffinity_Pre_CanGetAffinity_Prepare(
  RtemsIntrReqGetAffinity_Context           *ctx,
  RtemsIntrReqGetAffinity_Pre_CanGetAffinity state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAffinity_Pre_CanGetAffinity_Yes: {
      /*
       * While getting the affinity for the interrupt vector specified by
       * ``vector`` parameter is supported.
       */
      /* Validation done by CheckGetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_CanGetAffinity_No: {
      /*
       * While getting the affinity for the interrupt vector specified by
       * ``vector`` parameter is not supported.
       */
      /* Validation done by CheckGetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqGetAffinity_Pre_CanGetAffinity_NA:
      break;
  }
}

static void RtemsIntrReqGetAffinity_Post_Status_Check(
  RtemsIntrReqGetAffinity_Context    *ctx,
  RtemsIntrReqGetAffinity_Post_Status state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAffinity_Post_Status_Ok: {
      /*
       * The return status of rtems_interrupt_get_affinity() shall be
       * RTEMS_SUCCESSFUL.
       */
      /* Validation done by CheckGetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqGetAffinity_Post_Status_InvAddr: {
      /*
       * The return status of rtems_interrupt_get_affinity() shall be
       * RTEMS_INVALID_ADDRESS.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ADDRESS );
      break;
    }

    case RtemsIntrReqGetAffinity_Post_Status_InvId: {
      /*
       * The return status of rtems_interrupt_get_affinity() shall be
       * RTEMS_INVALID_ID.
       */
      T_rsc( ctx->status, RTEMS_INVALID_ID );
      break;
    }

    case RtemsIntrReqGetAffinity_Post_Status_InvSize: {
      /*
       * The return status of rtems_interrupt_get_affinity() shall be
       * RTEMS_INVALID_SIZE.
       */
      T_rsc( ctx->status, RTEMS_INVALID_SIZE );
      break;
    }

    case RtemsIntrReqGetAffinity_Post_Status_Unsat: {
      /*
       * The return status of rtems_interrupt_get_affinity() shall be
       * RTEMS_UNSATISFIED.
       */
      /* Validation done by CheckGetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqGetAffinity_Post_Status_NA:
      break;
  }
}

static void RtemsIntrReqGetAffinity_Post_CPUSetObj_Check(
  RtemsIntrReqGetAffinity_Context       *ctx,
  RtemsIntrReqGetAffinity_Post_CPUSetObj state
)
{
  switch ( state ) {
    case RtemsIntrReqGetAffinity_Post_CPUSetObj_Set: {
      /*
       * The value of the object referenced by the ``affinity`` parameter shall
       * be set to the processor affinity set of the interrupt specified by the
       * ``vector`` parameter at some point during the call after the return of
       * the rtems_interrupt_get_affinity() call.
       */
      /* Validation done by CheckGetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqGetAffinity_Post_CPUSetObj_Partial: {
      /*
       * The value of the object referenced by the ``affinity`` parameter shall
       * be set to the subset of the processor affinity set which fits into the
       * object of the interrupt specified by the ``vector`` parameter at some
       * point during the call after the return of the
       * rtems_interrupt_get_affinity() call.
       */
      /* Validation done by CheckGetAffinity() for each interrupt vector */
      break;
    }

    case RtemsIntrReqGetAffinity_Post_CPUSetObj_Zero: {
      /*
       * The value of the object referenced by the ``affinity`` parameter shall
       * be set to zero.
       */
      /*
       * Validation done by CheckGetAffinity() for each interrupt vector and in
       * the action code.
       */
      break;
    }

    case RtemsIntrReqGetAffinity_Post_CPUSetObj_Nop: {
      /*
       * Objects referenced by the ``affinity`` parameter in past calls to
       * rtems_interrupt_get_affinity() shall not be accessed by the
       * rtems_interrupt_get_affinity() call.
       */
      /*
       * Validation done by CheckGetAffinity() for each interrupt vector and in
       * the action code.
       */
      break;
    }

    case RtemsIntrReqGetAffinity_Post_CPUSetObj_NA:
      break;
  }
}

static void RtemsIntrReqGetAffinity_Setup(
  RtemsIntrReqGetAffinity_Context *ctx
)
{
  rtems_interrupt_attributes required;

  memset( &required, 0, sizeof( required ) );
  required.can_get_affinity = true;
  ctx->some_vector = GetValidInterruptVectorNumber( &required );
}

static void RtemsIntrReqGetAffinity_Setup_Wrap( void *arg )
{
  RtemsIntrReqGetAffinity_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsIntrReqGetAffinity_Setup( ctx );
}

static void RtemsIntrReqGetAffinity_Action(
  RtemsIntrReqGetAffinity_Context *ctx
)
{
  if (
    ctx->valid_vector && ctx->cpusetsize == sizeof( ctx->cpuset_obj ) &&
    ctx->cpuset == ctx->cpuset_obj
  ) {
    for (
      ctx->vector = 0;
      ctx->vector < BSP_INTERRUPT_VECTOR_COUNT;
      ++ctx->vector
    ) {
      rtems_status_code          sc;
      rtems_interrupt_attributes attr;

      memset( &attr, 0, sizeof( attr ) );
      sc = rtems_interrupt_get_attributes( ctx->vector, &attr );

      if ( sc == RTEMS_INVALID_ID ) {
        continue;
      }

      T_rsc_success( sc );

      CheckGetAffinity( ctx, &attr );
    }
  } else {
    cpu_set_t set[ 2 ];

    if ( ctx->valid_vector ) {
      ctx->vector = ctx->some_vector;
    } else {
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    }

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Warray-bounds"

    CPU_FILL_S( sizeof( ctx->cpuset_obj ), ctx->cpuset_obj );

    ctx->status = rtems_interrupt_get_affinity(
      ctx->vector,
      ctx->cpusetsize,
      ctx->cpuset
    );

    if (
      ctx->cpuset == NULL ||
      ctx->cpusetsize != sizeof( ctx->cpuset_obj )
    ) {
      CPU_FILL_S( sizeof( set ), set );
    } else {
      CPU_ZERO_S( sizeof( set ), set );
    }

    T_true( CPU_EQUAL_S( sizeof( set ), ctx->cpuset_obj, set ) );

    #pragma GCC diagnostic pop
  }
}

static const RtemsIntrReqGetAffinity_Entry
RtemsIntrReqGetAffinity_Entries[] = {
  { 0, 0, 0, 0, 0, RtemsIntrReqGetAffinity_Post_Status_InvAddr,
    RtemsIntrReqGetAffinity_Post_CPUSetObj_Nop },
  { 0, 0, 0, 0, 1, RtemsIntrReqGetAffinity_Post_Status_InvId,
    RtemsIntrReqGetAffinity_Post_CPUSetObj_Zero },
  { 0, 0, 0, 0, 1, RtemsIntrReqGetAffinity_Post_Status_InvAddr,
    RtemsIntrReqGetAffinity_Post_CPUSetObj_Nop },
  { 0, 0, 0, 0, 0, RtemsIntrReqGetAffinity_Post_Status_Unsat,
    RtemsIntrReqGetAffinity_Post_CPUSetObj_Zero },
  { 0, 0, 0, 0, 0, RtemsIntrReqGetAffinity_Post_Status_InvSize,
    RtemsIntrReqGetAffinity_Post_CPUSetObj_Nop },
  { 0, 0, 0, 0, 0, RtemsIntrReqGetAffinity_Post_Status_Ok,
    RtemsIntrReqGetAffinity_Post_CPUSetObj_Set },
  { 0, 0, 0, 0, 0, RtemsIntrReqGetAffinity_Post_Status_InvSize,
    RtemsIntrReqGetAffinity_Post_CPUSetObj_Partial }
};

static const uint8_t
RtemsIntrReqGetAffinity_Map[] = {
  5, 3, 0, 0, 6, 3, 0, 0, 4, 4, 0, 0, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2
};

static size_t RtemsIntrReqGetAffinity_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqGetAffinity_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqGetAffinity_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture RtemsIntrReqGetAffinity_Fixture = {
  .setup = RtemsIntrReqGetAffinity_Setup_Wrap,
  .stop = NULL,
  .teardown = NULL,
  .scope = RtemsIntrReqGetAffinity_Scope,
  .initial_context = &RtemsIntrReqGetAffinity_Instance
};

static inline RtemsIntrReqGetAffinity_Entry RtemsIntrReqGetAffinity_PopEntry(
  RtemsIntrReqGetAffinity_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqGetAffinity_Entries[
    RtemsIntrReqGetAffinity_Map[ index ]
  ];
}

static void RtemsIntrReqGetAffinity_SetPreConditionStates(
  RtemsIntrReqGetAffinity_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];

  if ( ctx->Map.entry.Pre_CanGetAffinity_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsIntrReqGetAffinity_Pre_CanGetAffinity_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }
}

static void RtemsIntrReqGetAffinity_TestVariant(
  RtemsIntrReqGetAffinity_Context *ctx
)
{
  RtemsIntrReqGetAffinity_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqGetAffinity_Pre_CPUSetSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqGetAffinity_Pre_CPUSet_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqGetAffinity_Pre_CanGetAffinity_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsIntrReqGetAffinity_Action( ctx );
  RtemsIntrReqGetAffinity_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsIntrReqGetAffinity_Post_CPUSetObj_Check(
    ctx,
    ctx->Map.entry.Post_CPUSetObj
  );
}

/**
 * @fn void T_case_body_RtemsIntrReqGetAffinity( void )
 */
T_TEST_CASE_FIXTURE(
  RtemsIntrReqGetAffinity,
  &RtemsIntrReqGetAffinity_Fixture
)
{
  RtemsIntrReqGetAffinity_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqGetAffinity_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqGetAffinity_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqGetAffinity_Pre_CPUSetSize_Valid;
      ctx->Map.pci[ 1 ] < RtemsIntrReqGetAffinity_Pre_CPUSetSize_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqGetAffinity_Pre_CPUSet_Valid;
        ctx->Map.pci[ 2 ] < RtemsIntrReqGetAffinity_Pre_CPUSet_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsIntrReqGetAffinity_Pre_CanGetAffinity_Yes;
          ctx->Map.pci[ 3 ] < RtemsIntrReqGetAffinity_Pre_CanGetAffinity_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          ctx->Map.entry = RtemsIntrReqGetAffinity_PopEntry( ctx );
          RtemsIntrReqGetAffinity_SetPreConditionStates( ctx );
          RtemsIntrReqGetAffinity_TestVariant( ctx );
        }
      }
    }
  }
}

/** @} */
