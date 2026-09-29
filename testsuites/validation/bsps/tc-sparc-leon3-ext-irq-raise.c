/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValExtIrqRaise
 */

/*
 * Copyright (C) 2026 Critical Software S.A
 * Copyright (C) 2026 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <bsp.h>
#include <bsp/irq-generic.h>
#include <bsp/irq.h>
#include <bsp/leon3.h>
#include <grlib/io.h>
#include <rtems/irq-extension.h>

#include "tx-support.h"

#include <rtems/test.h>

/**
 * @defgroup BspSparcLeon3ValExtIrqRaise \
 *   spec:/bsp/sparc/leon3/val/ext-irq-raise
 *
 * @ingroup TestsuitesBspsValidationBsp0
 *
 * @brief This test case validates the extended interrupt raise path for LEON3
 *   BSPs.
 *
 * The test raises an extended controller line through a read-modify-write of
 * the pending register.  This is safe only in the test, since no peripheral
 * drives the tested bus lines.
 *
 * This test case performs the following actions:
 *
 * - Iterate through each interrupt vector whose bus line maps to an extended
 *   controller line and which has no interrupt handler installed.  Get the
 *   attributes of the vector and try to raise it through the directive.
 *   Install an interrupt handler, enable the vector, raise the extended
 *   controller line through the pending register and wait for the handler.
 *   Clear and disable the vector and remove the handler.
 *
 *   - Check that the test exercised each extended controller line.
 *
 *   - Check that the attributes of each tested interrupt vector state that the
 *     vector can be enabled, disabled and cleared, and that it can neither be
 *     raised nor be raised on a processor.
 *
 *   - Check that the directive rejected the raise of each tested interrupt
 *     vector and that the pending state stayed unchanged.
 *
 *   - Check that the pending extended controller line called the interrupt
 *     handler of each tested interrupt vector exactly once.
 *
 * @{
 */

/**
 * @brief Test context for spec:/bsp/sparc/leon3/val/ext-irq-raise test case.
 */
typedef struct {
  /**
   * @brief This member contains the count of tested interrupt vectors.
   */
  uint32_t tested_vectors;

  /**
   * @brief This member contains the set of the tested controller lines.
   */
  uint32_t tested_controller_lines;

  /**
   * @brief This member contains the count of tested interrupt vectors which
   *   the BSP reported as not raisable.
   */
  uint32_t not_raisable;

  /**
   * @brief This member contains the count of tested interrupt vectors whose
   *   raise the directive rejected.
   */
  uint32_t raise_rejected;

  /**
   * @brief This member contains the count of tested interrupt vectors whose
   *   interrupt handler the BSP dispatched.
   */
  uint32_t dispatched;

  /**
   * @brief This member contains the call count of the interrupt handler.
   */
  volatile uint32_t call_count;

  /**
   * @brief This member contains the interrupt vector which the test expects.
   */
  rtems_vector_number expected_vector;

  /**
   * @brief This member contains the interrupt vector which the handler saw.
   */
  rtems_vector_number handled_vector;
} BspSparcLeon3ValExtIrqRaise_Context;

static BspSparcLeon3ValExtIrqRaise_Context
  BspSparcLeon3ValExtIrqRaise_Instance;

typedef BspSparcLeon3ValExtIrqRaise_Context Context;

static void Handler( void *arg )
{
  Context *ctx;

  ctx = arg;
  ctx->handled_vector = ctx->expected_vector;
  ++ctx->call_count;
}

static void UnsafeRaiseExtendedInterrupt( rtems_vector_number controller_line )
{
  rtems_interrupt_lock_context lock_context;
  uint32_t                     ipend;
  irqamp                      *regs;

  regs = LEON3_IrqCtrl_Regs;
  LEON3_IRQCTRL_ACQUIRE( &lock_context );
  ipend = grlib_load_32( &regs->ipend );
  ipend |= 1U << controller_line;
  grlib_store_32( &regs->ipend, ipend );
  LEON3_IRQCTRL_RELEASE( &lock_context );
}

static T_fixture BspSparcLeon3ValExtIrqRaise_Fixture = {
  .setup = NULL,
  .stop = NULL,
  .teardown = NULL,
  .scope = NULL,
  .initial_context = &BspSparcLeon3ValExtIrqRaise_Instance
};

/**
 * @brief Iterate through each interrupt vector whose bus line maps to an
 *   extended controller line and which has no interrupt handler installed.
 *   Get the attributes of the vector and try to raise it through the
 *   directive. Install an interrupt handler, enable the vector, raise the
 *   extended controller line through the pending register and wait for the
 *   handler. Clear and disable the vector and remove the handler.
 */
static void BspSparcLeon3ValExtIrqRaise_Action_0(
  BspSparcLeon3ValExtIrqRaise_Context *ctx
)
{
  rtems_vector_number vector;

  for ( vector = 0; vector < BSP_INTERRUPT_VECTOR_COUNT; ++vector ) {
    rtems_vector_number        controller_line;
    rtems_status_code          sc;
    rtems_interrupt_attributes attr;
    bool                       pending_before;
    bool                       pending_after;
    rtems_interrupt_entry      entry;

    if ( !bsp_interrupt_is_valid_vector( vector ) ) {
      continue;
    }

    controller_line = leon3_irqmap_get_unchecked( vector );

    if ( controller_line <= BSP_INTERRUPT_VECTOR_MAX_STD ) {
      continue;
    }

    if ( HasInterruptVectorEntriesInstalled( vector ) ) {
      continue;
    }

    ++ctx->tested_vectors;
    ctx->tested_controller_lines |= 1U << controller_line;

    attr = (rtems_interrupt_attributes) { 0 };
    sc = rtems_interrupt_get_attributes( vector, &attr );
    T_rsc_success( sc );

    if (
      !attr.can_raise && !attr.can_raise_on && attr.can_enable &&
      attr.can_disable && attr.can_clear
    ) {
      ++ctx->not_raisable;
    }

    pending_before = true;
    sc = rtems_interrupt_is_pending( vector, &pending_before );
    T_rsc_success( sc );

    sc = rtems_interrupt_raise( vector );

    pending_after = !pending_before;
    (void) rtems_interrupt_is_pending( vector, &pending_after );

    if ( sc == RTEMS_UNSATISFIED && pending_after == pending_before ) {
      ++ctx->raise_rejected;
    }

    ctx->call_count = 0;
    ctx->handled_vector = UINT32_MAX;
    ctx->expected_vector = vector;

    rtems_interrupt_entry_initialize( &entry, Handler, ctx, "Extended IRQ" );
    sc = rtems_interrupt_entry_install(
      vector,
      RTEMS_INTERRUPT_UNIQUE,
      &entry
    );
    T_rsc_success( sc );

    sc = rtems_interrupt_vector_enable( vector );
    T_rsc_success( sc );

    UnsafeRaiseExtendedInterrupt( controller_line );

    while ( ctx->call_count == 0 ) {
      /* Wait */
    }

    if ( ctx->call_count == 1 && ctx->handled_vector == vector ) {
      ++ctx->dispatched;
    }

    sc = rtems_interrupt_clear( vector );
    T_rsc_success( sc );

    sc = rtems_interrupt_vector_disable( vector );
    T_rsc_success( sc );

    sc = rtems_interrupt_entry_remove( vector, &entry );
    T_rsc_success( sc );
  }

  /*
   * Check that the test exercised each extended controller line.
   */
  T_eq_u32( ctx->tested_controller_lines, 0xffff0000 );

  /*
   * Check that the attributes of each tested interrupt vector state that the
   * vector can be enabled, disabled and cleared, and that it can neither be
   * raised nor be raised on a processor.
   */
  T_eq_u32( ctx->not_raisable, ctx->tested_vectors );

  /*
   * Check that the directive rejected the raise of each tested interrupt
   * vector and that the pending state stayed unchanged.
   */
  T_eq_u32( ctx->raise_rejected, ctx->tested_vectors );

  /*
   * Check that the pending extended controller line called the interrupt
   * handler of each tested interrupt vector exactly once.
   */
  T_eq_u32( ctx->dispatched, ctx->tested_vectors );
}

/**
 * @fn void T_case_body_BspSparcLeon3ValExtIrqRaise( void )
 */
T_TEST_CASE_FIXTURE(
  BspSparcLeon3ValExtIrqRaise,
  &BspSparcLeon3ValExtIrqRaise_Fixture
)
{
  BspSparcLeon3ValExtIrqRaise_Context *ctx;

  ctx = T_fixture_context();

  BspSparcLeon3ValExtIrqRaise_Action_0( ctx );
}

/** @} */
