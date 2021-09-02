/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSTestCaseBspReqInterruptSpurious
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

#include <setjmp.h>
#include <bsp/irq-generic.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup RTEMSTestCaseBspReqInterruptSpurious \
 *   spec:/bsp/req/interrupt-spurious
 *
 * @ingroup RTEMSTestSuiteTestsuitesValidationSmpOnly0
 *
 * @{
 */

typedef enum {
  BspReqInterruptSpurious_Pre_First_Null,
  BspReqInterruptSpurious_Pre_First_Entry,
  BspReqInterruptSpurious_Pre_First_NA
} BspReqInterruptSpurious_Pre_First;

typedef enum {
  BspReqInterruptSpurious_Post_Result_FatalError,
  BspReqInterruptSpurious_Post_Result_Dispatch,
  BspReqInterruptSpurious_Post_Result_NA
} BspReqInterruptSpurious_Post_Result;

typedef enum {
  BspReqInterruptSpurious_Post_FatalSource_SpuriousInterrupt,
  BspReqInterruptSpurious_Post_FatalSource_NA
} BspReqInterruptSpurious_Post_FatalSource;

typedef enum {
  BspReqInterruptSpurious_Post_FatalCode_Vector,
  BspReqInterruptSpurious_Post_FatalCode_NA
} BspReqInterruptSpurious_Post_FatalCode;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_First_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_FatalSource : 1;
  uint8_t Post_FatalCode : 1;
} BspReqInterruptSpurious_Entry;

/**
 * @brief Test context for spec:/bsp/req/interrupt-spurious test case.
 */
typedef struct {
  /**
   * @brief This member provides a jump buffer to return from the fatal error.
   */
  jmp_buf before_call;

  /**
   * @brief This member provides an interrupt entry to be dispatched.
   */
  rtems_interrupt_entry entry;

  /**
   * @brief If this member is true, then the interrupt shall be cleared.
   */
  bool do_clear;

  /**
   * @brief This member is true, then an interrupt occurred.
   */
  volatile bool interrupt_occurred;

  /**
   * @brief This member provides an entry dispatch counter.
   */
  uint32_t entry_counter;

  /**
   * @brief This member provides a fatal error counter.
   */
  uint32_t fatal_counter;

  /**
   * @brief This member contains the fatal source.
   */
  rtems_fatal_source fatal_source;

  /**
   * @brief This member contains a fatal code.
   */
  rtems_fatal_code fatal_code;

  /**
   * @brief This member contains the vector number of a testable interrupt.
   */
  rtems_vector_number test_vector;

  /**
   * @brief This member references the pointer to the first entry of the
   *   interrupt vector.
   */
  rtems_interrupt_entry **first;

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
    BspReqInterruptSpurious_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} BspReqInterruptSpurious_Context;

static BspReqInterruptSpurious_Context
  BspReqInterruptSpurious_Instance;

static const char * const BspReqInterruptSpurious_PreDesc_First[] = {
  "Null",
  "Entry",
  "NA"
};

static const char * const * const BspReqInterruptSpurious_PreDesc[] = {
  BspReqInterruptSpurious_PreDesc_First,
  NULL
};

typedef BspReqInterruptSpurious_Context Context;

static bool test_case_active;

static void Disable( const Context *ctx )
{
  (void) rtems_interrupt_vector_disable( ctx->test_vector );
}

static void ProcessInterrupt( Context *ctx )
{
  ctx->interrupt_occurred = true;

  if ( ctx->do_clear ) {
    rtems_status_code sc;

    sc = rtems_interrupt_clear( ctx->test_vector );
    T_rsc_success( sc );
  }

  Disable( ctx );
}

static void EntryRoutine( void *arg )
{
  Context *ctx;

  ctx = arg;
  ++ctx->entry_counter;
  ProcessInterrupt( ctx );
}

static void Fatal(
  rtems_fatal_source source,
  rtems_fatal_code   code,
  void              *arg
)
{
  Context *ctx;

  ctx = arg;
  ctx->fatal_source = source;
  ctx->fatal_code = code;
  ++ctx->fatal_counter;
  longjmp( ctx->before_call, 1 );
}

void __real_bsp_interrupt_handler_default( rtems_vector_number vector );

void __wrap_bsp_interrupt_handler_default( rtems_vector_number vector );

void __wrap_bsp_interrupt_handler_default( rtems_vector_number vector )
{
  if ( test_case_active ) {
    Context *ctx;

    ctx = T_fixture_context();
    ProcessInterrupt( ctx );

    if ( setjmp( ctx->before_call ) == 0 ) {
      __real_bsp_interrupt_handler_default( vector );
    }
  } else {
    __real_bsp_interrupt_handler_default( vector );
  }
}

static void BspReqInterruptSpurious_Pre_First_Prepare(
  BspReqInterruptSpurious_Context  *ctx,
  BspReqInterruptSpurious_Pre_First state
)
{
  switch ( state ) {
    case BspReqInterruptSpurious_Pre_First_Null: {
      /*
       * While the pointer to the first interrupt entry of the interrupt vector
       * specified by the ``vector`` parameter is equal to NULL.
       */
      *ctx->first = NULL;
      break;
    }

    case BspReqInterruptSpurious_Pre_First_Entry: {
      /*
       * While the pointer to the first interrupt entry of the interrupt vector
       * specified by the ``vector`` parameter references an object of type
       * rtems_interrupt_entry.
       */
      *ctx->first = &ctx->entry;
      break;
    }

    case BspReqInterruptSpurious_Pre_First_NA:
      break;
  }
}

static void BspReqInterruptSpurious_Post_Result_Check(
  BspReqInterruptSpurious_Context    *ctx,
  BspReqInterruptSpurious_Post_Result state
)
{
  switch ( state ) {
    case BspReqInterruptSpurious_Post_Result_FatalError: {
      /*
       * A fatal error shall occur.
       */
      T_eq_u32( ctx->entry_counter, 0 );
      T_eq_u32( ctx->fatal_counter, 1 );
      break;
    }

    case BspReqInterruptSpurious_Post_Result_Dispatch: {
      /*
       * The interrupt entries installed at the interrupt vector specified by
       * the ``vector`` parameter shall be dispatched.
       */
      T_eq_u32( ctx->entry_counter, 1 );
      T_eq_u32( ctx->fatal_counter, 0 );
      break;
    }

    case BspReqInterruptSpurious_Post_Result_NA:
      break;
  }
}

static void BspReqInterruptSpurious_Post_FatalSource_Check(
  BspReqInterruptSpurious_Context         *ctx,
  BspReqInterruptSpurious_Post_FatalSource state
)
{
  switch ( state ) {
    case BspReqInterruptSpurious_Post_FatalSource_SpuriousInterrupt: {
      /*
       * The fatal source shall be equal to
       * RTEMS_FATAL_SOURCE_SPURIOUS_INTERRUPT.
       */
      T_eq_int( ctx->fatal_source, RTEMS_FATAL_SOURCE_SPURIOUS_INTERRUPT );
      break;
    }

    case BspReqInterruptSpurious_Post_FatalSource_NA:
      break;
  }
}

static void BspReqInterruptSpurious_Post_FatalCode_Check(
  BspReqInterruptSpurious_Context       *ctx,
  BspReqInterruptSpurious_Post_FatalCode state
)
{
  switch ( state ) {
    case BspReqInterruptSpurious_Post_FatalCode_Vector: {
      /*
       * The fatal code shall be equal to the ``vector`` parameter.
       */
      T_eq_ulong( ctx->fatal_code, ctx->test_vector );
      break;
    }

    case BspReqInterruptSpurious_Post_FatalCode_NA:
      break;
  }
}

static void BspReqInterruptSpurious_Setup(
  BspReqInterruptSpurious_Context *ctx
)
{
  rtems_interrupt_attributes attr = {
    .can_raise = true
  };
  rtems_status_code sc;

  ctx->test_vector = GetTestableInterruptVector( &attr );
  T_assert_lt_u32( ctx->test_vector, BSP_INTERRUPT_VECTOR_COUNT );
  ctx->first = &bsp_interrupt_handler_table[
    bsp_interrupt_handler_index( ctx->test_vector )
  ];

  sc = rtems_interrupt_get_attributes( ctx->test_vector, &attr );
  T_rsc_success( sc );
  ctx->do_clear = attr.can_clear && !attr.cleared_by_acknowledge;

  rtems_interrupt_entry_initialize( &ctx->entry, EntryRoutine, ctx, "Info" );
  test_case_active = true;
  SetFatalHandler( Fatal, ctx );
}

static void BspReqInterruptSpurious_Setup_Wrap( void *arg )
{
  BspReqInterruptSpurious_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  BspReqInterruptSpurious_Setup( ctx );
}

static void BspReqInterruptSpurious_Teardown(
  BspReqInterruptSpurious_Context *ctx
)
{
  SetFatalHandler( NULL, NULL );
  test_case_active = false;
}

static void BspReqInterruptSpurious_Teardown_Wrap( void *arg )
{
  BspReqInterruptSpurious_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  BspReqInterruptSpurious_Teardown( ctx );
}

static void BspReqInterruptSpurious_Action(
  BspReqInterruptSpurious_Context *ctx
)
{
  ctx->interrupt_occurred = false;
  ctx->entry_counter = 0;
  ctx->fatal_counter = 0;
  ctx->fatal_source = RTEMS_FATAL_SOURCE_LAST;
  ctx->fatal_code = UINT32_MAX;

  if ( *ctx->first == NULL ) {
    rtems_status_code sc;

    (void) rtems_interrupt_vector_enable( ctx->test_vector );

    sc = rtems_interrupt_raise( ctx->test_vector );
    T_rsc_success( sc );

    while ( !ctx->interrupt_occurred ) {
      /* Wait */
    }

    Disable( ctx );
  } else {
    bsp_interrupt_spurious( ctx->test_vector );
  }
}

static const BspReqInterruptSpurious_Entry
BspReqInterruptSpurious_Entries[] = {
  { 0, 0, BspReqInterruptSpurious_Post_Result_FatalError,
    BspReqInterruptSpurious_Post_FatalSource_SpuriousInterrupt,
    BspReqInterruptSpurious_Post_FatalCode_Vector },
  { 0, 0, BspReqInterruptSpurious_Post_Result_Dispatch,
    BspReqInterruptSpurious_Post_FatalSource_NA,
    BspReqInterruptSpurious_Post_FatalCode_NA }
};

static const uint8_t
BspReqInterruptSpurious_Map[] = {
  0, 1
};

static size_t BspReqInterruptSpurious_Scope( void *arg, char *buf, size_t n )
{
  BspReqInterruptSpurious_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      BspReqInterruptSpurious_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture BspReqInterruptSpurious_Fixture = {
  .setup = BspReqInterruptSpurious_Setup_Wrap,
  .stop = NULL,
  .teardown = BspReqInterruptSpurious_Teardown_Wrap,
  .scope = BspReqInterruptSpurious_Scope,
  .initial_context = &BspReqInterruptSpurious_Instance
};

static inline BspReqInterruptSpurious_Entry BspReqInterruptSpurious_PopEntry(
  BspReqInterruptSpurious_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return BspReqInterruptSpurious_Entries[
    BspReqInterruptSpurious_Map[ index ]
  ];
}

static void BspReqInterruptSpurious_TestVariant(
  BspReqInterruptSpurious_Context *ctx
)
{
  BspReqInterruptSpurious_Pre_First_Prepare( ctx, ctx->Map.pcs[ 0 ] );
  BspReqInterruptSpurious_Action( ctx );
  BspReqInterruptSpurious_Post_Result_Check( ctx, ctx->Map.entry.Post_Result );
  BspReqInterruptSpurious_Post_FatalSource_Check(
    ctx,
    ctx->Map.entry.Post_FatalSource
  );
  BspReqInterruptSpurious_Post_FatalCode_Check(
    ctx,
    ctx->Map.entry.Post_FatalCode
  );
}

/**
 * @fn void T_case_body_BspReqInterruptSpurious( void )
 */
T_TEST_CASE_FIXTURE(
  BspReqInterruptSpurious,
  &BspReqInterruptSpurious_Fixture
)
{
  BspReqInterruptSpurious_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pcs[ 0 ] = BspReqInterruptSpurious_Pre_First_Null;
    ctx->Map.pcs[ 0 ] < BspReqInterruptSpurious_Pre_First_NA;
    ++ctx->Map.pcs[ 0 ]
  ) {
    ctx->Map.entry = BspReqInterruptSpurious_PopEntry( ctx );
    BspReqInterruptSpurious_TestVariant( ctx );
  }
}

/** @} */
