/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsIntrReqSetAffinity
 */

/*
 * Copyright (C) 2021, 2022 embedded brains GmbH & Co. KG
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
 * @defgroup RtemsIntrReqSetAffinity spec:/rtems/intr/req/set-affinity
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  RtemsIntrReqSetAffinity_Pre_Vector_Valid,
  RtemsIntrReqSetAffinity_Pre_Vector_Invalid,
  RtemsIntrReqSetAffinity_Pre_Vector_NA
} RtemsIntrReqSetAffinity_Pre_Vector;

typedef enum {
  RtemsIntrReqSetAffinity_Pre_CPUSetSize_Askew,
  RtemsIntrReqSetAffinity_Pre_CPUSetSize_Normal,
  RtemsIntrReqSetAffinity_Pre_CPUSetSize_Huge,
  RtemsIntrReqSetAffinity_Pre_CPUSetSize_NA
} RtemsIntrReqSetAffinity_Pre_CPUSetSize;

typedef enum {
  RtemsIntrReqSetAffinity_Pre_CPUSetOnline_Valid,
  RtemsIntrReqSetAffinity_Pre_CPUSetOnline_Empty,
  RtemsIntrReqSetAffinity_Pre_CPUSetOnline_NA
} RtemsIntrReqSetAffinity_Pre_CPUSetOnline;

typedef enum {
  RtemsIntrReqSetAffinity_Pre_CPUSetHuge_NotZero,
  RtemsIntrReqSetAffinity_Pre_CPUSetHuge_Zero,
  RtemsIntrReqSetAffinity_Pre_CPUSetHuge_NA
} RtemsIntrReqSetAffinity_Pre_CPUSetHuge;

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
  RtemsIntrReqSetAffinity_Post_SetAffinity_Set,
  RtemsIntrReqSetAffinity_Post_SetAffinity_Nop,
  RtemsIntrReqSetAffinity_Post_SetAffinity_NA
} RtemsIntrReqSetAffinity_Post_SetAffinity;

typedef struct {
  uint16_t Skip : 1;
  uint16_t Pre_Vector_NA : 1;
  uint16_t Pre_CPUSetSize_NA : 1;
  uint16_t Pre_CPUSetOnline_NA : 1;
  uint16_t Pre_CPUSetHuge_NA : 1;
  uint16_t Pre_CPUSet_NA : 1;
  uint16_t Pre_CanSetAffinity_NA : 1;
  uint16_t Post_Status : 3;
  uint16_t Post_SetAffinity : 2;
} RtemsIntrReqSetAffinity_Entry;

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

  struct {
    /**
     * @brief This member defines the pre-condition indices for the next
     *   action.
     */
    size_t pci[ 6 ];

    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 6 ];

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
    RtemsIntrReqSetAffinity_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsIntrReqSetAffinity_Context;

static RtemsIntrReqSetAffinity_Context
  RtemsIntrReqSetAffinity_Instance;

static const char * const RtemsIntrReqSetAffinity_PreDesc_Vector[] = {
  "Valid",
  "Invalid",
  "NA"
};

static const char * const RtemsIntrReqSetAffinity_PreDesc_CPUSetSize[] = {
  "Askew",
  "Normal",
  "Huge",
  "NA"
};

static const char * const RtemsIntrReqSetAffinity_PreDesc_CPUSetOnline[] = {
  "Valid",
  "Empty",
  "NA"
};

static const char * const RtemsIntrReqSetAffinity_PreDesc_CPUSetHuge[] = {
  "NotZero",
  "Zero",
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
  RtemsIntrReqSetAffinity_PreDesc_CPUSetSize,
  RtemsIntrReqSetAffinity_PreDesc_CPUSetOnline,
  RtemsIntrReqSetAffinity_PreDesc_CPUSetHuge,
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

static void RtemsIntrReqSetAffinity_Pre_CPUSetSize_Prepare(
  RtemsIntrReqSetAffinity_Context       *ctx,
  RtemsIntrReqSetAffinity_Pre_CPUSetSize state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Pre_CPUSetSize_Askew: {
      /*
       * While the ``affinity_size`` parameter is not an integral multiple of
       * the size of long.
       */
      ctx->cpusetsize = SIZE_MAX;
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetSize_Normal: {
      /*
       * While the ``affinity_size`` parameter is an integral multiple of the
       * size of long, while the ``affinity_size`` parameter is less than or
       * equal to the maximum processor set size storable in the system.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj[ 0 ] );
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetSize_Huge: {
      /*
       * While the ``affinity_size`` parameter is an integral multiple of the
       * size of long, while the ``affinity_size`` parameter is greater than
       * the maximum processor set size storable in the system.
       */
      ctx->cpusetsize = sizeof( ctx->cpuset_obj );
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetSize_NA:
      break;
  }
}

static void RtemsIntrReqSetAffinity_Pre_CPUSetOnline_Prepare(
  RtemsIntrReqSetAffinity_Context         *ctx,
  RtemsIntrReqSetAffinity_Pre_CPUSetOnline state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Pre_CPUSetOnline_Valid: {
      /*
       * While the intersection of the processor set specified by the
       * ``affinity_size`` and ``affinity`` parameters and the set of online
       * processors is not empty, while the intersection of the processor set
       * specified by the ``affinity_size`` and ``affinity`` parameters and the
       * set of online processors is a processor affinity set supported by the
       * interrupt vector.
       */
      /* Already prepared */
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetOnline_Empty: {
      /*
       * While the intersection of the processor set specified by the
       * ``affinity_size`` and ``affinity`` parameters and the set of online
       * processors is empty.
       */
      CPU_ZERO( &ctx->cpuset_obj[ 0 ] );
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetOnline_NA:
      break;
  }
}

static void RtemsIntrReqSetAffinity_Pre_CPUSetHuge_Prepare(
  RtemsIntrReqSetAffinity_Context       *ctx,
  RtemsIntrReqSetAffinity_Pre_CPUSetHuge state
)
{
  switch ( state ) {
    case RtemsIntrReqSetAffinity_Pre_CPUSetHuge_NotZero: {
      /*
       * While the processor set specified by the ``affinity_size`` and
       * ``affinity`` parameters contains at least one processor which is not
       * storable in a processor set supported by the system.
       */
      /* Already prepared */
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetHuge_Zero: {
      /*
       * While the processor set specified by the ``affinity_size`` and
       * ``affinity`` parameters contains no processor which is not storable in
       * a processor set supported by the system.
       */
      CPU_ZERO( &ctx->cpuset_obj[ 1 ] );
      break;
    }

    case RtemsIntrReqSetAffinity_Pre_CPUSetHuge_NA:
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
    case RtemsIntrReqSetAffinity_Post_SetAffinity_Set: {
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
  ctx->Map.in_action_loop = false;
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
    ctx->cpuset == &ctx->cpuset_obj[ 0 ] && !CPU_EMPTY( &ctx->cpuset_obj[ 0 ] )
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
      ctx->status = RTEMS_SUCCESSFUL;
    }
  } else {
    cpu_set_t set;
    cpu_set_t set2;

    CPU_ZERO( &set );
    CPU_ZERO( &set2 );
    CPU_SET( 0, &set );
    CPU_SET( 0, &set2 );

    if ( ctx->valid_vector ) {
      ctx->vector = ctx->some_vector;

      sc = rtems_interrupt_get_affinity( ctx->vector, sizeof( set ), &set );
      T_true( sc == RTEMS_SUCCESSFUL || sc == RTEMS_UNSATISFIED );
    } else {
      ctx->vector = BSP_INTERRUPT_VECTOR_COUNT;
    }

    if ( !CPU_EMPTY( &ctx->cpuset_obj[ 0 ] ) ) {
      CPU_COPY( &set, &ctx->cpuset_obj[ 0 ] );
    }

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

static const RtemsIntrReqSetAffinity_Entry
RtemsIntrReqSetAffinity_Entries[] = {
  { 0, 0, 0, 1, 1, 0, 0, RtemsIntrReqSetAffinity_Post_Status_InvAddr,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 1, 1, 0, 1, RtemsIntrReqSetAffinity_Post_Status_InvAddr,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 1, 1, 0, 1, RtemsIntrReqSetAffinity_Post_Status_InvId,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 1, 0, 0, RtemsIntrReqSetAffinity_Post_Status_InvNum,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 1, 0, 0, 1, RtemsIntrReqSetAffinity_Post_Status_InvId,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqSetAffinity_Post_Status_InvNum,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 1, 0, 0, RtemsIntrReqSetAffinity_Post_Status_Ok,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Set },
  { 0, 0, 0, 0, 1, 0, 0, RtemsIntrReqSetAffinity_Post_Status_Unsat,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop },
  { 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqSetAffinity_Post_Status_Ok,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Set },
  { 0, 0, 0, 0, 0, 0, 0, RtemsIntrReqSetAffinity_Post_Status_Unsat,
    RtemsIntrReqSetAffinity_Post_SetAffinity_Nop }
};

static const uint8_t
RtemsIntrReqSetAffinity_Map[] = {
  3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 3, 3, 0, 0, 6, 7, 0, 0, 6, 7, 0, 0, 3, 3,
  0, 0, 3, 3, 0, 0, 8, 9, 0, 0, 8, 9, 0, 0, 5, 5, 0, 0, 5, 5, 0, 0, 2, 2, 1, 1,
  2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2, 1, 1, 2, 2,
  1, 1, 4, 4, 1, 1, 4, 4, 1, 1, 4, 4, 1, 1, 4, 4, 1, 1
};

static size_t RtemsIntrReqSetAffinity_Scope( void *arg, char *buf, size_t n )
{
  RtemsIntrReqSetAffinity_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      RtemsIntrReqSetAffinity_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
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

static inline RtemsIntrReqSetAffinity_Entry RtemsIntrReqSetAffinity_PopEntry(
  RtemsIntrReqSetAffinity_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsIntrReqSetAffinity_Entries[
    RtemsIntrReqSetAffinity_Map[ index ]
  ];
}

static void RtemsIntrReqSetAffinity_SetPreConditionStates(
  RtemsIntrReqSetAffinity_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];
  ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];

  if ( ctx->Map.entry.Pre_CPUSetOnline_NA ) {
    ctx->Map.pcs[ 2 ] = RtemsIntrReqSetAffinity_Pre_CPUSetOnline_NA;
  } else {
    ctx->Map.pcs[ 2 ] = ctx->Map.pci[ 2 ];
  }

  if ( ctx->Map.entry.Pre_CPUSetHuge_NA ) {
    ctx->Map.pcs[ 3 ] = RtemsIntrReqSetAffinity_Pre_CPUSetHuge_NA;
  } else {
    ctx->Map.pcs[ 3 ] = ctx->Map.pci[ 3 ];
  }

  ctx->Map.pcs[ 4 ] = ctx->Map.pci[ 4 ];

  if ( ctx->Map.entry.Pre_CanSetAffinity_NA ) {
    ctx->Map.pcs[ 5 ] = RtemsIntrReqSetAffinity_Pre_CanSetAffinity_NA;
  } else {
    ctx->Map.pcs[ 5 ] = ctx->Map.pci[ 5 ];
  }
}

static void RtemsIntrReqSetAffinity_TestVariant(
  RtemsIntrReqSetAffinity_Context *ctx
)
{
  RtemsIntrReqSetAffinity_Pre_Vector_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsIntrReqSetAffinity_Pre_CPUSetSize_Prepare( ctx, ctx->Map.pcs[ 1 ] );
  RtemsIntrReqSetAffinity_Pre_CPUSetOnline_Prepare( ctx, ctx->Map.pcs[ 2 ] );
  RtemsIntrReqSetAffinity_Pre_CPUSetHuge_Prepare( ctx, ctx->Map.pcs[ 3 ] );
  RtemsIntrReqSetAffinity_Pre_CPUSet_Prepare( ctx, ctx->Map.pcs[ 4 ] );
  RtemsIntrReqSetAffinity_Pre_CanSetAffinity_Prepare( ctx, ctx->Map.pcs[ 5 ] );
  RtemsIntrReqSetAffinity_Action( ctx );
  RtemsIntrReqSetAffinity_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsIntrReqSetAffinity_Post_SetAffinity_Check(
    ctx,
    ctx->Map.entry.Post_SetAffinity
  );
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

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = RtemsIntrReqSetAffinity_Pre_Vector_Valid;
    ctx->Map.pci[ 0 ] < RtemsIntrReqSetAffinity_Pre_Vector_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = RtemsIntrReqSetAffinity_Pre_CPUSetSize_Askew;
      ctx->Map.pci[ 1 ] < RtemsIntrReqSetAffinity_Pre_CPUSetSize_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      for (
        ctx->Map.pci[ 2 ] = RtemsIntrReqSetAffinity_Pre_CPUSetOnline_Valid;
        ctx->Map.pci[ 2 ] < RtemsIntrReqSetAffinity_Pre_CPUSetOnline_NA;
        ++ctx->Map.pci[ 2 ]
      ) {
        for (
          ctx->Map.pci[ 3 ] = RtemsIntrReqSetAffinity_Pre_CPUSetHuge_NotZero;
          ctx->Map.pci[ 3 ] < RtemsIntrReqSetAffinity_Pre_CPUSetHuge_NA;
          ++ctx->Map.pci[ 3 ]
        ) {
          for (
            ctx->Map.pci[ 4 ] = RtemsIntrReqSetAffinity_Pre_CPUSet_Valid;
            ctx->Map.pci[ 4 ] < RtemsIntrReqSetAffinity_Pre_CPUSet_NA;
            ++ctx->Map.pci[ 4 ]
          ) {
            for (
              ctx->Map.pci[ 5 ] = RtemsIntrReqSetAffinity_Pre_CanSetAffinity_Yes;
              ctx->Map.pci[ 5 ] < RtemsIntrReqSetAffinity_Pre_CanSetAffinity_NA;
              ++ctx->Map.pci[ 5 ]
            ) {
              ctx->Map.entry = RtemsIntrReqSetAffinity_PopEntry( ctx );
              RtemsIntrReqSetAffinity_SetPreConditionStates( ctx );
              RtemsIntrReqSetAffinity_Prepare( ctx );
              RtemsIntrReqSetAffinity_TestVariant( ctx );
            }
          }
        }
      }
    }
  }
}

/** @} */
