/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSImplClassicIntr
 *
 * @brief Interrupt controller support of the TX3904.
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
#include <rtems/score/mips.h>
#include <rtems/mips/idtcpu.h>

/*
 * Interrupt controller of the TX3904.  A source passes when its level in the
 * interrupt level registers is greater than the level in the interrupt mask
 * register, so a level of zero disables a source.  Each interrupt level
 * register holds the three bit level of four sources, one per byte.
 */
#define TX3904_IRC_BASE 0xffffc000

#define TX3904_IRC_ISR ( TX3904_IRC_BASE + 0x00 )
#define TX3904_IRC_IMR ( TX3904_IRC_BASE + 0x04 )
#define TX3904_IRC_ILR ( TX3904_IRC_BASE + 0x10 )

#define TX3904_IRC_LEVEL_MAX 7U

/* Sources which the controller levels, INT1 up to TMR2 */
#define TX3904_IRC_SOURCE_COUNT 16U

/*
 * The two software interrupts of the processor are pending in the cause
 * register.  They are the only sources this board can raise.
 */
#define TX3904_CAUSE_SOFTWARE_1 ( UINT32_C( 1 ) << ( CAUSE_IPSHIFT + 1 ) )
#define TX3904_CAUSE_SOFTWARE_2 ( UINT32_C( 1 ) << CAUSE_IPSHIFT )

/*
 * The level a source gets when it is enabled.  A zero entry means the source
 * keeps the default, which is the highest level, since the interrupt mask
 * register stays at zero and every source is meant to pass.
 */
static uint8_t tx3904_irc_priority[ TX3904_IRC_SOURCE_COUNT ];

static uint32_t tx3904_irc_priority_of( uint32_t source )
{
  uint8_t priority;

  priority = tx3904_irc_priority[ source ];

  return priority != 0 ? priority : TX3904_IRC_LEVEL_MAX;
}

static volatile uint32_t *tx3904_irc_reg( uint32_t address )
{
  return (volatile uint32_t *) address;
}

static uint32_t tx3904_irc_source( rtems_vector_number vector )
{
  return vector - MIPS_INTERRUPT_BASE;
}

static bool tx3904_irc_is_leveled( rtems_vector_number vector )
{
  uint32_t source;

  if ( vector < MIPS_INTERRUPT_BASE ) {
    return false;
  }

  source = tx3904_irc_source( vector );

  return source < TX3904_IRC_SOURCE_COUNT;
}

static uint32_t tx3904_irc_get_level( uint32_t source )
{
  uint32_t ilr;

  ilr = *tx3904_irc_reg( TX3904_IRC_ILR + ( source / 4 ) * 4 );

  return ( ilr >> ( ( source % 4 ) * 8 ) ) & TX3904_IRC_LEVEL_MAX;
}

static void tx3904_irc_set_level( uint32_t source, uint32_t level )
{
  volatile uint32_t *ilr;
  unsigned int       shift;
  rtems_interrupt_level isr_level;

  ilr = tx3904_irc_reg( TX3904_IRC_ILR + ( source / 4 ) * 4 );
  shift = ( source % 4 ) * 8;

  rtems_interrupt_local_disable( isr_level );
  *ilr = ( *ilr & ~( TX3904_IRC_LEVEL_MAX << shift ) ) | ( level << shift );
  rtems_interrupt_local_enable( isr_level );
}

static bool tx3904_is_software( rtems_vector_number vector )
{
  return vector == TX3904_IRQ_SOFTWARE_1 || vector == TX3904_IRQ_SOFTWARE_2;
}

static uint32_t tx3904_software_bit( rtems_vector_number vector )
{
  return vector == TX3904_IRQ_SOFTWARE_1 ?
    TX3904_CAUSE_SOFTWARE_1 : TX3904_CAUSE_SOFTWARE_2;
}

/*
 * The sources which do not pass the controller are masked in the processor.
 * INT0 is tied to IP[5] and the two software interrupts are IP[1] and IP[0].
 * The pending bit in the cause register and the mask bit in the status
 * register have the same position.
 */
static uint32_t tx3904_cpu_bit( rtems_vector_number vector )
{
  if ( vector == TX3904_IRQ_INT0 ) {
    return UINT32_C( 1 ) << ( CAUSE_IPSHIFT + 7 );
  }

  if ( tx3904_is_software( vector ) ) {
    return tx3904_software_bit( vector );
  }

  return 0;
}

rtems_status_code bsp_interrupt_get_attributes(
  rtems_vector_number         vector,
  rtems_interrupt_attributes *attributes
)
{
  if ( vector < MIPS_INTERRUPT_BASE ) {
    /*
     * The vectors below the first interrupt are the exception causes of the
     * processor.  They are not interrupt sources and nothing can mask them.
     */
    return RTEMS_SUCCESSFUL;
  }

  attributes->is_maskable = true;

  if ( tx3904_irc_is_leveled( vector ) ) {
    attributes->can_enable = true;
    attributes->can_disable = true;
    attributes->can_get_priority = true;
    attributes->can_set_priority = true;
    attributes->maximum_priority = TX3904_IRC_LEVEL_MAX;
  } else if ( tx3904_is_software( vector ) ) {
    attributes->can_raise = true;
    attributes->can_clear = true;
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

  if ( tx3904_irc_is_leveled( vector ) ) {
    uint32_t source;

    /*
     * A bit of the interrupt status register reads as zero while its source
     * is asserted.  Every bit is set after a reset.
     */
    source = tx3904_irc_source( vector );
    *pending = ( *tx3904_irc_reg( TX3904_IRC_ISR ) &
      ( UINT32_C( 1 ) << source ) ) == 0;
  } else if ( tx3904_cpu_bit( vector ) != 0 ) {
    uint32_t cause;

    mips_get_cause( cause );
    *pending = ( cause & tx3904_cpu_bit( vector ) ) != 0;
  } else {
    /* An exception is never pending */
    *pending = false;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_raise( rtems_vector_number vector )
{
  uint32_t cause;
  rtems_interrupt_level level;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( !tx3904_is_software( vector ) ) {
    return RTEMS_UNSATISFIED;
  }

  rtems_interrupt_local_disable( level );
  mips_get_cause( cause );
  cause |= tx3904_software_bit( vector );
  mips_set_cause( cause );
  rtems_interrupt_local_enable( level );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_clear( rtems_vector_number vector )
{
  uint32_t cause;
  rtems_interrupt_level level;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( !tx3904_is_software( vector ) ) {
    return RTEMS_UNSATISFIED;
  }

  rtems_interrupt_local_disable( level );
  mips_get_cause( cause );
  cause &= ~tx3904_software_bit( vector );
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

  if ( tx3904_irc_is_leveled( vector ) ) {
    uint32_t source;

    source = tx3904_irc_source( vector );
    *enabled = tx3904_irc_get_level( source ) >
      *tx3904_irc_reg( TX3904_IRC_IMR );
  } else {
    /*
     * The remaining sources are masked in the status register, which the
     * context of every thread enables completely, so they are always enabled.
     */
    *enabled = true;
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_enable( rtems_vector_number vector )
{
  uint32_t source;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( !tx3904_irc_is_leveled( vector ) ) {
    return RTEMS_UNSATISFIED;
  }

  source = tx3904_irc_source( vector );
  tx3904_irc_set_level( source, tx3904_irc_priority_of( source ) );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_vector_disable( rtems_vector_number vector )
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( !tx3904_irc_is_leveled( vector ) ) {
    return RTEMS_UNSATISFIED;
  }

  tx3904_irc_set_level( tx3904_irc_source( vector ), 0 );

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_set_priority(
  rtems_vector_number vector,
  uint32_t            priority
)
{
  uint32_t source;

  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );

  if ( !tx3904_irc_is_leveled( vector ) ) {
    return RTEMS_UNSATISFIED;
  }

  if ( priority == 0 || priority > TX3904_IRC_LEVEL_MAX ) {
    return RTEMS_INVALID_PRIORITY;
  }

  source = tx3904_irc_source( vector );
  tx3904_irc_priority[ source ] = (uint8_t) priority;

  /* Take the new priority only if the source is not disabled */
  if ( tx3904_irc_get_level( source ) != 0 ) {
    tx3904_irc_set_level( source, priority );
  }

  return RTEMS_SUCCESSFUL;
}

rtems_status_code bsp_interrupt_get_priority(
  rtems_vector_number vector,
  uint32_t           *priority
)
{
  bsp_interrupt_assert( bsp_interrupt_is_valid_vector( vector ) );
  bsp_interrupt_assert( priority != NULL );

  if ( !tx3904_irc_is_leveled( vector ) ) {
    return RTEMS_UNSATISFIED;
  }

  *priority = tx3904_irc_priority_of( tx3904_irc_source( vector ) );

  return RTEMS_SUCCESSFUL;
}
