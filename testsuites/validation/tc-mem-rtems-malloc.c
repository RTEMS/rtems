/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RtemsMallocReqMalloc
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

#include <rtems/malloc.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RtemsMallocReqMalloc spec:/rtems/malloc/req/malloc
 *
 * @ingroup TestsuitesValidation0
 *
 * @{
 */

typedef enum {
  RtemsMallocReqMalloc_Pre_Size_Huge,
  RtemsMallocReqMalloc_Pre_Size_Zero,
  RtemsMallocReqMalloc_Pre_Size_Valid,
  RtemsMallocReqMalloc_Pre_Size_NA
} RtemsMallocReqMalloc_Pre_Size;

typedef enum {
  RtemsMallocReqMalloc_Post_Status_Null,
  RtemsMallocReqMalloc_Post_Status_AreaBegin,
  RtemsMallocReqMalloc_Post_Status_NA
} RtemsMallocReqMalloc_Post_Status;

typedef enum {
  RtemsMallocReqMalloc_Post_Alignment_Valid,
  RtemsMallocReqMalloc_Post_Alignment_NA
} RtemsMallocReqMalloc_Post_Alignment;

typedef enum {
  RtemsMallocReqMalloc_Post_Size_Valid,
  RtemsMallocReqMalloc_Post_Size_NA
} RtemsMallocReqMalloc_Post_Size;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_Size_NA : 1;
  uint8_t Post_Status : 2;
  uint8_t Post_Alignment : 1;
  uint8_t Post_Size : 1;
} RtemsMallocReqMalloc_Entry;

/**
 * @brief Test context for spec:/rtems/malloc/req/malloc test case.
 */
typedef struct {
  /**
   * @brief This member provides a memory support context.
   */
  MemoryContext mem_ctx;

  /**
   * @brief This member contains the return value of the rtems_malloc() call.
   */
  void *ptr;

  /**
   * @brief This member specifies if the ``size`` parameter value.
   */
  size_t size;

  struct {
    /**
     * @brief This member defines the pre-condition states for the next action.
     */
    size_t pcs[ 1 ];

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
    RtemsMallocReqMalloc_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} RtemsMallocReqMalloc_Context;

static RtemsMallocReqMalloc_Context
  RtemsMallocReqMalloc_Instance;

static const char * const RtemsMallocReqMalloc_PreDesc_Size[] = {
  "Huge",
  "Zero",
  "Valid",
  "NA"
};

static const char * const * const RtemsMallocReqMalloc_PreDesc[] = {
  RtemsMallocReqMalloc_PreDesc_Size,
  NULL
};

static void RtemsMallocReqMalloc_Pre_Size_Prepare(
  RtemsMallocReqMalloc_Context *ctx,
  RtemsMallocReqMalloc_Pre_Size state
)
{
  switch ( state ) {
    case RtemsMallocReqMalloc_Pre_Size_Huge: {
      /*
       * While the ``size`` parameter is not equal to zero, while the ``size``
       * parameter is too large to allocate a memory area with the specified
       * size.
       */
      ctx->size = SIZE_MAX;
      break;
    }

    case RtemsMallocReqMalloc_Pre_Size_Zero: {
      /*
       * While the ``size`` parameter is equal to zero.
       */
      ctx->size = 0;
      break;
    }

    case RtemsMallocReqMalloc_Pre_Size_Valid: {
      /*
       * While the ``size`` parameter is not equal to zero, while the ``size``
       * parameter is small enough to allocate a memory area with the specified
       * size.
       */
      ctx->size = 1;
      break;
    }

    case RtemsMallocReqMalloc_Pre_Size_NA:
      break;
  }
}

static void RtemsMallocReqMalloc_Post_Status_Check(
  RtemsMallocReqMalloc_Context    *ctx,
  RtemsMallocReqMalloc_Post_Status state
)
{
  switch ( state ) {
    case RtemsMallocReqMalloc_Post_Status_Null: {
      /*
       * The return value of rtems_malloc() shall be equal to NULL.
       */
      T_null( ctx->ptr );
      break;
    }

    case RtemsMallocReqMalloc_Post_Status_AreaBegin: {
      /*
       * The return value of rtems_malloc() shall be equal to the begin address
       * of the allocated memory area.
       */
      T_not_null( ctx->ptr );
      break;
    }

    case RtemsMallocReqMalloc_Post_Status_NA:
      break;
  }
}

static void RtemsMallocReqMalloc_Post_Alignment_Check(
  RtemsMallocReqMalloc_Context       *ctx,
  RtemsMallocReqMalloc_Post_Alignment state
)
{
  switch ( state ) {
    case RtemsMallocReqMalloc_Post_Alignment_Valid: {
      /*
       * The begin address of the allocated memory area shall be an integral
       * multiple of the heap alignment of the target architecture.
       */
      T_eq_uptr( (uintptr_t) ctx->ptr % CPU_HEAP_ALIGNMENT, 0 );
      break;
    }

    case RtemsMallocReqMalloc_Post_Alignment_NA:
      break;
  }
}

static void RtemsMallocReqMalloc_Post_Size_Check(
  RtemsMallocReqMalloc_Context  *ctx,
  RtemsMallocReqMalloc_Post_Size state
)
{
  void     *ptr;
  uintptr_t a;
  uintptr_t b;
  uintptr_t size;

  switch ( state ) {
    case RtemsMallocReqMalloc_Post_Size_Valid: {
      /*
       * The size of the allocated memory area shall greater than or equal to
       * the ``size`` parameter.
       */
      /* Assume that the next allocation is done from adjacent memory */
      ptr = ctx->ptr;
      ctx->ptr = rtems_malloc( ctx->size );
      T_not_null( ptr );
      a = (uintptr_t) ptr;
      b = (uintptr_t) ctx->ptr;
      size = a < b ? b - a : a - b;
      T_ge_uptr( size, ctx->size );
      break;
    }

    case RtemsMallocReqMalloc_Post_Size_NA:
      break;
  }
}

static void RtemsMallocReqMalloc_Setup( RtemsMallocReqMalloc_Context *ctx )
{
  MemorySave( &ctx->mem_ctx );
}

static void RtemsMallocReqMalloc_Setup_Wrap( void *arg )
{
  RtemsMallocReqMalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMallocReqMalloc_Setup( ctx );
}

static void RtemsMallocReqMalloc_Teardown( RtemsMallocReqMalloc_Context *ctx )
{
  MemoryRestore( &ctx->mem_ctx );
}

static void RtemsMallocReqMalloc_Teardown_Wrap( void *arg )
{
  RtemsMallocReqMalloc_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  RtemsMallocReqMalloc_Teardown( ctx );
}

static void RtemsMallocReqMalloc_Action( RtemsMallocReqMalloc_Context *ctx )
{
  ctx->ptr = rtems_malloc( ctx->size );
}

static const RtemsMallocReqMalloc_Entry
RtemsMallocReqMalloc_Entries[] = {
  { 0, 0, RtemsMallocReqMalloc_Post_Status_Null,
    RtemsMallocReqMalloc_Post_Alignment_Valid,
    RtemsMallocReqMalloc_Post_Size_NA },
  { 0, 0, RtemsMallocReqMalloc_Post_Status_AreaBegin,
    RtemsMallocReqMalloc_Post_Alignment_Valid,
    RtemsMallocReqMalloc_Post_Size_Valid }
};

static const uint8_t
RtemsMallocReqMalloc_Map[] = {
  0, 0, 1
};

static size_t RtemsMallocReqMalloc_Scope( void *arg, char *buf, size_t n )
{
  RtemsMallocReqMalloc_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope( RtemsMallocReqMalloc_PreDesc, buf, n, ctx->Map.pcs );
  }

  return 0;
}

static T_fixture RtemsMallocReqMalloc_Fixture = {
  .setup = RtemsMallocReqMalloc_Setup_Wrap,
  .stop = NULL,
  .teardown = RtemsMallocReqMalloc_Teardown_Wrap,
  .scope = RtemsMallocReqMalloc_Scope,
  .initial_context = &RtemsMallocReqMalloc_Instance
};

static inline RtemsMallocReqMalloc_Entry RtemsMallocReqMalloc_PopEntry(
  RtemsMallocReqMalloc_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return RtemsMallocReqMalloc_Entries[
    RtemsMallocReqMalloc_Map[ index ]
  ];
}

static void RtemsMallocReqMalloc_TestVariant(
  RtemsMallocReqMalloc_Context *ctx
)
{
  RtemsMallocReqMalloc_Pre_Size_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  RtemsMallocReqMalloc_Action( ctx );
  RtemsMallocReqMalloc_Post_Status_Check( ctx, ctx->Map.entry.Post_Status );
  RtemsMallocReqMalloc_Post_Alignment_Check(
    ctx,
    ctx->Map.entry.Post_Alignment
  );
  RtemsMallocReqMalloc_Post_Size_Check( ctx, ctx->Map.entry.Post_Size );
}

/**
 * @fn void T_case_body_RtemsMallocReqMalloc( void )
 */
T_TEST_CASE_FIXTURE( RtemsMallocReqMalloc, &RtemsMallocReqMalloc_Fixture )
{
  RtemsMallocReqMalloc_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = RtemsMallocReqMalloc_Pre_Size_Huge;
    ctx->Map.pcs[ 0 ] < RtemsMallocReqMalloc_Pre_Size_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = RtemsMallocReqMalloc_PopEntry( ctx );
    RtemsMallocReqMalloc_TestVariant( ctx );
  }
}

/** @} */
