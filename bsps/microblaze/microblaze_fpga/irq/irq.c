/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMicroblaze
 *
 * @brief MicroBlaze interrupt support
 */

/*
 * Copyright (C) 2021 On-Line Applications Research Corporation (OAR)
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

#include <bsp.h>
#include <bsp/intc.h>
#include <bsp/irq-generic.h>

#include <rtems/score/cpu.h>

static volatile Microblaze_INTC *mblaze_intc;

static void ack_interrupt( uint8_t source )
{
  mblaze_intc->iar = 0x1 << source;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  attributes->is_maskable = true;
  attributes->maybe_enable = true;
  attributes->maybe_disable = true;
  attributes->can_clear = true;
  attributes->cleared_by_acknowledge = true;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( pending != NULL );
  *pending = false;
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_raise( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_clear( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  mblaze_intc->iar = 0x1 << vector;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( enabled != NULL );

  uint32_t mask = 1 << vector;

  *enabled = (mblaze_intc->ier & mask) != 0;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  uint32_t mask = 1 << vector;

  mblaze_intc->ier |= mask;

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  uint32_t mask = 1 << vector;

  mblaze_intc->ier &= ~mask;

  return RTEMS_SUCCESSFUL;
}

void bsp_interrupt_facility_initialize( void )
{
  /*
   * Enable HW interrupts on the interrupt controller. This happens before
   * interrupts are enabled on the processor.
   */
   mblaze_intc = (volatile Microblaze_INTC *) try_get_prop_from_device_tree(
    "xlnx,xps-intc-1.00.a",
    "reg",
    BSP_MICROBLAZE_FPGA_INTC_BASE
   );

  mblaze_intc->mer = MICROBLAZE_INTC_MER_ME | MICROBLAZE_INTC_MER_HIE;
}

void bsp_interrupt_dispatch( uint32_t source )
{
  uint32_t vector_number = 0;

  if ( source == 0xFF ) {
    /* Read interrupt controller to get the source */
    vector_number = mblaze_intc->isr & mblaze_intc->ier;

    /* Handle and the first interrupt that is set */
    uint8_t interrupt_status = 0;
    for ( int i = 0; i < 32; i++ ) {
      interrupt_status = vector_number >> i & 0x1;
      if ( interrupt_status != 0 ) {
        /* save current ILR */
        uint32_t interrupt_levels = mblaze_intc->ilr;
        /* set ILR to block out every interrupt less than or equal to priority of i */
        mblaze_intc->ilr = 0xFFFFFFFF >> (32 - i);
        bsp_interrupt_handler_dispatch( i );
        ack_interrupt( i );
        /* restore ILR */
        mblaze_intc->ilr = interrupt_levels;
        break;
      }
    }
  } else {
    vector_number = source;

    /* Fast interrupt mode. Handle interrupt. Ack happens automatically */
    bsp_interrupt_handler_dispatch( vector_number );
  }
}
