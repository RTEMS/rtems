/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspReqInterruptHandlerDispatchUnchecked
 */

/*
 * Copyright (C) 2021, 2023 embedded brains GmbH & Co. KG
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
 * @defgroup BspReqInterruptHandlerDispatchUnchecked \
 *   spec:/bsp/req/interrupt-handler-dispatch-unchecked
 *
 * @ingroup TestsuitesValidationIntr
 *
 * @{
 */

typedef enum {
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_Null,
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_Entry,
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_NA
} BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst;

typedef enum {
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_Null,
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_Entry,
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_NA
} BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain;

typedef enum {
  BspReqInterruptHandlerDispatchUnchecked_Post_Result_FatalError,
  BspReqInterruptHandlerDispatchUnchecked_Post_Result_Dispatch,
  BspReqInterruptHandlerDispatchUnchecked_Post_Result_NA
} BspReqInterruptHandlerDispatchUnchecked_Post_Result;

typedef enum {
  BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_SpuriousInterrupt,
  BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_NA
} BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource;

typedef enum {
  BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_Vector,
  BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_NA
} BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode;

typedef struct {
  uint8_t Skip : 1;
  uint8_t Pre_LoadFirst_NA : 1;
  uint8_t Pre_LoadFirstAgain_NA : 1;
  uint8_t Post_Result : 2;
  uint8_t Post_FatalSource : 1;
  uint8_t Post_FatalCode : 1;
} BspReqInterruptHandlerDispatchUnchecked_Entry;

/**
 * @brief Test context for spec:/bsp/req/interrupt-handler-dispatch-unchecked
 *   test case.
 */
typedef struct {
  /**
   * @brief This member references the interrupt entry to restore during test
   *   case teardown.
   */
  rtems_interrupt_entry *entry_to_restore;

  /**
   * @brief This member provides a jump buffer to return from the fatal error.
   */
  jmp_buf before_call;

  /**
   * @brief This member provides an interrupt entry to be dispatched.
   */
  rtems_interrupt_entry entry;

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

  /**
   * @brief This member references an interrupt entry for the first entry of
   *   the interrupt vector or is NULL.
   */
  rtems_interrupt_entry *first_again;

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
    BspReqInterruptHandlerDispatchUnchecked_Entry entry;

    /**
     * @brief If this member is true, then the current transition variant
     *   should be skipped.
     */
    bool skip;
  } Map;
} BspReqInterruptHandlerDispatchUnchecked_Context;

static BspReqInterruptHandlerDispatchUnchecked_Context
  BspReqInterruptHandlerDispatchUnchecked_Instance;

static const char * const BspReqInterruptHandlerDispatchUnchecked_PreDesc_LoadFirst[] = {
  "Null",
  "Entry",
  "NA"
};

static const char * const BspReqInterruptHandlerDispatchUnchecked_PreDesc_LoadFirstAgain[] = {
  "Null",
  "Entry",
  "NA"
};

static const char * const * const BspReqInterruptHandlerDispatchUnchecked_PreDesc[] = {
  BspReqInterruptHandlerDispatchUnchecked_PreDesc_LoadFirst,
  BspReqInterruptHandlerDispatchUnchecked_PreDesc_LoadFirstAgain,
  NULL
};

typedef BspReqInterruptHandlerDispatchUnchecked_Context Context;

static bool test_case_active;

static void Disable( const Context *ctx )
{
  (void) rtems_interrupt_vector_disable( ctx->test_vector );
}

static void ProcessInterrupt( Context *ctx )
{
  ctx->interrupt_occurred = true;
  CallWithinISRClear();
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

#if defined(RTEMS_SMP)
void __real_bsp_interrupt_spurious( rtems_vector_number vector );

void __wrap_bsp_interrupt_spurious( rtems_vector_number vector );

void __wrap_bsp_interrupt_spurious( rtems_vector_number vector )
{
  if ( test_case_active ) {
    Context *ctx;

    ctx = T_fixture_context();
    *ctx->first = ctx->first_again;
  }

  __real_bsp_interrupt_spurious( vector );
}
#endif

static void BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_Prepare(
  BspReqInterruptHandlerDispatchUnchecked_Context      *ctx,
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst state
)
{
  switch ( state ) {
    case BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_Null: {
      /*
       * While the first loaded value of the pointer to the first interrupt
       * entry of the interrupt vector specified by the ``vector`` parameter is
       * equal to NULL.
       */
      *ctx->first = NULL;
      break;
    }

    case BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_Entry: {
      /*
       * While the first loaded value of the pointer to the first interrupt
       * entry of the interrupt vector specified by the ``vector`` parameter
       * references an object of type rtems_interrupt_entry.
       */
      *ctx->first = &ctx->entry;
      break;
    }

    case BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_NA:
      break;
  }
}

static void BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_Prepare(
  BspReqInterruptHandlerDispatchUnchecked_Context           *ctx,
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain state
)
{
  switch ( state ) {
    case BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_Null: {
      /*
       * While the second loaded value of the pointer to the first interrupt
       * entry of the interrupt vector specified by the ``vector`` parameter is
       * equal to NULL.
       */
      ctx->first_again = NULL;
      break;
    }

    case BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_Entry: {
      /*
       * While the second loaded value of the pointer to the first interrupt
       * entry of the interrupt vector specified by the ``vector`` parameter
       * references an object of type rtems_interrupt_entry.
       */
      ctx->first_again = &ctx->entry;
      break;
    }

    case BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_NA:
      break;
  }
}

static void BspReqInterruptHandlerDispatchUnchecked_Post_Result_Check(
  BspReqInterruptHandlerDispatchUnchecked_Context    *ctx,
  BspReqInterruptHandlerDispatchUnchecked_Post_Result state
)
{
  switch ( state ) {
    case BspReqInterruptHandlerDispatchUnchecked_Post_Result_FatalError: {
      /*
       * A fatal error shall occur.
       */
      T_eq_u32( ctx->entry_counter, 0 );
      T_eq_u32( ctx->fatal_counter, 1 );
      break;
    }

    case BspReqInterruptHandlerDispatchUnchecked_Post_Result_Dispatch: {
      /*
       * The interrupt entries installed at the interrupt vector specified by
       * the ``vector`` parameter shall be dispatched.
       */
      T_eq_u32( ctx->entry_counter, 1 );
      T_eq_u32( ctx->fatal_counter, 0 );
      break;
    }

    case BspReqInterruptHandlerDispatchUnchecked_Post_Result_NA:
      break;
  }
}

static void BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_Check(
  BspReqInterruptHandlerDispatchUnchecked_Context         *ctx,
  BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource state
)
{
  switch ( state ) {
    case BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_SpuriousInterrupt: {
      /*
       * The fatal source shall be equal to
       * RTEMS_FATAL_SOURCE_SPURIOUS_INTERRUPT.
       */
      T_eq_int( ctx->fatal_source, RTEMS_FATAL_SOURCE_SPURIOUS_INTERRUPT );
      break;
    }

    case BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_NA:
      break;
  }
}

static void BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_Check(
  BspReqInterruptHandlerDispatchUnchecked_Context       *ctx,
  BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode state
)
{
  switch ( state ) {
    case BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_Vector: {
      /*
       * The fatal code shall be equal to the ``vector`` parameter.
       */
      T_eq_ulong( ctx->fatal_code, ctx->test_vector );
      break;
    }

    case BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_NA:
      break;
  }
}

static void BspReqInterruptHandlerDispatchUnchecked_Setup(
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx
)
{
  ctx->first = NULL;
  ctx->test_vector = CallWithinISRGetVector();
  T_assert_lt_u32( ctx->test_vector, BSP_INTERRUPT_VECTOR_COUNT );
  ctx->first = &bsp_interrupt_dispatch_table[
    bsp_interrupt_dispatch_index( ctx->test_vector )
  ];
  ctx->entry_to_restore = *ctx->first;

  rtems_interrupt_entry_initialize( &ctx->entry, EntryRoutine, ctx, "Info" );
  test_case_active = true;
  SetFatalHandler( Fatal, ctx );
}

static void BspReqInterruptHandlerDispatchUnchecked_Setup_Wrap( void *arg )
{
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  BspReqInterruptHandlerDispatchUnchecked_Setup( ctx );
}

static void BspReqInterruptHandlerDispatchUnchecked_Teardown(
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx
)
{
  SetFatalHandler( NULL, NULL );
  test_case_active = false;

  if ( ctx->first != NULL ) {
    *ctx->first = ctx->entry_to_restore;
  }
}

static void BspReqInterruptHandlerDispatchUnchecked_Teardown_Wrap( void *arg )
{
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx;

  ctx = arg;
  ctx->Map.in_action_loop = false;
  BspReqInterruptHandlerDispatchUnchecked_Teardown( ctx );
}

static void BspReqInterruptHandlerDispatchUnchecked_Action(
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx
)
{
  ctx->interrupt_occurred = false;
  ctx->entry_counter = 0;
  ctx->fatal_counter = 0;
  ctx->fatal_source = RTEMS_FATAL_SOURCE_LAST;
  ctx->fatal_code = UINT32_MAX;

  (void) rtems_interrupt_vector_enable( ctx->test_vector );

  CallWithinISRRaise();

  while ( !ctx->interrupt_occurred ) {
    /* Wait */
  }

  Disable( ctx );
}

static const BspReqInterruptHandlerDispatchUnchecked_Entry
BspReqInterruptHandlerDispatchUnchecked_Entries[] = {
  { 0, 0, 1, BspReqInterruptHandlerDispatchUnchecked_Post_Result_Dispatch,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_NA,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_NA },
#if defined(RTEMS_SMP)
  { 0, 0, 0, BspReqInterruptHandlerDispatchUnchecked_Post_Result_FatalError,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_SpuriousInterrupt,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_Vector },
#else
  { 0, 0, 1, BspReqInterruptHandlerDispatchUnchecked_Post_Result_FatalError,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_SpuriousInterrupt,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_Vector },
#endif
#if defined(RTEMS_SMP)
  { 0, 0, 0, BspReqInterruptHandlerDispatchUnchecked_Post_Result_Dispatch,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_NA,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_NA }
#else
  { 0, 0, 1, BspReqInterruptHandlerDispatchUnchecked_Post_Result_FatalError,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_SpuriousInterrupt,
    BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_Vector }
#endif
};

static const uint8_t
BspReqInterruptHandlerDispatchUnchecked_Map[] = {
  1, 2, 0, 0
};

static size_t BspReqInterruptHandlerDispatchUnchecked_Scope(
  void  *arg,
  char  *buf,
  size_t n
)
{
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx;

  ctx = arg;

  if ( ctx->Map.in_action_loop ) {
    return T_get_scope(
      BspReqInterruptHandlerDispatchUnchecked_PreDesc,
      buf,
      n,
      ctx->Map.pcs
    );
  }

  return 0;
}

static T_fixture BspReqInterruptHandlerDispatchUnchecked_Fixture = {
  .setup = BspReqInterruptHandlerDispatchUnchecked_Setup_Wrap,
  .stop = NULL,
  .teardown = BspReqInterruptHandlerDispatchUnchecked_Teardown_Wrap,
  .scope = BspReqInterruptHandlerDispatchUnchecked_Scope,
  .initial_context = &BspReqInterruptHandlerDispatchUnchecked_Instance
};

static inline BspReqInterruptHandlerDispatchUnchecked_Entry
BspReqInterruptHandlerDispatchUnchecked_PopEntry(
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx
)
{
  size_t index;

  index = ctx->Map.index;
  ctx->Map.index = index + 1;
  return BspReqInterruptHandlerDispatchUnchecked_Entries[
    BspReqInterruptHandlerDispatchUnchecked_Map[ index ]
  ];
}

static void BspReqInterruptHandlerDispatchUnchecked_SetPreConditionStates(
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx
)
{
  ctx->Map.pcs[ 0 ] = ctx->Map.pci[ 0 ];

  if ( ctx->Map.entry.Pre_LoadFirstAgain_NA ) {
    ctx->Map.pcs[ 1 ] = BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_NA;
  } else {
    ctx->Map.pcs[ 1 ] = ctx->Map.pci[ 1 ];
  }
}

static void BspReqInterruptHandlerDispatchUnchecked_TestVariant(
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx
)
{
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_Prepare(
    ctx,
    ctx->Map.pcs[ 0 ]
  );
  BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_Prepare(
    ctx,
    ctx->Map.pcs[ 1 ]
  );
  BspReqInterruptHandlerDispatchUnchecked_Action( ctx );
  BspReqInterruptHandlerDispatchUnchecked_Post_Result_Check(
    ctx,
    ctx->Map.entry.Post_Result
  );
  BspReqInterruptHandlerDispatchUnchecked_Post_FatalSource_Check(
    ctx,
    ctx->Map.entry.Post_FatalSource
  );
  BspReqInterruptHandlerDispatchUnchecked_Post_FatalCode_Check(
    ctx,
    ctx->Map.entry.Post_FatalCode
  );
}

/**
 * @fn void T_case_body_BspReqInterruptHandlerDispatchUnchecked( void )
 */
T_TEST_CASE_FIXTURE(
  BspReqInterruptHandlerDispatchUnchecked,
  &BspReqInterruptHandlerDispatchUnchecked_Fixture
)
{
  BspReqInterruptHandlerDispatchUnchecked_Context *ctx;

  ctx = T_fixture_context();
  ctx->Map.in_action_loop = true;
  ctx->Map.index = 0;

  for (
    ctx->Map.pci[ 0 ] = BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_Null;
    ctx->Map.pci[ 0 ] < BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirst_NA;
    ++ctx->Map.pci[ 0 ]
  ) {
    for (
      ctx->Map.pci[ 1 ] = BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_Null;
      ctx->Map.pci[ 1 ] < BspReqInterruptHandlerDispatchUnchecked_Pre_LoadFirstAgain_NA;
      ++ctx->Map.pci[ 1 ]
    ) {
      ctx->Map.entry = BspReqInterruptHandlerDispatchUnchecked_PopEntry( ctx );
      BspReqInterruptHandlerDispatchUnchecked_SetPreConditionStates( ctx );
      BspReqInterruptHandlerDispatchUnchecked_TestVariant( ctx );
    }
  }
}

/** @} */
