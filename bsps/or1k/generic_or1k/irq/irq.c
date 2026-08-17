/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief This source file contains the interrupt controller support of the
 *   generic_or1k BSP.
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

#include <bsp/generic_or1k.h>
#include <bsp/irq-generic.h>

#include <rtems/score/cpu.h>
#include <rtems/score/or1k-utility.h>

/*
 * The interrupt controller of the architecture cannot raise an interrupt in
 * software, its status register can only be cleared.  The transmit holding
 * register of the UART is always empty, because the console driver polls, so
 * enabling the interrupt of that condition raises the interrupt of the UART
 * and disabling it takes the interrupt back.
 */
#define GENERIC_OR1K_UART_VECTOR 2

static uint32_t generic_or1k_vector_bit( rtems_vector_number vector )
{
  return UINT32_C( 1 ) << vector;
}

void bsp_interrupt_dispatch( uint32_t vector, CPU_Exception_frame *frame );

void bsp_interrupt_facility_initialize( void )
{
  CPU_ISR_handler old;

  _OR1K_mtspr( CPU_OR1K_SPR_PICMR, 0 );
  _OR1K_mtspr( CPU_OR1K_SPR_PICSR, 0xffffffff );

  /*
   * The exception handlers of this architecture take the vector number and the
   * frame of the interrupted context.
   */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wcast-function-type"
  _CPU_ISR_install_vector(
    OR1K_EXCEPTION_IRQ,
    (CPU_ISR_handler) bsp_interrupt_dispatch,
    &old
  );
#pragma GCC diagnostic pop
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  attributes->is_maskable = true;
  attributes->can_enable = true;
  attributes->maybe_enable = true;
  attributes->can_disable = true;
  attributes->maybe_disable = true;
  attributes->can_clear = true;
  attributes->can_raise = vector == GENERIC_OR1K_UART_VECTOR;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_is_pending(
  rtems_vector_number vector,
  bool               *pending
)
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( pending != NULL );
  *pending = ( _OR1K_mfspr( CPU_OR1K_SPR_PICSR ) &
    generic_or1k_vector_bit( vector ) ) != 0;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( vector != GENERIC_OR1K_UART_VECTOR ) {
    return RTEMS_UNSATISFIED;
  }

  OR1K_REG( OR1K_BSP_UART_REG_INT_ENABLE ) = OR1K_BSP_UART_REG_INT_ENABLE_THRI;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_clear( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( vector == GENERIC_OR1K_UART_VECTOR ) {
    OR1K_REG( OR1K_BSP_UART_REG_INT_ENABLE ) = 0;
  }

  _OR1K_mtspr( CPU_OR1K_SPR_PICSR, generic_or1k_vector_bit( vector ) );
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

  /* The interrupt controller of the architecture has no priorities */
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
  return RTEMS_UNSATISFIED;
}

rtems_status_code bsp_interrupt_vector_is_enabled(
  rtems_vector_number vector,
  bool               *enabled
)
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( enabled != NULL );
  *enabled = ( _OR1K_mfspr( CPU_OR1K_SPR_PICMR ) &
    generic_or1k_vector_bit( vector ) ) != 0;
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector )
{
  rtems_interrupt_level level;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  rtems_interrupt_local_disable( level );
  _OR1K_mtspr(
    CPU_OR1K_SPR_PICMR,
    _OR1K_mfspr( CPU_OR1K_SPR_PICMR ) | generic_or1k_vector_bit( vector )
  );
  rtems_interrupt_local_enable( level );
  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector )
{
  rtems_interrupt_level level;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  rtems_interrupt_local_disable( level );
  _OR1K_mtspr(
    CPU_OR1K_SPR_PICMR,
    _OR1K_mfspr( CPU_OR1K_SPR_PICMR ) & ~generic_or1k_vector_bit( vector )
  );
  rtems_interrupt_local_enable( level );
  return RTEMS_SUCCESSFUL;
}
