/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup BspSparcLeon3ValExtIrqRaise
 */

/*
 * Copyright (C) 2026 Critical Software S.A
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
 * This test case performs the following actions:
 *
 * - Iterate through each testable extended interrupt vector (16 to 31).
 *
 *   - Check that API raise operations are not available for extended vectors.
 *
 *   - Check that rtems_interrupt_raise() returns RTEMS_UNSATISFIED and does
 *     not alter the pending state.
 *
 *   - Raise the vector using an unsafe test-only support operation.
 *
 *   - Check that the interrupt handler installed for the vector is called.
 *
 * @{
 */

typedef struct {
  volatile uint32_t   call_count;
  rtems_vector_number expected_vector;
  rtems_vector_number handled_vector;
} BspSparcLeon3ValExtIrqRaise_Context;

static void BspSparcLeon3ValExtIrqRaise_Handler( void *arg )
{
  BspSparcLeon3ValExtIrqRaise_Context *ctx;

  ctx = arg;
  ctx->handled_vector = ctx->expected_vector;
  ++ctx->call_count;
}

static rtems_status_code UnsafeRaiseExtendedInterrupt(
  rtems_vector_number vector
)
{
  rtems_interrupt_lock_context lock_context;
  uint32_t                     bit;
  uint32_t                     ipend;
  irqamp                      *regs;

  regs = LEON3_IrqCtrl_Regs;
  bit = 1U << vector;

  /*
   * This deliberately uses an unsafe read-modify-write sequence to exercise
   * the extended interrupt raise path in BSP-specific validation tests.
   */
  LEON3_IRQCTRL_ACQUIRE( &lock_context );
  ipend = grlib_load_32( &regs->ipend );
  ipend |= bit;
  grlib_store_32( &regs->ipend, ipend );
  LEON3_IRQCTRL_RELEASE( &lock_context );

  return RTEMS_SUCCESSFUL;
}

/**
 * @fn void T_case_body_BspSparcLeon3ValExtIrqRaise( void )
 */
T_TEST_CASE( BspSparcLeon3ValExtIrqRaise )
{
  rtems_vector_number vector;
  uint32_t            tested_vectors = 0;

  for (
    vector = BSP_INTERRUPT_VECTOR_MAX_STD + 1;
    vector < BSP_INTERRUPT_VECTOR_COUNT;
    ++vector
  ) {
    rtems_status_code                    sc;
    rtems_interrupt_attributes           attr;
    bool                                 pending_before;
    bool                                 pending_after;
    rtems_interrupt_entry                entry;
    BspSparcLeon3ValExtIrqRaise_Context  ctx;

    if ( !bsp_interrupt_is_valid_vector( vector ) ) {
      continue;
    }

    if ( HasInterruptVectorEntriesInstalled( vector ) ) {
      continue;
    }

    attr = (rtems_interrupt_attributes) { 0 };
    sc = rtems_interrupt_get_attributes( vector, &attr );
    T_rsc_success( sc );

    T_false( attr.can_raise );
    T_false( attr.can_raise_on );
    T_true( attr.can_enable );
    T_true( attr.can_disable );
    T_true( attr.can_clear );

    pending_before = true;
    sc = rtems_interrupt_is_pending( vector, &pending_before );
    T_rsc_success( sc );

    /* The API must reject software raises of extended interrupt vectors. */
    sc = rtems_interrupt_raise( vector );
    T_rsc( sc, RTEMS_UNSATISFIED );

    pending_after = !pending_before;
    sc = rtems_interrupt_is_pending( vector, &pending_after );
    T_rsc_success( sc );
    T_eq( pending_after, pending_before );

    ctx.call_count = 0;
    ctx.expected_vector = vector;
    ctx.handled_vector = UINT32_MAX;

    rtems_interrupt_entry_initialize(
      &entry,
      BspSparcLeon3ValExtIrqRaise_Handler,
      &ctx,
      "Extended IRQ"
    );

    sc = rtems_interrupt_entry_install( vector, RTEMS_INTERRUPT_UNIQUE, &entry );
    T_rsc_success( sc );

    sc = rtems_interrupt_vector_enable( vector );
    T_rsc_success( sc );

    /* 
     * Raise the extended interrupt vector using the unsafe test-only
     * support operation.
     */
    sc = UnsafeRaiseExtendedInterrupt( vector );
    T_rsc_success( sc );

    while ( ctx.call_count == 0 ) {
      /* Wait */
    }

    T_eq_u32( ctx.handled_vector, vector );

    sc = rtems_interrupt_clear( vector );
    T_rsc_success( sc );

    sc = rtems_interrupt_vector_disable( vector );
    T_rsc_success( sc );

    sc = rtems_interrupt_entry_remove( vector, &entry );
    T_rsc_success( sc );

    pending_after = true;
    sc = rtems_interrupt_is_pending( vector, &pending_after );
    T_rsc_success( sc );
    T_false( pending_after );

    ++tested_vectors;
  }

  /* Ensure that all extended vectors were effectively exercised. */
  T_eq(
    tested_vectors,
    BSP_INTERRUPT_VECTOR_MAX_EXT - BSP_INTERRUPT_VECTOR_MAX_STD
  );
}

/** @} */
