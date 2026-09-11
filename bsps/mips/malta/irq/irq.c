/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsMIPSMalta
 *
 * @brief This source file contains the interrupt controller support.
 */

/*
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

#include <bsp.h>
#include <bsp/irq.h>
#include <bsp/irq-generic.h>
#include <bsp/i8259.h>

#include <rtems/irq.h>
#include <rtems/score/mips.h>

/*
 * The eight interrupt sources of the processor occupy the first eight vectors
 * of the interrupt range.  They follow the order of the pending field of the
 * cause register.  The first two of them are the software interrupts, which a
 * write to that register raises and clears.  The other six arrive from outside
 * the processor and only their mask is writable.
 */
static bool malta_irq_is_cpu( rtems_vector_number vector )
{
  return vector >= MALTA_CPU_INT_START && vector <= MALTA_CPU_INT_LAST;
}

static bool malta_irq_is_software( rtems_vector_number vector )
{
  return vector == MALTA_CPU_INT_SW0 || vector == MALTA_CPU_INT_SW1;
}

static uint32_t malta_irq_cpu_bit( rtems_vector_number vector )
{
  return UINT32_C( 1 ) << ( 8 + ( vector - MALTA_CPU_INT_START ) );
}

static bool malta_irq_is_south_bridge( rtems_vector_number vector )
{
  return vector >= MALTA_SB_IRQ_START && vector <= MALTA_SB_IRQ_LAST;
}

static rtems_irq_number malta_irq_south_bridge_line(
  rtems_vector_number vector
)
{
  return (rtems_irq_number) ( vector - MALTA_SB_IRQ_START );
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  if ( vector < MIPS_INTERRUPT_BASE ) {
    /*
     * The vectors below the first interrupt are the exception causes of the
     * processor.  They are not interrupt sources and nothing masks them.
     */
    return RTEMS_SUCCESSFUL;
  }

  attributes->is_maskable = true;

  if ( malta_irq_is_cpu( vector ) ) {
    /*
     * The mask of the status register is global state of the controller, so a
     * disable of one of these sources survives a context switch.
     */
    attributes->can_enable = true;
    attributes->maybe_enable = true;
    attributes->can_disable = true;
    attributes->maybe_disable = true;

    if ( malta_irq_is_software( vector ) ) {
      attributes->can_raise = true;
      attributes->can_clear = true;
      attributes->cleared_by_acknowledge = false;
    }
  } else if ( malta_irq_is_south_bridge( vector ) ) {
    attributes->can_enable = true;
    attributes->maybe_enable = true;
    attributes->can_disable = true;
    attributes->maybe_disable = true;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( pending != NULL );

  if ( malta_irq_is_cpu( vector ) ) {
    uint32_t cause;

    mips_get_cause( cause );
    *pending = ( cause & malta_irq_cpu_bit( vector ) ) != 0;
  } else {
    /* An exception and a line of the south bridge are never pending */
    *pending = false;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise( rtems_vector_number vector )
{
  uint32_t              cause;
  rtems_interrupt_level level;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( !malta_irq_is_software( vector ) ) {
    return RTEMS_UNSATISFIED;
  }

  rtems_interrupt_local_disable( level );
  mips_get_cause( cause );
  cause |= malta_irq_cpu_bit( vector );
  mips_set_cause( cause );
  rtems_interrupt_local_enable( level );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_clear( rtems_vector_number vector )
{
  uint32_t              cause;
  rtems_interrupt_level level;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( !malta_irq_is_software( vector ) ) {
    return RTEMS_UNSATISFIED;
  }

  rtems_interrupt_local_disable( level );
  mips_get_cause( cause );
  cause &= ~malta_irq_cpu_bit( vector );
  mips_set_cause( cause );
  rtems_interrupt_local_enable( level );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( enabled != NULL );

  if ( malta_irq_is_cpu( vector ) ) {
    uint32_t sr;

    mips_get_sr( sr );
    *enabled = ( sr & malta_irq_cpu_bit( vector ) ) != 0;
  } else if ( malta_irq_is_south_bridge( vector ) ) {
    *enabled = BSP_irq_enabled_at_i8259s(
      malta_irq_south_bridge_line( vector )
    ) != 0;
  } else {
    *enabled = false;
    return RTEMS_UNSATISFIED;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( malta_irq_is_cpu( vector ) ) {
    uint32_t              sr;
    rtems_interrupt_level level;

    rtems_interrupt_local_disable( level );
    mips_get_sr( sr );
    sr |= malta_irq_cpu_bit( vector );
    mips_set_sr( sr );
    rtems_interrupt_local_enable( level );
  } else if ( malta_irq_is_south_bridge( vector ) ) {
    BSP_irq_enable_at_i8259s( malta_irq_south_bridge_line( vector ) );
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( malta_irq_is_cpu( vector ) ) {
    uint32_t              sr;
    rtems_interrupt_level level;

    rtems_interrupt_local_disable( level );
    mips_get_sr( sr );
    sr &= ~malta_irq_cpu_bit( vector );
    mips_set_sr( sr );
    rtems_interrupt_local_enable( level );
  } else if ( malta_irq_is_south_bridge( vector ) ) {
    BSP_irq_disable_at_i8259s( malta_irq_south_bridge_line( vector ) );
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t            priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  /* Neither the processor nor the south bridge orders its sources */
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t           *priority
)
{
  (void) vector;
  (void) priority;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( priority != NULL );

  return RTEMS_UNSATISFIED;
}
